#include "Castor3D/Shader/TextureConfigurationBuffer/TextureAnimationBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	namespace
	{
		TextureAnimationBuffer::TextureAnimationsData doBindData( uint8_t * buffer
			, VkDeviceSize size
			, uint32_t count )
		{
			CU_Require( ( count * sizeof( TextureAnimationData ) ) <= size );
			return makeArrayView( reinterpret_cast< TextureAnimationData * >( buffer )
				, reinterpret_cast< TextureAnimationData * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	TextureAnimationBuffer::TextureAnimationBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "TextureAnimationBuffer" ) }
		, m_data{ doBindData( m_buffer.getPtr(), m_buffer.getSize(), count ) }
		, m_animations{ count + 1u, nullptr }
	{
	}

	void TextureAnimationBuffer::addTextureAnimation( AnimatedTexture & texture )
	{
		auto & unit = texture.getTexture();
		CU_Require( unit.getId() != 0u );
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_animations[unit.getId()] = &texture;
		++m_count;
	}

	void TextureAnimationBuffer::removeTextureAnimation( AnimatedTexture & texture )
	{
		auto & unit = texture.getTexture();
		--m_count;
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_animations[unit.getId()] = nullptr;
	}

	void TextureAnimationBuffer::update()
	{
		if ( m_count )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			auto buffer = m_data.data();

			for ( auto anim : castor::makeArrayView( std::next( m_animations.begin() ), m_animations.end() ) )
			{
				if ( anim )
				{
					anim->fillBuffer( buffer );
				}

				++buffer;
			}

			m_buffer.update();
		}
	}

	VkDescriptorSetLayoutBinding TextureAnimationBuffer::createLayoutBinding( uint32_t binding )const
	{
		return m_buffer.createLayoutBinding( binding );
	}

	void TextureAnimationBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}
}
