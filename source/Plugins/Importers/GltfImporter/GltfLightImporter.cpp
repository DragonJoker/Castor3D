#include "GltfImporter/GltfLightImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/LightGroup.hpp>
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

		auto & impAsset = file.getAsset();
		fastgltf::Light const & impLight = impAsset.lights[lightIt->lightIndex];
		doImportLightCategory( impLight, *light.getCategory() );
		auto node = doLoadNode( *light.getScene(), lightIt->nodeName, impLight.type != fastgltf::LightType::Point );
		node->attachObject( light );

		return true;
	}

	bool GltfLightImporter::doImportLightGroup( castor3d::LightGroup & light )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto lightName = light.getName();
		auto lightIt = file.getLightGroups().find( lightName );

		if ( lightIt == file.getLightGroups().end() )
		{
			return false;
		}

		auto & impAsset = file.getAsset();
		auto & group = lightIt->second;
		fastgltf::Light const & impLight = impAsset.lights[group.lightIndex];
		doImportLightCategory( impLight, *light.getCategory() );
		bool invertY = ( impLight.type != fastgltf::LightType::Point );
		auto & scene = *light.getScene();

		for ( auto const & nodeName : group.nodeNames )
		{
			auto node = doLoadNode( scene, nodeName, invertY );
			light.addInstance( *node );
		}

		return true;
	}

	bool GltfLightImporter::doImportLightCategory( fastgltf::Light const & impLight
		, castor3d::LightCategory & category )
	{
		if ( impLight.type == fastgltf::LightType::Point )
		{
			auto point = &static_cast< castor3d::PointLight & >( category );
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
			if ( impLight.type == fastgltf::LightType::Spot )
			{
				auto spot = &static_cast< castor3d::SpotLight & >( category );
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
				auto directional = &static_cast< castor3d::DirectionalLight & >( category );
				directional->setIllumination( castor::Illumination{ impLight.intensity } );
			}
		}

		category.setColour( castor::Point3f{ impLight.color[0], impLight.color[1], impLight.color[2] } );
	}

	castor3d::SceneNode * GltfLightImporter::doLoadNode( castor3d::Scene & scene
		, castor::String const & nodeName
		, bool invertY )
	{
		castor3d::SceneNodeRPtr result{};

		if ( scene.hasSceneNode( nodeName ) )
		{
			result = scene.findSceneNode( nodeName );
		}
		else
		{
			auto ownNode = scene.createSceneNode( nodeName, scene );
			ownNode->attachTo( *scene.getObjectRootNode() );
			result = scene.addSceneNode( nodeName, ownNode );
		}


		if ( invertY )
		{
			result->yaw( 180.0_degrees );
		}

		return result;
	}
}
