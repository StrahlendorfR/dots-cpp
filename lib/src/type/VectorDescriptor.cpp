#include <dots/type/VectorDescriptor.h>

namespace dots::type
{
    Descriptor<Vector<Typeless>>::Descriptor(key_t key, std::string name, const std::shared_ptr<Descriptor<>>& valueDescriptor, size_t size, size_t alignment):
        Descriptor<Typeless>(key, Type::Vector, std::move(name), size, alignment),
        m_valueDescriptor(valueDescriptor)
    {
        /* do nothing */
    }

    const std::shared_ptr<Descriptor<>>& Descriptor<Vector<Typeless>>::valueDescriptorPtr() const
    {
        return m_valueDescriptor;
    }

    const Descriptor<Typeless>& Descriptor<Vector<Typeless>>::valueDescriptor() const
    {
        return *m_valueDescriptor;
    }
}
