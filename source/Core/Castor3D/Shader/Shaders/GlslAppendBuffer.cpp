#include "Castor3D/Shader/Shaders/GlslAppendBuffer.hpp"

#include <ShaderWriter/Intrinsics/IntrinsicFunctions.hpp>

namespace c3d::shader
{
	AppendBufferBase::AppendBufferBase( sdw::ShaderWriter & writer
		, MbString blockName
		, MbString variableName
		, uint32_t binding
		, uint32_t set
		, bool enabled )
		: m_writer{ writer }
		, m_variableName{ variableName }
	{
		if ( enabled )
		{
			m_ssbo = makeRawUnique< sdw::StorageBuffer >( m_writer
				, c3d::move( blockName )
				, c3d::move( variableName )
				, binding
				, set );
			m_count = makeRawUnique< sdw::UInt32 >( m_ssbo->declMember< sdw::UInt32 >( "count" ) );
			m_ssbo->declMember< sdw::UInt32 >( "pad0" );
			m_ssbo->declMember< sdw::UInt32 >( "pad1" );
			m_ssbo->declMember< sdw::UInt32 >( "pad2" );
		}
		else
		{
			m_count = makeRawUnique< sdw::UInt32 >( m_writer.declLocale( "disabled_" + m_variableName + "_cnt", 0_u32, false ) );
		}
	}

	sdw::UInt AppendBufferBase::incrementCount()const
	{
		return ( m_ssbo
			? sdw::atomicAdd( *m_count, 1_u32 )
			: *m_count );
	}
}
