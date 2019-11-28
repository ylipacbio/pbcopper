// Author: Nathaniel Echols, Derek Barnett

#ifndef PBCOPPER_UTILITY_ALARM_H
#define PBCOPPER_UTILITY_ALARM_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <iosfwd>
#include <string>
#include <vector>

namespace PacBio {
namespace Utility {

class Alarm
{
public:
    static void WriteAlarms(const std::string& fn, const std::vector<Alarm>& alarms);
    static void WriteAlarms(std::ostream& out, const std::vector<Alarm>& alarms);

public:
    Alarm(std::string name, std::string message);
    Alarm(std::string name, std::string message, std::string severity, std::string info,
          std::string exception);

    const std::string& Exception() const;
    Alarm& Exception(std::string exception);

    const std::string& Id() const;
    Alarm& Id(std::string uuid);

    const std::string& Info() const;
    Alarm& Info(std::string info);

    const std::string& Message() const;
    Alarm& Message(std::string message);

    const std::string& Name() const;
    Alarm& Name(std::string name);

    const std::string& Owner() const;
    Alarm& Owner(std::string owner);

    const std::string& Severity() const;
    Alarm& Severity(std::string severity);

    // print json format
    void Print(const std::string& fn) const;
    void Print(std::ostream& out) const;

private:
    std::string message_;
    std::string name_;
    std::string exception_;
    std::string info_;
    std::string severity_;
    std::string owner_;
    std::string uuid_;
};

std::ostream& operator<<(std::ostream& out, const Alarm& alarm);

// AlarmException does not inherit from std::exception on purpose:
// The idea is that AlarmException is orthogonal to the standard C++
// exception hierarchy in order to bypass the standard
// try { ... } catch(const std::exception&) { ... }
// blocks, such that only CLIv2 will eventually catch it.
class AlarmException
{
public:
    AlarmException(std::string sourceFilename, std::string functionName, int32_t lineNumber,
                   std::string name, std::string message, std::string severity, std::string info,
                   std::string exception) noexcept;

    // Debugging Info
    const char* SourceFilename() const noexcept;

    const char* FunctionName() const noexcept;

    int32_t LineNumber() const noexcept;

    // Used by Alarms API
    const char* Name() const noexcept;

    const char* Message() const noexcept;

    const char* Severity() const noexcept;

    const char* Info() const noexcept;

    const char* Exception() const noexcept;

protected:
    std::string sourceFilename_;
    std::string functionName_;
    int32_t lineNumber_;

    std::string name_;
    std::string message_;
    std::string severity_;
    std::string info_;
    std::string exception_;
};

}  // namespace Utility
}  // namespace PacBio

#define PB_ALARM_EXCEPTION_IMPL(name, message, severity, info, exception)                        \
    PacBio::Utility::AlarmException(__FILE__, __func__, __LINE__, name, message, severity, info, \
                                    exception)

#endif  // PBCOPPER_UTILITY_ALARM_H
