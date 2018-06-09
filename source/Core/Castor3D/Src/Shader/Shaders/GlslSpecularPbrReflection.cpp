#include "GlslSpecularPbrReflection.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		SpecularPbrReflectionModel::SpecularPbrReflectionModel( GlslWriter & writer )
			: m_writer{ writer }
		{
			doDeclareComputeIncident();
			doDeclareComputeRefl();
			doDeclareComputeRefr();
		}

		glsl::Vec3 SpecularPbrReflectionModel::computeIncident( glsl::Vec3 const & wsPosition
			, glsl::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		glsl::Vec3 SpecularPbrReflectionModel::computeRefl( glsl::Vec3 const & wsIncident
			, glsl::Vec3 const & wsNormal
			, glsl::Float const & occlusion
			, glsl::SamplerCube const & envMap
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

		glsl::Vec3 SpecularPbrReflectionModel::computeRefr( glsl::Vec3 const & wsIncident
			, glsl::Vec3 const & wsNormal
			, glsl::Float const & occlusion
			, glsl::SamplerCube const & envMap
			, glsl::Float const & refractionRatio
			, glsl::Vec3 const & reflection
			, glsl::Vec3 const & albedo
			, glsl::Float const & roughness )const
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
			m_computeIncident = m_writer.implementFunction< Vec3 >( cuT( "computeIncident" )
				, [&]( Vec3 const & wsPosition
					, Vec3 const & wsCamera )
				{
					m_writer.returnStmt( normalize( wsPosition - wsCamera ) );
				}
				, InVec3{ &m_writer, cuT( "wsPosition" ) }
				, InVec3{ &m_writer, cuT( "wsCamera" ) } );
		}

		void SpecularPbrReflectionModel::doDeclareComputeRefl()
		{
			m_computeRefl = m_writer.implementFunction< Vec3 >( cuT( "computeRefl" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap
					, Vec3 const & ambientLight
					, Vec3 const & diffuse )
				{
					auto reflected = m_writer.declLocale( cuT( "reflected" )
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( ambientLight.xyz()
						* occlusion
						* texture( envMap, reflected ).xyz()
						* diffuse / length( diffuse ) );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) }
				, InVec3{ &m_writer, cuT( "ambientLight" ) }
				, InVec3{ &m_writer, cuT( "diffuse" ) } );
		}

		void SpecularPbrReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Vec3 >( cuT( "computeRefr" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap
					, Float const & refractionRatio
					, Vec3 const & reflection
					, Vec3 const & diffuse
					, Float const & glossiness )
				{
					auto subRatio = m_writer.declLocale( cuT( "subRatio" )
						, 1.0_f - refractionRatio );
					auto addRatio = m_writer.declLocale( cuT( "addRatio" )
						, 1.0_f + refractionRatio );
					auto reflectance = m_writer.declLocale( cuT( "reflectance" )
						, m_writer.paren( subRatio * subRatio ) / m_writer.paren( addRatio * addRatio ) );
					auto product = m_writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
					auto fresnel = m_writer.declLocale( cuT( "fresnel" )
						, glsl::fma( max( glossiness, reflectance ) - reflectance
							, pow( 1.0_f - product, 5.0_f )
							, reflectance ) );
					auto refracted = m_writer.declLocale( cuT( "refracted" )
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( mix( texture( envMap, refracted ).xyz() * diffuse / length( diffuse )
						, reflection
						, fresnel ) );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) }
				, InFloat{ &m_writer, cuT( "refractionRatio" ) }
				, InVec3{ &m_writer, cuT( "reflection" ) }
				, InVec3{ &m_writer, cuT( "diffuse" ) }
				, InFloat{ &m_writer, cuT( "glossiness" ) } );
		}
	}
}
