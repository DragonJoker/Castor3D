#include "Castor3D/Material/Pass/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLambertianBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslOrenNayarBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSpecularBRDF.hpp"

namespace c3d
{
	String const PbrPass::Type = cuT( "pbr" );
	String const PbrPass::LightingModel = shader::PbrLightingModel::getName();

	shader::DiffuseBrdfDesc const PbrPass::DefaultDiffuseBrdf = { String{ shader::EnergyConservativeOrenNayarBRDF::Name }, shader::EnergyConservativeOrenNayarBRDF::create };
	shader::DiffuseBrdfArray const PbrPass::DiffuseBrdfs{ { String{ shader::LambertianBRDF::Name }, shader::LambertianBRDF::create }
		, { String{ shader::QualitativeOrenNayarBRDF::Name }, shader::QualitativeOrenNayarBRDF::create }
		, { String{ shader::FujiiOrenNayarBRDF::Name }, shader::FujiiOrenNayarBRDF::create }
	, { String{ shader::EnergyConservativeOrenNayarBRDF::Name }, shader::EnergyConservativeOrenNayarBRDF::create } };

	shader::SpecularBrdfDesc const PbrPass::DefaultSpecularBrdf = { String{ shader::SpecularBRDF::Name }, shader::SpecularBRDF::create };
	shader::SpecularBrdfArray const PbrPass::SpecularBrdfs{ { String{ shader::SpecularBRDF::Name }, shader::SpecularBRDF::create } };

	shader::SheenBrdfDesc const PbrPass::DefaultSheenBrdf = { String{ shader::SheenBRDF::Name }, shader::SheenBRDF::create };
	shader::SheenBrdfArray const PbrPass::SheenBrdfs{ { String{ shader::SheenBRDF::Name }, shader::SheenBRDF::create } };

	shader::ClearcoatBrdfDesc const PbrPass::DefaultClearcoatBrdf = { String{ shader::ClearcoatBRDF::Name }, shader::ClearcoatBRDF::create };
	shader::ClearcoatBrdfArray const PbrPass::ClearcoatBrdfs{ { String{ shader::ClearcoatBRDF::Name }, shader::ClearcoatBRDF::create } };

	shader::ScatteringModelDesc const PbrPass::DefaultScatteringModel = { String{ shader::ScatteringModel::Name }, shader::ScatteringModel::create };
	shader::ScatteringModelArray const PbrPass::ScatteringModels{ DefaultScatteringModel };

	PassUPtr PbrPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = makeUnique< Pass >( parent
			, lightingModelId );

		result->createComponent< MetalnessComponent >();

		createDefaultTextureComponent< MetalnessMapComponent >( *result );

		return result;
	}
}
