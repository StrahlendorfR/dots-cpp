#include <optional>
#include <dots/type/DynamicStruct.h>
#include <dots/io/Registry.h>
#include <dots/io/DescriptorConverter.h>
#include <dots/type/FundamentalTypes.h>
#include <dots/type/PropertyInitializer.h>
#include <StructDescriptorData.dots.h>
#include <gtest/gtest.h>

using namespace dots::type;
using namespace dots::types;

struct TestDynamicStruct : ::testing::Test
{
protected:

    TestDynamicStruct() :
        m_descriptorConverter{ m_registry }
    {
        StructDescriptorData testDynamicSubStructData{
            StructDescriptorData::name_i{ "TestDynamicSubStruct" },
            StructDescriptorData::flags_i{
                DotsStructFlags::cached_i{ true }
            },
            StructDescriptorData::properties_i{ vector_t<StructPropertyData>{
                StructPropertyData{
                    StructPropertyData::name_i{ "p1" },
                    StructPropertyData::tag_i{ 1 },
                    StructPropertyData::isKey_i{ true },
                    StructPropertyData::type_i{ "bool" }
                },
                StructPropertyData{
                    StructPropertyData::name_i{ "p2" },
                    StructPropertyData::tag_i{ 2 },
                    StructPropertyData::isKey_i{ false },
                    StructPropertyData::type_i{ "bool" }
                }
            } }
        };
        m_testDynamicSubStructDescriptor = std::static_pointer_cast<Descriptor<DynamicStruct>>(m_descriptorConverter(testDynamicSubStructData));

        StructDescriptorData testDynamicStructData{
            StructDescriptorData::name_i{ "TestDynamicStruct" },
            StructDescriptorData::flags_i{
                DotsStructFlags::cached_i{ true }
            },
            StructDescriptorData::properties_i{ vector_t<StructPropertyData>{
                StructPropertyData{
                    StructPropertyData::name_i{ "intProperty" },
                    StructPropertyData::tag_i{ 1 },
                    StructPropertyData::isKey_i{ true },
                    StructPropertyData::type_i{ "int32" }
                },
                StructPropertyData{
                    StructPropertyData::name_i{ "stringProperty" },
                    StructPropertyData::tag_i{ 2 },
                    StructPropertyData::isKey_i{ false },
                    StructPropertyData::type_i{ "string" }
                },
                StructPropertyData{
                    StructPropertyData::name_i{ "boolProperty" },
                    StructPropertyData::tag_i{ 3 },
                    StructPropertyData::isKey_i{ false },
                    StructPropertyData::type_i{ "bool" }
                },
                StructPropertyData{
                    StructPropertyData::name_i{ "floatVectorProperty" },
                    StructPropertyData::tag_i{ 4 },
                    StructPropertyData::isKey_i{ false },
                    StructPropertyData::type_i{ "vector<float32>" }
                },
                StructPropertyData{
                    StructPropertyData::name_i{ "subStructProperty" },
                    StructPropertyData::tag_i{ 5 },
                    StructPropertyData::isKey_i{ false },
                    StructPropertyData::type_i{ "TestDynamicSubStruct" }
                }
            } },
        };
        m_testDynamicStructDescriptor = std::static_pointer_cast<Descriptor<DynamicStruct>>(m_descriptorConverter(testDynamicStructData));
    }

    dots::io::Registry m_registry;
    dots::io::DescriptorConverter m_descriptorConverter;
    std::shared_ptr<Descriptor<DynamicStruct>> m_testDynamicSubStructDescriptor;
    std::shared_ptr<Descriptor<DynamicStruct>> m_testDynamicStructDescriptor;
};

