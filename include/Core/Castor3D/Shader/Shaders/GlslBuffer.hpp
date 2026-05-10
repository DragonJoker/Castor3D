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

namespace c3d::shader
{
	class BufferBase
	{
	public:
		BufferBase( sdw::ShaderWriter & writer
			, MbString blockName
			, MbString variableName
			, uint32_t binding
			, uint32_t set
			, bool enabled = true )
			: m_writer{ writer }
			, m_ssbo{ m_writer.declStorageBuffer( variableName, binding, set, sdw::type::MemoryLayout::eStd430, enabled ) }
			, m_counts{ ( m_ssbo.isEnabled()
				? m_ssbo.declMember< sdw::UVec4 >( "counts" )
				: m_writer.declGlobal< sdw::UVec4 >( "disabled_" + variableName + "_cnt", false ) ) }
		{
		}

		sdw::UInt getFirstCount()const
		{
			return m_counts.x();
		}

		sdw::UInt getSecondCount()const
		{
			return m_counts.y();
		}

		sdw::UInt getThirdCount()const
		{
			return m_counts.z();
		}

		sdw::UInt getFourthCount()const
		{
			return m_counts.w();
		}

		sdw::UInt getCount()const
		{
			return getFirstCount();
		}

		bool isEnabled()const noexcept
		{
			return m_ssbo.isEnabled();
		}

		sdw::ShaderWriter * getWriter()const noexcept
		{
			return &m_writer;
		}

	protected:
		sdw::ShaderWriter & m_writer;
		sdw::StorageBuffer m_ssbo;
		sdw::UVec4 m_counts;
	};

	template< typename DataT >
	class BufferT
		: public BufferBase
	{
	public:
		template< typename ... ParamsT >
		BufferT( sdw::ShaderWriter & writer
			, MbString blockName
			, MbString variableName
			, uint32_t binding
			, uint32_t set
			, bool enabled = true
			, ParamsT && ... params )
			: BufferBase{ writer
				, c3d::move( blockName )
				, variableName
				, binding
				, set
				, enabled }
			, m_variableName{ c3d::move( variableName ) }
		{
			if ( isEnabled() )
			{
				m_ssbo.declMemberArray< DataT >( "d", isEnabled(), c3d::forward< ParamsT >( params )... );
				m_ssbo.end();
			}
		}

		template< typename ... ParamsT >
		DataT getData( sdw::UInt const & index
			, ParamsT && ... params )const
		{
			return ( isEnabled()
				? DataT{ m_ssbo.getMemberArray< DataT >( "d", isEnabled() )[index] }
				: m_writer.declLocale< DataT >( "disabled_" + m_variableName + "_data", false, c3d::forward< ParamsT >( params )... ) );
		}

	private:
		MbString m_variableName;
	};

	template< typename DataT >
	class PagedBufferT
	{
	private:
		PagedBufferT( sdw::ShaderWriter & writer, MbString const & name, bool enabled, uint32_t pageCount, uint32_t pageElementCount, uint32_t binding, uint32_t set )
			: m_writer{ writer }
			, m_pageElementCount{ pageElementCount }
			, m_buffers{ writer.declArrayStorageBufferArray< DataT >( name
				, binding, set, pageCount, sdw::type::MemoryLayout::eStd430, enabled ) }
		{
		}

	public:
		PagedBufferT( sdw::ShaderWriter & writer, MbString const & name, uint32_t pageCount, uint32_t pageElementCount, uint32_t & binding, uint32_t set, bool enabled = true )
			: PagedBufferT{ writer, name, enabled, pageCount, pageElementCount, binding, set }
		{
			binding += pageCount;
		}

		template< typename BindingT >
		PagedBufferT( sdw::ShaderWriter & writer, MbString const & name, uint32_t pageCount, uint32_t pageElementCount, BindingT binding, uint32_t set, bool enabled = true )
			: PagedBufferT{ writer, name, enabled, pageCount, pageElementCount, uint32_t( binding ), set }
		{
		}

		sdw::ReturnWrapperT< DataT > operator[]( sdw::UInt const & index )const
		{
			auto tmpIndex = m_writer.declLocale( "c3d_tmpIndex", index );
			auto pageIndex = tmpIndex / m_pageElementCount;
			auto localIndex = tmpIndex % m_pageElementCount;
			return m_buffers[pageIndex][localIndex];
		}

		sdw::ReturnWrapperT< DataT > operator[]( sdw::Int const & index )const
		{
			auto tmpIndex = m_writer.declLocale( "c3d_tmpIndex", m_writer.cast< sdw::UInt >( index ) );
			auto pageIndex = tmpIndex / m_pageElementCount;
			auto localIndex = tmpIndex % m_pageElementCount;
			return m_buffers[pageIndex][localIndex];
		}

		sdw::ReturnWrapperT< DataT > operator[]( uint32_t index )const
		{
			auto pageIndex = index / m_pageElementCount;
			auto localIndex = index % m_pageElementCount;
			return m_buffers[pageIndex][localIndex];
		}

		sdw::ReturnWrapperT< DataT > loadInPage( sdw::UInt const & page, sdw::UInt const & index )const
		{
			return m_buffers[page][index];
		}

		void storeInPage( sdw::UInt const & page, sdw::UInt const & index, DataT const & aabb )const
		{
			m_buffers[page][index] = aabb;
		}

	private:
		sdw::ShaderWriter & m_writer;
		uint32_t m_pageElementCount;
		sdw::Array< sdw::ArrayStorageBufferT< DataT > > m_buffers;
	};
}

#endif
