#pragma once
#include <string_view>
#include <variant>
#include <dots/type/Struct.h>

namespace dots::io
{
    template<typename T>
    struct Event;
}

namespace dots::type
{
    template <typename T>
    struct DynamicPropertyInitializer
    {
        DynamicPropertyInitializer(const std::string_view& name, const T& value) : name(name), value(value) {}
        DynamicPropertyInitializer(const std::string_view& name, T&& value) : name(name), value(std::move(value)) {}
        DynamicPropertyInitializer(const DynamicPropertyInitializer& other) = default;
        DynamicPropertyInitializer(DynamicPropertyInitializer&& other) = default;
        ~DynamicPropertyInitializer() = default;

        DynamicPropertyInitializer& operator = (const DynamicPropertyInitializer& rhs) = default;
        DynamicPropertyInitializer& operator = (DynamicPropertyInitializer&& rhs) = default;

        std::string_view name;
        T value;
    };

    template <typename T>
    struct is_dynamic_property_initializer : std::false_type {};

    template <typename T>
    struct is_dynamic_property_initializer<DynamicPropertyInitializer<T>> : std::true_type {};

    template <typename T>
    using is_dynamic_property_initializer_t = typename is_dynamic_property_initializer<T>::type;

    template <typename T>
    constexpr bool is_dynamic_property_initializer_v = is_dynamic_property_initializer_t<T>::value;

    struct DynamicStruct;

    template <>
    struct Descriptor<DynamicStruct>;

    struct DynamicStruct : Struct
    {
        using Cbd = io::Event<DynamicStruct>;
        template <typename T>
        using property_i = DynamicPropertyInitializer<T>;

        static constexpr bool _UseStaticDescriptorOperations = false;

        DynamicStruct(const Descriptor<DynamicStruct>& descriptor);
        DynamicStruct(const Descriptor<DynamicStruct>& descriptor, PropertyArea* propertyArea);

        template <typename... DynamicPropertyInitializers, std::enable_if_t<sizeof...(DynamicPropertyInitializers) >= 1 && std::conjunction_v<type::is_dynamic_property_initializer_t<std::remove_pointer_t<std::decay_t<DynamicPropertyInitializers>>>...>, int> = 0>
        DynamicStruct(const Descriptor<DynamicStruct>& descriptor, DynamicPropertyInitializers&&... dynamicPropertyInitializers) :
            DynamicStruct(descriptor)
        {
            (this->operator[](dynamicPropertyInitializers.name)->template construct<false>(Typeless::From(std::forward<decltype(dynamicPropertyInitializers)>(dynamicPropertyInitializers).value)), ...);
        }

        template <typename... DynamicPropertyInitializers, std::enable_if_t<sizeof...(DynamicPropertyInitializers) >= 1 && std::conjunction_v<type::is_dynamic_property_initializer_t<std::remove_pointer_t<std::decay_t<DynamicPropertyInitializers>>>...>, int> = 0>
        DynamicStruct(const Descriptor<DynamicStruct>& descriptor, PropertyArea* propertyArea, DynamicPropertyInitializers&&... dynamicPropertyInitializers) :
            DynamicStruct(descriptor, propertyArea)
        {
            (this->operator[](dynamicPropertyInitializers.name)->template construct<false>(Typeless::From(std::forward<decltype(dynamicPropertyInitializers)>(dynamicPropertyInitializers).value)), ...);
        }

        DynamicStruct(const DynamicStruct& other);
        DynamicStruct(DynamicStruct&& other);
        ~DynamicStruct();

        DynamicStruct& operator = (const DynamicStruct& rhs);
        DynamicStruct& operator = (DynamicStruct&& rhs);

        bool operator == (const DynamicStruct& rhs) const;
        bool operator != (const DynamicStruct& rhs) const;
        bool operator < (const DynamicStruct& rhs) const;
        bool operator <= (const DynamicStruct& rhs) const;
        bool operator > (const DynamicStruct& rhs) const;
        bool operator >= (const DynamicStruct& rhs) const;

        using Struct::_assign;
        using Struct::_copy;
        using Struct::_merge;
        using Struct::_swap;
        using Struct::_clear;

        using Struct::_equal;
        using Struct::_same;

        using Struct::_less;
        using Struct::_lessEqual;
        using Struct::_greater;
        using Struct::_greaterEqual;

        using Struct::_diffProperties;

        DynamicStruct& _assign(const DynamicStruct& other, const PropertySet& includedProperties = PropertySet::All);
        DynamicStruct& _assign(DynamicStruct&& other, const PropertySet& includedProperties = PropertySet::All);
        DynamicStruct& _copy(const DynamicStruct& other, const PropertySet& includedProperties = PropertySet::All);
        DynamicStruct& _merge(const DynamicStruct& other, const PropertySet& includedProperties = PropertySet::All);
        void _swap(DynamicStruct& other, const PropertySet& includedProperties = PropertySet::All);
        void _clear(const PropertySet& includedProperties = PropertySet::All);

        bool _equal(const DynamicStruct& rhs, const PropertySet& includedProperties = PropertySet::All) const;
        bool _same(const DynamicStruct& rhs) const;

        bool _less(const DynamicStruct& rhs, const PropertySet& includedProperties = PropertySet::All) const;
        bool _lessEqual(const DynamicStruct& rhs, const PropertySet& includedProperties = PropertySet::All) const;
        bool _greater(const DynamicStruct& rhs, const PropertySet& includedProperties = PropertySet::All) const;
        bool _greaterEqual(const DynamicStruct& rhs, const PropertySet& includedProperties = PropertySet::All) const;

        PropertySet _diffProperties(const DynamicStruct& other, const PropertySet& includedProperties = PropertySet::All) const;

        const PropertyArea& _propertyArea() const;
        PropertyArea& _propertyArea();

    private:

        template <typename T>
        using strip_t = std::remove_pointer_t<std::decay_t<T>>;

        using pointer_t = std::variant<PropertyArea*, std::unique_ptr<PropertyArea>>;

        using Struct::_propertyArea;

        const PropertyArea* propertyAreaGet() const;
        PropertyArea* propertyAreaGet();

        pointer_t m_propertyArea;
    };

    template <>
    struct Descriptor<DynamicStruct> : StructDescriptor<DynamicStruct>
    {
        using dynamic_descriptor_tag_t = void;

        Descriptor(std::string name, uint8_t flags, const property_descriptor_container_t& propertyDescriptors, size_t size) :
            StructDescriptor<DynamicStruct>(std::move(name), flags, propertyDescriptors, sizeof(DynamicStruct), size, alignof(DynamicStruct))
        {
            /* do nothing */
        }
        Descriptor(const Descriptor& other) = default;
        Descriptor(Descriptor&& other) = default;
        ~Descriptor() = default;

        Descriptor& operator = (const Descriptor& rhs) = default;
        Descriptor& operator = (Descriptor&& rhs) = default;

        using StructDescriptor<DynamicStruct>::construct;

        Typeless& construct(Typeless& value) const override
        {
            return Typeless::From(construct(value.to<DynamicStruct>(), *this, reinterpret_cast<PropertyArea*>(&value.to<std::byte>() + sizeof(DynamicStruct))));
        }

        Typeless& construct(Typeless& value, const Typeless& other) const override
        {
            Typeless& instance = construct(value);
            return assign(instance, other);
        }

        Typeless& construct(Typeless& value, Typeless&& other) const override
        {
            Typeless& instance = construct(value);
            return assign(instance, std::move(other));
        }
    };
}