TEST_F(TestDynamicStruct, PropertyOffsetsMatchExpectedOffsets)
{
	DynamicStruct sut{ *m_testDynamicStructDescriptor };
	
	EXPECT_EQ(sut["intProperty"]->descriptor().offset(), sizeof(PropertyArea));
    EXPECT_EQ(sut["stringProperty"]->descriptor().offset(), sut["intProperty"]->descriptor().offset() + 4);
    EXPECT_EQ(sut["boolProperty"]->descriptor().offset(), sut["stringProperty"]->descriptor().offset() + sizeof(string_t));
    EXPECT_EQ(sut["floatVectorProperty"]->descriptor().offset(), sut["boolProperty"]->descriptor().offset() + 8);
    EXPECT_EQ(sut["subStructProperty"]->descriptor().offset(), sut["floatVectorProperty"]->descriptor().offset() + sizeof(vector_t<float32_t>));
}

TEST_F(TestDynamicStruct, PropertiesHaveExpectedTags)
{
    DynamicStruct sut{ *m_testDynamicStructDescriptor };
	
	EXPECT_EQ(sut["intProperty"]->descriptor().tag(), 1);
    EXPECT_EQ(sut["stringProperty"]->descriptor().tag(), 2);
    EXPECT_EQ(sut["boolProperty"]->descriptor().tag(), 3);
    EXPECT_EQ(sut["floatVectorProperty"]->descriptor().tag(), 4);
    EXPECT_EQ(sut["subStructProperty"]->descriptor().tag(), 5);
}

TEST_F(TestDynamicStruct, PropertiesHaveExpectedNames)
{
	DynamicStruct sut{ *m_testDynamicStructDescriptor };
	
	EXPECT_EQ(sut["intProperty"]->descriptor().name(), "intProperty");
    EXPECT_EQ(sut["stringProperty"]->descriptor().name(), "stringProperty");
    EXPECT_EQ(sut["boolProperty"]->descriptor().name(), "boolProperty");
    EXPECT_EQ(sut["floatVectorProperty"]->descriptor().name(), "floatVectorProperty");
    EXPECT_EQ(sut["subStructProperty"]->descriptor().name(), "subStructProperty");
}

TEST_F(TestDynamicStruct, PropertiesHaveExpectedSet)
{
    DynamicStruct sut{ *m_testDynamicStructDescriptor };
	
	EXPECT_EQ(sut["intProperty"]->descriptor().set(), PropertySet{ 0x1 << 1 });
    EXPECT_EQ(sut["stringProperty"]->descriptor().set(), PropertySet{ 0x1 << 2 });
    EXPECT_EQ(sut["boolProperty"]->descriptor().set(), PropertySet{ 0x1 << 3 });
    EXPECT_EQ(sut["floatVectorProperty"]->descriptor().set(), PropertySet{ 0x1 << 4 });
    EXPECT_EQ(sut["subStructProperty"]->descriptor().set(), PropertySet{ 0x1 << 5 });
}

TEST_F(TestDynamicStruct, _descriptor_SizeMatchesActualSize)
{
	EXPECT_EQ(DynamicStruct{ *m_testDynamicStructDescriptor }._descriptor().size(), sizeof(DynamicStruct));
}

TEST_F(TestDynamicStruct, _descriptor_AlignmentMatchesActualAlignment)
{
	EXPECT_EQ(DynamicStruct{ *m_testDynamicStructDescriptor }._descriptor().alignment(), alignof(DynamicStruct));
}

TEST_F(TestDynamicStruct, _descriptor_AllocateSizeMatchesActualSize)
{
    DynamicStruct sut{ *m_testDynamicStructDescriptor };
    const PropertyDescriptor& lastPropertyDescriptor = sut["subStructProperty"]->descriptor();

    // note: this expectation only holds true if the last property has an 8-byte alignment
	EXPECT_EQ(m_testDynamicStructDescriptor->allocateSize(), lastPropertyDescriptor.offset() + lastPropertyDescriptor.valueDescriptor().size());
}

TEST_F(TestDynamicStruct, _descriptor_AllocateAlignmentMatchesActualAlignment)
{
	EXPECT_EQ(m_testDynamicStructDescriptor->allocateAlignment(), alignof(Struct));
}

