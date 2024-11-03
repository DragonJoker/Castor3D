#include "Castor3D/Material/Pass/PhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongBRDF.hpp"

namespace castor3d
{
	castor::StringView const PhongPass::Type = cuT( "blinn_phong" );
	castor::StringView const PhongPass::LightingModel = shader::PhongLightingModel::getName();

	shader::DiffuseBrdfDesc const PhongPass::DefaultDiffuseBrdf = { castor::String{ shader::PhongDiffuseBRDF::Name }, shader::PhongDiffuseBRDF::create };
	shader::DiffuseBrdfArray const PhongPass::DiffuseBrdfs{ DefaultDiffuseBrdf };

	shader::SpecularBrdfDesc const PhongPass::DefaultSpecularBrdf = { castor::String{ shader::PhongSpecularBRDF::Name }, shader::PhongSpecularBRDF::create };
	shader::SpecularBrdfArray const PhongPass::SpecularBrdfs{ DefaultSpecularBrdf };

	shader::SheenBrdfDesc const PhongPass::DefaultSheenBrdf = { castor::String{ cuT( "none" ) }, []( sdw::ShaderWriter &, shader::BRDFHelpers & ){ return nullptr; } };
	shader::SheenBrdfArray const PhongPass::SheenBrdfs{ DefaultSheenBrdf };

	shader::ClearcoatBrdfDesc const PhongPass::DefaultClearcoatBrdf = { castor::String{ shader::PhongClearcoatBRDF::Name }, shader::PhongClearcoatBRDF::create };
	shader::ClearcoatBrdfArray const PhongPass::ClearcoatBrdfs{ DefaultClearcoatBrdf };

	PassUPtr PhongPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = castor::makeUnique< Pass >( parent
			, lightingModelId );

		result->createComponent< RoughnessComponent >()->setShininess( 50.0f );

		return result;
	}
}
