#include "Castor3D/Scene/Light/LightGroup.hpp"

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

CU_ImplementSmartPtr( c3d, LightGroup )

namespace c3d
{
	//*********************************************************************************************

	namespace lgtgrp
	{
		static CU_ImplementAttributeParserNewBlock( parserLightGroup, SceneContext, LightGroupContext )
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
		CU_EndAttributePushNewBlock( CSCNSection::eLightGroup )

		static CU_ImplementAttributeParserBlock( parserType, LightGroupContext )
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
				blockContext->light = blockContext->scene->scene->tryFindLightGroup( blockContext->name );

				if ( !blockContext->light )
				{
					blockContext->ownLight = blockContext->scene->scene->createLightGroup( blockContext->name
						, *blockContext->scene->scene
						, blockContext->scene->scene->getLightsFactory()
						, blockContext->lightType );
					blockContext->light = blockContext->ownLight.get();
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserColour, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserIntensity, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserIllumination, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserAttenuation, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserRange, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserCutOff, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserInnerCutOff, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserOuterCutOff, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserExponent, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
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

		static CU_ImplementAttributeParserBlock( parserEnd, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised. Have you set it's type?" ) );
			}
			else
			{
				log::info << "Loaded light [" << blockContext->light->getName() << "]" << std::endl;

				if ( blockContext->ownLight )
				{
					blockContext->scene->scene->addLightGroup( blockContext->light->getName()
						, blockContext->ownLight
						, true );
				}

				blockContext->light = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserInstances, LightGroupContext )
		{
			// Only need to push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupInstances, blockContext )

		static CU_ImplementAttributeParserBlock( parserInstance, LightGroupContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No LightGroup initialised." ) );
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
					blockContext->light->addInstance( *parent );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserInstancesEnd, LightGroupContext )
		{
			// Only need to pop the block
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserNewBlock( parserShadows, LightGroupContext, ShadowContext )
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

				newBlockContext->lightGroup = blockContext;
				newBlockContext->shadowConfig = blockContext->shadowConfig.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eLightGroupShadows )

		static CU_ImplementAttributeParserBlock( parserShadowProducer, LightGroupContext )
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
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsRaw, blockContext )

		static CU_ImplementAttributeParserBlock( parserPcfConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsPcf, blockContext )

		static CU_ImplementAttributeParserBlock( parserVsmConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsVsm, blockContext )

		static CU_ImplementAttributeParserBlock( parserLpvConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsLpv, blockContext )

		static CU_ImplementAttributeParserBlock( parserRsmConfig, ShadowContext )
		{
			if ( !blockContext->shadowConfig )
			{
				CU_ParsingError( cuT( "No shadow configuration initialised." ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eLightGroupShadowsRsm, blockContext )
	}

	//*********************************************************************************************

	LightGroup::LightGroup( String const & name
		, LightGroupCreateInfo const & createInfo )
		: LightGroup{ name
			, *createInfo.scene
			, *createInfo.factory
			, createInfo.lightType }
	{
	}

	LightGroup::LightGroup( String const & name
		, Scene & scene
		, LightFactory const& factory
		, LightType lightType )
		: OwnedBy< Scene >{ scene }
		, Named{ name }
		, m_enabled{ m_dirty, true, [this](){ markDirty(); } }
	{
		m_category = factory.create( lightType, m_dirty, [this](){ markDirty(); } );
	}

	void LightGroup::addInstance( SceneNode & node )
	{
		m_instances.emplace_back( m_category->instantiate( node, [this](){ return m_enabled.value(); } ) );
	}

	void LightGroup::markDirty()
	{
		getScene()->markDirty( *this );
	}

	void LightGroup::accept( ConfigurationVisitorBase & vis )
	{
		vis.visit( cuT( "LightGroup" ) );
		vis.visit( cuT( "Enabled" ), m_enabled );
		m_category->accept( vis );
	}

	void LightGroup::cloneInto( LightGroup & output )const
	{
		output.m_enabled = m_enabled;
		m_category->cloneInto( *output.m_category );
	}

	void LightGroup::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SceneContext > sceneCtx{ result, CSCNSection::eScene, CSCNSection::eRoot };
		BlockParserContextT< LightGroupContext > groupCtx{ result, CSCNSection::eLightGroup, CSCNSection::eScene };
		BlockParserContextT< LightGroupContext > instancesCtx{ result, CSCNSection::eLightGroupInstances, CSCNSection::eLightGroup };

		sceneCtx.addPushParser( cuT( "light_group" ), CSCNSection::eLightGroup, lgtgrp::parserLightGroup, { makeParameter< ParameterType::eName >() } );

		groupCtx.addParser( cuT( "type" ), lgtgrp::parserType, { makeParameter< ParameterType::eCheckedText, LightType >() } );
		groupCtx.addParser( cuT( "colour" ), lgtgrp::parserColour, { makeParameter< ParameterType::ePoint3F >() } );
		groupCtx.addParser( cuT( "intensity" ), lgtgrp::parserIntensity, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addParser( cuT( "illumination" ), lgtgrp::parserIllumination, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addParser( cuT( "attenuation" ), lgtgrp::parserAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
		groupCtx.addParser( cuT( "range" ), lgtgrp::parserRange, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addParser( cuT( "cut_off" ), lgtgrp::parserCutOff, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addParser( cuT( "inner_cut_off" ), lgtgrp::parserInnerCutOff, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addParser( cuT( "outer_cut_off" ), lgtgrp::parserOuterCutOff, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addParser( cuT( "exponent" ), lgtgrp::parserExponent, { makeParameter< ParameterType::eFloat >() } );
		groupCtx.addPushParser( cuT( "instances" ), CSCNSection::eLightGroupInstances, lgtgrp::parserInstances );
		groupCtx.addPopParser( cuT( "}" ), lgtgrp::parserEnd );

		instancesCtx.addParser( cuT( "instance" ), lgtgrp::parserInstance, { makeParameter< ParameterType::eName >() } );
		instancesCtx.addPopParser( cuT( "}" ), lgtgrp::parserInstancesEnd );

		ShadowConfig::addParsers( result
			, CSCNSection::eLightGroup, CSCNSection::eLightGroupShadows
			, CSCNSection::eLightGroupShadowsRaw, CSCNSection::eLightGroupShadowsPcf, CSCNSection::eLightGroupShadowsVsm
			, RawParserFunctionT< void >( lgtgrp::parserShadows ), RawParserFunctionT< void >( lgtgrp::parserShadowProducer )
			, lgtgrp::parserRawConfig, lgtgrp::parserPcfConfig, lgtgrp::parserVsmConfig );
		LpvConfig::addParsers( result
			, CSCNSection::eLightGroupShadows, CSCNSection::eLightGroupShadowsLpv
			, lgtgrp::parserLpvConfig );
		RsmConfig::addParsers( result
			, CSCNSection::eLightGroupShadows, CSCNSection::eLightGroupShadowsRsm
			, lgtgrp::parserRsmConfig );
	}

	DirectionalLightRPtr LightGroup::getDirectionalLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eDirectional );
		return static_cast< DirectionalLight * >( m_category.get() );
	}

	PointLightRPtr LightGroup::getPointLight()const
	{
		CU_Require( m_category->getLightType() == LightType::ePoint );
		return static_cast< PointLight * >( m_category.get() );
	}

	SpotLightRPtr LightGroup::getSpotLight()const
	{
		CU_Require( m_category->getLightType() == LightType::eSpot );
		return static_cast< SpotLight * >( m_category.get() );
	}

	//*********************************************************************************************

	String getPrefix( LightGroupContext const & context )
	{
		return context.scene
			? getPrefix( *context.scene )
			: String{};
	}

	//*********************************************************************************************
}
