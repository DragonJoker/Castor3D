#include "Castor3D/Material/Pass/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslClearcoatBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLambertianBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslOrenNayarBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslSheenBRDF.hpp"

namespace castor3d
{
	castor::StringView const PbrPass::Type = cuT( "pbr" );
	castor::StringView const PbrPass::LightingModel = shader::PbrLightingModel::getName();

	shader::DiffuseBrdfDesc const PbrPass::DefaultDiffuseBrdf = { castor::String{ shader::EnergyConservativeOrenNayarBRDF::Name }, shader::EnergyConservativeOrenNayarBRDF::create };
	shader::DiffuseBrdfArray const PbrPass::DiffuseBrdfs{ { castor::String{ shader::LambertianBRDF::Name }, shader::LambertianBRDF::create }
		, { castor::String{ shader::QualitativeOrenNayarBRDF::Name }, shader::QualitativeOrenNayarBRDF::create }
		, { castor::String{ shader::FujiiOrenNayarBRDF::Name }, shader::FujiiOrenNayarBRDF::create }
	, { castor::String{ shader::EnergyConservativeOrenNayarBRDF::Name }, shader::EnergyConservativeOrenNayarBRDF::create } };

	shader::SpecularBrdfDesc const PbrPass::DefaultSpecularBrdf = { castor::String{ shader::CookTorranceBRDF::Name }, shader::CookTorranceBRDF::create };
	shader::SpecularBrdfArray const PbrPass::SpecularBrdfs{ { castor::String{ shader::CookTorranceBRDF::Name }, shader::CookTorranceBRDF::create } };

	shader::SheenBrdfDesc const PbrPass::DefaultSheenBrdf = { castor::String{ shader::SheenBRDF::Name }, shader::SheenBRDF::create };
	shader::SheenBrdfArray const PbrPass::SheenBrdfs{ { castor::String{ shader::SheenBRDF::Name }, shader::SheenBRDF::create } };

	shader::ClearcoatBrdfDesc const PbrPass::DefaultClearcoatBrdf = { castor::String{ shader::ClearcoatBRDF::Name }, shader::ClearcoatBRDF::create };
	shader::ClearcoatBrdfArray const PbrPass::ClearcoatBrdfs{ { castor::String{ shader::ClearcoatBRDF::Name }, shader::ClearcoatBRDF::create } };

	PassUPtr PbrPass::create( LightingModelID lightingModelId
		, Material & parent )
	{
		auto result = castor::makeUnique< Pass >( parent
			, lightingModelId );

		result->createComponent< MetalnessComponent >();

		createDefaultTextureComponent< MetalnessMapComponent >( *result );

		return result;
	}
}
