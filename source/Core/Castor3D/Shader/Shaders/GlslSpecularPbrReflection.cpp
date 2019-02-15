#include "Castor3D/Shader/Shaders/GlslSpecularPbrReflection.hpp"

#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Shaders/GlslShadow.hpp"
#include "Castor3D/Shader/Shaders/GlslLight.hpp"

using namespace castor;
using namespace sdw;

namespace castor3d
{
	namespace shader
	{
		SpecularPbrReflectionModel::SpecularPbrReflectionModel( ShaderWriter & m_writer )
			: m_writer{ m_writer }
		{
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			m_writer.inlineComment( "// REFLECTIONS" );
			m_writer.inlineComment( "//////////////////////////////////////////////////////////////////////////////" );
			doDeclareComputeIncident();
			doDeclareComputeRefl();
			doDeclareComputeRefr();
		}

		sdw::Vec3 SpecularPbrReflectionModel::computeIncident( sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		sdw::Vec3 SpecularPbrReflectionModel::computeRefl( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, Vec3 const & ambientLight
			, Vec3 const & albedo )const
		{
			return m_computeRefl( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, ambientLight
				, albedo );
		}

		sdw::Vec3 SpecularPbrReflectionModel::computeRefr( sdw::Vec3 const & wsIncident
			, sdw::Vec3 const & wsNormal
			, sdw::Float const & occlusion
			, sdw::SampledImageCubeRgba32 const & envMap
			, sdw::Float const & refractionRatio
			, sdw::Vec3 const & reflection
			, sdw::Vec3 const & albedo
			, sdw::Float const & roughness )const
		{
			return m_computeRefr( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, refractionRatio
				, reflection
				, albedo
				, roughness );
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
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Vec3 const & ambientLight
					, Vec3 const & diffuse )
				{
					auto reflected = m_writer.declLocale( "reflected"
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( ambientLight.xyz()
						* occlusion
						* texture( envMap, reflected ).xyz()
						* diffuse / length( diffuse ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InVec3{ m_writer, "ambientLight" }
				, InVec3{ m_writer, "diffuse" } );
		}

		void SpecularPbrReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Vec3 >( "computeRefr"
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SampledImageCubeRgba32 const & envMap
					, Float const & refractionRatio
					, Vec3 const & reflection
					, Vec3 const & diffuse
					, Float const & glossiness )
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
						, sdw::fma( max( glossiness, reflectance ) - reflectance
							, pow( 1.0_f - product, 5.0_f )
							, reflectance ) );
					auto refracted = m_writer.declLocale( "refracted"
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( mix( texture( envMap, refracted ).xyz() * diffuse / length( diffuse )
						, reflection
						, vec3( fresnel ) ) );
				}
				, InVec3{ m_writer, "wsIncident" }
				, InVec3{ m_writer, "wsNormal" }
				, InFloat{ m_writer, "occlusion" }
				, InSampledImageCubeRgba32{ m_writer, "envMap" }
				, InFloat{ m_writer, "refractionRatio" }
				, InVec3{ m_writer, "reflection" }
				, InVec3{ m_writer, "diffuse" }
				, InFloat{ m_writer, "glossiness" } );
		}
	}
}
