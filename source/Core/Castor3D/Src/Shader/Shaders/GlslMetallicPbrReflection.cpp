#include "GlslMetallicPbrReflection.hpp"

#include "GlslMaterial.hpp"
#include "GlslShadow.hpp"
#include "GlslLight.hpp"

using namespace castor;
using namespace glsl;

namespace castor3d
{
	namespace shader
	{
		MetallicPbrReflectionModel::MetallicPbrReflectionModel( GlslWriter & writer )
			: m_writer{ writer }
		{
			doDeclareComputeIncident();
			doDeclareComputeRefl();
			doDeclareComputeRefr();
		}

		glsl::Vec3 MetallicPbrReflectionModel::computeIncident( glsl::Vec3 const & wsPosition
			, glsl::Vec3 const & wsCamera )const
		{
			return m_computeIncident( wsPosition
				, wsCamera );
		}

		glsl::Vec3 MetallicPbrReflectionModel::computeRefl( glsl::Vec3 const & wsIncident
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

		glsl::Vec3 MetallicPbrReflectionModel::computeRefr( glsl::Vec3 const & wsIncident
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

		void MetallicPbrReflectionModel::doDeclareComputeIncident()
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

		void MetallicPbrReflectionModel::doDeclareComputeRefl()
		{
			m_computeRefl = m_writer.implementFunction< Vec3 >( cuT( "computeRefl" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap
					, Vec3 const & ambientLight
					, Vec3 const & albedo )
				{
					auto reflected = m_writer.declLocale( cuT( "reflected" )
						, reflect( wsIncident, wsNormal ) );
					m_writer.returnStmt( ambientLight.xyz()
						* occlusion
						* texture( envMap, reflected ).xyz()
						* albedo / length( albedo ) );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) }
				, InVec3{ &m_writer, cuT( "ambientLight" ) }
				, InVec3{ &m_writer, cuT( "albedo" ) } );
		}

		void MetallicPbrReflectionModel::doDeclareComputeRefr()
		{
			m_computeRefr = m_writer.implementFunction< Vec3 >( cuT( "computeRefr" )
				, [&]( Vec3 const & wsIncident
					, Vec3 const & wsNormal
					, Float const & occlusion
					, SamplerCube const & envMap
					, Float const & refractionRatio
					, Vec3 const & reflection
					, Vec3 const & albedo
					, Float const & roughness )
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
						, glsl::fma( max( 1.0_f - roughness, reflectance ) - reflectance
							, pow( 1.0_f - product, 5.0_f )
							, reflectance ) );
					auto refracted = m_writer.declLocale( cuT( "refracted" )
						, refract( wsIncident, wsNormal, refractionRatio ) );
					m_writer.returnStmt( mix( texture( envMap, refracted ).xyz() * albedo / length( albedo )
						, reflection
						, fresnel ) );
				}
				, InVec3{ &m_writer, cuT( "wsIncident" ) }
				, InVec3{ &m_writer, cuT( "wsNormal" ) }
				, InFloat{ &m_writer, cuT( "occlusion" ) }
				, InSamplerCube{ &m_writer, cuT( "envMap" ) }
				, InFloat{ &m_writer, cuT( "refractionRatio" ) }
				, InVec3{ &m_writer, cuT( "reflection" ) }
				, InVec3{ &m_writer, cuT( "albedo" ) }
				, InFloat{ &m_writer, cuT( "roughness" ) } );
		}
	}
}
