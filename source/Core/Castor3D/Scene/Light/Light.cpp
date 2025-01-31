#include "Castor3D/Scene/Light/Light.hpp"

#include "Castor3D/Miscellaneous/ConfigurationVisitor.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Light/DirectionalLight.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>

CU_ImplementSmartPtr( castor3d, Light )

namespace castor3d
{
	namespace light
	{
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
				auto name = getPrefixedName( params[0]->get< castor::String >(), *blockContext );

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
				blockContext->light->setColour( params[0]->get< castor::Point3f >() );
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
					blockContext->light->getPointLight()->setIntensity( castor::LuminousIntensity{ params[0]->get< float >() } );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setIntensity( castor::LuminousIntensity{ params[0]->get< float >() } );
				}
				else if ( blockContext->lightType == LightType::eDirectional )
				{
					CU_ParsingDeprecated();
					blockContext->light->getDirectionalLight()->setIllumination( castor::Illumination{ params[0]->get< float >() } );
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
					blockContext->light->getDirectionalLight()->setIllumination( castor::Illumination{ params[0]->get< float >() } );
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
					blockContext->light->getPointLight()->setAttenuation( params[0]->get< castor::Point3f >() );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setAttenuation( params[0]->get< castor::Point3f >() );
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
					blockContext->light->getSpotLight()->setInnerCutOff( castor::Angle::fromDegrees( angle / 2.0f ) );
					blockContext->light->getSpotLight()->setOuterCutOff( castor::Angle::fromDegrees( angle ) );
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
					blockContext->light->getSpotLight()->setInnerCutOff( castor::Angle::fromDegrees( params[0]->get< float >() ) );
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
					blockContext->light->getSpotLight()->setOuterCutOff( castor::Angle::fromDegrees( params[0]->get< float >() ) );
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
	}

	Light::Light( castor::String const & name
		, LightCreateInfo const & createInfo )
		: Light{ name
			, *createInfo.scene
			, *createInfo.parentNode
			, *createInfo.factory
			, createInfo.lightType }
	{
	}

	Light::Light( castor::String const & name
		, Scene & scene
		, SceneNode & node
		, LightFactory & factory
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

	void Light::addParsers( castor::AttributeParsers & result )
	{
		using namespace castor;
		BlockParserContextT< LightContext > context{ result, CSCNSection::eLight, CSCNSection::eScene };

		context.addParser( cuT( "parent" ), light::parserParent, { makeParameter< ParameterType::eName >() } );
		context.addParser( cuT( "type" ), light::parserType, { makeParameter< ParameterType::eCheckedText, LightType >() } );
		context.addParser( cuT( "colour" ), light::parserColour, { makeParameter< ParameterType::ePoint3F >() } );
		context.addParser( cuT( "intensity" ), light::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "illumination" ), light::parserIllumination, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "attenuation" ), light::parserAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
		context.addParser( cuT( "range" ), light::parserRange, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "cut_off" ), light::parserCutOff, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "inner_cut_off" ), light::parserInnerCutOff, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "outer_cut_off" ), light::parserOuterCutOff, { makeParameter< ParameterType::eFloat >() } );
		context.addParser( cuT( "exponent" ), light::parserExponent, { makeParameter< ParameterType::eFloat >() } );
		context.addPopParser( cuT( "}" ), light::parserEnd );
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
