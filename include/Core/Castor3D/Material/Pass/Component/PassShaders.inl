#include "PassShaders.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d::shader
{
	template< typename TexcoordT >
	void PassShaders::computeTexcoord( TextureFlags const & texturesFlags
		, TextureConfigData const & config
		, TextureAnimData const & anim
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordT & texCoords
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, anim, map );
		auto texCoord = writer.declLocale( "c3d_texCoord"
			, config.toUv( texCoords ) );
		config.transformUV( m_utils, anim, texCoord );
		config.setUv( texCoords, texCoord );
	}

	template< typename TexcoordT >
	void PassShaders::computeTexcoord( PipelineFlags const & flags
		, TextureConfigData const & config
		, TextureAnimData const & anim
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordT & texCoords
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, anim, map );
		auto texCoord = writer.declLocale( "c3d_texCoord"
			, config.toUv( texCoords ) );
		config.transformUV( m_utils, anim, texCoord );
		config.setUv( texCoords, texCoord );
		this->applyParallax( flags, config, map, texCoords, texCoord, components );
	}

	template< typename TexcoordT >
	void PassShaders::computeMapContribution( TextureFlags const & texturesFlags
		, TextureConfigData const & config
		, TextureAnimData const & anim
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordT & texCoords
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, anim, map );
		this->computeTexcoord( texturesFlags, config, anim, map, texCoords, components );
		auto sampled = writer.declLocale( "c3d_sampled"
			, m_utils.sampleMap( map, texCoords ) );
		applyComponents( texturesFlags, config, sampled, components );
	}

	template< typename TexcoordT >
	void PassShaders::computeMapContribution( PipelineFlags const & flags
		, TextureConfigData const & config
		, TextureAnimData const & anim
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordT & texCoords
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, anim, map );
		this->computeTexcoord( flags, config, anim, map, texCoords, components );
		auto sampled = writer.declLocale( "c3d_sampled"
			, m_utils.sampleMap( flags, map, texCoords ) );
		applyComponents( flags, config, sampled, components );
	}

	template< typename TexcoordsT, typename TexcoordT >
	void PassShaders::applyParallax( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordsT & texCoords
		, TexcoordT & texCoord
		, BlendComponents & components )const
	{
		if ( flags.enableParallaxOcclusionMapping()
			&& components.hasMember( "tangentSpaceViewPosition" )
			&& components.hasMember( "tangentSpaceFragPosition" ) )
		{
			auto & writer = findWriterMandat( config, map );

			IF( writer, config.hgtEnbl() != 0.0_f )
			{
				auto tangentSpaceViewPosition = components.getMember< sdw::Vec3 >( "tangentSpaceViewPosition", true );
				auto tangentSpaceFragPosition = components.getMember< sdw::Vec3 >( "tangentSpaceFragPosition", true );
				m_utils.parallaxMapping( texCoord
					, normalize( tangentSpaceViewPosition - tangentSpaceFragPosition )
					, map
					, config );
				config.setUv( texCoords, texCoord );

				if ( flags.enableParallaxOcclusionMappingOne() )
				{
					IF( writer, config.getUv( texCoords ).x() > 1.0_f
						|| config.getUv( texCoords ).y() > 1.0_f
						|| config.getUv( texCoords ).x() < 0.0_f
						|| config.getUv( texCoords ).y() < 0.0_f )
					{
						writer.demote();
					}
					FI;
				}

				auto texCoords0 = components.getMember< TexcoordsT >( "texture0" );
				auto texCoords1 = components.getMember< TexcoordsT >( "texture1", true );
				auto texCoords2 = components.getMember< TexcoordsT >( "texture2", true );
				auto texCoords3 = components.getMember< TexcoordsT >( "texture3", true );
				TextureConfigurations::setTexcoord( config
					, texCoords
					, texCoords0
					, texCoords1
					, texCoords2
					, texCoords3 );
			}
			FI;
		}
	}
}
