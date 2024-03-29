// Author: Lance Hepler & Armin Töpfer

#ifndef PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H
#define PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H

#include <pbcopper/PbcopperConfig.h>

#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <future>
#include <mutex>
#include <queue>

#include <boost/optional.hpp>

namespace PacBio {
namespace Parallel {

/**
 Differences from normal FireAndForget:

 * ProduceWith expects a function taking Index followed by user-defined Args.
 * The Index is from 0 to size-1.
 * Each thread knows its own Index.
 * As a result, each task can use its Index to look into a predefined vector (user-controlled).
 * A 'finish' function is called for each thread after Finalize() pushes the sentinel onto the queue.
 
 The 'finish' function is not really needed, but it's helpful for debugging. Or
 to summarize results / close output streams.

 Caution: If an exception is thrown by your task, the 'finish' function is not called!
*/
class FireAndForgetIndexed
{
public:
    using Index = size_t;
    using TFunc = std::function<void(Index)>;
    using TPTask = std::packaged_task<void(Index)>;

public:
    FireAndForgetIndexed(const size_t size, const size_t mul = 2,
                         TFunc finish = TFunc{[](Index) {}})
        : exc{nullptr}, sz{size * mul}, abort{false}
    {
        for (Index index = 0; index < size; ++index) {
            threads.emplace_back(std::thread([this, finish, index]() {
                // Get the first task per thread
                TTask task = PopTask();
                do {
                    try {
                        // Check if queue should be aborted and
                        // if there is a task
                        if (!abort && task) {
                            // Execute task
                            (*task)(index);
                            // Check for the return value / exception
                            task->get_future().get();
                        };
                    } catch (...) {
                        std::lock_guard<std::mutex> g(m);
                        // If there is an exception, signal to abort queue
                        abort = true;
                        // And store exception
                        exc = std::current_exception();
                        popped.notify_one();
                    }
                    task = PopTask();
                } while (!abort && task);  // Stop if there are no tasks or abort has been signaled
                try {
                    if (!abort) finish(index);
                } catch (...) {
                    std::lock_guard<std::mutex> g(m);
                    // If there is an exception, signal to abort queue
                    abort = true;
                    // And store exception
                    exc = std::current_exception();
                }
            }));
        }
    }

    template <typename F, typename... Args>
    void ProduceWith(F&& f, Args&&... args)
    {
        using namespace std::placeholders;  // for _1, _2, _3...

        // Create a function taking Index, which delegates to
        // a function taking Index followed by args.
        TPTask task{std::bind(std::forward<F>(f), _1, std::forward<Args>(args)...)};

        {
            std::unique_lock<std::mutex> lk(m);
            popped.wait(lk, [&task, this]() {
                if (exc) std::rethrow_exception(exc);

                if (head.size() < sz) {
                    head.emplace(std::move(task));
                    return true;
                }

                return false;
            });
        }
        pushed.notify_one();
    }

    void Finalize()
    {
        {
            std::lock_guard<std::mutex> g(m);
            // Push boost::none to signal that there are no further tasks
            head.emplace(boost::none);
        }
        // Let all workers know that they should look that there is no further work
        pushed.notify_all();

        // Wait for all threads to join and do not continue before all tasks
        // have been finishshed.
        for (auto& thread : threads)
            thread.join();

        // Is there a final exception, throw if so..
        if (exc) std::rethrow_exception(exc);
    }

private:
    using TTask = boost::optional<TPTask>;

    TTask PopTask()
    {
        TTask task(boost::none);

        {
            std::unique_lock<std::mutex> lk(m);
            pushed.wait(lk, [&task, this]() {
                if (head.empty()) return false;

                if ((task = std::move(head.front()))) {
                    head.pop();
                }

                return true;
            });
        }
        popped.notify_one();

        return task;
    }

    std::vector<std::thread> threads;
    std::queue<TTask> head;
    std::condition_variable popped;
    std::condition_variable pushed;
    std::exception_ptr exc;
    std::mutex m;
    size_t sz;
    std::atomic_bool abort;
};

}  // namespace Parallel
}  // namespace PacBio

#endif  // PBCOPPER_PARALLEL_FIREANDFORGETINDEXED_H
