#include <pbcopper/data/Interval.h>

#include <gtest/gtest.h>

TEST(Data_Interval, default_interval_contains_zeroes)
{
    PacBio::Data::Interval empty;
    EXPECT_EQ(0, empty.Start());
    EXPECT_EQ(0, empty.End());
}

TEST(Data_Interval, construction_with_explicit_start_and_end_retained)
{
    PacBio::Data::Interval normal(5, 8);
    EXPECT_EQ(5, normal.Start());
    EXPECT_EQ(8, normal.End());
}

TEST(Data_Interval, construction_without_explict_endpoint_yields_endpoint_of_start_plus_one)
{
    PacBio::Data::Interval singleton(4);
    EXPECT_EQ(4, singleton.Start());
    EXPECT_EQ(5, singleton.End());
}

TEST(Data_Interval, self_equals_self)
{
    PacBio::Data::Interval empty;
    PacBio::Data::Interval singleton(4);
    PacBio::Data::Interval normal(5, 8);

    EXPECT_TRUE(empty == empty);
    EXPECT_TRUE(singleton == singleton);
    EXPECT_TRUE(normal == normal);
}

TEST(Data_Interval, intervals_with_same_endpoints_are_equal)
{
    PacBio::Data::Interval empty;
    PacBio::Data::Interval sameAsEmpty;
    PacBio::Data::Interval singleton(4);
    PacBio::Data::Interval sameAsSingleton(4, 5);
    PacBio::Data::Interval normal(5, 8);
    PacBio::Data::Interval sameAsNormal(5, 8);

    EXPECT_TRUE(empty == sameAsEmpty);
    EXPECT_TRUE(singleton == sameAsSingleton);
    EXPECT_TRUE(normal == sameAsNormal);
}

TEST(Data_Interval, intervals_with_different_endpoints_are_not_equal)
{
    PacBio::Data::Interval empty;
    PacBio::Data::Interval singleton(4);
    PacBio::Data::Interval normal(5, 8);
    PacBio::Data::Interval different(20, 40);

    EXPECT_FALSE(empty == singleton);
    EXPECT_FALSE(empty == normal);
    EXPECT_FALSE(empty == different);
    EXPECT_FALSE(singleton == normal);
    EXPECT_FALSE(normal == different);
}

TEST(Data_Interval, can_be_copy_constructed)
{
    PacBio::Data::Interval original(5, 8);
    PacBio::Data::Interval copy(original);
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, can_be_copy_assigned)
{
    PacBio::Data::Interval original(5, 8);
    PacBio::Data::Interval copy = original;
    EXPECT_TRUE(original == copy);
}

TEST(Data_Interval, modification_of_copy_does_not_affect_original)
{
    PacBio::Data::Interval original(5, 8);
    PacBio::Data::Interval copy(original);
    copy.Start(2);
    copy.End(10);

    EXPECT_FALSE(original == copy);
    EXPECT_EQ(2, copy.Start());
    EXPECT_EQ(10, copy.End());
}

TEST(Data_Interval, self_covers_and_is_covered_by_self)
{
    PacBio::Data::Interval interval(2, 4);
    EXPECT_TRUE(interval.Covers(interval));
    EXPECT_TRUE(interval.CoveredBy(interval));
}

TEST(Data_Interval, covers_and_coveredby_are_reciprocal)
{
    PacBio::Data::Interval inner(3, 5);
    PacBio::Data::Interval outer(1, 7);

    EXPECT_TRUE(inner.CoveredBy(outer));  // a.coveredBy(b)
    EXPECT_TRUE(outer.Covers(inner));     // thus b.covers(a)

    EXPECT_FALSE(inner == outer);       // if a != b
    EXPECT_FALSE(inner.Covers(outer));  // then !a.covers(b)
}

TEST(Data_Interval, completely_disjoint_intervals_do_not_cover_each_other)
{
    PacBio::Data::Interval left(3, 5);
    PacBio::Data::Interval right(6, 8);

    EXPECT_FALSE(left.Covers(right));
    EXPECT_FALSE(right.Covers(left));
    EXPECT_FALSE(left.CoveredBy(right));
    EXPECT_FALSE(right.CoveredBy(left));
}

TEST(Data_Interval, no_coverage_when_left_stop_same_as_right_start)
{
    PacBio::Data::Interval left(3, 5);
    PacBio::Data::Interval right(5, 8);

    EXPECT_FALSE(left.Covers(right));
    EXPECT_FALSE(left.CoveredBy(right));
}

TEST(Data_Interval, coverage_when_endpoints_are_same_and_inner_start_contained_by_outer)
{
    PacBio::Data::Interval inner(6, 8);
    PacBio::Data::Interval outer(5, 8);

    EXPECT_TRUE(outer.Covers(inner));
    EXPECT_TRUE(inner.CoveredBy(outer));
}

