/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslPagedBuffer_H___
#define ___C3D_GlslPagedBuffer_H___

#include "SdwModule.hpp"

#include <ShaderWriter/Writer.hpp>
#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StorageBuffer.hpp>

namespace c3d::shader
{
	template< typename DataT >
	class PagedBufferT
	{
	private:
		PagedBufferT( sdw::ShaderWriter & writer, MbString const & name, bool enabled, uint32_t pageCount, uint32_t pageElementCount, uint32_t binding, uint32_t set )
			: m_writer{ writer }
			, m_name{ name }
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
			auto pageIndex = m_writer.declLocale( m_name + "PageIndex", index / m_pageElementCount );
			auto localIndex = m_writer.declLocale( m_name + "LocalIndex", index % m_pageElementCount );
			return m_buffers[pageIndex][localIndex];
		}

		sdw::ReturnWrapperT< DataT > operator[]( sdw::Int const & index )const
		{
			auto pageIndex = m_writer.declLocale( m_name + "PageIndex", m_writer.cast< sdw::UInt >( index ) / m_pageElementCount );
			auto localIndex = m_writer.declLocale( m_name + "LocalIndex", m_writer.cast< sdw::UInt >( index ) % m_pageElementCount );
			return m_buffers[pageIndex][localIndex];
		}

		sdw::ReturnWrapperT< DataT > operator[]( uint32_t index )const
		{
			auto pageIndex = m_writer.declLocale( m_name + "PageIndex", index / m_pageElementCount );
			auto localIndex = m_writer.declLocale( m_name + "LocalIndex", index % m_pageElementCount );
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
		MbString m_name;
		uint32_t m_pageElementCount;
		sdw::Array< sdw::ArrayStorageBufferT< DataT > > m_buffers;
	};
}

#endif