TEST_F(TestDynamicStruct, _descriptor_FlagsHaveExpectedValues)
{
    DynamicStruct sut{ *m_testDynamicStructDescriptor };
    const StructDescriptor<>& descriptor = sut._descriptor();

	EXPECT_TRUE(descriptor.cached());
	EXPECT_FALSE(descriptor.internal());
	EXPECT_FALSE(descriptor.persistent());
	EXPECT_FALSE(descriptor.cleanup());
	EXPECT_FALSE(descriptor.local());
	EXPECT_FALSE(descriptor.substructOnly());
}

TEST_F(TestDynamicStruct, _keyProperties)
{
    DynamicStruct sut{ *m_testDynamicStructDescriptor };
    const StructDescriptor<>& descriptor = sut._descriptor();

	EXPECT_EQ(descriptor.keyProperties(), sut["intProperty"]->descriptor().set());
}

TEST_F(TestDynamicStruct, ctor_Initializer)
{
	DynamicStruct sut{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f, 2.7183f } }
	};
	
	EXPECT_EQ(sut["intProperty"]->value().to<int32_t>(), 1);
	EXPECT_EQ(sut["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_FALSE(sut["boolProperty"]->isValid());
	EXPECT_EQ(sut["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>({ 3.1415f, 2.7183f }));
}

TEST_F(TestDynamicStruct, ctor_Copy)
{
	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
        DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f, 2.7183f } }
	};
	DynamicStruct sutThis{ sutOther };

	EXPECT_EQ(sutThis["intProperty"], sutOther["intProperty"]);
	EXPECT_EQ(sutThis["stringProperty"], sutOther["stringProperty"]);
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"], sutOther["floatVectorProperty"]);
}

TEST_F(TestDynamicStruct, ctor_Move)
{
	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f, 2.7183f } }
	};
	DynamicStruct sutThis{ std::move(sutOther) };

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 1);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>({ 3.1415f, 2.7183f }));
}

TEST_F(TestDynamicStruct, assignment_Copy)
{
	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f, 2.7183f } }
	};
	DynamicStruct sutThis = sutOther;

	EXPECT_EQ(sutThis["intProperty"], sutOther["intProperty"]);
	EXPECT_EQ(sutThis["stringProperty"], sutOther["stringProperty"]);
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"], sutOther["floatVectorProperty"]);
}

TEST_F(TestDynamicStruct, assignment_Move)
{
	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f, 2.7183f } }
	};
	DynamicStruct sutThis = std::move(sutOther);

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 1);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>({ 3.1415f, 2.7183f }));
}

TEST_F(TestDynamicStruct, assign_CompleteAssign)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};

	sutThis._assign(sutOther);

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 2.7183f });
}

TEST_F(TestDynamicStruct, assign_PartialAssign)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" }
	};

	sutThis._assign(sutOther, ~sutThis["floatVectorProperty"]->descriptor().set());

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_FALSE(sutThis["floatVectorProperty"]->isValid());
}

TEST_F(TestDynamicStruct, copy_CompleteCopy)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};

	sutThis._copy(sutOther);

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 2.7183f });
}

TEST_F(TestDynamicStruct, copy_PartialCopy)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" }
	};

	sutThis._copy(sutOther, ~sutThis["floatVectorProperty"]->descriptor().set());

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 3.1415f });
}

TEST_F(TestDynamicStruct, merge_CompleteMerge)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" }
	};

	sutThis._merge(sutOther);

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 3.1415f });
}

TEST_F(TestDynamicStruct, merge_PartialMerge)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};

	sutThis._merge(sutOther, ~sutThis["stringProperty"]->descriptor().set());

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 2.7183f });
}