TEST(Data_Interval, calculates_proper_intersection)
{
    PacBio::Data::Interval interval1(2, 4);
    PacBio::Data::Interval interval2(3, 5);
    PacBio::Data::Interval interval3(6, 8);
    PacBio::Data::Interval interval4(1, 7);
    PacBio::Data::Interval interval5(5, 8);

    EXPECT_TRUE(interval1.Intersects(interval1));  // self-intersection: a.intersects(a)

    EXPECT_TRUE(interval1.Intersects(interval2));  // if a.intersects(b)
    EXPECT_TRUE(interval2.Intersects(interval1));  // then b.intersects(a)

    EXPECT_TRUE(interval4.Covers(interval1));      // if b.covers(a),
    EXPECT_TRUE(interval1.Intersects(interval4));  // then a.intersects(b)
    EXPECT_TRUE(interval4.Intersects(interval1));  // and b.intersects(a)

    EXPECT_FALSE(interval2.Intersects(interval3));  // b.start > a.stop (obvious disjoint)
    EXPECT_FALSE(interval2.Intersects(
        interval5));  // b.start == a.stop (intervals are right open, so disjoint)
}

TEST(Data_Interval, acceptable_endpoints_are_valid)
{
    PacBio::Data::Interval zeroStartInterval(0, 1);
    PacBio::Data::Interval nonZeroStartInterval(4, 5);

    EXPECT_TRUE(zeroStartInterval.IsValid());
    EXPECT_TRUE(nonZeroStartInterval.IsValid());
}

TEST(Data_Interval, nonsensical_endpoints_are_invalid)
{
    PacBio::Data::Interval defaultConstructedInterval;
    PacBio::Data::Interval zeroEmptyValue(0, 0);
    PacBio::Data::Interval nonZeroEmptyInterval(4, 4);
    PacBio::Data::Interval badOrderingInterval(5, 4);

    EXPECT_FALSE(defaultConstructedInterval.IsValid());
    EXPECT_FALSE(zeroEmptyValue.IsValid());
    EXPECT_FALSE(nonZeroEmptyInterval.IsValid());
    EXPECT_FALSE(badOrderingInterval.IsValid());
}

TEST(Data_Interval, calculates_proper_length)
{
    PacBio::Data::Interval interval1(2, 4);
    PacBio::Data::Interval interval2(3, 5);
    PacBio::Data::Interval interval3(6, 8);
    PacBio::Data::Interval interval4(1, 7);
    PacBio::Data::Interval interval5(5, 8);

    EXPECT_EQ(2, interval1.Length());
    EXPECT_EQ(2, interval2.Length());
    EXPECT_EQ(2, interval3.Length());
    EXPECT_EQ(6, interval4.Length());
    EXPECT_EQ(3, interval5.Length());
}

TEST(Data_Interval, constructors)
{
    PacBio::Data::Interval empty;
    PacBio::Data::Interval singleton(4);
    PacBio::Data::Interval normal(5, 8);

    EXPECT_EQ(0, empty.Start());
    EXPECT_EQ(0, empty.End());

    EXPECT_EQ(4, singleton.Start());
    EXPECT_EQ(5, singleton.End());

    EXPECT_EQ(5, normal.Start());
    EXPECT_EQ(8, normal.End());

    // TODO: check out-of-order intervals, etc
}

TEST(Data_Interval, equality_test)
{
    PacBio::Data::Interval empty;
    PacBio::Data::Interval empty2;

    PacBio::Data::Interval singleton(4);
    PacBio::Data::Interval sameAsSingleton(4, 5);

    PacBio::Data::Interval normal(5, 8);
    PacBio::Data::Interval sameAsNormal(5, 8);

    PacBio::Data::Interval different(20, 40);

    // self-equality
    EXPECT_TRUE(empty == empty);
    EXPECT_TRUE(singleton == singleton);
    EXPECT_TRUE(normal == normal);
    EXPECT_TRUE(different == different);

    // same values equality
    EXPECT_TRUE(empty == empty2);
    EXPECT_TRUE(singleton == sameAsSingleton);
    EXPECT_TRUE(normal == sameAsNormal);

    // different values
    EXPECT_FALSE(empty == singleton);
    EXPECT_FALSE(empty == normal);
    EXPECT_FALSE(empty == different);
    EXPECT_FALSE(singleton == normal);
    EXPECT_FALSE(normal == different);
}

TEST(Data_Interval, copy)
{
    PacBio::Data::Interval interval1(5, 8);
    PacBio::Data::Interval interval2(interval1);
    PacBio::Data::Interval interval3 = interval1;

    EXPECT_TRUE(interval1 == interval1);
    EXPECT_TRUE(interval1 == interval2);
    EXPECT_TRUE(interval1 == interval3);
}

