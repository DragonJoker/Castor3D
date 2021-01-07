#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"
#include "Castor3D/Shader/Shaders/GlslUtils.hpp"

#include <ShaderWriter/Source.hpp>

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		PhongReflectionModel::PhongReflectionModel( ShaderWriter & writer
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
			doDeclareComputeRefr();
			doDeclareComputeReflRefr();
		}

		sdw::Vec3 PhongReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		sdw::Vec3 PhongReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & shininess
			, sdw::Vec3 const & diffuse )const
		{
			return m_computeRefl( wsIncident
				, wsNormal
				, envMap
				, shininess
				, diffuse );
		}

		void PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & diffuse
			, sdw::Float const & shininess
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const
		{
			m_computeRefr( wsIncident
				, wsNormal
				, envMap
				, refractionRatio
				, diffuse
				, shininess
				, reflection
				, refraction );
		}

		void PhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & specular
			, sdw::Vec3 const & transmission
			, sdw::Float const & shininess
			, sdw::Vec3 & reflection
			, sdw::Vec3 & refraction )const
		{
			m_computeReflRefr( wsIncident
				, wsNormal
				, envMap
				, refractionRatio
				, specular
				, transmission
				, shininess
				, reflection
				, refraction );
		}

		void PhongReflectionModel::doDeclareComputeIncident()
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

		void PhongReflectionModel::doDeclareComputeRefl()
		{
			m_computeRefl = m_writer.implementFunction< Vec3 >( "computeRefl"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & shininess
					, Vec3 const & specular )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( envMap.lod( reflected, ( 256.0f - shininess ) / 32.0f ).xyz()
						* specular );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "shininess" }
				, InVec3{ m_writer, "diffuse" } );
		}

		void PhongReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Void >( "computeRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & transmission
					, Float const & shininess
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
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					reflection = mix( vec3( 0.0_f )
						, reflection
						, vec3( fresnel ) );
					refraction = mix( envMap.lod( refracted, ( 256.0f - shininess ) / 32.0f ).xyz() * transmission
						, vec3( 0.0_f )
						, vec3( fresnel ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "transmission" }
				, InFloat{ m_writer, "shininess" }
				, InOutVec3{ m_writer, "reflection" }
				, OutVec3{ m_writer, "refraction" } );
		}

		void PhongReflectionModel::doDeclareComputeReflRefr()
		{
			m_computeReflRefr = m_writer.implementFunction< Void >( "computeReflRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & specular
					, Vec3 const & transmission
					, Float const & shininess
					, sdw::Vec3 reflection
					, sdw::Vec3 refraction )
				{
					reflection = computeRefl( wsIncident
						, wsNormal
						, envMap
						, shininess
						, specular );
					computeRefr( wsIncident
						, wsNormal
						, envMap
						, refractionRatio
						, transmission
						, shininess
						, reflection
						, refraction );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "specular" }
				, InVec3{ m_writer, "transmission" }
				, InFloat{ m_writer, "shininess" }
				, OutVec3{ m_writer, "reflection" }
				, OutVec3{ m_writer, "refraction" } );
		}
	}
}
