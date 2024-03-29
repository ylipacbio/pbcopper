// Author: Derek Barnett

#ifndef PBCOPPER_UTILITY_DELETERS_H
#define PBCOPPER_UTILITY_DELETERS_H

#include <cstdio>

namespace PacBio {
namespace Utility {

//
// Convenience deleters intended for use with std::shared_ptr<T>,
// std::unique_ptr<T>, etc.
//

struct FileDeleter
{
    void operator()(std::FILE* fp) const;
};

// For pointers originating from C 'malloc' (and friends), instead of 'new'
struct FreeDeleter
{
    void operator()(void* p) const;

    void operator()(const void* p) const;
};

}  // namespace Utility
}  // namespace PacBio

#endif  // PBCOPPER_UTILITY_DELETERS_H