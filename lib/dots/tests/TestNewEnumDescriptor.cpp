#include <dots/type/NewEnumDescriptor.h>
#include <dots/type/NewFundamentalTypes.h>
#include <gtest/gtest.h>

namespace dots::types
{
    enum class TestEnumSimple : int32_t
    {
        enumerator2 = 2,
        enumerator3 = 3, 
        enumerator5 = 5, 
        enumerator7 = 7,
        enumerator11 = 11,
    	enumerator13 = 13
    };
}

using namespace dots::type;
using namespace dots::types;

struct TestNewEnumDescriptor : ::testing::Test
{
protected:

	TestNewEnumDescriptor() :
		m_sutSimple{ "TestEnumSimple", {
			NewEnumeratorDescriptor{ 2, "enumerator2", TestEnumSimple::enumerator2 },
			NewEnumeratorDescriptor{ 3, "enumerator3", TestEnumSimple::enumerator3 },
			NewEnumeratorDescriptor{ 5, "enumerator5", TestEnumSimple::enumerator5 },
			NewEnumeratorDescriptor{ 7, "enumerator7", TestEnumSimple::enumerator7 },
			NewEnumeratorDescriptor{ 11, "enumerator11", TestEnumSimple::enumerator11 },
			NewEnumeratorDescriptor{ 13, "enumerator13", TestEnumSimple::enumerator13 }
		} },
		m_sutGeneric{ "TestEnumGeneric", {
			NewEnumeratorDescriptor<vector_t<string_t>>{ 1, "enumerator1", { "foo", "bar"} },
			NewEnumeratorDescriptor<vector_t<string_t>>{ 4, "enumerator4", { "baz", "qux" } },
			NewEnumeratorDescriptor<vector_t<string_t>>{ 6, "enumerator6", { "bla", "blubb" } },
			NewEnumeratorDescriptor<vector_t<string_t>>{ 8, "enumerator8", { "meow", "bark" } },
			NewEnumeratorDescriptor<vector_t<string_t>>{ 9, "enumerator9", { "1", "3", "5" } },
			NewEnumeratorDescriptor<vector_t<string_t>>{ 14, "enumerator14", { "a", "b" } },
			NewEnumeratorDescriptor<vector_t<string_t>>{ 16, "enumerator16", { "alpha", "beta", "delta" } }
		} } {}

	NewEnumDescriptor<TestEnumSimple> m_sutSimple;
	NewEnumDescriptor<vector_t<string_t>> m_sutGeneric;
};

TEST_F(TestNewEnumDescriptor, underlyingDescriptor)
{
	EXPECT_EQ(m_sutSimple.underlyingDescriptor().name(), "int32");
	EXPECT_EQ(m_sutGeneric.underlyingDescriptor().name(), "vector<string>");
}

TEST_F(TestNewEnumDescriptor, enumerators_expectedSize)
{
	EXPECT_EQ(m_sutSimple.enumerators().size(), 6);
	EXPECT_EQ(m_sutGeneric.enumerators().size(), 7);
}

TEST_F(TestNewEnumDescriptor, enumeratorsTypeless_expectedSize)
{
	EXPECT_EQ(m_sutSimple.enumeratorsTypeless().size(), 6);
	EXPECT_EQ(m_sutGeneric.enumeratorsTypeless().size(), 7);
}

TEST_F(TestNewEnumDescriptor, enumerators_expectedElements)
{
	auto expect_eq_enumerator = [](const auto& enumerator, uint32_t tag, const std::string_view& name, const auto& value)
	{
		EXPECT_EQ(enumerator.tag(), tag);
		EXPECT_EQ(enumerator.name(), name);
		EXPECT_EQ(enumerator.value(), value);
	};
	
	expect_eq_enumerator(m_sutSimple.enumerators()[0], 2, "enumerator2", TestEnumSimple::enumerator2);
	expect_eq_enumerator(m_sutSimple.enumerators()[1], 3, "enumerator3", TestEnumSimple::enumerator3);
	expect_eq_enumerator(m_sutSimple.enumerators()[2], 5, "enumerator5", TestEnumSimple::enumerator5);
	expect_eq_enumerator(m_sutSimple.enumerators()[3], 7, "enumerator7", TestEnumSimple::enumerator7);
	expect_eq_enumerator(m_sutSimple.enumerators()[4], 11, "enumerator11", TestEnumSimple::enumerator11);
	expect_eq_enumerator(m_sutSimple.enumerators()[5], 13, "enumerator13", TestEnumSimple::enumerator13);

	expect_eq_enumerator(m_sutGeneric.enumerators()[0], 1, "enumerator1", vector_t<string_t>{ "foo", "bar"});
	expect_eq_enumerator(m_sutGeneric.enumerators()[1], 4, "enumerator4", vector_t<string_t>{ "baz", "qux" });
	expect_eq_enumerator(m_sutGeneric.enumerators()[2], 6, "enumerator6", vector_t<string_t>{ "bla", "blubb" });
	expect_eq_enumerator(m_sutGeneric.enumerators()[3], 8, "enumerator8", vector_t<string_t>{ "meow", "bark" });
	expect_eq_enumerator(m_sutGeneric.enumerators()[4], 9, "enumerator9", vector_t<string_t>{ "1", "3", "5" });
	expect_eq_enumerator(m_sutGeneric.enumerators()[5], 14, "enumerator14", vector_t<string_t>{ "a", "b" });
	expect_eq_enumerator(m_sutGeneric.enumerators()[6], 16, "enumerator16", vector_t<string_t>{ "alpha", "beta", "delta" } );
}

