#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include <CastorUtils/Graphics/ImageCache.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( c3d, TextureSourceInfo )

namespace c3d
{
	//************************************************************************************************

	TextureSourceInfo::TextureSourceInfo( TextureSourceInfo const & rhs
		, TextureConfiguration textureConfig )
		: m_name{ rhs.m_name }
		, m_textureConfig{ c3d::move( textureConfig ) }
		, m_renderTarget{ rhs.m_renderTarget }
		, m_folder{ rhs.m_folder }
		, m_relative{ rhs.m_relative }
		, m_loadConfig{ rhs.m_loadConfig }
		, m_type{ rhs.m_type }
		, m_data{ rhs.m_data }
		, m_createInfo{ rhs.m_createInfo }
	{
	}

	TextureSourceInfo::TextureSourceInfo( String name
		, TextureConfiguration textureConfig
		, Path folder
		, Path relative
		, ImageLoaderConfig loadConfig )
		: m_name{ c3d::move( name ) }
		, m_textureConfig{ c3d::move( textureConfig ) }
		, m_folder{ c3d::move( folder ) }
		, m_relative{ c3d::move( relative ) }
		, m_loadConfig{ c3d::move( loadConfig ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( String name
		, TextureConfiguration textureConfig
		, String type
		, ByteArray data
		, ImageLoaderConfig loadConfig )
		: m_name{ c3d::move( name ) }
		, m_textureConfig{ c3d::move( textureConfig ) }
		, m_loadConfig{ c3d::move( loadConfig ) }
		, m_type{ c3d::move( type ) }
		, m_data{ c3d::move( data ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( String name
		, TextureConfiguration textureConfig
		, RenderTargetRPtr renderTarget )
		: m_name{ c3d::move( name ) }
		, m_textureConfig{ c3d::move( textureConfig ) }
		, m_renderTarget{ c3d::move( renderTarget ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( String name
		, TextureConfiguration textureConfig
		, ImageCreateInfo createInfo )
		: m_name{ c3d::move( name ) }
		, m_textureConfig{ c3d::move( textureConfig ) }
		, m_createInfo{ c3d::move( createInfo ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( String name
		, TextureConfiguration textureConfig
		, ImageCreateParams imageParams )
		: m_name{ c3d::move( name ) }
		, m_textureConfig{ c3d::move( textureConfig ) }
		, m_folder{ imageParams.path.getPath() }
		, m_relative{ imageParams.path.getFileName( true ) }
		, m_loadConfig{ c3d::move( imageParams.loadConfig ) }
		, m_type{ c3d::move( imageParams.type ) }
		, m_data{ c3d::move( imageParams.data ) }
	{
	}

	//************************************************************************************************

	size_t TextureSourceInfoHasher::operator()( TextureSourceInfo const & value )const noexcept
	{
		auto result = getHash( value.textureConfig() );

		if ( value.isVulkanImage() )
		{
			return hashCombinePtr( result, value );
		}

		if ( value.isRenderTarget() )
		{
			return hashCombinePtr( result, *value.renderTarget() );
		}

		if ( value.isBufferImage() )
		{
			result = hashCombine( result, value.name() );
			result = hashCombine( result, value.type() );
			result = hashCombine( result, value.buffer().size() );
		}
		else
		{
			result = hashCombine( result, static_cast< String const & >( value.folder() ) );
			result = hashCombine( result, static_cast< String const & >( value.relative() ) );
		}

		result = hashCombine( result, value.allowCompression() );
		result = hashCombine( result, value.allowSRGB() );
		return hashCombine( result, value.generateMips() );
	}

	//************************************************************************************************

	size_t PassTextureConfigHasher::operator()( PassTextureConfig const & value )const noexcept
	{
		auto result = std::hash< uint32_t >{}( value.texcoordSet );
		return hashCombinePtr( result, *value.sampler );
	}

	//************************************************************************************************
}
