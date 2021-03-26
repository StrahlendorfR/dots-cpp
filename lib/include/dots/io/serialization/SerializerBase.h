#pragma once
#include <dots/type/TypeVisitor.h>

namespace dots::io
{
    template <typename Data, typename Derived, bool Static = true>
    struct SerializerBase : type::TypeVisitor<std::conditional_t<Static, Derived, void>>
    {
        using data_t = Data;
        using value_t = typename data_t::value_type;

        SerializerBase() = default;
        SerializerBase(const SerializerBase& other) = default;
        SerializerBase(SerializerBase&& other) = default;
        ~SerializerBase() = default;

        SerializerBase& operator = (const SerializerBase& rhs) = default;
        SerializerBase& operator = (SerializerBase&& rhs) = default;

        const data_t& output() const
        {
            return m_output;
        }

        data_t& output()
        {
            return m_output;
        }

        void setInput(const value_t* inputData, size_t inputDataSize)
        {
            m_inputData = inputData;
            m_inputDataEnd = m_inputData + inputDataSize;
        }

        void setInput(const data_t& input)
        {
            setInput(input.data(), input.size());
        }

        void setInput(data_t&& input) = delete;

        size_t inputAvailable() const
        {
            return m_inputDataEnd - m_inputData;
        }

        template <typename T, std::enable_if_t<std::is_base_of_v<type::Struct, T>, int> = 0>
        size_t serialize(const T& instance, const property_set_t& includedProperties)
        {
            initSerialize();
            visit(instance, includedProperties);

            return m_output.size() - m_outputSizeBegin;
        }

        template <typename... Ts, std::enable_if_t<std::disjunction_v<std::conjunction<std::bool_constant<sizeof...(Ts) == 1>, type::is_property_t<Ts>...>, std::conjunction<std::negation<type::is_property_t<Ts>>...>>, int> = 0>
        size_t serialize(const Ts&... values)
        {
            initSerialize();
            visit(values...);

            return m_output.size() - m_outputSizeBegin;
        }

        template <typename... Ts, std::enable_if_t<std::disjunction_v<std::conjunction<std::bool_constant<sizeof...(Ts) == 1>, type::is_property_t<Ts>...>, std::conjunction<std::negation<type::is_property_t<Ts>>...>>, int> = 0>
        size_t deserialize(Ts&... values)
        {
            initDeserialize();
            visit(values...);

            return static_cast<size_t>(m_inputData - m_inputDataBegin);
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T> && !std::is_reference_v<T>, int> = 0>
        T deserialize()
        {
            T value;
            deserialize(value);

            return value;
        }

        template <typename... Ts, std::enable_if_t<std::conjunction_v<std::bool_constant<sizeof...(Ts) >= 2>, std::negation<type::is_property_t<Ts>>...>, int> = 0>
        std::tuple<Ts...> deserialize()
        {
            std::tuple<Ts...> values;
            std::apply([this](auto&... values)
            {
                deserialize(values...);
            }, values);
            

            return values;
        }

        template <typename T, std::enable_if_t<std::is_base_of_v<type::Struct, T>, int> = 0>
        static data_t Serialize(const T& instance, const property_set_t& includedProperties = property_set_t::All)
        {
            Derived serializer;
            serializer.serialize(instance, includedProperties);

            return std::move(serializer.output());
        }

        template <typename T, std::enable_if_t<!std::is_base_of_v<type::Struct, T>, int> = 0>
        static data_t Serialize(const T& value)
        {
            Derived serializer;
            serializer.serialize(value);

            return std::move(serializer.output());
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
        static size_t Deserialize(const value_t* data, size_t size, T& value)
        {
            Derived serializer;
            serializer.setInput(data, size);

            return serializer.deserialize(value);
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T>, int> = 0>
        static size_t Deserialize(const data_t& data, T& value)
        {
            return Deserialize(data.data(), data.size(), value);
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T> && !std::is_reference_v<T>, int> = 0>
        static T Deserialize(const value_t* data, size_t size)
        {
            Derived serializer;
            serializer.setInput(data, size);

            return serializer.template deserialize<T>();
        }

        template <typename T, std::enable_if_t<!std::is_const_v<T> && !std::is_reference_v<T>, int> = 0>
        static T Deserialize(const data_t& data)
        {
            return Deserialize<T>(data.data(), data.size());
        }

    protected:

        using visitor_base_t = type::TypeVisitor<std::conditional_t<Static, Derived, void>>;
        using visitor_base_t::visit;

        const value_t*& inputData()
        {
            return m_inputData;
        }

        const value_t*& inputDataEnd()
        {
            return m_inputDataEnd;
        }

        void initSerializeDerived()
        {
            /* do nothing */
        }

        void initDeserializeDerived()
        {
            /* do nothing */
        }

    private:

        void initSerialize()
        {
            m_outputSizeBegin = m_output.size();
            derived().initSerializeDerived();
        }

        void initDeserialize()
        {
            if (m_inputData >= m_inputDataEnd)
            {
                throw std::logic_error{ "attempt to deserialize from invalid or empty input buffer" };
            }

            m_inputDataBegin = m_inputData;
            derived().initDeserializeDerived();
        }

        Derived& derived()
        {
            return static_cast<Derived&>(*this);
        }

        data_t m_output;
        size_t m_outputSizeBegin = 0;
        const value_t* m_inputData = nullptr;
        const value_t* m_inputDataBegin = nullptr;
        const value_t* m_inputDataEnd = nullptr;
    };
}