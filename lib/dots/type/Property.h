#pragma once
#include <string_view>
#include <functional>
#include <type_traits>
#include <iostream>
#include <cstddef>
#include <dots/type/PropertyArea.h>
#include <dots/type/PropertyDescriptor.h>

namespace dots::type
{
	template <typename T, typename Derived>
	struct Property
	{
		static_assert(std::conjunction_v<std::negation<std::is_pointer<T>>, std::negation<std::is_reference<T>>>);
		using value_t = T;
		static constexpr bool IsTypeless = std::is_same_v<T, Typeless>;

		template <typename U, std::enable_if_t<!std::disjunction_v<std::is_same<std::remove_reference_t<U>, Property>, std::is_same<std::remove_reference_t<U>, Derived>>, int> = 0>
		Derived& operator = (U&& rhs)
		{
			Property<T, Derived>::constructOrAssign(std::forward<U>(rhs));
			return static_cast<Derived&>(*this);
		}

		template <typename... Args>
		T& operator () (Args&&... args)
		{
			return construct(std::forward<Args>(args)...);
		}

		T& operator * ()
		{
			return value();
		}

		const T& operator * () const
		{
			return value();
		}

		T* operator -> ()
		{
			return &value();
		}

		const T* operator -> () const
		{
			return &value();
		}

		operator T& ()
		{
			return value();
		}

		operator const T& () const
		{
			return value();
		}

		bool operator == (const T& rhs) const
		{
			return equal(rhs);
		}

		bool operator == (const Derived& rhs) const
		{
			return equal(rhs);
		}

		bool operator != (const T& rhs) const
		{
			return !(*this == rhs);
		}

		bool operator != (const Derived& rhs) const
		{
			return !(*this == rhs);
		}

		bool operator < (const T& rhs) const
		{
			return less(rhs);
		}

		bool operator < (const Derived& rhs) const
		{
			return less(rhs);
		}

		bool operator <= (const T& rhs) const
		{
			return lessEqual(rhs);
		}

		bool operator <= (const Derived& rhs) const
		{
			return lessEqual(rhs);
		}

		bool operator > (const T& rhs) const
		{
			return greater(rhs);
		}

		bool operator > (const Derived& rhs) const
		{
			return greater(rhs);
		}

		bool operator >= (const T& rhs) const
		{
			return greaterEqual(rhs);
		}

		bool operator >= (const Derived& rhs) const
		{
			return greaterEqual(rhs);
		}

		bool isValid() const
		{
			return metadata().set() <= validProperties();
		}

		template <bool AssertInvalidity = true>
		T& construct(const Derived& rhs)
		{
			construct<AssertInvalidity>(rhs.storage());
			return *this;
		}

		template <bool AssertInvalidity = true>
		T& construct(Derived&& rhs)
		{
			construct<AssertInvalidity>(std::move(rhs.storage()));
			rhs.destroy();

			return *this;
		}

		template <bool AssertInvalidity = true, typename... Args>
		T& construct(Args&&... args)
		{
			if constexpr (AssertInvalidity)
			{
				if (isValid())
				{
					throw std::runtime_error{ std::string{ "attempt to construct already valid property: " } + metadata().name().data() };
				}
			}

			if constexpr (!IsTypeless)
			{
				Descriptor<T>::construct(storage(), std::forward<Args>(args)...);
			}
			else if constexpr (sizeof...(Args) == 1)
			{
				descriptor().valueDescriptor().construct(storage(), std::forward<Args>(args)...);
			}
			else if constexpr (sizeof...(Args) == 0)
			{
				descriptor().valueDescriptor().construct(storage());
			}
			
			setValid();

			return storage();
		}

		void destroy()
		{
			if (isValid())
			{
				if constexpr (IsTypeless)
				{
					descriptor().valueDescriptor().destruct(storage());
				}
				else
				{
					Descriptor<T>::destruct(storage());
				}
				
				setInvalid();
			}
		}

		T& value()
		{
			if (!isValid())
			{
				throw std::runtime_error{ std::string{ "attempt to access invalid property: " } + metadata().name().data() };
			}

			return storage();
		}

		const T& value() const
		{
			return const_cast<Property&>(*this).value();
		}

		template <typename... Args>
		T valueOrDefault(Args&&... args) const
		{
			if (isValid())
			{
				return storage();
			}
			else
			{
				return T(std::forward<Args>(args)...);
			}
		}

		template <typename... Args>
		T& constructOrValue(Args&&... args)
		{
			if (isValid())
			{
				return storage();
			}
			else
			{
				return construct<false>(std::forward<Args>(args)...);
			}
		}

		template <bool AssertValidity = true>
		T& assign(const Derived& rhs)
		{
			assign<AssertValidity>(rhs.storage());
			return *this;
		}

		template <bool AssertValidity = true>
		T& assign(Derived&& rhs)
		{
			assign<AssertValidity>(std::move(rhs.storage()));
			rhs.destroy();

			return *this;
		}

