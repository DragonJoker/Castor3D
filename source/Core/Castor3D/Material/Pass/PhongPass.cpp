#include "Castor3D/Material/Pass/PhongPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPhongLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslPhongBRDF.hpp"

namespace c3d
{
	String const PhongPass::Type = cuT( "blinn_phong" );
	String const PhongPass::LightingModel = shader::PhongLightingModel::getName();

	shader::DiffuseBrdfDesc const PhongPass::DefaultDiffuseBrdf = { String{ shader::PhongDiffuseBRDF::Name }, shader::PhongDiffuseBRDF::create };
	shader::DiffuseBrdfArray const PhongPass::DiffuseBrdfs{ DefaultDiffuseBrdf };

	shader::SpecularBrdfDesc const PhongPass::DefaultSpecularBrdf = { String{ shader::PhongSpecularBRDF::Name }, shader::PhongSpecularBRDF::create };
	shader::SpecularBrdfArray const PhongPass::SpecularBrdfs{ DefaultSpecularBrdf };

	shader::SheenBrdfDesc const PhongPass::DefaultSheenBrdf = { String{ cuT( "none" ) }, []( sdw::ShaderWriter const &, shader::BRDFHelpers const & ){ return nullptr; } };
	shader::SheenBrdfArray const PhongPass::SheenBrdfs{ DefaultSheenBrdf };

	shader::ClearcoatBrdfDesc const PhongPass::DefaultClearcoatBrdf = { String{ shader::PhongClearcoatBRDF::Name }, shader::PhongClearcoatBRDF::create };
	shader::ClearcoatBrdfArray const PhongPass::ClearcoatBrdfs{ DefaultClearcoatBrdf };

	shader::ScatteringModelDesc const PhongPass::DefaultScatteringModel = { String{ shader::ScatteringModel::Name }, shader::ScatteringModel::create };
	shader::ScatteringModelArray const PhongPass::ScatteringModels{ DefaultScatteringModel };

	PassUPtr PhongPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = makeUnique< Pass >( parent
			, lightingModelId );

		result->createComponent< RoughnessComponent >()->setShininess( 50.0f );

		return result;
	}
}
