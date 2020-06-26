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

		void PhongReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & shininess
			, sdw::Vec3 & ambient
			, sdw::Vec3 & diffuse )const
		{
			m_computeRefl( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, shininess
				, ambient
				, diffuse );
		}

		void PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Float const & shininess
			, sdw::Vec3 & ambient
			, sdw::Vec3 & diffuse )const
		{
			m_computeRefr( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, refractionRatio
				, shininess
				, ambient
				, diffuse );
		}

		void PhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Float const & shininess
			, sdw::Vec3 & ambient
			, sdw::Vec3 & diffuse )const
		{
			m_computeReflRefr( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, refractionRatio
				, shininess
				, ambient
				, diffuse );
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
			m_computeRefl = m_writer.implementFunction< Void >( "computeRefl"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Float const & shininess
					, Vec3 ambient
					, Vec3 diffuse )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					ambient = occlusion * textureLod( envMap, reflected, ( 256.0f - shininess ) / 32.0f ).xyz() * diffuse;
					diffuse = vec3( 0.0_f );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "shininess" }
				, OutVec3{ m_writer, "ambient" }
				, OutVec3{ m_writer, "diffuse" } );
		}

		void PhongReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Void >( "computeRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Float const & shininess
					, Vec3 ambient
					, Vec3 diffuse )
				{
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					ambient = vec3( 0.0_f );
					diffuse = textureLod( envMap, refracted, ( 256.0f - shininess ) / 32.0f ).xyz() * ( diffuse / length( diffuse ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InFloat{ m_writer, "shininess" }
				, OutVec3{ m_writer, "ambient" }
				, OutVec3{ m_writer, "diffuse" } );
		}

		void PhongReflectionModel::doDeclareComputeReflRefr()
		{
			m_computeReflRefr = m_writer.implementFunction< Void >( "computeReflRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Float const & shininess
					, Vec3 ambient
					, Vec3 diffuse )
				{
					auto save = m_writer.declLocale( "save"
						, diffuse );
					computeRefl( wsIncident
						, wsNormal
						, occlusion
						, envMap
						, shininess
						, ambient // Reflection affects ambient
						, save );
					computeRefr( wsIncident
						, wsNormal
						, occlusion
						, envMap
						, refractionRatio
						, shininess
						, save
						, diffuse ); // Refraction affects diffuse
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
					ambient = mix( diffuse
						, ambient
						, vec3( fresnel ) );
					diffuse = vec3( 0.0_f );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InFloat{ m_writer, "shininess" }
				, OutVec3{ m_writer, "ambient" }
				, OutVec3{ m_writer, "diffuse" } );
		}
	}
}
