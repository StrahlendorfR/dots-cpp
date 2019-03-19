#include "dots/type/StructDescriptor.h"
#include "dots/type/EnumDescriptor.h"
#include "dots/type/Registry.h"
#include "DotsTestStruct.dots.h"
#include "StructDescriptorData.dots.h"
#include <gtest/gtest.h>

TEST(TestTStruct, SizeInDescriptorMatchesActualSize)
{
	DotsTestStruct dts;
	EXPECT_EQ(dts._descriptor().sizeOf(), sizeof(dts));
}

TEST(TestTStruct, AlignmentInDescriptorMatchesActualAlignment)
{
	DotsTestStruct dts;
	EXPECT_EQ(dts._descriptor().alignOf(), alignof(dts));
}

TEST(TestTStruct, PropertyOffsetsMatchActualOffsets)
{
	DotsTestStruct dts;

	auto determine_offset = [&](const auto& property) { return reinterpret_cast<size_t>(&property) - reinterpret_cast<size_t>(&dts); };
	EXPECT_EQ(DotsTestStruct::stringField_t::Offset(), determine_offset(dts.stringField));
	EXPECT_EQ(DotsTestStruct::indKeyfField_t::Offset(), determine_offset(dts.indKeyfField));
	EXPECT_EQ(DotsTestStruct::floatField_t::Offset(), determine_offset(dts.floatField));
	EXPECT_EQ(DotsTestStruct::enumField_t::Offset(), determine_offset(dts.enumField));
	EXPECT_EQ(DotsTestStruct::tp_t::Offset(), determine_offset(dts.tp));
	EXPECT_EQ(DotsTestStruct::subStruct_t::Offset(), determine_offset(dts.subStruct));
	EXPECT_EQ(DotsTestStruct::uuid_t::Offset(), determine_offset(dts.uuid));
}

TEST(TestTStruct, PropertyOffsetsInDescriptorMatchActualOffsets)
{
	DotsTestStruct dts;

	auto determine_offset = [&](const auto& property) { return reinterpret_cast<size_t>(&property) - reinterpret_cast<size_t>(&dts); };
	EXPECT_EQ(dts._descriptor().properties()[0].offset(), determine_offset(dts.stringField));
	EXPECT_EQ(dts._descriptor().properties()[1].offset(), determine_offset(dts.indKeyfField));
	EXPECT_EQ(dts._descriptor().properties()[2].offset(), determine_offset(dts.floatField));
	EXPECT_EQ(dts._descriptor().properties()[3].offset(), determine_offset(dts.enumField));
	EXPECT_EQ(dts._descriptor().properties()[4].offset(), determine_offset(dts.tp));
	EXPECT_EQ(dts._descriptor().properties()[5].offset(), determine_offset(dts.subStruct));
	EXPECT_EQ(dts._descriptor().properties()[6].offset(), determine_offset(dts.uuid));
}

TEST(TestTStruct, PropertiesHaveExpectedTags)
{
	DotsTestStruct dts;

	EXPECT_EQ(DotsTestStruct::stringField_t::Tag(), 1);
	EXPECT_EQ(DotsTestStruct::indKeyfField_t::Tag(), 2);
	EXPECT_EQ(DotsTestStruct::floatField_t::Tag(), 3);
	EXPECT_EQ(DotsTestStruct::enumField_t::Tag(), 4);
	EXPECT_EQ(DotsTestStruct::tp_t::Tag(), 5);
	EXPECT_EQ(DotsTestStruct::subStruct_t::Tag(), 6);
	EXPECT_EQ(DotsTestStruct::uuid_t::Tag(), 7);
}

TEST(TestTStruct, PropertiesHaveExpectedNames)
{
	DotsTestStruct dts;

	EXPECT_EQ(DotsTestStruct::stringField_t::Name(), "stringField");
	EXPECT_EQ(DotsTestStruct::indKeyfField_t::Name(), "indKeyfField");
	EXPECT_EQ(DotsTestStruct::floatField_t::Name(), "floatField");
	EXPECT_EQ(DotsTestStruct::enumField_t::Name(), "enumField");
	EXPECT_EQ(DotsTestStruct::tp_t::Name(), "tp");
	EXPECT_EQ(DotsTestStruct::subStruct_t::Name(), "subStruct");
	EXPECT_EQ(DotsTestStruct::uuid_t::Name(), "uuid");
}

