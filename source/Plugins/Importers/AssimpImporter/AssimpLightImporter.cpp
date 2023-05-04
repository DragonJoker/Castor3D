#include "AssimpImporter/AssimpLightImporter.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/SceneNode.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>

namespace c3d_assimp
{
	AssimpLightImporter::AssimpLightImporter( castor3d::Engine & engine )
		: castor3d::LightImporter{ engine }
	{
	}

	bool AssimpLightImporter::doImportLight( castor3d::Light & light )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = light.getName();
		auto it = file.getLights().find( name );

		if ( it == file.getLights().end() )
		{
			return false;
		}

		auto & aiLight = *it->second;
		auto & scene = *light.getScene();
		castor3d::SceneNodeRPtr node{};
		castor3d::log::info << cuT( "  Light found: [" ) << name << cuT( "]" ) << std::endl;

		if ( scene.hasSceneNode( name ) )
		{
			node = scene.findSceneNode( name );
		}
		else
		{
			auto ownNode = scene.createSceneNode( name, scene );
			ownNode->attachTo( *scene.getObjectRootNode() );
			node = scene.addSceneNode( name, ownNode );
		}

		switch ( aiLight.mType )
		{
		case aiLightSource_POINT:
			{
				auto point = light.getPointLight();
				point->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
			}
			break;
		case aiLightSource_SPOT:
			{
				auto spot = light.getSpotLight();
				spot->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
				spot->setInnerCutOff( castor::Angle::fromRadians( aiLight.mAngleInnerCone ) );
				spot->setOuterCutOff( castor::Angle::fromRadians( aiLight.mAngleOuterCone ) );
			}
			break;
		default:
			break;
		}

		castor::Point3f colour{ aiLight.mColorDiffuse.r, aiLight.mColorDiffuse.g, aiLight.mColorDiffuse.b };
		auto max = std::max( { colour->x, colour->y, colour->z } );

		if ( max != 0.0 )
		{
			colour /= max;
			light.setIntensity( max, max );
		}

		light.setColour( castor::RgbColour::fromComponents( colour->x, colour->y, colour->z ) );
		node->attachObject( light );
		return true;
	}
}
