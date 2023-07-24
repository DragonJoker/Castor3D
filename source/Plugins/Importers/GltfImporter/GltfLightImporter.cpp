#include "GltfImporter/GltfLightImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>

namespace c3d_gltf
{
	GltfLightImporter::GltfLightImporter( castor3d::Engine & engine )
		: castor3d::LightImporter{ engine }
	{
	}

	bool GltfLightImporter::doImportLight( castor3d::Light & light )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		auto name = light.getName();
		uint32_t index{};
		auto it = std::find_if( impAsset.lights.begin()
			, impAsset.lights.end()
			, [&name, &file, &index]( fastgltf::Light const & lookup )
			{
				return name == file.getLightName( index++ );
			} );

		if ( it == impAsset.lights.end() )
		{
			return false;
		}

		castor3d::log::info << cuT( "  Light found: [" ) << name << cuT( "]" ) << std::endl;
		fastgltf::Light const & impLight = *it;

		if ( impLight.type == fastgltf::LightType::Point )
		{
			auto point = light.getPointLight();

			if ( impLight.range )
			{
				point->setRange( *impLight.range );
			}
		}
		else if ( impLight.type == fastgltf::LightType::Spot )
		{
			auto spot = light.getSpotLight();

			if ( impLight.range )
			{
				spot->setRange( *impLight.range );
			}

			if ( impLight.innerConeAngle )
			{
				spot->setInnerCutOff( castor::Angle::fromRadians( *impLight.innerConeAngle ) );
			}
			else if ( impLight.outerConeAngle )
			{
				spot->setInnerCutOff( castor::Angle::fromRadians( *impLight.outerConeAngle ) );
			}

			if ( impLight.outerConeAngle )
			{
				spot->setOuterCutOff( castor::Angle::fromRadians( *impLight.outerConeAngle ) );
			}
			else if ( impLight.innerConeAngle )
			{
				spot->setOuterCutOff( castor::Angle::fromRadians( *impLight.innerConeAngle ) );
			}
		}

		light.setColour( castor::RgbColour::fromComponents( impLight.color[0], impLight.color[1], impLight.color[2] ) );
		light.setDiffuseIntensity( impLight.intensity );
		light.setSpecularIntensity( impLight.intensity );

		return true;
	}
}
