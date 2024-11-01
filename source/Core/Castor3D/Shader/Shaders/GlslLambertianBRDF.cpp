#include "Castor3D/Shader/Shaders/GlslLambertianBRDF.hpp"

#include "Castor3D/Shader/Shaders/GlslBlendComponents.hpp"
#include "Castor3D/Shader/Shaders/GlslLightSurface.hpp"

namespace castor3d::shader
{
	LambertianBRDF::LambertianBRDF( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
		: DiffuseBRDF{ writer, brdfHelpers }
	{
	}

	void LambertianBRDF::doGenerate( BlendComponents const & pcomponents
		, LightSurface const & plightSurface )
	{
		m_compute = m_writer.implementFunction< sdw::Vec3 >( "c3d_computeLambertian"
			, [this]( BlendComponents const & components
				, LightSurface const & lightSurface
				, sdw::Vec3 const & radiance
				, sdw::Float const & intensity
				, sdw::Float const & NdotL )
			{
				auto diffuseFactor = m_writer.declLocale( "diffuseFactor"
					, vec3( 1.0_f ) - lightSurface.difF().value() );
				auto diffuseReflectance = m_writer.declLocale( "diffuseReflectance"
					, radiance / sdw::Float{ castor::Pi< float > } );
				m_writer.returnStmt( max( diffuseReflectance * intensity * diffuseFactor, vec3( 0.0_f ) ) );
			}
			, InBlendComponents{ m_writer, "components", pcomponents }
			, InLightSurface{ m_writer, "lightSurface", plightSurface }
			, sdw::InVec3{ m_writer, "radiance" }
			, sdw::InFloat{ m_writer, "intensity" }
			, sdw::InFloat{ m_writer, "NdotL" } );
	}

	DiffuseBRDFUPtr LambertianBRDF::create( sdw::ShaderWriter & writer
		, BRDFHelpers & brdfHelpers )
	{
		return castor::makeUniqueDerived< DiffuseBRDF, LambertianBRDF >( writer, brdfHelpers );
	}
}
