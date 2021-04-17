#include "Castor3D/Shader/Shaders/GlslMetallicPbrReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		MetallicPbrReflectionModel::MetallicPbrReflectionModel( ShaderWriter & writer
			, Utils & utils )
			: m_writer{ writer }
			, m_utils{ utils }
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// REFLECTIONS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_utils.declareNegateVec3Y();
			doDeclareComputeIncident();
			doDeclareComputeRefl();
			doDeclareComputeRefrEnvMap();
			doDeclareComputeRefrSkybox();
		}

		sdw::Vec3 MetallicPbrReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		sdw::Vec3 MetallicPbrReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Vec3 const & albedo
			, sdw::Float const & metalness
			, sdw::Float const & roughness )const
		{
			return m_computeRefl( wsIncident
				, wsNormal
				, envMap
				, albedo
				, metalness
				, roughness );
		}

		sdw::Void MetallicPbrReflectionModel::computeRefrEnvMap( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Float const & roughness
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const
		{
			return m_computeRefrEnvMap( wsIncident
				, wsNormal
				, envMap
				, refractionRatio
				, transmission
				, roughness
				, reflection
				, refraction );
		}

		sdw::Void MetallicPbrReflectionModel::computeRefrSkybox( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & transmission
			, sdw::Float const & roughness
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const
		{
			return m_computeRefrSkybox( wsIncident
				, wsNormal
				, envMap
				, refractionRatio
				, transmission
				, roughness
				, reflection
				, refraction );
		}

		void MetallicPbrReflectionModel::doDeclareComputeIncident()
		{
			m_computeIncident = m_writer.implementFunction< Vec3 >( "computeIncident"
				, [&]( Vec3 const & wsPosition
					, Vec3 const & wsCamera )
				{
					m_writer.returnStmt( normalize( wsPosition - wsCamera ) );
				}
				, InVec3{ m_writer, "wsPosition" }
				, InVec3{ m_writer, "wsCamera" } );
		}

		void MetallicPbrReflectionModel::doDeclareComputeRefl()
		{
			m_computeRefl = m_writer.implementFunction< Vec3 >( "computeRefl"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Vec3 const & albedo
					, Float const & metalness
					, Float const & roughness )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					auto radiance = m_writer.declLocale( "radiance"
						, envMap.lod( reflected, roughness * Float( float( Utils::MaxIblReflectionLod ) ) ).xyz() );
					m_writer.returnStmt( radiance * mix( vec3( 0.04_f ), albedo, vec3( metalness ) ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InVec3{ m_writer, "albedo" }
				, InFloat{ m_writer, "metalness" }
				, InFloat{ m_writer, "roughness" } );
		}

		void MetallicPbrReflectionModel::doDeclareComputeRefrEnvMap()
		{
			m_computeRefrEnvMap = m_writer.implementFunction< Void >( "computeRefrEnvMap"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & transmission
					, Float const & roughness
					, Vec3 reflection
					, Vec3 refraction )
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
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( envMap.lod( refracted, roughness * Float( float( Utils::MaxIblReflectionLod ) ) ).xyz() * transmission
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "transmission" }
				, InFloat{ m_writer, "roughness" }
				, InOutVec3{ m_writer, "reflection" }
				, OutVec3{ m_writer, "refraction" } );
		}

		void MetallicPbrReflectionModel::doDeclareComputeRefrSkybox()
		{
			m_computeRefrSkybox = m_writer.implementFunction< Void >( "computeRefrSkybox"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & transmission
					, Float const & roughness
					, Vec3 reflection
					, Vec3 refraction )
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
					auto refracted = m_writer.declLocale( "refracted"
						, m_utils.negateTopDownToBottomUp( refract( wsIncident, wsNormal, refractionRatio ) ) );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( envMap.sample( refracted ).xyz() * transmission
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "transmission" }
				, InFloat{ m_writer, "roughness" }
				, InOutVec3{ m_writer, "reflection" }
				, OutVec3{ m_writer, "refraction" } );
		}
	}
}