#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"

#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace castor3d::shader
{
	AppendBufferBase::AppendBufferBase( sdw::ShaderWriter & writer
		, std::string blockName
		, std::string variableName
		, uint32_t binding
		, uint32_t set
		, bool enabled )
		: m_writer{ writer }
		, m_variableName{ variableName }
	{
		if ( enabled )
		{
			m_ssbo = std::make_unique< sdw::StorageBuffer >( m_writer
				, std::move( blockName )
				, std::move( variableName )
				, binding
				, set );
			m_count = std::make_unique< sdw::UInt32 >( m_ssbo->declMember< sdw::UInt32 >( "count" ) );
			m_ssbo->declMember< sdw::UInt32 >( "pad0" );
			m_ssbo->declMember< sdw::UInt32 >( "pad1" );
			m_ssbo->declMember< sdw::UInt32 >( "pad2" );
		}
		else
		{
			m_count = std::make_unique< sdw::UInt32 >( m_writer.declLocale( "disabled_" + m_variableName + "_cnt", 0_u32, false ) );
		}
	}

	sdw::UInt AppendBufferBase::incrementCount()const
	{
		return ( m_ssbo
			? sdw::atomicAdd( *m_count, 1_u32 )
			: *m_count );
	}
}