TEST(Data_Interval, modifier)
{
    PacBio::Data::Interval interval1(5, 8);
    PacBio::Data::Interval interval2(interval1);
    interval2.Start(2);
    interval2.End(10);

    EXPECT_FALSE(interval1 == interval2);
    EXPECT_EQ(2, interval2.Start());
    EXPECT_EQ(10, interval2.End());
}

TEST(Data_Interval, cover_test)
{
    PacBio::Data::Interval interval1(2, 4);
    PacBio::Data::Interval interval2(3, 5);
    PacBio::Data::Interval interval3(6, 8);
    PacBio::Data::Interval interval4(1, 7);
    PacBio::Data::Interval interval5(5, 8);

    EXPECT_TRUE(interval1.Covers(interval1));     // self-cover: a.covers(a)
    EXPECT_TRUE(interval1.CoveredBy(interval1));  // self-cover: a.coveredBy(a)

    EXPECT_TRUE(interval2.CoveredBy(interval4));  // a.coveredBy(b)
    EXPECT_TRUE(interval4.Covers(interval2));     // thus b.covers(a)
    EXPECT_FALSE(interval2 == interval4);         // if a != b
    EXPECT_FALSE(interval2.Covers(interval4));    // then !a.covers(b)

    EXPECT_FALSE(interval2.Covers(interval3));  // completely disjoint
    EXPECT_FALSE(interval3.Covers(interval2));
    EXPECT_FALSE(interval2.CoveredBy(interval3));
    EXPECT_FALSE(interval3.CoveredBy(interval2));

    EXPECT_FALSE(interval2.Covers(interval5));  // a.stop == b.start
    EXPECT_FALSE(interval2.CoveredBy(interval5));

    EXPECT_TRUE(interval5.Covers(interval3));  // shared endpoint, start contained, thus a.covers(b)
    EXPECT_TRUE(interval3.CoveredBy(interval5));  // and b.coveredBy(a)
}

TEST(Data_Interval, intersect_test)
{
    PacBio::Data::Interval interval1(2, 4);
    PacBio::Data::Interval interval2(3, 5);
    PacBio::Data::Interval interval3(6, 8);
    PacBio::Data::Interval interval4(1, 7);
    PacBio::Data::Interval interval5(5, 8);

    EXPECT_TRUE(interval1.Intersects(interval1));  // self-intersection: a.intersects(a)

    EXPECT_TRUE(interval1.Intersects(interval2));  // if a.intersects(b)
    EXPECT_TRUE(interval2.Intersects(interval1));  // then b.intersects(a)

    EXPECT_TRUE(interval4.Covers(interval1));      // if b.covers(a),
    EXPECT_TRUE(interval1.Intersects(interval4));  // then a.intersects(b)
    EXPECT_TRUE(interval4.Intersects(interval1));  // and b.intersects(a)

    EXPECT_FALSE(interval2.Intersects(interval3));  // b.start > a.stop (obvious disjoint)
    EXPECT_FALSE(interval2.Intersects(
        interval5));  // b.start == a.stop (intervals are right open, so disjoint)
}

TEST(Data_Interval, validity_test)
{
    PacBio::Data::Interval interval1;        // default ctor
    PacBio::Data::Interval interval2(0, 0);  // start == stop (zero)
    PacBio::Data::Interval interval3(4, 4);  // start == stop (nonzero)
    PacBio::Data::Interval interval4(0, 1);  // start < stop  (start is zero)
    PacBio::Data::Interval interval5(4, 5);  // start < stop  (start is nonzero)
    PacBio::Data::Interval interval6(5, 4);  // start > stop

    EXPECT_FALSE(interval1.IsValid());
    EXPECT_FALSE(interval2.IsValid());
    EXPECT_FALSE(interval3.IsValid());
    EXPECT_TRUE(interval4.IsValid());
    EXPECT_TRUE(interval5.IsValid());
    EXPECT_FALSE(interval6.IsValid());
}

TEST(Data_Interval, length_test)
{
    PacBio::Data::Interval interval1(2, 4);
    PacBio::Data::Interval interval2(3, 5);
    PacBio::Data::Interval interval3(6, 8);
    PacBio::Data::Interval interval4(1, 7);
    PacBio::Data::Interval interval5(5, 8);

    EXPECT_EQ(2, interval1.Length());
    EXPECT_EQ(2, interval2.Length());
    EXPECT_EQ(2, interval3.Length());
    EXPECT_EQ(6, interval4.Length());
    EXPECT_EQ(3, interval5.Length());

    // TODO: check out-of-order intervals, etc
}
