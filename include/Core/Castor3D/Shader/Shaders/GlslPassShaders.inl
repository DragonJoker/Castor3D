#include "Castor3D/Shader/Shaders/GlslPassShaders.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslTextureConfiguration.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

namespace castor3d::shader
{
	template< typename TexcoordT >
	void PassShaders::computeMapContribution( TextureCombine const & combine
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, TextureAnimData const & anim
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordT & texCoords
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, anim, map );
		this->computeTexcoord( combine, config, imgCompConfig, anim, map, texCoords, components );
		auto sampled = writer.declLocale( "c3d_sampled"
			, m_utils.sampleMap( map, texCoords ) );
		applyComponents( combine, config, imgCompConfig, sampled, components );
	}

	template< typename TexcoordT >
	void PassShaders::computeMapContribution( PipelineFlags const & flags
		, TextureConfigData const & config
		, sdw::U32Vec3 const & imgCompConfig
		, TextureAnimData const & anim
		, sdw::CombinedImage2DRgba32 const & map
		, TexcoordT & texCoords
		, BlendComponents & components )const
	{
		auto & writer = findWriterMandat( config, anim, map );
		this->computeTexcoord( flags, config, imgCompConfig, anim, map, texCoords, components );
		auto sampled = writer.declLocale( "c3d_sampled"
			, m_utils.sampleMap( flags, map, texCoords ) );
		applyComponents( flags, config, imgCompConfig, sampled, components );
	}
}
