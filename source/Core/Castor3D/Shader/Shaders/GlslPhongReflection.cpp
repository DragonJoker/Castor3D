#include "Castor3D/Shader/Shaders/GlslPhongReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		PhongReflectionModel::PhongReflectionModel( ShaderWriter & writer )
			: m_writer{ writer }
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// REFLECTIONS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
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
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap )const
		{
			return m_computeRefl( wsIncident
				, wsNormal
				, occlusion
				, envMap );
		}

		sdw::Vec3 PhongReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & diffuse )const
		{
			return m_computeRefr( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, refractionRatio
				, diffuse );
		}

		sdw::Vec3 PhongReflectionModel::computeReflRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & diffuse )const
		{
			return m_computeReflRefr( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, refractionRatio
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
			m_computeRefl = m_writer.implementFunction< Vec3 >( "computeRefl"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( occlusion * texture( envMap, reflected ).xyz() );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" } );
		}

		void PhongReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Vec3 >( "computeRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & diffuse )
				{
					auto subRatio = m_writer.declLocale( "subRatio"
						, 1.0_f - refractionRatio );
					auto addRatio = m_writer.declLocale( "addRatio"
						, 1.0_f + refractionRatio );
					auto reflectance = m_writer.declLocale( "reflectance"
						, m_writer.paren( subRatio * subRatio ) / m_writer.paren( addRatio * addRatio ) );
					auto product = m_writer.declLocale( "product"
						, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
					auto fresnel = m_writer.declLocale( "fresnel"
						, sdw::fma( 1.0_f - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( texture( envMap, refracted ).xyz() * diffuse / length( diffuse ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "diffuse" } );
		}

		void PhongReflectionModel::doDeclareComputeReflRefr()
		{
			m_computeReflRefr = m_writer.implementFunction< Vec3 >( "computeReflRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & diffuse )
				{
					auto reflection = m_writer.declLocale( "reflection"
						, computeRefl( wsIncident, wsNormal, occlusion, envMap ) );
					auto subRatio = m_writer.declLocale( "subRatio"
						, 1.0_f - refractionRatio );
					auto addRatio = m_writer.declLocale( "addRatio"
						, 1.0_f + refractionRatio );
					auto reflectance = m_writer.declLocale( "reflectance"
						, m_writer.paren( subRatio * subRatio ) / m_writer.paren( addRatio * addRatio ) );
					auto product = m_writer.declLocale( "product"
						, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
					auto fresnel = m_writer.declLocale( "fresnel"
						, sdw::fma( 1.0_f - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( mix( texture( envMap, refracted ).xyz() * diffuse / length( diffuse )
						, reflection * reflection / length( reflection )
						, vec3( fresnel ) ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "diffuse" } );
		}
	}
}
