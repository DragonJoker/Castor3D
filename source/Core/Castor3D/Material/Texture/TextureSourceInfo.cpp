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

	//************************************************************************************************

	size_t TextureSourceInfoHasher::operator()( TextureSourceInfo const & value )const noexcept
	{
		auto result = std::hash< RenderTargetSPtr >{}( value.renderTarget() );
		result = castor::hashCombinePtr( result, *value.sampler() );

		if ( !value.renderTarget() )
		{
			result = castor::hashCombine( result, static_cast< castor::String const & >( value.folder() ) );
			result = castor::hashCombine( result, static_cast< castor::String const & >( value.relative() ) );
			result = castor::hashCombine( result, value.allowCompression() );
			result = castor::hashCombine( result, value.generateMips() );
		}

		return result;
	}

	bool operator==( TextureSourceInfo const & lhs
		, TextureSourceInfo const & rhs )noexcept
	{
		auto result = lhs.sampler() == rhs.sampler();
		result = result && ( lhs.renderTarget() == rhs.renderTarget() );

		if ( result && !lhs.renderTarget() )
		{
			result = result && ( lhs.folder() == rhs.folder() );
			result = result && ( lhs.relative() == rhs.relative() );
			result = result && ( lhs.allowCompression() == rhs.allowCompression() );
			result = result && ( lhs.generateMips() == rhs.generateMips() );
		}

		return result;
	}

	//************************************************************************************************
}
