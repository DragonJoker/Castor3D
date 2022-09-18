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
		BufferBase( std::string const & bufferName
			, std::string const & arrayName
			, sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true )
			: m_writer{ writer }
			, m_arrayName{ arrayName }
		{
			if ( enable )
			{
				m_ssbo = std::make_unique< sdw::StorageBuffer >( m_writer
					, bufferName
					, binding
					, set );
				m_ssbo->declMember< sdw::UVec4 >( "counts" );
			}
		}

		sdw::UInt getCount()const
		{
			return ( m_ssbo
				? m_ssbo->getMember< sdw::UVec4 >( "counts" ).x()
				: m_writer.declLocale( "disabled_" + m_arrayName + "_cnt", 0_u, false ) );
		}

		sdw::UInt getSecondaryCount()const
		{
			return ( m_ssbo
				? m_ssbo->getMember< sdw::UVec4 >( "counts" ).y()
				: m_writer.declLocale( "disabled_" + m_arrayName + "_cnt", 0_u, false ) );
		}

		bool isEnabled()const noexcept
		{
			return m_ssbo != nullptr;
		}

	protected:
		sdw::ShaderWriter & m_writer;
		std::string m_arrayName;
		std::unique_ptr< sdw::StorageBuffer > m_ssbo;
	};

	template< typename DataT >
	class BufferT
		: public BufferBase
	{
	public:
		BufferT( std::string const & bufferName
			, std::string const & arrayName
			, sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true )
			: BufferBase{ bufferName, arrayName, writer, binding, set, enable }
		{
			if ( enable )
			{
				m_ssbo->declMemberArray< DataT >( m_arrayName );
				m_ssbo->end();
			}
		}

		DataT getData( sdw::UInt const & index )const
		{
			return ( m_ssbo
				? m_ssbo->getMemberArray< DataT >( m_arrayName )[index]
				: m_writer.declLocale< DataT >( "disabled_" + m_arrayName + "_data", false ) );
		}
	};
}

#endif
