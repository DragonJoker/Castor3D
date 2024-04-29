#include "CastorUtils/Miscellaneous/CpuInformations.hpp"

#include "CastorUtils/Exception/Assertion.hpp"
#include "CastorUtils/Miscellaneous/StringUtils.hpp"

namespace castor
{
	namespace platform
	{
		uint32_t getCoreCount();
		String getCPUModel();
		String getCPUVendor();
	}

	CpuInformations::CpuInformationsInternal::CpuInformationsInternal()
	{
		m_coreCount = platform::getCoreCount();
		m_model = platform::getCPUModel();
		m_vendor = platform::getCPUVendor();

		if ( m_vendor == cuT( "GenuineIntel" ) )
		{
			m_isIntel = true;
		}
		else if ( m_vendor == cuT( "AuthenticAMD" ) )
		{
			m_isAMD = true;
		}
	}

	CpuInformations::CpuInformationsInternal const CpuInformations::m_internal;

	OutputStream & operator<<( OutputStream & stream, CpuInformations const & object )
	{
		stream << "CPU informations:" << std::endl;
		stream << "    Vendor: " << object.getVendor() << std::endl;
		stream << "    Model: " << object.getModel() << std::endl;
		stream << "    Core count: " << object.getCoreCount();
		return stream;
	}
}
