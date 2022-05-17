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
		castor3d::SceneNodeSPtr node;
		castor3d::log::info << cuT( "  Light found: [" ) << name << cuT( "]" ) << std::endl;

		if ( scene.hasSceneNode( name ) )
		{
			node = scene.findSceneNode( name ).lock();
		}
		else
		{
			node = scene.createSceneNode( name, scene );

			if ( aiLight.mType == aiLightSource_DIRECTIONAL
				|| aiLight.mType == aiLightSource_SPOT )
			{
				auto direction = castor::point::getNormalised( castor::Point3f{ aiLight.mDirection.x, aiLight.mDirection.y, aiLight.mDirection.z } );
				auto up = castor::point::getNormalised( castor::Point3f{ aiLight.mUp.x, aiLight.mUp.y, aiLight.mUp.z } );
				node->setOrientation( castor::Quaternion::fromMatrix( fromAssimp( direction, up ) ) );
			}

			if ( aiLight.mType != aiLightSource_DIRECTIONAL )
			{
				auto position = castor::Point3f{ aiLight.mPosition.x, aiLight.mPosition.y, aiLight.mPosition.z };
				node->setPosition( position );
			}

			node->attachTo( *scene.getObjectRootNode() );
			node = scene.addSceneNode( name, node ).lock();
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
