// Author: Derek Barnett

#ifndef PBCOPPER_CLI_v2_OPTIONDATA_H
#define PBCOPPER_CLI_v2_OPTIONDATA_H

#include <pbcopper/PbcopperConfig.h>

#include <algorithm>
#include <string>
#include <vector>

#include <boost/optional.hpp>

#include <pbcopper/cli2/OptionValue.h>

namespace PacBio {
namespace CLI_v2 {
namespace internal {

///
/// Properties for interface options
///
struct OptionData
{
    std::vector<std::string> names;
    std::vector<std::string> hiddenNames;
    std::string description;

    OptionValueType type = OptionValueType::BOOL;
    boost::optional<OptionValue> defaultValue;

    bool isHidden = false;

    std::vector<OptionValue> choices;
    bool isChoicesHidden = false;

    inline bool operator==(const OptionData& other) const
    {
        return std::equal(names.cbegin(), names.cend(), other.names.cbegin());
    }
};

///
/// Named group of options
///
struct OptionGroupData
{
    OptionGroupData() = default;

    OptionGroupData(std::string n, std::vector<OptionData> opts)
        : name{std::move(n)}, options{std::move(opts)}
    {
    }

    std::string name;
    std::vector<OptionData> options;
};

}  // namespace internal
}  // namespace CLI_v2
}  // namespace PacBio

namespace std {

template <>
struct hash<PacBio::CLI_v2::internal::OptionData>
{
    size_t operator()(const PacBio::CLI_v2::internal::OptionData& k) const
    {
        return std::hash<std::string>()(k.names[0]);
    }
};

}  // namespace std

#endif  // PBCOPPER_CLI_v2_OPTIONDATA_H
