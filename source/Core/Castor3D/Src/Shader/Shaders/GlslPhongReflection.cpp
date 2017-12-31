#include "GlslPhongReflection.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		PhongReflectionModel::PhongReflectionModel( GlslWriter & writer )
			: m_writer{ writer }
		{
			doDeclareComputeIncident();
			doDeclareComputeRefl();
			doDeclareComputeRefr();
			doDeclareComputeReflRefr();
		}

		glsl::Vec3 PhongReflectionModel::computeIncident( glsl::Vec3 const & wsPosition
			, glsl::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		glsl::Vec3 PhongReflectionModel::computeRefl( glsl::Vec3 const & wsIncident
			, glsl::Vec3 const & wsNormal
			, glsl::Float const & occlusion
			, glsl::SamplerCube const & envMap )const
		{
			return m_computeRefl( wsIncident
				, wsNormal
				, occlusion
				, envMap );
		}

		glsl::Vec3 PhongReflectionModel::computeRefr( glsl::Vec3 const & wsIncident
			, glsl::Vec3 const & wsNormal
			, glsl::Float const & occlusion
			, glsl::SamplerCube const & envMap
			, glsl::Float const & refractionRatio
			, glsl::Vec3 const & diffuse )const
		{
			return m_computeRefr( wsIncident
				, wsNormal
				, occlusion
				, envMap
				, refractionRatio
				, diffuse );
		}

		glsl::Vec3 PhongReflectionModel::computeReflRefr( glsl::Vec3 const & wsIncident
			, glsl::Vec3 const & wsNormal
			, glsl::Float const & occlusion
			, glsl::SamplerCube const & envMap
			, glsl::Float const & refractionRatio
			, glsl::Vec3 const & diffuse )const
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
			m_computeIncident = m_writer.implementFunction< Vec3 >( cuT( "computeIncident" )
				, [&]( Vec3 const & wsPosition
					, Vec3 const & wsCamera )
				{
					m_writer.returnStmt( normalize( wsPosition - wsCamera ) );
				}
				, InVec3{ &m_writer, cuT( "wsPosition" ) }
				, InVec3{ &m_writer, cuT( "wsCamera" ) } );
		}

		void PhongReflectionModel::doDeclareComputeRefl()
		{
			m_computeRefl = m_writer.implementFunction< Vec3 >( cuT( "computeRefl" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap )
				{
					auto reflected = m_writer.declLocale( cuT( "reflected" )
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( occlusion * texture( envMap, reflected ).xyz() );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) } );
		}

		void PhongReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Vec3 >( cuT( "computeRefr" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap
					, Float const & refractionRatio
					, Vec3 const & diffuse )
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
						, glsl::fma( 1.0_f - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = m_writer.declLocale( cuT( "refracted" )
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( texture( envMap, refracted ).xyz() * diffuse / length( diffuse ) );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) }
				, InFloat{ &m_writer, cuT( "refractionRatio" ) }
				, InVec3{ &m_writer, cuT( "diffuse" ) } );
		}

		void PhongReflectionModel::doDeclareComputeReflRefr()
		{
			m_computeReflRefr = m_writer.implementFunction< Vec3 >( cuT( "computeReflRefr" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap
					, Float const & refractionRatio
					, Vec3 const & diffuse )
				{
					auto reflection = m_writer.declLocale( cuT( "reflection" )
						, computeRefl( wsIncident, wsNormal, occlusion, envMap ) );
					auto subRatio = m_writer.declLocale( cuT( "subRatio" )
						, 1.0_f - refractionRatio );
					auto addRatio = m_writer.declLocale( cuT( "addRatio" )
						, 1.0_f + refractionRatio );
					auto reflectance = m_writer.declLocale( cuT( "reflectance" )
						, m_writer.paren( subRatio * subRatio ) / m_writer.paren( addRatio * addRatio ) );
					auto product = m_writer.declLocale( cuT( "product" )
						, max( 0.0_f, dot( -wsIncident, wsNormal ) ) );
					auto fresnel = m_writer.declLocale( cuT( "fresnel" )
						, glsl::fma( 1.0_f - reflectance, pow( 1.0_f - product, 5.0_f ), reflectance ) );
					auto refracted = m_writer.declLocale( cuT( "refracted" )
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( mix( texture( envMap, refracted ).xyz() * diffuse / length( diffuse )
						, reflection * reflection / length( reflection )
						, fresnel ) );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) }
				, InFloat{ &m_writer, cuT( "refractionRatio" ) }
				, InVec3{ &m_writer, cuT( "diffuse" ) } );
		}
	}
}
