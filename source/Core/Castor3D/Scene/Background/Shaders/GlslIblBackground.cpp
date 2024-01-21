#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	namespace iblbg
	{
		static sdw::Vec3 getPrefiltered( sdw::CombinedImageCubeRgba32 const & prefiltered
			, sdw::Vec3 const & coord
			, sdw::Vec3 const & albedo
			, sdw::Float const & roughness )
		{
			return prefiltered.lod( coord, roughness * float( MaxIblReflectionLod ) ).rgb() * albedo;
		}
	}

	castor::String const IblBackgroundModel::Name = cuT( "c3d.ibl" );

	IblBackgroundModel::IblBackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
		: BackgroundModel{ writer, utils, std::move( targetSize ) }
	{
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapBackground"
			, binding
			, set );
		++binding;
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, binding
			, set );
		++binding;
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, binding
			, set );
		++binding;
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapPrefilteredSheen"
			, binding
			, set );
		++binding;
	}

	BackgroundModelPtr IblBackgroundModel::create( Engine const & /*engine*/
		, sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, bool /*needsForeground*/
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< IblBackgroundModel >( writer
			, utils
			, std::move( targetSize )
			, binding
			, set );
	}

	sdw::RetVec3 IblBackgroundModel::computeDiffuseReflections( sdw::Vec3 const & palbedo
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pfresnel
		, sdw::Float const & pmetalness )
	{
		if ( !m_computeDiffuseReflections )
		{
			m_computeDiffuseReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeReflections"
				, [this]( sdw::Vec3 const & albedo
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & fresnel
					, sdw::Float const & metalness
					, sdw::CombinedImageCubeRgba32 const & irradianceMap )
				{
					auto kD = m_writer.declLocale( "kD"
						, vec3( 1.0_f ) - fresnel );
					kD *= 1.0_f - metalness;
					auto irradiance = m_writer.declLocale( "irradiance"
						, irradianceMap.lod( vec3( wsNormal.x(), -wsNormal.y(), wsNormal.z() ), 0.0_f ).rgb() );
					m_writer.returnStmt( kD * irradiance * albedo );
				}
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "fresnel" }
				, sdw::InFloat{ m_writer, "metalness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "irradianceMap" } );
		}

		auto irradianceMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapIrradiance" );
		return m_computeDiffuseReflections( palbedo
			, pwsNormal
			, pfresnel
			, pmetalness
			, irradianceMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeSpecularReflections( sdw::Vec3 const & pfresnel
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, sdw::Float const & proughness
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSpecularReflections )
		{
			m_computeSpecularReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSpecularReflections"
				, [this]( sdw::Vec3 const & F
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Float const & NdotV
					, sdw::Float const & roughness
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap
					, sdw::CombinedImage2DRgba32 const & brdfMap )
				{
					auto reflection = m_writer.declLocale( "reflection"
						, reflect( -V, N ) );
					reflection.y() = -reflection.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, iblbg::getPrefiltered( prefilteredEnvMap, reflection, vec3( 1.0_f ), roughness ) );
					auto brdf = m_writer.declLocale( "brdf"
						, getBrdf( brdfMap, NdotV, roughness ) );
					m_writer.returnStmt( prefilteredColor * sdw::fma( F
						, vec3( brdf.x() )
						, vec3( brdf.y() ) ) );
				}
				, sdw::InVec3{ m_writer, "F" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" } );
		}

		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeSpecularReflections( pfresnel
			, pwsNormal
			, pV
			, pNdotV
			, proughness
			, prefilteredEnvMap
			, pbrdfMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeSheenReflections( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSheenReflections )
		{
			m_computeSheenReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSheenReflections"
				, [this]( sdw::Vec3 const & sheenColour
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Float const & NdotV
					, sdw::Float const & sheenRoughness
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap
					, sdw::CombinedImage2DRgba32 const & brdfMap )
				{
					auto reflection = m_writer.declLocale( "reflection"
						, normalize( reflect( -V, N ) ) );
					reflection.y() = -reflection.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, iblbg::getPrefiltered( prefilteredEnvMap
							, reflection
							, sheenColour
							, sheenRoughness ) );
					auto brdf = m_writer.declLocale( "brdf"
						, getBrdf( brdfMap, NdotV, sheenRoughness ) );

					m_writer.returnStmt( prefilteredColor.rgb() * brdf.z() );
				}
				, sdw::InVec3{ m_writer, "sheenColour" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "sheenRoughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" } );
		}

		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefilteredSheen" );
		return m_computeSheenReflections( components.sheenFactor
			, pwsNormal
			, pV
			, pNdotV
			, components.sheenRoughness
			, prefilteredEnvMap
			, pbrdfMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeRefractions( sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pV
		, sdw::Float const & prefractionRatio
		, BlendComponents & components )
	{
		if ( !m_computeRefractions )
		{
			m_computeRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeRefractions"
				, [this]( sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & albedo
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( -V, N, refractionRatio ) );
					refracted.y() = -refracted.y();
					m_writer.returnStmt( iblbg::getPrefiltered( prefiltered
						, refracted
						, albedo
						, roughness ) );
				}
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeRefractions( prefiltered
			, prefractionRatio
			, components.colour
			, pwsNormal
			, pV
			, components.roughness );
	}

	sdw::RetVec3 IblBackgroundModel::computeSpecularRefractions( sdw::Vec3 const & pfresnel
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pV
		, sdw::Float const & pNdotV
		, sdw::Float const & proughness
		, sdw::Float const & prefractionRatio
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSpecularRefractions )
		{
			m_computeSpecularRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSpecularRefractions"
				, [this]( sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::CombinedImage2DRgba32 const & brdfMap
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & albedo
					, sdw::Vec3 const & N
					, sdw::Vec3 const & V
					, sdw::Vec3 const & F
					, sdw::Float const & NdotV
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( -V, N, refractionRatio ) );
					refracted.y() = -refracted.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, iblbg::getPrefiltered( prefiltered
							, refracted
							, albedo
							, roughness ) );
					auto brdf = m_writer.declLocale( "brdf"
						, getBrdf( brdfMap, NdotV, roughness ) );
					m_writer.returnStmt( prefilteredColor * sdw::fma( F
						, vec3( brdf.x() )
						, vec3( brdf.y() ) ) );
				}
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "N" }
				, sdw::InVec3{ m_writer, "V" }
				, sdw::InVec3{ m_writer, "F" }
				, sdw::InFloat{ m_writer, "NdotV" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeSpecularRefractions( prefiltered
			, pbrdfMap
			, prefractionRatio
			, components.colour
			, pwsNormal
			, pV
			, pfresnel
			, pNdotV
			, proughness );
	}
}
