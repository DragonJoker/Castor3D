#include "Castor3D/Material/Texture/TextureSourceInfo.hpp"

#include "Castor3D/Material/Texture/Sampler.hpp"

#include <CastorUtils/Miscellaneous/Hash.hpp>

namespace castor3d
{
	//************************************************************************************************

	TextureSourceInfo::TextureSourceInfo( SamplerRes sampler
		, castor::Path folder
		, castor::Path relative
		, castor::ImageLoaderConfig loadConfig )
		: m_sampler{ std::move( sampler ) }
		, m_folder{ std::move( folder ) }
		, m_relative{ std::move( relative ) }
		, m_loadConfig{ std::move( loadConfig ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( SamplerRes sampler
		, RenderTargetSPtr renderTarget )
		: m_sampler{ std::move( sampler ) }
		, m_renderTarget{ std::move( renderTarget ) }
	{
	}

	TextureSourceInfo::TextureSourceInfo( SamplerRes sampler
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

		auto result = std::hash< SamplerRes >{}( value.sampler() );

		if ( value.isRenderTarget() )
		{
			return castor::hashCombinePtr( result, *value.renderTarget() );
		}

		result = castor::hashCombine( result, static_cast< castor::String const & >( value.folder() ) );
		result = castor::hashCombine( result, static_cast< castor::String const & >( value.relative() ) );
		result = castor::hashCombine( result, value.allowCompression() );
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

		result = result && ( lhs.folder() == rhs.folder() );
		result = result && ( lhs.relative() == rhs.relative() );
		result = result && ( lhs.allowCompression() == rhs.allowCompression() );
		return result && ( lhs.generateMips() == rhs.generateMips() );
	}

	//************************************************************************************************
}
