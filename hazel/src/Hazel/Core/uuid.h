#pragma once

#include <Hazel/Core/base.h>
#include <xhash>

namespace Hazel
{

class HAZEL_API UUID
{
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID& uuid) = default;

    operator uint64_t() const
    {
        return m_uuid;
    }

private:
    uint64_t m_uuid;
};

}  // namespace Hazel

namespace std
{

template <>
struct hash<Hazel::UUID>
{
    size_t operator()(const Hazel::UUID& uuid) const
    {
        return std::hash<uint64_t>()((uint64_t)uuid);
    }
};

}  // namespace std