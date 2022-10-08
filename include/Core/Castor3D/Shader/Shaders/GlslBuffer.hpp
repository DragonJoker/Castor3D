/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslBuffer_H___
#define ___C3D_GlslBuffer_H___

#include "SdwModule.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	class BufferBase
	{
	public:
		BufferBase( sdw::ShaderWriter & writer
			, std::string blockName
			, std::string variableName
			, uint32_t binding
			, uint32_t set
			, bool enabled = true )
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
				m_ssbo->declMember< sdw::UVec4 >( "counts" );
			}
		}

		sdw::UInt getCount()const
		{
			return ( m_ssbo
				? m_ssbo->getMember< sdw::UVec4 >( "counts" ).x()
				: m_writer.declLocale( "disabled_" + m_variableName + "_cnt", 0_u, false ) );
		}

		sdw::UInt getSecondaryCount()const
		{
			return ( m_ssbo
				? m_ssbo->getMember< sdw::UVec4 >( "counts" ).y()
				: m_writer.declLocale( "disabled_" + m_variableName + "_cnt", 0_u, false ) );
		}

		bool isEnabled()const noexcept
		{
			return m_ssbo != nullptr;
		}

		sdw::ShaderWriter * getWriter()const noexcept
		{
			return &m_writer;
		}

	protected:
		sdw::ShaderWriter & m_writer;
		std::unique_ptr< sdw::StorageBuffer > m_ssbo;
		std::string m_variableName;
	};

	template< typename DataT >
	class BufferT
		: public BufferBase
	{
	public:
		template< typename ... ParamsT >
		BufferT( sdw::ShaderWriter & writer
			, std::string blockName
			, std::string variableName
			, uint32_t binding
			, uint32_t set
			, bool enabled = true
			, ParamsT && ... params )
			: BufferBase{ writer
				, std::move( blockName )
				, std::move( variableName )
				, binding
				, set
				, enabled }
		{
			if ( isEnabled() )
			{
				m_ssbo->declMemberArray< DataT >( "d", isEnabled(), std::forward< ParamsT >( params )... );
				m_ssbo->end();
			}
		}

		template< typename ... ParamsT >
		DataT getData( sdw::UInt const & index
			, ParamsT && ... params )const
		{
			return ( m_ssbo
				? m_ssbo->getMemberArray< DataT >( "d", isEnabled() )[index]
				: m_writer.declLocale< DataT >( "disabled_" + m_variableName + "_data", false, std::forward< ParamsT >( params )... ) );
		}
	};
}

#endif
