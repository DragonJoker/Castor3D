#include "Castor3D/Shader/ShaderBuffers/TextureAnimationBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureAnimation.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"

#include <ashespp/Descriptor/DescriptorSetLayout.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

CU_ImplementSmartPtr( castor3d, TextureAnimationBuffer )

namespace castor3d
{
	//*********************************************************************************************

	namespace texanmbuf
	{
		static TextureAnimationBuffer::TextureAnimationsData doBindData( uint8_t * buffer
			, VkDeviceSize size
			, uint32_t count )
		{
			CU_Require( ( count * sizeof( TextureAnimationData ) ) <= size );
			return castor::makeArrayView( reinterpret_cast< TextureAnimationData * >( buffer )
				, reinterpret_cast< TextureAnimationData * >( buffer ) + count );
		}
	}

	//*********************************************************************************************

	TextureAnimationBuffer::TextureAnimationBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count )
		: m_buffer{ engine, device, count * DataSize, cuT( "TextureAnimationBuffer" ) }
		, m_data{ texanmbuf::doBindData( m_buffer.getPtr(), m_buffer.getSize(), count ) }
		, m_animations{ count + 1u, nullptr }
	{
	}

	void TextureAnimationBuffer::addTextureAnimation( AnimatedTexture const & texture )
	{
		CU_Require( texture.hasTextureUnit() );
		auto & unit = texture.getTextureUnit();
		CU_Require( unit.getId() != 0u );
		auto lock( castor::makeUniqueLock( m_mutex ) );
		m_animations[unit.getId()] = &texture;
		++m_count;
	}

	void TextureAnimationBuffer::removeTextureAnimation( AnimatedTexture const & texture )noexcept
	{
		if ( texture.hasTextureUnit() )
		{
			auto & unit = texture.getTextureUnit();
			--m_count;
			auto lock( castor::makeUniqueLock( m_mutex ) );
			m_animations[unit.getId()] = nullptr;
		}
	}

	void TextureAnimationBuffer::update( UploadData & uploader )
	{
		if ( m_count )
		{
			auto lock( castor::makeUniqueLock( m_mutex ) );
			auto buffer = m_data.begin();
			uint32_t count = 0u;

			for ( auto anim : castor::makeArrayView( std::next( m_animations.begin() ), m_animations.end() ) )
			{
				if ( buffer == m_data.end() )
				{
					log::warn << "TextureAnimation [" << anim->getName() << "] is out of buffer boundaries, ignoring it." << std::endl;
					break;
				}

				if ( anim )
				{
					anim->fillBuffer( buffer );
					count = uint32_t( std::distance( m_data.begin(), buffer ) ) + 2u;
				}

				++buffer;
			}

			m_buffer.setCount( uint32_t( std::min( m_data.size(), m_animations.size() ) ) );
			m_buffer.upload( uploader
				, 0u
				, VkDeviceSize( sizeof( TextureAnimationData ) * count ) );
		}
	}

	VkDescriptorSetLayoutBinding TextureAnimationBuffer::createLayoutBinding( uint32_t binding
		, VkShaderStageFlags stages )const
	{
		return m_buffer.createLayoutBinding( binding, stages );
	}

	ashes::WriteDescriptorSet TextureAnimationBuffer::getBinding( uint32_t binding )const
	{
		return m_buffer.getBinding( binding );
	}

	void TextureAnimationBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}
}
