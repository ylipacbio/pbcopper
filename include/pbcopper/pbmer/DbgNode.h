// Authors: Chris Dunn, Zev Kronenberg, Derek Barnett

#ifndef PBCOPPER_PBMER_DBGNODE_H
#define PBCOPPER_PBMER_DBGNODE_H

#include <pbcopper/PbcopperConfig.h>

#include <cstddef>
#include <cstdint>

#include <iterator>

#include <boost/dynamic_bitset.hpp>

#include <pbcopper/pbmer/DnaBit.h>

namespace PacBio {
namespace Pbmer {

class DbgNode
{
public:
    ///
    /// Construct DbgNode
    ///
    /// \param d    dna class
    /// \param o    out edges
    ///
    DbgNode(const DnaBit& d, uint8_t o);

    ///
    /// Adds a read id to the readIds_ variable.
    ///
    /// \param rid  read id
    ///
    bool AddLoad(uint32_t rid);

    ///
    /// \brief Uses a bit field to set out edges, possibilities {bit0:A, bit2:C,
    ///        bit3:G, bit4:T}
    ///
    /// \param o    out edges to set: `outEdges_ |= o`
    ///
    void SetEdges(uint8_t o);

    ///
    /// \return number of left edges
    ///
    int LeftEdgeCount() const;

    ///
    /// \return number of right edges
    ///
    int RightEdgeCount() const;

    ///
    /// \return number of edges
    ///
    int TotalEdgeCount() const;

public:
    /// \name Iterable
    /// \{

    class iterator_base
    {
    public:
        using value_type = DnaBit;
        using difference_type = std::ptrdiff_t;
        using iterator_category = std::input_iterator_tag;

        ~iterator_base();
        bool operator==(const iterator_base& other) const;
        bool operator!=(const iterator_base& other) const;

    protected:
        iterator_base();
        iterator_base(DbgNode& node);
        void LoadNext();

    protected:
        DbgNode* node_ = nullptr;
        uint8_t index_ = 0;
        DnaBit value_;
    };

    class iterator : public iterator_base
    {
    public:
        using pointer = DnaBit*;
        using reference = DnaBit&;

        iterator();
        iterator(DbgNode& node);
        DnaBit& operator*();
        DnaBit* operator->();
        iterator& operator++();
        iterator operator++(int);
    };

    struct const_iterator : public iterator_base
    {
    public:
        using pointer = const DnaBit*;
        using reference = const DnaBit&;

        const_iterator();
        const_iterator(const DbgNode& node);
        const DnaBit& operator*() const;
        const DnaBit* operator->() const;
        const_iterator& operator++();
        const_iterator operator++(int);
    };

    const_iterator begin() const;
    const_iterator cbegin() const;
    iterator begin();

    const_iterator end() const;
    const_iterator cend() const;
    iterator end();

    /// \}

private:
    DnaBit dna_;
    uint8_t edges_;
    boost::dynamic_bitset<> readIds2_;
    friend class Dbg;
};

}  // namespace Pbmer
}  // namespace PacBio

#endif  // PBCOPPER_PBMER_DBGNODE_H