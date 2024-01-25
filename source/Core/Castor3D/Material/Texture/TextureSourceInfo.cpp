#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( castor3d, TextureSourceInfo )

namespace castor3d
{
	//************************************************************************************************

	TextureSourceInfo::TextureSourceInfo( TextureSourceInfo const & rhs
		, TextureConfiguration textureConfig )
		: m_name{ rhs.m_name }
		, m_textureConfig{ castor::move( textureConfig ) }
		, m_renderTarget{ rhs.m_renderTarget }
		, m_folder{ rhs.m_folder }
		, m_relative{ rhs.m_relative }
		, m_loadConfig{ rhs.m_loadConfig }
		, m_type{ rhs.m_type }
		, m_data{ rhs.m_data }
		, m_createInfo{ rhs.m_createInfo }
	{
	}

	TextureSourceInfo::TextureSourceInfo( castor::String name
		, TextureConfiguration textureConfig
		, castor::Path folder
		, castor::Path relative
		, castor::ImageLoaderConfig loadConfig )
		: m_name{ castor::move( name ) }
		, m_textureConfig{ castor::move( textureConfig ) }
		, m_folder{ castor::move( folder ) }
		, m_relative{ castor::move( relative ) }
		, m_loadConfig{ castor::move( loadConfig ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( castor::String name
		, TextureConfiguration textureConfig
		, castor::String type
		, castor::ByteArray data
		, castor::ImageLoaderConfig loadConfig )
		: m_name{ castor::move( name ) }
		, m_textureConfig{ castor::move( textureConfig ) }
		, m_loadConfig{ castor::move( loadConfig ) }
		, m_type{ castor::move( type ) }
		, m_data{ castor::move( data ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( castor::String name
		, TextureConfiguration textureConfig
		, RenderTargetRPtr renderTarget )
		: m_name{ castor::move( name ) }
		, m_textureConfig{ castor::move( textureConfig ) }
		, m_renderTarget{ castor::move( renderTarget ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( castor::String name
		, TextureConfiguration textureConfig
		, ashes::ImageCreateInfo const & createInfo )
		: m_name{ castor::move( name ) }
		, m_textureConfig{ castor::move( textureConfig ) }
		, m_createInfo{ static_cast< VkImageCreateInfo  const & >( createInfo ) }
	{
	}

	//************************************************************************************************

	size_t TextureSourceInfoHasher::operator()( TextureSourceInfo const & value )const noexcept
	{
		auto result = getHash( value.textureConfig() );

		if ( value.isVulkanImage() )
		{
			return castor::hashCombinePtr( result, value );
		}

		if ( value.isRenderTarget() )
		{
			return castor::hashCombinePtr( result, *value.renderTarget() );
		}

		if ( value.isBufferImage() )
		{
			result = castor::hashCombine( result, value.name() );
			result = castor::hashCombine( result, value.type() );
			result = castor::hashCombine( result, value.buffer().size() );
		}
		else
		{
			result = castor::hashCombine( result, static_cast< castor::String const & >( value.folder() ) );
			result = castor::hashCombine( result, static_cast< castor::String const & >( value.relative() ) );
		}

		result = castor::hashCombine( result, value.allowCompression() );
		result = castor::hashCombine( result, value.allowSRGB() );
		return castor::hashCombine( result, value.generateMips() );
	}

	bool operator==( TextureSourceInfo const & lhs
		, TextureSourceInfo const & rhs )noexcept
	{
		if ( lhs.isVulkanImage() || rhs.isVulkanImage() )
		{
			return &lhs == &rhs;
		}

		if ( lhs.isRenderTarget() || rhs.isRenderTarget() )
		{
			return ( lhs.isRenderTarget() && rhs.isRenderTarget() )
				&& ( lhs.renderTarget() == rhs.renderTarget() );
		}

		bool result{ true };

		if ( lhs.isBufferImage() || rhs.isBufferImage() )
		{
			result = ( lhs.isBufferImage() && rhs.isBufferImage() )
				&& ( lhs.name() == rhs.name() )
				&& ( lhs.type() == rhs.type() )
				&& ( lhs.buffer().size() == rhs.buffer().size() );
		}
		else if ( lhs.isFileImage() || rhs.isFileImage() )
		{
			result = ( lhs.isFileImage() && rhs.isFileImage() )
				&& ( lhs.folder() == rhs.folder() )
				&& ( lhs.relative() == rhs.relative() );
		}

		return result
			&& ( lhs.allowCompression() == rhs.allowCompression() )
			&& ( lhs.generateMips() == rhs.generateMips() );
	}

	//************************************************************************************************

	size_t PassTextureConfigHasher::operator()( PassTextureConfig const & value )const noexcept
	{
		auto result = std::hash< uint32_t >{}( value.texcoordSet );
		return castor::hashCombinePtr( result, *value.sampler );
	}

	bool operator==( PassTextureConfig const & lhs
		, PassTextureConfig const & rhs )noexcept
	{
		return lhs.sampler == rhs.sampler
			&& lhs.texcoordSet == rhs.texcoordSet;
	}

	//************************************************************************************************
}