		template <bool AssertValidity = true, typename... Args>
		T& assign(Args&&... args)
		{
			if constexpr (AssertValidity)
			{
				if (!isValid())
				{
					throw std::runtime_error{ std::string{ "attempt to assign invalid property: " } + metadata().name().data() };
				}
			}

			static_assert(!IsTypeless || sizeof...(Args) <= 1, "typeless assignment only supports a single argument");
			if constexpr (!IsTypeless)
			{
				Descriptor<T>::assign(storage(), std::forward<Args>(args)...);
			}
			else if constexpr (sizeof...(Args) == 1)
			{
				descriptor().valueDescriptor().assign(storage(), std::forward<Args>(args)...);
			}
			else if constexpr (sizeof...(Args) == 0)
			{
				descriptor().valueDescriptor().assign(storage());
			}
			
			setValid();

			return storage();
		}

		template <typename... Args>
		T& constructOrAssign(Args&&... args)
		{
			if (isValid())
			{
				return assign<false>(std::forward<Args>(args)...);
			}
			else
			{
				return construct<false>(std::forward<Args>(args)...);
			}
		}

		void swap(Derived& other)
		{
			if (isValid())
			{
				if (other.isValid())
				{
					if constexpr (IsTypeless)
					{
						return descriptor().valueDescriptor().swap(storage(), other);
					}
					else
					{
						return Descriptor<T>::swap(storage(), other);
					}
				}
				else
				{
					other.template construct<false>(std::move(storage()));
					destroy();
				}
			}
			else if (other.isValid())
			{
				construct<false>(std::move(other.storage()));
				other.destroy();
			}
		}

		bool equal(const T& rhs) const
		{
			if (isValid())
			{
				if constexpr (IsTypeless)
				{
					return descriptor().valueDescriptor().equal(storage(), rhs);
				}
				else
				{
					return Descriptor<T>::equal(storage(), rhs);
				}
			}
			else
			{
				return false;
			}
		}

		bool equal(const Derived& rhs) const
		{
			if (rhs.isValid())
			{
				return equal(rhs.storage());
			}
			else
			{
				return !isValid();
			}			
		}

		bool less(const T& rhs) const
		{
			if (isValid())
			{
				if constexpr (IsTypeless)
				{
					return descriptor().valueDescriptor().less(storage(), rhs);
				}
				else
				{
					return Descriptor<T>::less(storage(), rhs);
				}
			}
			else
			{
				return false;
			}
		}

		bool less(const Derived& rhs) const
		{
			if (rhs.isValid())
			{
				return less(rhs.storage());
			}
			else
			{
				return isValid();
			}
		}

		bool lessEqual(const T& rhs) const
		{
			return !greater(rhs);
		}

		bool lessEqual(const Derived& rhs) const
		{
			return !greater(rhs);
		}

		bool greater(const T& rhs) const
		{
			if (isValid())
			{
				if constexpr (IsTypeless)
				{
					return descriptor().valueDescriptor().less(rhs, storage());
				}
				else
				{
					return Descriptor<T>::less(rhs, storage());
				}
			}
			else
			{
				return true;
			}
		}

		bool greater(const Derived& rhs) const
		{
			return rhs.less(static_cast<const Derived&>(*this));
		}

		bool greaterEqual(const T& rhs) const
		{
			return !less(rhs);
		}

		bool greaterEqual(const Derived& rhs) const
		{
			return !less(rhs);
		}

		constexpr const PropertyMetadata<T>& metadata() const
		{
			return static_cast<const Derived&>(*this).derivedMetadata();
		}

		constexpr const PropertyDescriptor& descriptor() const
		{
			return static_cast<const Derived&>(*this).derivedDescriptor();
		}

		constexpr bool isPartOf(const PropertySet& propertySet) const
		{
			return metadata().set() <= propertySet;
		}

		constexpr T& storage()
		{
			return static_cast<Derived&>(*this).derivedStorage();
		}

		constexpr const T& storage() const
		{
			return const_cast<Property&>(*this).storage();
		}

	protected:

		constexpr Property() = default;
		constexpr Property(const Property& other) = default;
		constexpr Property(Property&& other) = default;
		~Property() = default;

		constexpr Property& operator = (const Property& rhs) = default;
		constexpr Property& operator = (Property&& rhs) = default;

	private:

		const PropertySet& validProperties() const
		{
			return PropertyArea::GetArea(storage(), metadata().offset()).validProperties();
		}

		PropertySet& validProperties()
		{
			return const_cast<PropertySet&>(std::as_const(*this).validProperties());
		}		

		void setValid()
		{
			validProperties() += metadata().set();
		}

		void setInvalid()
		{
			validProperties() -= metadata().set();
		}
	};

	template <typename T, typename Derived>
	std::ostream& operator << (std::ostream& os, const Property<T, Derived>& property)
	{
		if (property.isValid())
		{
			os << *property;
		}
		else
		{
			os << "<invalid-property>";
		}

		return os;
	}
}