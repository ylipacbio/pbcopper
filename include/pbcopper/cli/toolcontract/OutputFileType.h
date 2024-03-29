// Author: Derek Barnett

#ifndef PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_H
#define PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_H

#include <pbcopper/PbcopperConfig.h>

#include <string>

namespace PacBio {
namespace CLI {
namespace ToolContract {

class OutputFileType
{
public:
    OutputFileType(std::string id, std::string title, std::string description, std::string type,
                   std::string defaultName);

public:
    const std::string& Id() const;
    const std::string& Title() const;
    const std::string& DefaultName() const;
    const std::string& Description() const;
    const std::string& Type() const;

private:
    std::string id_;
    std::string title_;
    std::string description_;
    std::string type_;
    std::string defaultName_;
};

}  // namespace ToolContract
}  // namespace CLI
}  // namespace PacBio

#include <pbcopper/cli/toolcontract/internal/OutputFileType-inl.h>

#endif  // PBCOPPER_CLI_TOOLCONTRACT_OUTPUTFILETYPE_H
