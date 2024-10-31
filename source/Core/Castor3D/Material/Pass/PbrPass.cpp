#include "Castor3D/Material/Pass/PbrPass.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp"
#include "Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp"
#include "Castor3D/Material/Pass/Shaders/GlslPbrLighting.hpp"
#include "Castor3D/Shader/Shaders/GlslCookTorranceBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslLambertianBRDF.hpp"
#include "Castor3D/Shader/Shaders/GlslOrenNayarBRDF.hpp"

namespace castor3d
{
	castor::StringView const PbrPass::Type = cuT( "pbr" );
	castor::StringView const PbrPass::LightingModel = shader::PbrLightingModel::getName();
	shader::DiffuseBrdfDesc const PbrPass::DefaultDiffuseBrdf = { castor::String{ shader::EnergyConservativeOrenNayarBRDF::Name }, shader::EnergyConservativeOrenNayarBRDF::create };
	shader::SpecularBrdfDesc const PbrPass::DefaultSpecularBrdf = { castor::String{ shader::CookTorranceBRDF::Name }, shader::CookTorranceBRDF::create };
	shader::DiffuseBrdfArray const PbrPass::DiffuseBrdfs{ { castor::String{ shader::LambertianBRDF::Name }, shader::LambertianBRDF::create }
		, { castor::String{ shader::QualitativeOrenNayarBRDF::Name }, shader::QualitativeOrenNayarBRDF::create }
		, { castor::String{ shader::FujiiOrenNayarBRDF::Name }, shader::FujiiOrenNayarBRDF::create }
		, { castor::String{ shader::EnergyConservativeOrenNayarBRDF::Name }, shader::EnergyConservativeOrenNayarBRDF::create } };
	shader::SpecularBrdfArray const PbrPass::SpecularBrdfs{ { castor::String{ shader::CookTorranceBRDF::Name }, shader::CookTorranceBRDF::create } };

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