TEST(TestTStruct, PropertiesHaveExpectedSet)
{
	DotsTestStruct dts;

	EXPECT_EQ(DotsTestStruct::stringField_t::Set(), dots::property_set{ 0x1 << 1 });
	EXPECT_EQ(DotsTestStruct::indKeyfField_t::Set(), dots::property_set{ 0x1 << 2 });
	EXPECT_EQ(DotsTestStruct::floatField_t::Set(), dots::property_set{ 0x1 << 3 });
	EXPECT_EQ(DotsTestStruct::enumField_t::Set(), dots::property_set{ 0x1 << 4 });
	EXPECT_EQ(DotsTestStruct::tp_t::Set(), dots::property_set{ 0x1 << 5 });
	EXPECT_EQ(DotsTestStruct::subStruct_t::Set(), dots::property_set{ 0x1 << 6 });
	EXPECT_EQ(DotsTestStruct::uuid_t::Set(), dots::property_set{ 0x1 << 7 });
}

TEST(TestTStruct, FlagsHaveExpectedValues)
{
	EXPECT_TRUE(DotsTestStruct::_IsCached());
	EXPECT_FALSE(DotsTestStruct::_IsInternal());
	EXPECT_FALSE(DotsTestStruct::_IsPersistent());
	EXPECT_FALSE(DotsTestStruct::_IsCleanup());
	EXPECT_FALSE(DotsTestStruct::_IsLocal());
	EXPECT_FALSE(DotsTestStruct::_IsSubstructOnly());
}

TEST(TestTStruct, KeyPropertiesHasExpectedValue)
{
	EXPECT_EQ(DotsTestStruct::_KeyProperties(), DotsTestStruct::indKeyfField_t::Set());
}

TEST(TestTStruct, ctor_EqualPropertiesAfterInitializerConstruction)
{
	DotsTestStruct dts1{
		DotsTestStruct::indKeyfField_t_i{ 1 },
		DotsTestStruct::stringField_t_i{ "foo" },
		DotsTestStruct::floatField_t_i{ 3.1415f }
	};

	EXPECT_EQ(dts1.indKeyfField, 1);
	EXPECT_EQ(dts1.stringField, "foo");
	EXPECT_EQ(dts1.floatField, 3.1415f);
}

TEST(TestTStruct, ctor_EqualPropertiesAfterCopyConstruction)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2{ dts1 };

	EXPECT_EQ(dts1.indKeyfField, dts2.indKeyfField);
	EXPECT_EQ(dts1.stringField, dts2.stringField);
	EXPECT_EQ(dts1.floatField, dts2.floatField);
}

TEST(TestTStruct, ctor_ExpecterPropertiesAfterMoveConstruction)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2{ std::move(dts1) };

	EXPECT_FALSE(dts1.indKeyfField.isValid());
	EXPECT_FALSE(dts1.stringField.isValid());
	EXPECT_FALSE(dts1.floatField.isValid());

	EXPECT_EQ(dts2.indKeyfField, 1);
	EXPECT_EQ(dts2.stringField, "foo");
	EXPECT_EQ(dts2.floatField, 3.1415f);
}

TEST(TestTStruct, assignment_EqualPropertiesAfterCopyAssignment)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2 = dts1;

	EXPECT_EQ(dts1.indKeyfField, dts2.indKeyfField);
	EXPECT_EQ(dts1.stringField, dts2.stringField);
	EXPECT_EQ(dts1.floatField, dts2.floatField);
}

TEST(TestTStruct, assignment_ExpectedPropertiesAfterMoveAssignment)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2 = std::move(dts1);

	EXPECT_FALSE(dts1.indKeyfField.isValid());
	EXPECT_FALSE(dts1.stringField.isValid());
	EXPECT_FALSE(dts1.floatField.isValid());

	EXPECT_EQ(dts2.indKeyfField, 1);
	EXPECT_EQ(dts2.stringField, "foo");
	EXPECT_EQ(dts2.floatField, 3.1415f);
}

TEST(TestTStruct, less_CompareLessToOtherStruct)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2;
	dts2.indKeyfField(2);
	dts2.stringField("bar");
	dts2.floatField(2.7183f);

	EXPECT_LT(dts1, dts2);
}

TEST(TestTStruct, equal_CompareEqualToOtherStruct)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2;
	dts2.indKeyfField(1);
	dts2.stringField("foo");
	dts2.floatField(3.1415f);

	EXPECT_TRUE(DotsTestStruct::_Descriptor().equal(&dts1, &dts2));
}

TEST(TestTStruct, assign_ExpectedPropertiesAfterCompleteAssign)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	DotsTestStruct other;
	other.indKeyfField(2);
	other.stringField("bar");
	other.floatField(2.7183f);

	sut._assign(other);

	EXPECT_EQ(sut.indKeyfField, 2);
	EXPECT_EQ(sut.stringField, "bar");
	EXPECT_EQ(sut.floatField, 2.7183f);
}

