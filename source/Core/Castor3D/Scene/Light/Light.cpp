#include "Castor3D/Scene/Light/Light.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( c3d, Light )

namespace c3d
{
	namespace light
	{
		static CU_ImplementAttributeParserNewBlock( parserLight, SceneContext, LightContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->scene = blockContext;
				newBlockContext->name = getPrefixedName( params[0]->get< String >(), *blockContext );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eLight )

		static CU_ImplementAttributeParserBlock( parserParent, LightContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );

				if ( auto parent = blockContext->scene->scene->findSceneNode( name ) )
				{
					blockContext->parentNode = parent;

					if ( blockContext->light )
					{
						blockContext->light->detach();
						blockContext->parentNode->attachObject( *blockContext->light );
						blockContext->parentNode = nullptr;
					}
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserType, LightContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->lightType = LightType( params[0]->get< uint32_t >() );
				blockContext->light = blockContext->scene->scene->tryFindLight( blockContext->name );

				if ( !blockContext->light )
				{
					auto node = blockContext->parentNode;

					if ( !node )
					{
						node = blockContext->scene->scene->getObjectRootNode();
					}

					blockContext->parentNode = nullptr;
					blockContext->ownLight = blockContext->scene->scene->createLight( blockContext->name
						, *blockContext->scene->scene
						, *node
						, blockContext->scene->scene->getLightsFactory()
						, blockContext->lightType );
					blockContext->light = blockContext->ownLight.get();
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserColour, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->light->setColour( params[0]->get< Point3f >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserIntensity, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::ePoint )
				{
					blockContext->light->getPointLight()->setIntensity( LuminousIntensity{ params[0]->get< float >() } );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setIntensity( LuminousIntensity{ params[0]->get< float >() } );
				}
				else if ( blockContext->lightType == LightType::eDirectional )
				{
					CU_ParsingDeprecated();
					blockContext->light->getDirectionalLight()->setIllumination( Illumination{ params[0]->get< float >() } );
				}
				else
				{
					CU_ParsingError( cuT( "Unsupported light type for intensity." ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserIllumination, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eDirectional )
				{
					blockContext->light->getDirectionalLight()->setIllumination( Illumination{ params[0]->get< float >() } );
				}
				else
				{
					CU_ParsingError( cuT( "Unsupported light type for illumination." ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAttenuation, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::ePoint )
				{
					blockContext->light->getPointLight()->setAttenuation( params[0]->get< Point3f >() );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setAttenuation( params[0]->get< Point3f >() );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRange, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::ePoint )
				{
					blockContext->light->getPointLight()->setRange( params[0]->get< float >() );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setRange( params[0]->get< float >() );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCutOff, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto angle = params[0]->get< float >();

				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setInnerCutOff( Angle::fromDegrees( angle / 2.0f ) );
					blockContext->light->getSpotLight()->setOuterCutOff( Angle::fromDegrees( angle ) );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserInnerCutOff, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setInnerCutOff( Angle::fromDegrees( params[0]->get< float >() ) );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOuterCutOff, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setOuterCutOff( Angle::fromDegrees( params[0]->get< float >() ) );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserExponent, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setExponent( params[0]->get< float >() );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply an exponent, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserEnd, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else
			{
				log::info << "Loaded light [" << blockContext->light->getName() << "]" << std::endl;
				blockContext->parentNode = nullptr;

				if ( blockContext->ownLight )
				{
					blockContext->scene->scene->addLight( blockContext->light->getName()
						, blockContext->ownLight
						, true );
				}

				blockContext->light = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserNewBlock( parserShadows, LightContext, ShadowContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else
			{
				if ( !blockContext->shadowConfig )
				{
					blockContext->shadowConfig = makeUnique< ShadowConfig >();
				}

				newBlockContext->light = blockContext;
				newBlockContext->shadowConfig = blockContext->shadowConfig.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eShadows )

		static CU_ImplementAttributeParserBlock( parserShadowProducer, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( !params.empty() )
			{
				blockContext->shadowConfig = makeUnique< ShadowConfig >();
				params[0]->get( blockContext->shadowConfig->enabled );
				blockContext->light->setShadowConfig( *blockContext->shadowConfig );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRawConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eRaw, blockContext )

		static CU_ImplementAttributeParserBlock( parserPcfConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::ePcf, blockContext )

		static CU_ImplementAttributeParserBlock( parserVsmConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eVsm, blockContext )

		static CU_ImplementAttributeParserBlock( parserLpvConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLpv, blockContext )

		static CU_ImplementAttributeParserBlock( parserRsmConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eRsm, blockContext )
	}

	Light::Light( String const & name
		, LightCreateInfo const & createInfo )
		: Light{ name
			, *createInfo.scene
			, *createInfo.parentNode
			, *createInfo.factory
			, createInfo.lightType }
	{
	}

	Light::Light( String const & name
		, Scene & scene
		, SceneNode & node
		, LightFactory const & factory
		, LightType lightType )
		: MovableObject{ name, scene, MovableType::eLight, node }
		, m_enabled{ m_dirty, true, [this](){ markDirty(); } }
	{
		m_category = factory.create( lightType, m_dirty, [this](){ markDirty(); } );
		m_instance = m_category->instantiate( node, [this](){ return isEnabled(); } );
	}

	void Light::attachTo( SceneNode & node )
	{
		m_instance->setNode( node );
		MovableObject::attachTo( node );
	}

	void Light::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "Light" ) );
		vis.visit( cuT( "Enabled" ), m_enabled );
		m_category->accept( vis );
	}

	void Light::cloneInto( Light & output )const
	{
		output.m_enabled = m_enabled;
		m_category->cloneInto( *output.m_category );
	}

	void Light::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SceneContext > sceneCtx{ result, CSCNSection::eScene, CSCNSection::eRoot };
		BlockParserContextT< LightContext > lightCtx{ result, CSCNSection::eLight, CSCNSection::eScene };

		sceneCtx.addPushParser( cuT( "light" ), CSCNSection::eLight, light::parserLight, { makeParameter< ParameterType::eName >() } );

		lightCtx.addParser( cuT( "parent" ), light::parserParent, { makeParameter< ParameterType::eName >() } );
		lightCtx.addParser( cuT( "type" ), light::parserType, { makeParameter< ParameterType::eCheckedText, LightType >() } );
		lightCtx.addParser( cuT( "colour" ), light::parserColour, { makeParameter< ParameterType::ePoint3F >() } );
		lightCtx.addParser( cuT( "intensity" ), light::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addParser( cuT( "illumination" ), light::parserIllumination, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addParser( cuT( "attenuation" ), light::parserAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
		lightCtx.addParser( cuT( "range" ), light::parserRange, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addParser( cuT( "cut_off" ), light::parserCutOff, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addParser( cuT( "inner_cut_off" ), light::parserInnerCutOff, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addParser( cuT( "outer_cut_off" ), light::parserOuterCutOff, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addParser( cuT( "exponent" ), light::parserExponent, { makeParameter< ParameterType::eFloat >() } );
		lightCtx.addPopParser( cuT( "}" ), light::parserEnd );

		ShadowConfig::addParsers( result
			, CSCNSection::eLight, CSCNSection::eShadows
			, CSCNSection::eRaw, CSCNSection::ePcf, CSCNSection::eVsm
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-strict"
			, RawParserFunctionT< void >( light::parserShadows ), RawParserFunctionT< void >( light::parserShadowProducer )
#pragma clang diagnostic pop
			, light::parserRawConfig, light::parserPcfConfig, light::parserVsmConfig );
		LpvConfig::addParsers( result
			, CSCNSection::eShadows, CSCNSection::eLpv
			, light::parserLpvConfig );
		RsmConfig::addParsers( result
			, CSCNSection::eShadows, CSCNSection::eRsm
			, light::parserRsmConfig );
	}

	DirectionalLightRPtr Light::getDirectionalLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eDirectional );
		return static_cast< DirectionalLight * >( m_category.get() );
	}

	PointLightRPtr Light::getPointLight()const
	{
		CU_Require( m_category->getLightType() == LightType::ePoint );
		return static_cast< PointLight * >( m_category.get() );
	}

	SpotLightRPtr Light::getSpotLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eSpot );
		return static_cast< SpotLight * >( m_category.get() );
	}
}
