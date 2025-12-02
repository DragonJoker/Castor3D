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
	AssimpLightImporter::AssimpLightImporter( c3d::Engine & engine )
		: c3d::LightImporter{ engine, cuT( "Assimp" ) }
	{
	}

	bool AssimpLightImporter::doImportLight( c3d::Light & light )
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
		c3d::SceneNodeRPtr node{};

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

		c3d::Point3f colour{ aiLight.mColorDiffuse.r, aiLight.mColorDiffuse.g, aiLight.mColorDiffuse.b };
		auto max = std::max( { colour->x, colour->y, colour->z } );

		if ( max != 0.0 )
		{
			colour /= max;
		}

		switch ( aiLight.mType )
		{
		case aiLightSource_POINT:
			{
				auto point = light.getPointLight();
				if ( max != 0.0 )
					point->setIntensity( c3d::LuminousIntensity{ max } );
				point->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
			}
			break;
		case aiLightSource_SPOT:
			{
				auto spot = light.getSpotLight();
				if ( max != 0.0 )
					spot->setIntensity( c3d::LuminousIntensity{ max } );
				spot->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
				spot->setInnerCutOff( c3d::Angle::fromRadians( aiLight.mAngleInnerCone ) );
				spot->setOuterCutOff( c3d::Angle::fromRadians( aiLight.mAngleOuterCone ) );
			}
			break;
		case aiLightSource_DIRECTIONAL:
			if ( max != 0.0 )
			{
				auto directional = light.getDirectionalLight();
				directional->setIllumination( c3d::Illumination{ max } );
			}
			break;
		default:
			break;
		}

		light.setColour( c3d::RgbColour::fromComponents( colour->x, colour->y, colour->z ) );
		node->attachObject( light );
		return true;
	}
}
