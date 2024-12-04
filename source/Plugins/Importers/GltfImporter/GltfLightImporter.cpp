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
		: castor3d::LightImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfLightImporter::doImportLight( castor3d::Light & light )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto lightName = light.getName();
		auto lightIt = std::find_if( file.getLights().begin()
			, file.getLights().end()
			, [&lightName]( GltfLightData const & lookup )
			{
				return lightName == lookup.name;
			} );

		if ( lightIt == file.getLights().end() )
		{
			return false;
		}

		auto nodeName = lightIt->nodeName;
		auto nodeIt = std::find_if( file.getNodes().begin()
			, file.getNodes().end()
			, [&nodeName]( GltfNodeData const & lookup )
			{
				return nodeName == lookup.name;
			} );

		if ( nodeIt == file.getNodes().end() )
		{
			return false;
		}

		auto & impAsset = file.getAsset();
		auto & scene = *light.getScene();
		castor3d::SceneNodeRPtr node{};

		if ( scene.hasSceneNode( nodeName ) )
		{
			node = scene.findSceneNode( nodeName );
		}
		else
		{
			auto ownNode = scene.createSceneNode( nodeName, scene );
			ownNode->attachTo( *scene.getObjectRootNode() );
			node = scene.addSceneNode( nodeName, ownNode );
		}

		fastgltf::Light const & impLight = impAsset.lights[lightIt->lightIndex];

		if ( impLight.type == fastgltf::LightType::Point )
		{
			auto point = light.getPointLight();
			point->setIntensity( castor::LuminousIntensity{ impLight.intensity / 1000.0f } );

			if ( impLight.range )
			{
				point->setRange( *impLight.range );
			}
			else
			{
				point->setRange( 1.0f );
			}
		}
		else
		{
			node->yaw( 180.0_degrees );

			if ( impLight.type == fastgltf::LightType::Spot )
			{
				auto spot = light.getSpotLight();
				spot->setIntensity( castor::LuminousIntensity{ impLight.intensity / 1000.0f } );

				if ( impLight.range )
				{
					spot->setRange( *impLight.range );
				}
				else
				{
					spot->setRange( 1.0f );
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
			else
			{
				auto directional = light.getDirectionalLight();
				directional->setIllumination( castor::Illumination{ impLight.intensity } );
			}
		}

		light.setColour( castor::RgbColour::fromComponents( impLight.color[0], impLight.color[1], impLight.color[2] ) );
		node->attachObject( light );

		return true;
	}
}
