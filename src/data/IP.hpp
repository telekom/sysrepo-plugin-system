#pragma once

namespace sys::ip
{
class IPAddress
{
  public:
	enum class Type {
		V4,
		V6,
	};

	IPAddress(Type type)
		: m_type(type)
	{
	}

	Type getType() const
	{
		return m_type;
	}

  protected:
	Type m_type;
};

class IPv4Address : public IPAddress
{
  public:
	IPv4Address()
		: IPAddress(IPAddress::Type::V4) {}
};
} // namespace sys::ip