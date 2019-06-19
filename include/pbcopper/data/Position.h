// Author: Derek Barnett

#ifndef PBCOPPER_DATA_POSITION_H
#define PBCOPPER_DATA_POSITION_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

namespace PacBio {
namespace Data {

typedef int32_t Position;

static const Position UnmappedPosition = Position{-1};

}  // namespace Data
}  // namespace PacBio

#endif  // PBCOPPER_DATA_POSITION_H
