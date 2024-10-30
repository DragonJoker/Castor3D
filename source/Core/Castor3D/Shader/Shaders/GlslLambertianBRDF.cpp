#include "Castor3D/Shader/Shaders/GlslLambertianBRDF.hpp"

namespace castor3d::shader
{
	LambertianBRDF::LambertianBRDF( sdw::ShaderWriter & writer )
		: DiffuseBRDF{ writer }
	{
	}

	sdw::RetVec3 LambertianBRDF::compute( sdw::Vec3 const & pradiance
		, sdw::Float const & pintensity
		, sdw::Float const & pNdotL
		, sdw::Float const & pNdotV
		, sdw::Float const & pLdotV
		, sdw::Vec3 const & pF
		, sdw::Float const & proughness )
	{
		if ( !m_compute )
		{
			m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeLambertian"
				, [this]( sdw::Vec3 const & radiance
					, sdw::Float const & intensity
					, sdw::Float const & NdotL
					, sdw::Float const & NdotV
					, sdw::Float const & LdotV
					, sdw::Vec3 const & F
					, sdw::Float const & roughness )
				{
					auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
						, vec3( 1.0_f ) - F );
					auto diffuseReflectance = m_writer.declLocale( "diffuseReflectance"
						, radiance / sdw::Float{ castor::Pi< float > } );
					m_writer.returnStmt( max( diffuseReflectance * intensity * diffuseFactor, vec3( 0.0_f ) ) );
				}
				, sdw::InVec3( m_writer, "radiance" )
				, sdw::InFloat( m_writer, "intensity" )
				, sdw::InFloat( m_writer, "NdotL" )
				, sdw::InFloat( m_writer, "NdotH" )
				, sdw::InFloat( m_writer, "NdotV" )
				, sdw::InVec3{ m_writer, "F" }
				, sdw::InFloat{ m_writer, "roughness" } );
		}

		return m_compute( pradiance
			, pintensity
			, pNdotL
			, pNdotV
			, pLdotV
			, pF
			, proughness );
	}
}