TEST_F(TestDynamicStruct, merge_PartialMergeSubStruct)
{
    DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
        DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
        DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
        DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } },
        DynamicStruct::property_i<DynamicStruct>{ "subStructProperty", DynamicStruct{ *m_testDynamicSubStructDescriptor,
			    DynamicStruct::property_i<bool_t>{ "p2", true }
            }
        }
    };

    DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
        DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
        DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
        DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } },
        DynamicStruct::property_i<DynamicStruct>{ "subStructProperty", DynamicStruct{ *m_testDynamicSubStructDescriptor,
			    DynamicStruct::property_i<bool_t>{ "p1", true }
            }
        }
    };

    sutThis._merge(sutOther, ~sutThis["stringProperty"]->descriptor().set());

    EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
    EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "foo");
    EXPECT_FALSE(sutThis["boolProperty"]->isValid());
    EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 2.7183f });

	auto sutThisSubIt = sutThis["subStructProperty"];
	const auto& sutThisSub = sutThisSubIt->value().to<DynamicStruct>();
	EXPECT_EQ(*sutThisSub["p1"], Typeless::From(true));
	EXPECT_EQ(*sutThisSub["p2"], Typeless::From(true));
}

TEST_F(TestDynamicStruct, swap_CompleteSwap)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" }
	};

	sutThis._swap(sutOther);

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_FALSE(sutThis["floatVectorProperty"]->isValid());

	EXPECT_EQ(sutOther["intProperty"]->value().to<int32_t>(), 1);
	EXPECT_EQ(sutOther["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_FALSE(sutOther["boolProperty"]->isValid());
	EXPECT_EQ(sutOther["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 3.1415f });
}

TEST_F(TestDynamicStruct, swap_PartialSwap)
{
	DynamicStruct sutThis{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutOther{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};

	sutThis._swap(sutOther, sutThis["floatVectorProperty"]->descriptor().set());

	EXPECT_EQ(sutThis["intProperty"]->value().to<int32_t>(), 1);
	EXPECT_EQ(sutThis["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_FALSE(sutThis["boolProperty"]->isValid());
	EXPECT_EQ(sutThis["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 2.7183f });

	EXPECT_EQ(sutOther["intProperty"]->value().to<int32_t>(), 2);
	EXPECT_EQ(sutOther["stringProperty"]->value().to<string_t>(), "bar");
	EXPECT_FALSE(sutOther["boolProperty"]->isValid());
	EXPECT_EQ(sutOther["floatVectorProperty"]->value().to<vector_t<float32_t>>(), vector_t<float32_t>{ 3.1415f });
}

TEST_F(TestDynamicStruct, clear_CompleteClear)
{
	DynamicStruct sut{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	sut._clear();

	EXPECT_FALSE(sut["intProperty"]->isValid());
	EXPECT_FALSE(sut["stringProperty"]->isValid());
	EXPECT_FALSE(sut["boolProperty"]->isValid());
	EXPECT_FALSE(sut["floatVectorProperty"]->isValid());
}

TEST_F(TestDynamicStruct, clear_PartialClear)
{
	DynamicStruct sut{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	sut._clear(~sut["stringProperty"]->descriptor().set());

	EXPECT_FALSE(sut["intProperty"]->isValid());
	EXPECT_EQ(sut["stringProperty"]->value().to<string_t>(), "foo");
	EXPECT_FALSE(sut["boolProperty"]->isValid());
	EXPECT_FALSE(sut["floatVectorProperty"]->isValid());
}

TEST_F(TestDynamicStruct, equal)
{
	DynamicStruct sutLhs{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutRhs{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	EXPECT_TRUE(sutLhs._equal(sutLhs));
	EXPECT_TRUE(sutRhs._equal(sutRhs));
	EXPECT_FALSE(sutLhs._equal(sutRhs));
	
	EXPECT_TRUE(sutLhs._equal(sutRhs, sutLhs["intProperty"]->descriptor().set()));
	EXPECT_TRUE(sutLhs._equal(sutRhs, sutLhs["floatVectorProperty"]->descriptor().set()));
	EXPECT_FALSE(sutLhs._equal(sutRhs, sutLhs["intProperty"]->descriptor().set() + sutLhs["stringProperty"]->descriptor().set()));
}

TEST_F(TestDynamicStruct, same)
{
	DynamicStruct sut1{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sut2{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sut3{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};

	EXPECT_TRUE(sut1._same(sut1));
	EXPECT_TRUE(sut1._same(sut2));
	EXPECT_FALSE(sut1._same(sut3));
}

TEST_F(TestDynamicStruct, less_lessEqual_greater_greaterEqual)
{
	DynamicStruct sutLhs{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutRhs{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};
	
	EXPECT_TRUE(sutLhs._less(sutRhs));
	EXPECT_FALSE(sutRhs._less(sutLhs));
	EXPECT_TRUE(sutRhs._less(sutLhs, sutLhs["floatVectorProperty"]->descriptor().set()));
	EXPECT_FALSE(sutLhs._less(sutLhs));
	EXPECT_FALSE(sutLhs._less(sutLhs, sutLhs["boolProperty"]->descriptor().set()));

	EXPECT_TRUE(sutLhs._lessEqual(sutRhs));
	EXPECT_FALSE(sutRhs._lessEqual(sutLhs));
	EXPECT_TRUE(sutRhs._lessEqual(sutLhs, sutLhs["floatVectorProperty"]->descriptor().set()));
	EXPECT_TRUE(sutLhs._lessEqual(sutLhs));
	EXPECT_TRUE(sutLhs._lessEqual(sutLhs, sutLhs["boolProperty"]->descriptor().set()));

	EXPECT_FALSE(sutLhs._greater(sutRhs));
	EXPECT_TRUE(sutRhs._greater(sutLhs));
	EXPECT_FALSE(sutRhs._greater(sutLhs, sutLhs["floatVectorProperty"]->descriptor().set()));
	EXPECT_FALSE(sutLhs._greater(sutLhs));
	EXPECT_FALSE(sutLhs._greater(sutLhs, sutLhs["boolProperty"]->descriptor().set()));

	EXPECT_FALSE(sutLhs._greaterEqual(sutRhs));
	EXPECT_TRUE(sutRhs._greaterEqual(sutLhs));
	EXPECT_FALSE(sutRhs._greaterEqual(sutLhs, sutLhs["floatVectorProperty"]->descriptor().set()));
	EXPECT_TRUE(sutLhs._greaterEqual(sutLhs));
	EXPECT_TRUE(sutLhs._greaterEqual(sutLhs, sutLhs["boolProperty"]->descriptor().set()));
}

TEST_F(TestDynamicStruct, diffProperties)
{
	DynamicStruct sutLhs{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "bar" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 3.1415f } }
	};

	DynamicStruct sutRhs{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 2 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" },
		DynamicStruct::property_i<vector_t<float32_t>>{ "floatVectorProperty", vector_t<float32_t>{ 2.7183f } }
	};
	
	EXPECT_EQ(sutLhs._diffProperties(sutRhs), sutLhs["stringProperty"]->descriptor().set() + sutLhs["floatVectorProperty"]->descriptor().set());
	EXPECT_EQ(sutLhs._diffProperties(sutRhs, (~sutLhs["floatVectorProperty"]->descriptor().set())), sutLhs["stringProperty"]->descriptor().set());
}

TEST_F(TestDynamicStruct, assertProperties)
{
	DynamicStruct sut{ *m_testDynamicStructDescriptor,
		DynamicStruct::property_i<int32_t>{ "intProperty", 1 },
		DynamicStruct::property_i<string_t>{ "stringProperty", "foo" }
	};
	
	EXPECT_NO_THROW(sut._assertHasProperties(sut["intProperty"]->descriptor().set()));
	EXPECT_THROW(sut._assertHasProperties<false>(sut["intProperty"]->descriptor().set()), std::logic_error);

	EXPECT_NO_THROW(sut._assertHasProperties(sut["intProperty"]->descriptor().set() + sut["stringProperty"]->descriptor().set()));
	EXPECT_NO_THROW(sut._assertHasProperties<false>(sut["intProperty"]->descriptor().set() + sut["stringProperty"]->descriptor().set()));

	EXPECT_THROW(sut._assertHasProperties(sut["intProperty"]->descriptor().set() + sut["floatVectorProperty"]->descriptor().set()), std::logic_error);
	EXPECT_THROW(sut._assertHasProperties<false>(sut["intProperty"]->descriptor().set() + sut["floatVectorProperty"]->descriptor().set()), std::logic_error);
}