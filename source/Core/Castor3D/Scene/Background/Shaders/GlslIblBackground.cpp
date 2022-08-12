#include "Castor3D/Scene/Background/Shaders/GlslIblBackground.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
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
		, LightMaterial const & pmaterial
		, sdw::CombinedImage2DRg32 const & pbrdfMap )
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
					, sdw::CombinedImage2DRg32 const & brdfMap )
				{
					auto V = m_writer.declLocale( "V"
						, -wsIncident );
					auto NdotV = m_writer.declLocale( "NdotV"
						, max( dot( wsNormal, V ), 0.0_f ) );
					auto F = m_writer.declLocale( "F"
						, m_utils.fresnelSchlick( NdotV, specular ) );
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
				, sdw::InCombinedImage2DRg32{ m_writer, "brdfMap" } );
		}
		auto irradianceMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapIrradiance" );
		auto prefilteredEnvMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_computeReflections( pwsIncident
			, pwsNormal
			, pmaterial.albedo
			, pmaterial.specular
			, pmaterial.getMetalness()
			, pmaterial.getRoughness()
			, irradianceMap
			, prefilteredEnvMap
			, pbrdfMap );
	}

	sdw::RetVec3 IblBackgroundModel::computeRefractions( sdw::Vec3 const & wsIncident
		, sdw::Vec3 const & wsNormal
		, sdw::Float const & refractionRatio
		, sdw::Vec3 const & transmission
		, LightMaterial const & material )
	{
		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return doComputeRefractions( wsIncident
			, wsNormal
			, prefiltered
			, refractionRatio
			, transmission
			, material.albedo );
	}

	sdw::RetVoid IblBackgroundModel::mergeReflRefr( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, LightMaterial const & pmaterial
		, sdw::Vec3 & preflection
		, sdw::Vec3 & prefraction )
	{
		if ( !m_mergeReflRefr )
		{
			m_mergeReflRefr = m_writer.implementFunction< sdw::Void >( "c3d_iblbg_mergeReflRefr"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & transmission
					, sdw::Vec3 const & albedo
					, sdw::Float const & roughness
					, sdw::Vec3 reflection
					, sdw::Vec3 refraction )
				{
					auto subRatio = m_writer.declLocale( "subRatio"
						, 1.0_f - refractionRatio );
					auto addRatio = m_writer.declLocale( "addRatio"
						, 1.0_f + refractionRatio );
					auto reflectance = m_writer.declLocale( "reflectance"
						, ( subRatio * subRatio ) / ( addRatio * addRatio ) );
					auto product = m_writer.declLocale( "product"
						, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
					auto fresnel = m_writer.declLocale( "fresnel"
						, sdw::fma( max( 1.0_f - roughness, reflectance ) - reflectance
							, pow( 1.0_f - product, 5.0_f )
							, reflectance ) );
					refraction = doComputeRefractions( wsIncident
						, wsNormal
						, prefiltered
						, refractionRatio
						, transmission
						, albedo );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( refraction
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "transmission" }
				, sdw::InVec3{ m_writer, "albedo" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InOutVec3{ m_writer, "reflection" }
				, sdw::OutVec3{ m_writer, "refraction" } );
		}

		auto prefiltered = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapPrefiltered" );
		return m_mergeReflRefr( pwsIncident
			, pwsNormal
			, prefiltered
			, prefractionRatio
			, ptransmission
			, pmaterial.albedo
			, pmaterial.getRoughness()
			, preflection
			, prefraction );
	}

	sdw::RetVec3 IblBackgroundModel::doComputeRefractions( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::CombinedImageCubeRgba32 const & pprefiltered
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, sdw::Vec3 const & palbedo )
	{
		if ( !m_computeRefractions )
		{
			m_computeRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_iblbg_computeRefractions"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & prefiltered
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & transmission
					, sdw::Vec3 const & albedo )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, m_utils.negateTopDownToBottomUp( refract( wsIncident, wsNormal, refractionRatio ) ) );
					m_writer.returnStmt( prefiltered.sample( refracted ).xyz()
						* transmission
						* albedo );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "prefiltered" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "transmission" }
				, sdw::InVec3{ m_writer, "albedo" } );
		}

		return m_computeRefractions( pwsIncident
			, pwsNormal
			, pprefiltered
			, prefractionRatio
			, ptransmission
			, palbedo );
	}
}
