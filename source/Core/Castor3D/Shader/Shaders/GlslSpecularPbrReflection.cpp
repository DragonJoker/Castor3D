#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		SpecularPbrReflectionModel::SpecularPbrReflectionModel( ShaderWriter & m_writer
			, Utils & utils )
			: m_writer{ m_writer }
			, m_utils{ utils }
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// REFLECTIONS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			utils.declareNegateVec3Y();
			doDeclareComputeIncident();
			doDeclareComputeRefl();
			doDeclareComputeRefrEnvMap();
			doDeclareComputeRefrSkybox();
		}

		sdw::Vec3 SpecularPbrReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		sdw::Vec3 SpecularPbrReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Vec3 const & specular
			, sdw::Float const & glossiness )const
		{
			return m_computeRefl( wsIncident
				, wsNormal
				, envMap
				, specular
				, glossiness );
		}

		sdw::Void SpecularPbrReflectionModel::computeRefrEnvMap( sdw::Vec3 const & wsIncident
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

		sdw::Void SpecularPbrReflectionModel::computeRefrSkybox( sdw::Vec3 const & wsIncident
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

		void SpecularPbrReflectionModel::doDeclareComputeIncident()
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

		void SpecularPbrReflectionModel::doDeclareComputeRefl()
		{
			m_computeRefl = m_writer.implementFunction< Vec3 >( "computeRefl"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Vec3 const & specular
					, Float const & glossiness )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					auto radiance = m_writer.declLocale( "radiance"
						, envMap.lod( reflected, ( 1.0_f - glossiness ) * Float( float( Utils::MaxIblReflectionLod ) ) ).xyz() );
					m_writer.returnStmt( radiance * specular );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InVec3{ m_writer, "specular" }
				, InFloat{ m_writer, "glossiness" } );
		}

		void SpecularPbrReflectionModel::doDeclareComputeRefrEnvMap()
		{
			m_computeRefrEnvMap = m_writer.implementFunction< Void >( "m_computeRefrEnvMap"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & transmission
					, Float const & glossiness
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
						, sdw::fma( max( glossiness, reflectance ) - reflectance
							, pow( 1.0_f - product, 5.0_f )
							, reflectance ) );
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( envMap.lod( refracted, ( 1.0_f - glossiness ) * Float( float( Utils::MaxIblReflectionLod ) ) ).xyz() * transmission
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "transmission" }
				, InFloat{ m_writer, "glossiness" }
				, InOutVec3{ m_writer, "reflection" }
				, OutVec3{ m_writer, "refraction" } );
		}

		void SpecularPbrReflectionModel::doDeclareComputeRefrSkybox()
		{
			m_computeRefrSkybox = m_writer.implementFunction< Void >( "m_computeRefrSkybox"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & transmission
					, Float const & glossiness
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
						, sdw::fma( max( glossiness, reflectance ) - reflectance
							, pow( 1.0_f - product, 5.0_f )
							, reflectance ) );
					auto refracted = m_writer.declLocale( "refracted"
						, m_utils.negateTopDownToBottomUp( refract( wsIncident, wsNormal, refractionRatio ) ) );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( envMap.lod( refracted, ( 1.0_f - glossiness ) * Float( float( Utils::MaxIblReflectionLod ) ) ).xyz() * transmission
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "transmission" }
				, InFloat{ m_writer, "glossiness" }
				, InOutVec3{ m_writer, "reflection" }
				, OutVec3{ m_writer, "refraction" } );
		}
	}
}
