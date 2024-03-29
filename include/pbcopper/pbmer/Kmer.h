// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBCOPPER_PBMER_KMER_H
#define PBCOPPER_PBMER_KMER_H

#include <pbcopper/PbcopperConfig.h>

#include <cstdint>

#include <string>

#include <pbcopper/data/Strand.h>

namespace PacBio {
namespace Pbmer {

class Kmer
{
public:
    // 32 DNA bases are packed into a uint64
    uint64_t mer = 0;

    // position of the kmer
    uint32_t pos = 1;

    // strand of the kmer
    Data::Strand strand = Data::Strand::FORWARD;

    Kmer();
    Kmer(Data::Strand s);
    Kmer(uint64_t k, uint32_t p, Data::Strand s);

    bool operator==(Kmer const& b) const;
    bool operator!=(Kmer const& b) const;

    // hashes both kmers then return a bool evaluating a <= b
    void ReverseComp(uint8_t kmerSize);
    bool LexSmaller(Kmer const& b, uint8_t kmerSize) const;
    std::string KmerToStr(uint8_t kmerSize) const;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_KMER_H
