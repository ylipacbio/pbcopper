// Author: Lance Hepler

#include <pbcopper/data/Read.h>

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include <pbcopper/data/Clipping.h>
#include <pbcopper/data/internal/ClippingImpl.h>

namespace PacBio {
namespace Data {

static_assert(std::is_copy_constructible<Read>::value, "Read(const Read&) is not = default");
static_assert(std::is_copy_assignable<Read>::value,
              "Read& operator=(const Read&) is not = default");

static_assert(std::is_nothrow_move_constructible<Read>::value, "Read(Read&&) is not = noexcept");
static_assert(std::is_nothrow_move_assignable<Read>::value ==
                  std::is_nothrow_move_assignable<std::string>::value,
              "");

Read::Read(Data::ReadId id, std::string seq, std::vector<uint8_t> pw, LocalContextFlags flags,
           Accuracy readAccuracy, SNR snr, std::string model)
    : Id{std::move(id)}
    , Seq{std::move(seq)}
    , PulseWidth{std::move(pw)}
    , Flags{std::move(flags)}
    , ReadAccuracy{std::move(readAccuracy)}
    , SignalToNoise{std::move(snr)}
    , Model{std::move(model)}
    , FullLength{Flags & Data::ADAPTER_BEFORE && Flags & Data::ADAPTER_AFTER}
{
    if (PulseWidth.size() != Seq.size()) {
        throw std::invalid_argument("Invalid Read (name=" + std::string(Id) +
                                    "): features PW/seq are of mismatched length: " +
                                    std::to_string(PulseWidth.size()) + " vs " +
                                    std::to_string(Seq.size()));
    }
}

Read::Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr)
    : Id{name}
    , Seq{std::move(seq)}
    , Qualities{std::move(qualities)}
    , QueryStart{0}
    , QueryEnd(Seq.size())
    , SignalToNoise{std::move(snr)}
{
}

Read::Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr,
           Position qStart, Position qEnd)
    : Id{name}
    , Seq{std::move(seq)}
    , Qualities{std::move(qualities)}
    , QueryStart{qStart}
    , QueryEnd{qEnd}
    , SignalToNoise{std::move(snr)}
{
}

Read::Read(const std::string& name, std::string seq, QualityValues qualities, SNR snr,
           Position qStart, Position qEnd, Frames pulseWidths, Frames ipd)
    : Id{name}
    , Seq{std::move(seq)}
    , PulseWidth{pulseWidths.Encode()}
    , Qualities{std::move(qualities)}
    , IPD{std::move(ipd)}
    , QueryStart{qStart}
    , QueryEnd{qEnd}
    , SignalToNoise{std::move(snr)}
{
}

Read Read::ClipTo(const int32_t begin, const int32_t end) const
{
    Read copy{
        this->Id,
        this->Seq.substr(begin, end - begin),
        std::vector<uint8_t>(this->PulseWidth.begin() + begin, this->PulseWidth.begin() + end),
        this->Flags,
        this->ReadAccuracy,
        this->SignalToNoise,
        this->Model};
    return copy;
}

size_t Read::Length() const { return Seq.length(); }

std::string Read::FullName() const { return Id; }

void ClipToQuery(Read& read, Position start, Position end)
{
    // skip out if clip not needed
    if (start <= read.QueryStart && end >= read.QueryEnd) return;

    // calculate clipping
    ClipToQueryConfig clipConfig{read.Seq.size(),
                                 read.QueryStart,
                                 read.QueryEnd,
                                 start,
                                 end,
                                 // TODO: these next are unused for unmapped clip-to-query
                                 UnmappedPosition,
                                 Strand::FORWARD,
                                 {},
                                 false};
    auto result = ClipToQuery(clipConfig);

    // apply clipping
    internal::ClipRead(read, std::move(result), start, end);
}

}  // namespace Data
}  // namespace PacBio