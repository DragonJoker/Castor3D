#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//************************************************************************************************

	TextureSourceInfo::TextureSourceInfo( SamplerObs sampler
		, castor::Path folder
		, castor::Path relative
		, castor::ImageLoaderConfig loadConfig )
		: m_sampler{ std::move( sampler ) }
		, m_folder{ std::move( folder ) }
		, m_relative{ std::move( relative ) }
		, m_loadConfig{ std::move( loadConfig ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( SamplerObs sampler
		, castor::String name
		, castor::String type
		, castor::ByteArray data
		, castor::ImageLoaderConfig loadConfig )
		: m_sampler{ std::move( sampler ) }
		, m_loadConfig{ std::move( loadConfig ) }
		, m_name{ std::move( name ) }
		, m_type{ std::move( type ) }
		, m_data{ std::move( data ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( SamplerObs sampler
		, RenderTargetRPtr renderTarget )
		: m_sampler{ std::move( sampler ) }
		, m_renderTarget{ std::move( renderTarget ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( SamplerObs sampler
		, ashes::ImageCreateInfo const & createInfo )
		: m_sampler{ std::move( sampler ) }
		, m_createInfo{ static_cast< VkImageCreateInfo  const & >( createInfo ) }
	{
	}

	//************************************************************************************************

	size_t TextureSourceInfoHasher::operator()( TextureSourceInfo const & value )const noexcept
	{
		if ( value.isVulkanImage() )
		{
			return std::hash< TextureSourceInfo const * >{}( &value );
		}

		auto result = std::hash< SamplerObs >{}( value.sampler() );

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

		auto result = lhs.sampler() == rhs.sampler();

		if ( lhs.isRenderTarget() || rhs.isRenderTarget() )
		{
			return result
				&& ( lhs.isRenderTarget() && rhs.isRenderTarget() )
				&& ( lhs.renderTarget() == rhs.renderTarget() );
		}

		if ( lhs.isBufferImage() || rhs.isBufferImage() )
		{
			result = result
				&& ( lhs.isBufferImage() && rhs.isBufferImage() )
				&& ( lhs.name() == rhs.name() )
				&& ( lhs.type() == rhs.type() )
				&& ( lhs.buffer().size() == rhs.buffer().size() );
		}
		else if ( lhs.isFileImage() || rhs.isFileImage() )
		{
			result = result
				&& ( lhs.isFileImage() && rhs.isFileImage() )
				&& ( lhs.folder() == rhs.folder() )
				&& ( lhs.relative() == rhs.relative() );
		}

		return result
			&& ( lhs.allowCompression() == rhs.allowCompression() )
			&& ( lhs.generateMips() == rhs.generateMips() );
	}

	//************************************************************************************************
}
