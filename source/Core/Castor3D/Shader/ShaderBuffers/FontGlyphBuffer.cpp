#include "Castor3D/Shader/ShaderBuffers/FontGlyphBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Overlay/FontTexture.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

CU_ImplementSmartPtr( castor3d, FontGlyphBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace ftglpbuf
	{
		static FontGlyphBuffer::FontGlyphsData doBindData( uint8_t * buffer
			, VkDeviceSize size
			, uint32_t count )
		{
			CU_Require( ( count * sizeof( FontGlyphBuffer::FontGlyphData ) ) <= size );
			return castor::makeArrayView( reinterpret_cast< FontGlyphBuffer::FontGlyphData * >( buffer )
				, reinterpret_cast< FontGlyphBuffer::FontGlyphData * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	FontGlyphBuffer::FontGlyphBuffer( Engine & engine
		, RenderDevice const & device
		, FontTexture const & texture
		, uint32_t count )
		: m_texture{ texture }
		, m_buffer{ engine
			, device
			, count * DataSize
			, cuT( "FontGlyphBuffer-" ) + texture.getFontName()
			, { VK_ACCESS_SHADER_READ_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT } }
		, m_data{ ftglpbuf::doBindData( m_buffer.getPtr(), m_buffer.getSize(), count ) }
	{
	}

	FontGlyphBuffer::~FontGlyphBuffer()noexcept
	{
	}

	void FontGlyphBuffer::add( castor::Glyph const & glyph )
	{
		auto lock( castor::makeUniqueLock( m_mutex ) );
		CU_Require( m_glyphs.size() < m_data.size() );
		auto & data = m_data[m_glyphs.size()];
		data.advance = float( glyph.getAdvance() );
		auto pos = glyph.getBearing();
		data.bearing = castor::Point2f{ pos.x(), pos.y() };
		pos = m_texture.getGlyphPosition( glyph.getCharacter() );
		data.position = castor::Point2f{ pos.x(), pos.y() };
		data.size = castor::Point2f{ glyph.getSize().getWidth(), glyph.getSize().getHeight() };
		m_glyphs.emplace_back( &glyph );
		m_dirty = true;
	}

	void FontGlyphBuffer::update( UploadData & uploader )
	{
		if ( !m_glyphs.empty() && m_dirty.exchange( false ) )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			auto buffer = m_data.data();

			for ( auto & glyph : castor::makeArrayView( m_glyphs.begin(), m_glyphs.end() ) )
			{
				auto pos = m_texture.getGlyphPosition( glyph->getCharacter() );
				buffer->position = castor::Point2f{ pos.x(), pos.y() };
				++buffer;
			}

			m_buffer.setCount( uint32_t( m_glyphs.size() ) );
			m_buffer.upload( uploader );
		}
	}

	VkDescriptorSetLayoutBinding FontGlyphBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	ashes::WriteDescriptorSet FontGlyphBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	void FontGlyphBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}
}