TEST_F(TestNewEnumDescriptor, enumeratorsTypeless_expectedElements)
{
	auto expect_eq_enumerator_simple = [&](const NewEnumeratorDescriptor<>& enumerator, uint32_t tag, const std::string_view& name, const TestEnumSimple& value)
	{
		EXPECT_EQ(enumerator.tag(), tag);
		EXPECT_EQ(enumerator.name(), name);
		EXPECT_TRUE(enumerator.underlyingDescriptor().equal(enumerator.valueTypeless(), NewTypeless::From(value)));
	};
	
	expect_eq_enumerator_simple(m_sutSimple.enumeratorsTypeless()[0], 2, "enumerator2", TestEnumSimple::enumerator2);
	expect_eq_enumerator_simple(m_sutSimple.enumeratorsTypeless()[1], 3, "enumerator3", TestEnumSimple::enumerator3);
	expect_eq_enumerator_simple(m_sutSimple.enumeratorsTypeless()[2], 5, "enumerator5", TestEnumSimple::enumerator5);
	expect_eq_enumerator_simple(m_sutSimple.enumeratorsTypeless()[3], 7, "enumerator7", TestEnumSimple::enumerator7);
	expect_eq_enumerator_simple(m_sutSimple.enumeratorsTypeless()[4], 11, "enumerator11", TestEnumSimple::enumerator11);
	expect_eq_enumerator_simple(m_sutSimple.enumeratorsTypeless()[5], 13, "enumerator13", TestEnumSimple::enumerator13);

	auto expect_eq_enumerator_generic = [&](const NewEnumeratorDescriptor<>& enumerator, uint32_t tag, const std::string_view& name, const vector_t<string_t>& value)
	{
		EXPECT_EQ(enumerator.tag(), tag);
		EXPECT_EQ(enumerator.name(), name);
		EXPECT_TRUE(enumerator.underlyingDescriptor().equal(enumerator.valueTypeless(), NewTypeless::From(value)));
	};

	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[0], 1, "enumerator1", vector_t<string_t>{ "foo", "bar"});
	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[1], 4, "enumerator4", vector_t<string_t>{ "baz", "qux" });
	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[2], 6, "enumerator6", vector_t<string_t>{ "bla", "blubb" });
	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[3], 8, "enumerator8", vector_t<string_t>{ "meow", "bark" });
	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[4], 9, "enumerator9", vector_t<string_t>{ "1", "3", "5" });
	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[5], 14, "enumerator14", vector_t<string_t>{ "a", "b" });
	expect_eq_enumerator_generic(m_sutGeneric.enumeratorsTypeless()[6], 16, "enumerator16", vector_t<string_t>{ "alpha", "beta", "delta" });
}

TEST_F(TestNewEnumDescriptor, enumeratorFromTag)
{
	EXPECT_EQ(m_sutSimple.enumeratorFromTag(2).name(), "enumerator2");
	EXPECT_EQ(m_sutSimple.enumeratorFromTag(7).value(), TestEnumSimple::enumerator7);

	EXPECT_EQ(m_sutGeneric.enumeratorFromTag(1).name(), "enumerator1");
	EXPECT_EQ(m_sutGeneric.enumeratorFromTag(8).value(), vector_t<string_t>({ "meow", "bark" }));

	EXPECT_THROW(m_sutSimple.enumeratorFromTag(1), std::logic_error);
	EXPECT_THROW(m_sutGeneric.enumeratorFromTag(2), std::logic_error);
}

TEST_F(TestNewEnumDescriptor, enumeratorFromName)
{
	EXPECT_EQ(m_sutSimple.enumeratorFromName("enumerator3").tag(), 3);
	EXPECT_EQ(m_sutSimple.enumeratorFromName("enumerator5").value(), TestEnumSimple::enumerator5);

	EXPECT_EQ(m_sutGeneric.enumeratorFromName("enumerator4").tag(), 4);
	EXPECT_EQ(m_sutGeneric.enumeratorFromName("enumerator9").value(), vector_t<string_t>({ "1", "3", "5" }));

	EXPECT_THROW(m_sutSimple.enumeratorFromName("enumerator4"), std::logic_error);
	EXPECT_THROW(m_sutGeneric.enumeratorFromName("enumerator3"), std::logic_error);
}

TEST_F(TestNewEnumDescriptor, enumeratorFromValue)
{
	EXPECT_EQ(m_sutSimple.enumeratorFromValue(TestEnumSimple::enumerator11).tag(), 11);
	EXPECT_EQ(m_sutSimple.enumeratorFromValue(TestEnumSimple::enumerator13).name(), "enumerator13");

	EXPECT_EQ(m_sutGeneric.enumeratorFromValue(vector_t<string_t>{ "bla", "blubb" }).tag(), 6);
	EXPECT_EQ(m_sutGeneric.enumeratorFromValue(vector_t<string_t>{ "a", "b" }).name(), "enumerator14");

	EXPECT_THROW(m_sutSimple.enumeratorFromValue(static_cast<TestEnumSimple>(6)), std::logic_error);
	EXPECT_THROW(m_sutGeneric.enumeratorFromValue(vector_t<string_t>({ "1", "2", "3" })), std::logic_error);
}