TEST(TestTStruct, assign_ExpectedPropertiesAfterPartialAssign)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	DotsTestStruct other;
	other.indKeyfField(2);
	other.stringField("bar");

	sut._assign(other, ~DotsTestStruct::floatField_t::Set());

	EXPECT_EQ(sut.indKeyfField, 2);
	EXPECT_EQ(sut.stringField, "bar");
	EXPECT_FALSE(sut.floatField.isValid());
}

TEST(TestTStruct, copy_ExpectedPropertiesAfterCompleteCopy)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	DotsTestStruct other;
	other.indKeyfField(2);
	other.stringField("bar");
	other.floatField(2.7183f);

	sut._copy(other);

	EXPECT_EQ(sut.indKeyfField, 2);
	EXPECT_EQ(sut.stringField, "bar");
	EXPECT_EQ(sut.floatField, 2.7183f);
}

TEST(TestTStruct, copy_ExpectedPropertiesAfterPartialCopy)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	DotsTestStruct other;
	other.indKeyfField(2);
	other.stringField("bar");

	sut._copy(other, ~DotsTestStruct::floatField_t::Set());

	EXPECT_EQ(sut.indKeyfField, 2);
	EXPECT_EQ(sut.stringField, "bar");
	EXPECT_EQ(sut.floatField, 3.1415f);
}

TEST(TestTStruct, merge_ExpectedPropertiesAfterCompleteMerge)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	DotsTestStruct other;
	other.indKeyfField(2);
	other.stringField("bar");

	sut._merge(other);

	EXPECT_EQ(sut.indKeyfField, 2);
	EXPECT_EQ(sut.stringField, "bar");
	EXPECT_EQ(sut.floatField, 3.1415f);
}

TEST(TestTStruct, merge_ExpectedPropertiesAfterPartialMerge)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	DotsTestStruct other;
	other.indKeyfField(2);
	other.stringField("bar");
	other.floatField(2.7183f);

	sut._merge(other, ~DotsTestStruct::stringField_t::Set());

	EXPECT_EQ(sut.indKeyfField, 2);
	EXPECT_EQ(sut.stringField, "foo");
	EXPECT_EQ(sut.floatField, 2.7183f);
}

TEST(TestTStruct, swap_ExpectedPropertiesAfterCompleteSwap)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2;
	dts2.indKeyfField(2);
	dts2.stringField("bar");

	dts1._swap(dts2);

	EXPECT_EQ(dts1.indKeyfField, 2);
	EXPECT_EQ(dts1.stringField, "bar");
	EXPECT_FALSE(dts1.floatField.isValid());

	EXPECT_EQ(dts2.indKeyfField, 1);
	EXPECT_EQ(dts2.stringField, "foo");
	EXPECT_TRUE(dts2.floatField.isValid());
	EXPECT_EQ(dts2.floatField, 3.1415f);
}

TEST(TestTStruct, swap_ExpectedPropertiesAfterPartialSwap)
{
	DotsTestStruct dts1;
	dts1.indKeyfField(1);
	dts1.stringField("foo");
	dts1.floatField(3.1415f);

	DotsTestStruct dts2;
	dts2.indKeyfField(2);
	dts2.stringField("bar");
	dts2.floatField(2.7183f);

	dts1._swap(dts2, DotsTestStruct::floatField_t::Set());

	EXPECT_EQ(dts1.indKeyfField, 1);
	EXPECT_EQ(dts1.stringField, "foo");
	EXPECT_EQ(dts1.floatField, 2.7183f);

	EXPECT_EQ(dts2.indKeyfField, 2);
	EXPECT_EQ(dts2.stringField, "bar");
	EXPECT_EQ(dts2.floatField, 3.1415f);
}

TEST(TestTStruct, clear_AllPropertiesInvalidAfterCompleteClear)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	sut._clear();

	EXPECT_FALSE(sut.indKeyfField.isValid());
	EXPECT_FALSE(sut.stringField.isValid());
	EXPECT_FALSE(sut.floatField.isValid());
}

TEST(TestTStruct, clear_OnlyClearedPropertiesInvalidAfterPartialClear)
{
	DotsTestStruct sut;
	sut.indKeyfField(1);
	sut.stringField("foo");
	sut.floatField(3.1415f);

	sut._clear(~DotsTestStruct::stringField_t::Set());

	EXPECT_FALSE(sut.indKeyfField.isValid());
	EXPECT_TRUE(sut.stringField.isValid());
	EXPECT_EQ(sut.stringField, "foo");
	EXPECT_FALSE(sut.floatField.isValid());
}