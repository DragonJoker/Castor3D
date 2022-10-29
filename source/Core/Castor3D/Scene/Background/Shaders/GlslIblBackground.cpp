#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	castor::String const IblBackgroundModel::Name = cuT( "c3d.ibl" );

	IblBackgroundModel::IblBackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
		: BackgroundModel{ writer, utils, std::move( targetSize ) }
	{
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapBackground"
			, binding++
			, set );
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapIrradiance"
			, binding++
			, set );
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapPrefiltered"
			, binding++
			, set );
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapPrefilteredSheen"
			, binding++
			, set );
	}

	BackgroundModelPtr IblBackgroundModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< IblBackgroundModel >( writer
			, utils
			, std::move( targetSize )
			, binding
			, set );
	}

	sdw::RetVec3 IblBackgroundModel::computeReflections( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeReflections )
		{
			m_computeReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeReflections"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & albedo
					, sdw::Vec3 const & specular
					, sdw::Float const & metalness
					, sdw::Float const & roughness
					, sdw::CombinedImageCubeRgba32 const & irradianceMap
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap
					, sdw::CombinedImage2DRgba32 const & brdfMap )
				{
					auto V = m_writer.declLocale( "V"
						, -wsIncident );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( dot( wsNormal, V ), 0.0_f ) );
					auto F = m_writer.declLocale( "F"
						, m_utils.conductorFresnel( NdotV, specular ) );
					auto kS = m_writer.declLocale( "kS"
						, F );
					auto kD = m_writer.declLocale( "kD"
						, vec3( 1.0_f ) - kS );
					kD *= 1.0_f - metalness;

					auto irradiance = m_writer.declLocale( "irradiance"
						, irradianceMap.sample( vec3( wsNormal.x(), -wsNormal.y(), wsNormal.z() ) ).rgb() );
					auto diffuseReflection = m_writer.declLocale( "diffuseReflection"
						, irradiance * albedo );
					auto R = m_writer.declLocale( "R"
						, reflect( -V, wsNormal ) );
					R.y() = -R.y();

					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, prefilteredEnvMap.lod( R, roughness * sdw::Float( float( MaxIblReflectionLod ) ) ).rgb() );
					auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
						, vec2( NdotV, roughness ) );
					auto envBRDF = m_writer.declLocale( "envBRDF"
						, brdfMap.sample( envBRDFCoord ) );
					auto specularReflection = m_writer.declLocale( "specularReflection"
						, prefilteredColor * sdw::fma( kS
							, vec3( envBRDF.x() )
							, vec3( envBRDF.y() ) ) );

					m_writer.returnStmt( sdw::fma( kD
						, diffuseReflection
						, specularReflection ) );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "specular" }
				, sdw::InFloat{ m_writer, "metalness" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "irradianceMap" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" } );
		}
		auto irradianceMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapIrradiance" );
		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeReflections( pwsIncident
			, pwsNormal
			, components.colour
			, components.specular
			, components.metalness
			, components.roughness
			, irradianceMap
			, prefilteredEnvMap
			, pbrdfMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeRefractions( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & prefractionRatio
		, BlendComponents & components )
	{
		if ( !m_computeRefractions )
		{
			m_computeRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeRefractions"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & albedo
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					refracted.y() = -refracted.y();
					m_writer.returnStmt( prefiltered.lod( refracted, roughness * sdw::Float( float( MaxIblReflectionLod ) ) ).rgb()
						* albedo );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeRefractions( pwsIncident
			, pwsNormal
			, prefiltered
			, prefractionRatio
			, components.colour
			, components.roughness );
	}

	sdw::RetVec3 IblBackgroundModel::computeSpecularReflections( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pspecular
		, sdw::Float const & proughness
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSpecularReflections )
		{
			m_computeSpecularReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSpecularReflections"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & albedo
					, sdw::Vec3 const & specular
					, sdw::Float const & metalness
					, sdw::Float const & roughness
					, sdw::CombinedImageCubeRgba32 const & irradianceMap
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap
					, sdw::CombinedImage2DRgba32 const & brdfMap )
				{
					auto V = m_writer.declLocale( "V"
						, -wsIncident );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( dot( wsNormal, V ), 0.0_f ) );
					auto F = m_writer.declLocale( "F"
						, m_utils.conductorFresnel( NdotV, specular ) );
					auto kS = m_writer.declLocale( "kS"
						, F );

					auto R = m_writer.declLocale( "R"
						, reflect( -V, wsNormal ) );
					R.y() = -R.y();

					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, prefilteredEnvMap.lod( R, roughness * sdw::Float( float( MaxIblReflectionLod ) ) ).rgb() );
					auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
						, vec2( NdotV, roughness ) );
					auto envBRDF = m_writer.declLocale( "envBRDF"
						, brdfMap.sample( envBRDFCoord ) );

					m_writer.returnStmt( prefilteredColor * sdw::fma( kS
						, vec3( envBRDF.x() )
						, vec3( envBRDF.y() ) ) );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "specular" }
				, sdw::InFloat{ m_writer, "metalness" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "irradianceMap" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" } );
		}
		auto irradianceMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapIrradiance" );
		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeReflections( pwsIncident
			, pwsNormal
			, components.colour
			, pspecular
			, components.metalness
			, proughness
			, irradianceMap
			, prefilteredEnvMap
			, pbrdfMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeSpecularRefractions( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Vec3 const & pspecular
		, sdw::Float const & proughness
		, sdw::Float const & prefractionRatio
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSpecularRefractions )
		{
			m_computeSpecularRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSpecularRefractions"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::CombinedImage2DRgba32 const & brdfMap
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & albedo
					, sdw::Vec3 const & specular
					, sdw::Float const & roughness )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					refracted.y() = -refracted.y();
					auto prefilteredColor = m_writer.declLocale( "prefilteredColor"
						, prefiltered.lod( refracted, roughness * sdw::Float( float( MaxIblReflectionLod ) ) ).rgb() * albedo );

					auto V = m_writer.declLocale( "V"
						, -wsIncident );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( dot( wsNormal, V ), 0.0_f ) );
					auto F = m_writer.declLocale( "F"
						, m_utils.conductorFresnel( NdotV, specular ) );
					auto kS = m_writer.declLocale( "kS"
						, F );
					auto envBRDFCoord = m_writer.declLocale( "envBRDFCoord"
						, vec2( NdotV, roughness ) );
					auto envBRDF = m_writer.declLocale( "envBRDF"
						, brdfMap.sample( envBRDFCoord ) );

					m_writer.returnStmt( prefilteredColor * sdw::fma( kS
						, vec3( envBRDF.x() )
						, vec3( envBRDF.y() ) ) );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InVec3{ m_writer, "specular" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeSpecularRefractions( pwsIncident
			, pwsNormal
			, prefiltered
			, pbrdfMap
			, prefractionRatio
			, components.colour
			, pspecular
			, proughness );
	}

	sdw::RetVec3 IblBackgroundModel::computeSheenReflections( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, BlendComponents & components
		, sdw::CombinedImage2DRgba32 const & pbrdfMap )
	{
		if ( !m_computeSheenReflections )
		{
			m_computeSheenReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeSheenReflections"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::Vec3 const & sheenColour
					, sdw::Float const & sheenRoughness
					, sdw::CombinedImageCubeRgba32 const & prefilteredEnvMap
					, sdw::CombinedImage2DRgba32 const & brdfMap )
				{
					auto V = m_writer.declLocale( "V"
						, -wsIncident );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( dot( wsNormal, V ), 0.0_f ) );
					auto lod = m_writer.declLocale( "lod"
						, sheenRoughness * float( MaxIblReflectionLod ) );
					auto reflection = m_writer.declLocale( "reflection"
						, normalize( reflect( -V, wsNormal ) ) );

					auto brdfSamplePoint = m_writer.declLocale( "brdfSamplePoint"
						, clamp( vec2( NdotV, sheenRoughness ), vec2( 0.0_f, 0.0_f ), vec2( 1.0_f, 1.0_f ) ) );
					auto brdf = m_writer.declLocale( "brdf"
						, brdfMap.sample( brdfSamplePoint ).z() );
					auto sheenSample = m_writer.declLocale( "sheenSample"
						, prefilteredEnvMap.lod( reflection, lod ) );

					m_writer.returnStmt( sheenSample.rgb() * sheenColour * brdf );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InVec3{ m_writer, "sheenColour" }
				, sdw::InFloat{ m_writer, "sheenRoughness" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefilteredEnvMap" }
				, sdw::InCombinedImage2DRgba32{ m_writer, "brdfMap" } );
		}
		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefilteredSheen" );
		return m_computeSheenReflections( pwsIncident
			, pwsNormal
			, components.sheenFactor
			, components.sheenRoughness
			, prefilteredEnvMap
			, pbrdfMap );
	}
}
