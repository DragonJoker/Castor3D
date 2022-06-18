#include "Castor3D/Scene/Background/Shaders/GlslNoIblBackground.hpp"

#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Shader/Shaders/GlslLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d::shader
{
	castor::String const NoIblBackgroundModel::Name = cuT( "c3d.no_ibl" );

	NoIblBackgroundModel::NoIblBackgroundModel( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & binding
		, uint32_t set )
		: BackgroundModel{ writer, utils }
	{
		m_writer.declCombinedImg< FImgCubeRgba32 >( "c3d_mapBackground"
			, binding++
			, set );
	}

	BackgroundModelPtr NoIblBackgroundModel::create( sdw::ShaderWriter & writer
		, Utils & utils
		, uint32_t & binding
		, uint32_t set )
	{
		return std::make_unique< NoIblBackgroundModel >( writer
			, utils
			, binding
			, set );
	}

	sdw::Vec3 NoIblBackgroundModel::computeReflections( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, LightMaterial const & pmaterial
		, sdw::CombinedImage2DRg32 const & pbrdf )
	{
		if ( !m_computeReflections )
		{
			m_computeReflections = m_writer.implementFunction< sdw::Vec3 >( "c3d_noiblbg_computeReflections"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & backgroundMap
					, sdw::Vec3 const & specular
					, sdw::Float const & roughness )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( backgroundMap.lod( reflected, roughness * 8.0_f ).xyz() * specular );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "backgroundMap" }
				, sdw::InVec3{ m_writer, "specular" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		auto backgroundMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapBackground" );
		return m_computeReflections( pwsIncident
			, pwsNormal
			, backgroundMap
			, pmaterial.specular
			, pmaterial.getRoughness() );
	}

	sdw::Vec3 NoIblBackgroundModel::computeRefractions( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, LightMaterial const & pmaterial )
	{
		auto backgroundMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapBackground" );
		return doComputeRefractions( pwsIncident
			, pwsNormal
			, backgroundMap
			, prefractionRatio
			, ptransmission
			, pmaterial.albedo
			, pmaterial.getRoughness() );
	}

	sdw::Void NoIblBackgroundModel::mergeReflRefr( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, LightMaterial const & pmaterial
		, sdw::Vec3 & preflection
		, sdw::Vec3 & prefraction )
	{
		if ( !m_mergeReflRefr )
		{
			m_mergeReflRefr = m_writer.implementFunction< sdw::Void >( "c3d_noiblbg_mergeReflRefr"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & envMap
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & transmission
					, sdw::Vec3 albedo
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
						, sdw::fma( pow( 1.0_f - product, 5.0_f )
							, 1.0_f - reflectance
							, reflectance ) );
					refraction = doComputeRefractions( wsIncident
						, wsNormal
						, envMap
						, refractionRatio
						, transmission
						, albedo
						, roughness );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( refraction
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "envMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "transmission" }
				, sdw::InOutVec3{ m_writer, "albedo" }
				, sdw::InFloat{ m_writer, "roughness" }
				, sdw::InOutVec3{ m_writer, "reflection" }
				, sdw::OutVec3{ m_writer, "refraction" } );
		}

		auto backgroundMap = m_writer.getVariable< sdw::CombinedImageCubeRgba32 >( "c3d_mapBackground" );
		return m_mergeReflRefr( pwsIncident
			, pwsNormal
			, backgroundMap
			, prefractionRatio
			, ptransmission
			, pmaterial.albedo
			, pmaterial.getRoughness()
			, preflection
			, prefraction );
	}

	sdw::Vec4 NoIblBackgroundModel::scatter( sdw::Vec2 const & fragPos
		, sdw::Vec2 const & fragSize
		, sdw::Float const & fragDepth )
	{
		return vec4( 0.0_f );
	}

	sdw::Vec3 NoIblBackgroundModel::doComputeRefractions( sdw::Vec3 const & pwsIncident
		, sdw::Vec3 const & pwsNormal
		, sdw::CombinedImageCubeRgba32 const & pbackgroundMap
		, sdw::Float const & prefractionRatio
		, sdw::Vec3 const & ptransmission
		, sdw::Vec3 const & palbedo
		, sdw::Float const & proughness )
	{
		if ( !m_computeRefractions )
		{
			m_computeRefractions = m_writer.implementFunction< sdw::Vec3 >( "c3d_noiblbg_computeRefractions"
				, [&]( sdw::Vec3 const & wsIncident
					, sdw::Vec3 const & wsNormal
					, sdw::CombinedImageCubeRgba32 const & backgroundMap
					, sdw::Float const & refractionRatio
					, sdw::Vec3 const & transmission
					, sdw::Vec3 albedo
					, sdw::Float const & roughness )
				{
					auto alb = m_writer.declLocale( "alb"
						, albedo );
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					albedo = vec3( 0.0_f );
					m_writer.returnStmt( backgroundMap.lod( refracted, roughness * 8.0_f ).xyz()
						* transmission
						* alb );
				}
				, sdw::InVec3{ m_writer, "wsIncident" }
				, sdw::InVec3{ m_writer, "wsNormal" }
				, sdw::InCombinedImageCubeRgba32{ m_writer, "backgroundMap" }
				, sdw::InFloat{ m_writer, "refractionRatio" }
				, sdw::InVec3{ m_writer, "transmission" }
				, sdw::InOutVec3{ m_writer, "albedo" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_computeRefractions( pwsIncident
			, pwsNormal
			, pbackgroundMap
			, prefractionRatio
			, ptransmission
			, palbedo
			, proughness );
	}
}
