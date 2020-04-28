#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/CameraCache.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/SceneCache.hpp"
#include "Castor3D/Cache/SceneNodeCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Cache/WindowCache.hpp"
#include "Castor3D/Event/Frame/InitialiseEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp"
#include "Castor3D/Material/Pass/PhongPass.hpp"
#include "Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Model/Mesh/Importer.hpp"
#include "Castor3D/Model/Mesh/ImporterFactory.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"
#include "Castor3D/Model/Mesh/Subdivider.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Background/Image.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/FileParser/ParserParameter.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		void createAlphaRejectionPass( PassSPtr srcPass
			, PassSPtr dstPass )
		{
			dstPass->setImplicit();
			dstPass->setTwoSided( true );
			dstPass->setAlphaFunc( VK_COMPARE_OP_GREATER );
			dstPass->setAlphaValue( 0.95f );
			dstPass->setEmissive( srcPass->getEmissive() );
			dstPass->setOpacity( srcPass->getOpacity() );
			dstPass->setRefractionRatio( srcPass->getRefractionRatio() );
			dstPass->setParallaxOcclusion( srcPass->hasParallaxOcclusion() );
			dstPass->setColourBlendMode( srcPass->getColourBlendMode() );

			if ( srcPass->hasSubsurfaceScattering() )
			{
				dstPass->setSubsurfaceScattering( std::make_unique< SubsurfaceScattering >( srcPass->getSubsurfaceScattering() ) );
			}

			for ( auto & unit : *srcPass )
			{
				dstPass->addTextureUnit( unit );
			}
		}

		void createAlphaRejectionPass( MaterialSPtr material
			, PhongPassSPtr srcPass )
		{
			auto dstPass = std::static_pointer_cast< PhongPass >( material->createPass() );
			dstPass->setAmbient( srcPass->getAmbient() );
			dstPass->setDiffuse( srcPass->getDiffuse() );
			dstPass->setShininess( srcPass->getShininess() );
			dstPass->setSpecular( srcPass->getSpecular() );
			createAlphaRejectionPass( srcPass, dstPass );
		}

		void createAlphaRejectionPass( MaterialSPtr material
			, MetallicRoughnessPbrPassSPtr srcPass )
		{
			auto dstPass = std::static_pointer_cast< MetallicRoughnessPbrPass >( material->createPass() );
			dstPass->setAlbedo( srcPass->getAlbedo() );
			dstPass->setRoughness( srcPass->getRoughness() );
			dstPass->setMetallic( srcPass->getMetallic() );
			createAlphaRejectionPass( srcPass, dstPass );
		}

		void createAlphaRejectionPass( MaterialSPtr material
			, SpecularGlossinessPbrPassSPtr srcPass )
		{
			auto dstPass = std::static_pointer_cast< SpecularGlossinessPbrPass >( material->createPass() );
			dstPass->setDiffuse( srcPass->getDiffuse() );
			dstPass->setGlossiness( srcPass->getGlossiness() );
			dstPass->setSpecular( srcPass->getSpecular() );
			createAlphaRejectionPass( srcPass, dstPass );
		}

		void mergeMasks( uint32_t toMerge
			, TextureFlag flag
			, TextureFlags & result )
		{
			result |= toMerge
				? flag
				: TextureFlag::eNone;
		}

		TextureFlags getUsedImageComponents( TextureConfiguration const & config )
		{
			TextureFlags result = TextureFlag::eNone;
			mergeMasks( config.colourMask[0], TextureFlag::eDiffuse, result );
			mergeMasks( config.specularMask[0], TextureFlag::eSpecular, result );
			mergeMasks( config.glossinessMask[0], TextureFlag::eGlossiness, result );
			mergeMasks( config.opacityMask[0], TextureFlag::eOpacity, result );
			mergeMasks( config.emissiveMask[0], TextureFlag::eEmissive, result );
			mergeMasks( config.normalMask[0], TextureFlag::eNormal, result );
			mergeMasks( config.heightMask[0], TextureFlag::eHeight, result );
			mergeMasks( config.occlusionMask[0], TextureFlag::eOcclusion, result );
			mergeMasks( config.transmittanceMask[0], TextureFlag::eTransmittance, result );
			return result;
		}
	}

	CU_ImplementAttributeParser( parserRootScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			params[0]->get( name );
			parsingContext->scene = parsingContext->m_pParser->getEngine()->getSceneCache().add( name );
			parsingContext->sceneNode = parsingContext->scene->getRootNode();
			parsingContext->mapScenes.insert( std::make_pair( name, parsingContext->scene ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->path.clear();

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eFont )

	CU_ImplementAttributeParser( parserRootMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->strName );
			parsingContext->material = parsingContext->m_pParser->getEngine()->getMaterialCache().add( parsingContext->strName
				, MaterialType::ePhong );
		}
	}
	CU_EndAttributePush( CSCNSection::eMaterial )

	CU_ImplementAttributeParser( parserRootMaterials )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext->m_pParser->getEngine()->setMaterialsType( MaterialType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRootPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( params[0]->get( name ), OverlayType::ePanel, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserRootBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( params[0]->get( name ), OverlayType::eBorderPanel, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserRootTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( params[0]->get( name ), OverlayType::eText, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserRootSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->sampler = parsingContext->m_pParser->getEngine()->getSamplerCache().add( params[0]->get( name ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSampler )

	CU_ImplementAttributeParser( parserRootDebugOverlays )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			parsingContext->m_pParser->getEngine()->getRenderLoop().showDebugOverlays( params[0]->get( value ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootWindow )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->window )
			{
				CU_ParsingError( cuT( "Can't create more than one render window" ) );
			}
			else
			{
				String name;
				params[0]->get( name );
				parsingContext->window = parsingContext->scene->getEngine()->getRenderWindowCache().add( name );
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eWindow )

	CU_ImplementAttributeParser( parserWindowRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->window )
		{
			parsingContext->renderTarget = parsingContext->m_pParser->getEngine()->getRenderTargetCache().add( TargetType::eWindow );
			parsingContext->iInt16 = 0;
		}
		else
		{
			CU_ParsingError( cuT( "No window initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eRenderTarget )

	CU_ImplementAttributeParser( parserWindowVSync )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->window )
		{
			CU_ParsingError( cuT( "No window initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext->window->enableVSync( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowFullscreen )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->window )
		{
			CU_ParsingError( cuT( "No window initialised." ) );
		}
		else if ( !params.empty() )
		{

			bool value;
			params[0]->get( value );
			parsingContext->window->setFullscreen( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !params.empty() )
		{
			ScenePtrStrMap::iterator it = parsingContext->mapScenes.find( params[0]->get( name ) );

			if ( it != parsingContext->mapScenes.end() )
			{
				parsingContext->renderTarget->setScene( it->second );
			}
			else
			{
				CU_ParsingError( cuT( "No scene found with name : [" ) + name + cuT( "]." ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->renderTarget->getScene() )
			{
				parsingContext->renderTarget->setCamera( parsingContext->renderTarget->getScene()->getCameraCache().find( params[0]->get( name ) ) );
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised for this window, set scene before camera." ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !params.empty() )
		{
			Size size;
			parsingContext->renderTarget->setSize( params[0]->get( size ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetFormat )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !params.empty() )
		{
			PixelFormat ePF;
			params[0]->get( ePF );

			if ( ePF < PixelFormat::eD16_UNORM )
			{
				parsingContext->renderTarget->setPixelFormat( convert( ePF ) );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong format for colour" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetStereo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !params.empty() )
		{
			float rIntraOcularDistance;
			params[0]->get( rIntraOcularDistance );

			if ( rIntraOcularDistance > 0 )
			{
				//l_parsingContext->renderTarget->setStereo( true );
				//l_parsingContext->renderTarget->setIntraOcularDistance( rIntraOcularDistance );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetPostEffect )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			Engine * engine = parsingContext->m_pParser->getEngine();

			if ( !engine->getRenderTargetCache().getPostEffectFactory().isTypeRegistered( string::lowerCase( name ) ) )
			{
				CU_ParsingError( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				PostEffectSPtr effect = engine->getRenderTargetCache().getPostEffectFactory().create( name, *parsingContext->renderTarget, *engine->getRenderSystem(), parameters );
				parsingContext->renderTarget->addPostEffect( effect );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetToneMapping )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			params[0]->get( name );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			parsingContext->renderTarget->setToneMappingType( name, parameters );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetSsao )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No render target initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSsao )
		
	CU_ImplementAttributeParser( parserRenderTargetHdrConfig )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eHdrConfig )

	CU_ImplementAttributeParser( parserRenderTargetEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget->getTargetType() == TargetType::eTexture )
		{
			parsingContext->textureUnit->setRenderTarget( parsingContext->renderTarget );
		}
		else
		{
			parsingContext->window->setRenderTarget( parsingContext->renderTarget );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSamplerMinFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setMinFilter( VkFilter( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMagFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setMagFilter( VkFilter( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMipFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setMipFilter( VkSamplerMipmapMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMinLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			float rValue = -1000;
			params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->sampler->setMinLod( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMaxLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			float rValue = 1000;
			params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->sampler->setMaxLod( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerLodBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			float rValue = 1000;
			params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->sampler->setLodBias( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerUWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setWrapS( VkSamplerAddressMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerVWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setWrapT( VkSamplerAddressMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerWWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setWrapR( VkSamplerAddressMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerBorderColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t colour;
			params[0]->get( colour );
			parsingContext->sampler->setBorderColour( VkBorderColor( colour ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerAnisotropicFiltering )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext->sampler->enableAnisotropicFiltering( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMaxAnisotropy )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			float rValue = 1000;
			params[0]->get( rValue );
			parsingContext->sampler->setMaxAnisotropy( rValue );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerComparisonMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t mode;
			params[0]->get( mode );
			parsingContext->sampler->enableCompare( bool( mode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerComparisonFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			parsingContext->sampler->setCompareOp( VkCompareOp( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneInclude )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			Path path;
			params[0]->get( path );
			SceneFileParser subparser{ *parsingContext->m_pParser->getEngine() };
			subparser.parseFile( parsingContext->m_file.getPath() / path, parsingContext );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBkColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			RgbColour clrBackground;
			params[0]->get( clrBackground );
			parsingContext->scene->setBackgroundColour( clrBackground );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBkImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto imgBackground = std::make_shared< ImageBackground >( *parsingContext->m_pParser->getEngine()
				, *parsingContext->scene );
			Path path;
			imgBackground->loadImage( context->m_file.getPath(), params[0]->get( path ) );
			parsingContext->scene->setBackground( imgBackground );
			parsingContext->skybox.reset();
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
			parsingContext->path.clear();
			params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eFont )

	CU_ImplementAttributeParser( parserSceneMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->strName );
			auto & view = parsingContext->scene->getMaterialView();
			parsingContext->createMaterial = !view.has( parsingContext->strName );
			parsingContext->material = view.add( parsingContext->strName, parsingContext->scene->getMaterialsType() );
			parsingContext->passIndex = 0u;
		}
	}
	CU_EndAttributePush( CSCNSection::eMaterial )

	CU_ImplementAttributeParser( parserSceneSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext->sampler = parsingContext->scene->getSamplerView().add( params[0]->get( name ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSampler )

	CU_ImplementAttributeParser( parserSceneCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->viewport.reset();
		parsingContext->sceneNode = parsingContext->scene->getCameraRootNode();

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eCamera )

	CU_ImplementAttributeParser( parserSceneLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->light.reset();
		parsingContext->strName.clear();
		parsingContext->sceneNode = parsingContext->scene->getObjectRootNode();

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eLight )

	CU_ImplementAttributeParser( parserSceneCameraNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->sceneNode = parsingContext->scene->getCameraRootNode();
		String name;

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( name );
			parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().add( name
				, *parsingContext->sceneNode );
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneSceneNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->sceneNode = parsingContext->scene->getObjectRootNode();
		String name;

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( name );
			parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().add( name
				, *parsingContext->sceneNode );
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->sceneNode = parsingContext->scene->getObjectRootNode();
		parsingContext->geometry.reset();
		String name;

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( name );
			parsingContext->geometry = std::make_shared< Geometry >( name
				, *parsingContext->scene
				, nullptr );
		}
	}
	CU_EndAttributePush( CSCNSection::eObject )

	CU_ImplementAttributeParser( parserSceneAmbientLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			RgbColour clColour;
			params[0]->get( clColour );
			parsingContext->scene->setAmbientLight( clColour );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		Path path;
		Path pathFile = context->m_file.getPath() / params[0]->get( path );

		Engine * engine = parsingContext->m_pParser->getEngine();
		auto extension = string::lowerCase( pathFile.getExtension() );

		//if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
		//{
		//	CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
		//}
		//else
		//{
		//	auto importer = engine->getImporterFactory().create( extension, *engine );
		//	parsingContext->scene->importExternal( pathFile, *importer );
		//}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBillboard )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			parsingContext->billboards = std::make_shared< BillboardList >( name, *parsingContext->scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eBillboard )

	CU_ImplementAttributeParser( parserSceneAnimatedObjectGroup )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			parsingContext->pAnimGroup = parsingContext->scene->getAnimatedObjectGroupCache().add( name );
		}
	}
	CU_EndAttributePush( CSCNSection::eAnimGroup )

	CU_ImplementAttributeParser( parserScenePanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( params[0]->get( name )
				, OverlayType::ePanel
				, parsingContext->scene
				, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserSceneBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->scene, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserSceneTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( params[0]->get( name ), OverlayType::eText, parsingContext->scene, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserSceneSkybox )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->skybox = std::make_shared< SkyboxBackground >( *parsingContext->m_pParser->getEngine()
					, *parsingContext->scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eSkybox )

	CU_ImplementAttributeParser( parserSceneFogType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext->scene->getFog().setType( FogType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFogDensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->scene->getFog().setDensity( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneParticleSystem )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			params[0]->get( value );
			parsingContext->strName = value;
			parsingContext->uiUInt32 = 0u;
			parsingContext->sceneNode = parsingContext->scene->getObjectRootNode();
			parsingContext->material.reset();
		}
	}
	CU_EndAttributePush( CSCNSection::eParticleSystem )

	CU_ImplementAttributeParser( parserMesh )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->bBool1 = false;
		params[0]->get( parsingContext->strName2 );

		if ( parsingContext->scene )
		{
			auto const & cache = parsingContext->scene->getMeshCache();

			if ( cache.has( parsingContext->strName2 ) )
			{
				parsingContext->mesh = cache.find( parsingContext->strName2 );
			}
			else
			{
				parsingContext->mesh.reset();
			}
		}
		else
		{
			CU_ParsingError( cuT( "No scene initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eMesh )

	CU_ImplementAttributeParser( parserDirectionalShadowCascades )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext->scene->setDirectionalShadowCascades( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			params[0]->get( value );

			if ( !parsingContext->scene->getSceneNodeCache().has( value ) )
			{
				CU_ParsingError( cuT( "No scene node named " ) + value );
			}
			else
			{
				parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().find( value );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext->uiUInt32 = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			params[0]->get( name );

			if ( cache.has( name ) )
			{
				parsingContext->material = cache.find( name );
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

		CU_ImplementAttributeParser( parserParticleSystemDimensions )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->point2f );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemParticle )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		
		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( parsingContext->uiUInt32 == 0 )
		{
			CU_ParsingError( cuT( "particles_count has not been specified." ) );
		}
		else if ( parsingContext->point2f[0] == 0 || parsingContext->point2f[1] == 0 )
		{
			CU_ParsingError( cuT( "one component of the particles dimensions is 0." ) );
		}
		else
		{
			if ( !parsingContext->material )
			{
				parsingContext->material = parsingContext->m_pParser->getEngine()->getMaterialCache().getDefaultMaterial();
			}

			parsingContext->particleSystem = parsingContext->scene->getParticleSystemCache().add( parsingContext->strName
				, *parsingContext->sceneNode
				, parsingContext->uiUInt32 );
			parsingContext->particleSystem->setMaterial( parsingContext->material );
			parsingContext->particleSystem->setDimensions( parsingContext->point2f );
		}
	}
	CU_EndAttributePush( CSCNSection::eParticle )

	CU_ImplementAttributeParser( parserParticleSystemCSShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderProgram.reset();
		parsingContext->shaderStage = VkShaderStageFlagBits( 0u );
		parsingContext->bBool1 = false;
		
		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->shaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( parsingContext->particleSystem->getName(), true );
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderProgram )

	CU_ImplementAttributeParser( parserParticleType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->particleSystem )
		{
			CU_ParsingError( cuT( "Particle system not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			params[0]->get( value );
			Engine * engine = parsingContext->m_pParser->getEngine();

			if ( !engine->getParticleFactory().isTypeRegistered( string::lowerCase( value ) ) )
			{
				CU_ParsingError( cuT( "Particle type [" ) + value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext->particleSystem->setParticleType( value );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		params[0]->get( parsingContext->strName2 );

		if ( !parsingContext->particleSystem )
		{
			CU_ParsingError( cuT( "Particle system not initialised" ) );
		}
		else if ( params.size() < 2 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			uint32_t type;
			String value;
			params[0]->get( name );
			params[1]->get( type );

			if ( params.size() > 2 )
			{
				params[2]->get( value );
			}

			parsingContext->particleSystem->addParticleVariable( name, ParticleFormat( type ), value );
		}
	}
	CU_EndAttributePush( CSCNSection::eUBOVariable )

	CU_ImplementAttributeParser( parserLightParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr pParent = parsingContext->scene->getSceneNodeCache().find( name );

			if ( pParent )
			{
				parsingContext->sceneNode = pParent;
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}

			if ( parsingContext->light )
			{
				parsingContext->light->detach();
				parsingContext->sceneNode->attachObject( *parsingContext->light );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiType;
			params[0]->get( uiType );
			parsingContext->lightType = LightType( uiType );
			auto & cache = parsingContext->scene->getLightCache();
			parsingContext->light = cache.add( parsingContext->strName
				, *parsingContext->sceneNode
				, parsingContext->lightType );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->light->setColour( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			Point2f value;
			params[0]->get( value );
			parsingContext->light->setIntensity( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightAttenuation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );

			if ( parsingContext->lightType == LightType::ePoint )
			{
				parsingContext->light->getPointLight()->setAttenuation( value );
			}
			else if ( parsingContext->lightType == LightType::eSpot )
			{
				parsingContext->light->getSpotLight()->setAttenuation( value );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightCutOff )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			float fFloat;
			params[0]->get( fFloat );

			if ( parsingContext->lightType == LightType::eSpot )
			{
				parsingContext->light->getSpotLight()->setCutOff( Angle::fromDegrees( fFloat ) );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightExponent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			float fFloat;
			params[0]->get( fFloat );

			if ( parsingContext->lightType == LightType::eSpot )
			{
				parsingContext->light->getSpotLight()->setExponent( fFloat );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply an exponent, needs spotlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShadows )

	CU_ImplementAttributeParser( parserShadowsProducer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext->light->setShadowProducer( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext->light->setShadowType( ShadowType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVolumetricSteps )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext->light->setVolumetricSteps( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVolumetricScatteringFactor )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext->light->setVolumetricScatteringFactor( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsMinOffset )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->light->setShadowMinOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsMaxSlopeOffset )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->light->setShadowMaxSlopeOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVarianceMax )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->light->setShadowMaxVariance( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVarianceBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->light->setShadowVarianceBias( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr parent;

			if ( name == Scene::ObjectRootNode )
			{
				parent = parsingContext->scene->getObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext->scene->getCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext->scene->getRootNode();
			}
			else
			{
				parent = parsingContext->scene->getSceneNodeCache().find( name );
			}

			if ( parent )
			{
				parsingContext->sceneNode->attachTo( *parent );
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodePosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->sceneNode->setPosition( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeOrientation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f axis;
			float angle;
			params[0]->get( axis );
			params[1]->get( angle );
			parsingContext->sceneNode->setOrientation( Quaternion::fromAxisAngle( axis, Angle::fromDegrees( angle ) ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeDirection )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f direction;
			params[0]->get( direction );
			Point3f up{ 0, 1, 0 };
			Point3f right{ point::cross( direction, up ) };
			parsingContext->sceneNode->setOrientation( Quaternion::fromAxes( right, up, direction ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->sceneNode->setScale( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr parent;

			if ( name == Scene::ObjectRootNode )
			{
				parent = parsingContext->scene->getObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext->scene->getCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext->scene->getRootNode();
			}
			else
			{
				parent = parsingContext->scene->getSceneNodeCache().find( name );
			}

			if ( parent )
			{
				parent->attachObject( *parsingContext->geometry );
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Geometry not initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->geometry->getMesh() )
			{
				auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
				String name;
				params[0]->get( name );

				if ( cache.has( name ) )
				{
					MaterialSPtr material = cache.find( name );

					for ( auto submesh : *parsingContext->geometry->getMesh() )
					{
						parsingContext->geometry->setMaterial( *submesh, material );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterials )
	{
	}
	CU_EndAttributePush( CSCNSection::eObjectMaterials )

	CU_ImplementAttributeParser( parserObjectCastShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext->geometry->setShadowCaster( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectReceivesShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext->geometry->setShadowReceiver( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->scene->getGeometryCache().add( parsingContext->geometry );
		parsingContext->geometry.reset();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserObjectMaterialsMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->geometry->getMesh() )
			{
				auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
				String name;
				uint16_t index;
				params[0]->get( index );
				params[1]->get( name );

				if ( cache.has( name ) )
				{
					if ( parsingContext->geometry->getMesh()->getSubmeshCount() > index )
					{
						SubmeshSPtr submesh = parsingContext->geometry->getMesh()->getSubmesh( index );
						MaterialSPtr material = cache.find( name );
						parsingContext->geometry->setMaterial( *submesh, material );
					}
					else
					{
						CU_ParsingError( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterialsEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String type;
		params[0]->get( type );

		if ( !parsingContext->mesh )
		{
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			if ( parsingContext->scene )
			{
				parsingContext->mesh = parsingContext->scene->getMeshCache().add( parsingContext->strName2 );
				parsingContext->scene->getEngine()->getMeshFactory().create( type )->generate( *parsingContext->mesh, parameters );
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised" ) );
			}
		}
		else
		{
			CU_ParsingWarning( cuT( "Mesh already initialised => ignored" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshSubmesh )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->bBool2 = false;
		parsingContext->bBool1 = true;
		parsingContext->face1 = -1;
		parsingContext->face2 = -1;
		parsingContext->submesh.reset();

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else
		{
			parsingContext->submesh = parsingContext->mesh->createSubmesh();
		}
	}
	CU_EndAttributePush( CSCNSection::eSubmesh )

	CU_ImplementAttributeParser( parserMeshImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			Path path;
			Path pathFile = context->m_file.getPath() / params[0]->get( path );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String meshParams;
				StringArray paramArray = string::split( params[1]->get( meshParams ), cuT( "-" ), 20, false );

				for ( auto param : paramArray )
				{
					if ( param.find( cuT( "smooth_normals" ) ) == 0 )
					{
						String strNml = cuT( "smooth" );
						parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
					}
					else if ( param.find( cuT( "flat_normals" ) ) == 0 )
					{
						String strNml = cuT( "flat" );
						parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
					}
					else if ( param.find( cuT( "tangent_space" ) ) == 0 )
					{
						parameters.add( cuT( "tangent_space" ), true );
					}
					else if ( param.find( cuT( "split_mesh" ) ) == 0 )
					{
						parameters.add( cuT( "split_mesh" ), true );
					}
					else if ( param.find( cuT( "rescale" ) ) == 0 )
					{
						auto eqIndex = param.find( cuT( '=' ) );

						if ( eqIndex != String::npos )
						{
							float value;
							string::parse< float >( param.substr( eqIndex + 1 ), value );
							parameters.add( cuT( "rescale" ), value );
						}
						else
						{
							CU_ParsingError( cuT( "Malformed parameter -rescale." ) );
						}
					}
				}
			}

			Engine * engine = parsingContext->m_pParser->getEngine();
			auto extension = string::lowerCase( pathFile.getExtension() );

			if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
			{
				CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext->mesh = parsingContext->scene->getMeshCache().add( parsingContext->strName2 );
				auto importer = engine->getImporterFactory().create( extension, *engine );

				if ( !importer->import( *parsingContext->mesh, pathFile, parameters, true ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
					parsingContext->mesh.reset();
					parsingContext->scene->getMeshCache().remove( parsingContext->strName2 );
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshMorphImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No mesh initialised." ) );
		}
		else
		{
			float timeIndex;
			params[1]->get( timeIndex );
			Path path;
			Path pathFile = context->m_file.getPath() / params[0]->get( path );
			Parameters parameters;

			if ( params.size() > 2 )
			{
				String tmp;
				StringArray arrayStrParams = string::split( params[2]->get( tmp ), cuT( "-" ), 20, false );

				if ( !arrayStrParams.empty() )
				{
					for ( StringArrayConstIt it = arrayStrParams.begin(); it != arrayStrParams.end(); ++it )
					{
						if ( it->find( cuT( "smooth_normals" ) ) == 0 )
						{
							String strNml = cuT( "smooth" );
							parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
						}
						else if ( it->find( cuT( "flat_normals" ) ) == 0 )
						{
							String strNml = cuT( "flat" );
							parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
						}
						else if ( it->find( cuT( "tangent_space" ) ) == 0 )
						{
							parameters.add( cuT( "tangent_space" ), true );
						}
					}
				}
			}

			Engine * engine = parsingContext->m_pParser->getEngine();
			auto extension = string::lowerCase( pathFile.getExtension() );

			if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
			{
				CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto importer = engine->getImporterFactory().create( extension, *engine );
				Mesh mesh{ cuT( "MorphImport" ), *parsingContext->scene };

				if ( !importer->import( mesh, pathFile, parameters, false ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
				}
				else if ( mesh.getSubmeshCount() == parsingContext->mesh->getSubmeshCount() )
				{
					String animName{ "Morph" };

					if ( !parsingContext->mesh->hasAnimation( animName ) )
					{
						auto & animation = parsingContext->mesh->createAnimation( animName );

						for ( auto submesh : *parsingContext->mesh )
						{
							animation.addChild( MeshAnimationSubmesh{ animation, *submesh } );
						}
					}

					MeshAnimation & animation{ static_cast< MeshAnimation & >( parsingContext->mesh->getAnimation( animName ) ) };
					uint32_t index = 0u;
					MeshAnimationKeyFrameUPtr keyFrame = std::make_unique< MeshAnimationKeyFrame >( animation
						, Milliseconds{ int64_t( timeIndex * 1000ll ) } );

					for ( auto & submesh : mesh )
					{
						auto & submeshAnim = animation.getSubmesh( index );
						std::clog << "Source: " << submeshAnim.getSubmesh().getPointsCount() << " - Anim: " << submesh->getPointsCount() << std::endl;


						if ( submesh->getPointsCount() == submeshAnim.getSubmesh().getPointsCount() )
						{
							keyFrame->addSubmeshBuffer( *submesh, submesh->getPoints() );
						}

						++index;
					}

					animation.addKeyFrame( std::move( keyFrame ) );
				}
				else
				{
					CU_ParsingError( cuT( "The new mesh doesn't match the original mesh" ) );
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDivide )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		Engine * engine = parsingContext->m_pParser->getEngine();

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			uint16_t count;
			params[0]->get( name );
			params[1]->get( count );

			if ( !engine->getSubdividerFactory().isTypeRegistered( string::lowerCase( name ) ) )
			{
				CU_ParsingError( cuT( "Subdivider [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto divider = engine->getSubdividerFactory().create( name );
				parsingContext->mesh->computeContainers();
				Point3f ptCenter = parsingContext->mesh->getBoundingBox().getCenter();

				for ( auto submesh : *parsingContext->mesh )
				{
					divider->subdivide( submesh, count, false );
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDefaultMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			params[0]->get( name );

			if ( cache.has( name ) )
			{
				MaterialSPtr material = cache.find( name );

				for ( auto submesh : *parsingContext->mesh )
				{
					submesh->setDefaultMaterial( material );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDefaultMaterials )
	{
	}
	CU_EndAttributePush( CSCNSection::eMeshDefaultMaterials )

	CU_ImplementAttributeParser( parserMeshEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else
		{
			if ( parsingContext->geometry )
			{
				parsingContext->geometry->setMesh( parsingContext->mesh );
			}

			parsingContext->mesh.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshDefaultMaterialsMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			uint16_t index;
			params[0]->get( index );
			params[1]->get( name );

			if ( cache.has( name ) )
			{
				if ( parsingContext->mesh->getSubmeshCount() > index )
				{
					SubmeshSPtr submesh = parsingContext->mesh->getSubmesh( index );
					MaterialSPtr material = cache.find( name );
					submesh->setDefaultMaterial( material );
				}
				else
				{
					CU_ParsingError( cuT( "Submesh index is too high" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDefaultMaterialsEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSubmeshVertex )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->vertexPos.push_back( value[0] );
			parsingContext->vertexPos.push_back( value[1] );
			parsingContext->vertexPos.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point2f value;
			params[0]->get( value );
			parsingContext->vertexTex.push_back( value[0] );
			parsingContext->vertexTex.push_back( value[1] );
			parsingContext->vertexTex.push_back( 0.0 );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->vertexTex.push_back( value[0] );
			parsingContext->vertexTex.push_back( value[1] );
			parsingContext->vertexTex.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshNormal )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->vertexNml.push_back( value[0] );
			parsingContext->vertexNml.push_back( value[1] );
			parsingContext->vertexNml.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshTangent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext->vertexTan.push_back( value[0] );
			parsingContext->vertexTan.push_back( value[1] );
			parsingContext->vertexTan.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ) );
			parsingContext->face1 = -1;
			parsingContext->face2 = -1;

			if ( arrayValues.size() >= 4 )
			{
				Point4i pt4Indices;

				if ( castor::parseValues( *parsingContext->logger, strParams, pt4Indices ) )
				{
					parsingContext->face1 = int( parsingContext->faces.size() );
					parsingContext->faces.push_back( pt4Indices[0] );
					parsingContext->faces.push_back( pt4Indices[1] );
					parsingContext->faces.push_back( pt4Indices[2] );
					parsingContext->face2 = int( parsingContext->faces.size() );
					parsingContext->faces.push_back( pt4Indices[0] );
					parsingContext->faces.push_back( pt4Indices[2] );
					parsingContext->faces.push_back( pt4Indices[3] );
				}
			}
			else if ( castor::parseValues( *parsingContext->logger, strParams, pt3Indices ) )
			{
				parsingContext->face1 = int( parsingContext->faces.size() );
				parsingContext->faces.push_back( pt3Indices[0] );
				parsingContext->faces.push_back( pt3Indices[1] );
				parsingContext->faces.push_back( pt3Indices[2] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexTex.empty() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 6 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[2] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[3] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[5] );
			}

			if ( arrayValues.size() >= 8 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[7] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexTex.empty() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[3] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[4] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[8] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[ 9] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[10] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceNormals )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexNml.empty() )
			{
				parsingContext->vertexNml.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[3] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[4] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[5] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[ 0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[ 1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[ 2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[ 6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[ 7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[ 8] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[ 9] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[10] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceTangents )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexTan.empty() )
			{
				parsingContext->vertexTan.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toFloat( arrayValues[2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[3] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[4] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toFloat( arrayValues[5] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[ 0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[ 1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toFloat( arrayValues[ 2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[ 6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[ 7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toFloat( arrayValues[ 8] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[ 9] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[10] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->vertexPos.empty() )
		{
			std::vector< InterleavedVertex > vertices{ parsingContext->vertexPos.size() / 3 };
			uint32_t index{ 0u };

			for ( auto & vertex : vertices )
			{
				std::memcpy( vertex.pos.ptr(), &parsingContext->vertexPos[index], sizeof( vertex.pos ) );

				if ( !parsingContext->vertexNml.empty() )
				{
					std::memcpy( vertex.nml.ptr(), &parsingContext->vertexNml[index], sizeof( vertex.nml ) );
				}

				if ( !parsingContext->vertexTan.empty() )
				{
					std::memcpy( vertex.tan.ptr(), &parsingContext->vertexTan[index], sizeof( vertex.tan ) );
				}

				if ( !parsingContext->vertexTex.empty() )
				{
					std::memcpy( vertex.tex.ptr(), &parsingContext->vertexTex[index], sizeof( vertex.tex ) );
				}

				index += 3;
			}

			parsingContext->submesh->addPoints( vertices );

			if ( !parsingContext->faces.empty() )
			{
				auto indices = reinterpret_cast< FaceIndices * >( &parsingContext->faces[0] );
				auto mapping = std::make_shared< TriFaceMapping >( *parsingContext->submesh );
				mapping->addFaceGroup( indices, indices + ( parsingContext->faces.size() / 3 ) );

				if ( !parsingContext->vertexNml.empty() )
				{
					if ( parsingContext->vertexTan.empty() )
					{
						mapping->computeTangentsFromNormals();
					}
				}
				else
				{
					mapping->computeNormals();
				}

				parsingContext->submesh->setIndexMapping( mapping );
			}

			parsingContext->vertexPos.clear();
			parsingContext->vertexNml.clear();
			parsingContext->vertexTan.clear();
			parsingContext->vertexTex.clear();
			parsingContext->faces.clear();
			parsingContext->submesh->getParent().getScene()->getListener().postEvent( makeInitialiseEvent( *parsingContext->submesh ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMaterialPass )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->strName.clear();

		if ( parsingContext->material )
		{
			if ( parsingContext->createMaterial
				|| parsingContext->material->getPassCount() < parsingContext->passIndex )
			{
				parsingContext->pass = parsingContext->material->createPass();
				parsingContext->createPass = true;

			}
			else
			{
				parsingContext->pass = parsingContext->material->getPass( parsingContext->passIndex );
				parsingContext->createPass = false;
			}

			switch ( parsingContext->pass->getType() )
			{
			case MaterialType::ePhong:
				parsingContext->phongPass = std::static_pointer_cast< PhongPass >( parsingContext->pass );
				break;

			case MaterialType::eMetallicRoughness:
				parsingContext->pbrMRPass = std::static_pointer_cast< MetallicRoughnessPbrPass >( parsingContext->pass );
				break;

			case MaterialType::eSpecularGlossiness:
				parsingContext->pbrSGPass = std::static_pointer_cast< SpecularGlossinessPbrPass >( parsingContext->pass );
				break;

			default:
				CU_ParsingError( "Unsupported pass type." );
				break;
			}

			++parsingContext->passIndex;
			parsingContext->unitIndex = 0u;
			parsingContext->createUnit = true;
		}
		else
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::ePass )

	CU_ImplementAttributeParser( parserMaterialEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->material )
		{
			auto material = parsingContext->material;

			if ( material->getPassCount() == 1u )
			{
				auto pass = *material->begin();

				if ( pass->hasAlphaBlending()
					&& parsingContext->bBool1
					&& checkFlag( pass->getTextures(), TextureFlag::eOpacity ) )
				{
					parsingContext->bBool1 = false;

					// Create a new pass with alpha rejection
					switch ( pass->getType() )
					{
					case MaterialType::ePhong:
						createAlphaRejectionPass( material, std::static_pointer_cast< PhongPass >( pass ) );
						break;
					case MaterialType::eMetallicRoughness:
						createAlphaRejectionPass( material, std::static_pointer_cast< MetallicRoughnessPbrPass >( pass ) );
						break;
					case MaterialType::eSpecularGlossiness:
						createAlphaRejectionPass( material, std::static_pointer_cast< SpecularGlossinessPbrPass >( pass ) );
						break;
					default:
						break;
					}
				}
			}
		}
		else
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPassDiffuse )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->phongPass
			&& !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Phong or Specular/Glossiness PBR)." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->phongPass )
			{
				RgbColour crColour;
				params[0]->get( crColour );
				parsingContext->phongPass->setDiffuse( crColour );
			}
			else
			{
				RgbColour crColour;
				params[0]->get( crColour );
				parsingContext->pbrSGPass->setDiffuse( crColour );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassSpecular )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->phongPass
			&& !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Phong or Specular/Glossiness PBR)." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->phongPass )
			{
				RgbColour crColour;
				params[0]->get( crColour );
				parsingContext->phongPass->setSpecular( crColour );
			}
			else
			{
				RgbColour crColour;
				params[0]->get( crColour );
				parsingContext->pbrSGPass->setSpecular( crColour );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAmbient )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->phongPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Phong)." ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext->phongPass->setAmbient( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassEmissive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext->pass->setEmissive( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassShininess )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->phongPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Phong)." ) );
		}
		else if ( !params.empty() )
		{
			float fFloat;
			params[0]->get( fFloat );
			parsingContext->phongPass->setShininess( fFloat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlbedo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->pbrMRPass
			&& !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected PBR)." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->pbrMRPass )
			{
				RgbColour crColour;
				params[0]->get( crColour );
				parsingContext->pbrMRPass->setAlbedo( crColour );
			}
			else
			{
				RgbColour crColour;
				params[0]->get( crColour );
				parsingContext->pbrSGPass->setDiffuse( crColour );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassRoughness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->pbrMRPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Metallic/Roughness PBR)." ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext->pbrMRPass->setRoughness( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassMetallic )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->pbrMRPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Metallic/Roughness PBR)." ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext->pbrMRPass->setMetallic( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassGlossiness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "Wrong pass type (expected Specular/Glossiness PBR)." ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext->pbrSGPass->setGlossiness( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlpha )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			float fFloat;
			params[0]->get( fFloat );
			parsingContext->pass->setOpacity( fFloat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassDoubleFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext->pass->setTwoSided( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassTextureUnit )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->textureUnit.reset();

		if ( parsingContext->pass )
		{
			if ( parsingContext->createPass
				|| parsingContext->pass->getTextureUnitsCount() < parsingContext->unitIndex )
			{
				parsingContext->textureUnit = std::make_shared< TextureUnit >( *parsingContext->m_pParser->getEngine() );
				parsingContext->createUnit = true;
				parsingContext->imageInfo->mipLevels = ~uint32_t( 0u );
			}
			else
			{
				parsingContext->createUnit = false;
				parsingContext->textureUnit = parsingContext->pass->getTextureUnit( parsingContext->unitIndex );
			}

			++parsingContext->unitIndex;
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextureUnit )

	CU_ImplementAttributeParser( parserPassShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderProgram.reset();
		parsingContext->shaderStage = VkShaderStageFlagBits( 0u );

		if ( parsingContext->pass )
		{
			parsingContext->shaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( parsingContext->material->getName() + castor::string::toString( parsingContext->pass->getId() )
				, true );
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderProgram )

	CU_ImplementAttributeParser( parserPassMixedInterpolative )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			parsingContext->bBool1 = true;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlphaBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t mode = 0;
			params[0]->get( mode );
			parsingContext->pass->setAlphaBlendMode( BlendMode( mode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassColourBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t mode = 0;
			params[0]->get( mode );
			parsingContext->pass->setColourBlendMode( BlendMode( mode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlphaFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiFunc;
			float fFloat;
			params[0]->get( uiFunc );
			params[1]->get( fFloat );
			parsingContext->pass->setAlphaFunc( VkCompareOp( uiFunc ) );
			parsingContext->pass->setAlphaValue( fFloat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassRefractionRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			float value = 0;
			params[0]->get( value );
			parsingContext->pass->setRefractionRatio( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassSubsurfaceScattering )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else
		{
			parsingContext->subsurfaceScattering = std::make_unique< SubsurfaceScattering >();
		}
	}
	CU_EndAttributePush( CSCNSection::eSubsurfaceScattering )

	CU_ImplementAttributeParser( parserPassParallaxOcclusion )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value = false;
			params[0]->get( value );
			parsingContext->pass->setParallaxOcclusion( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassBWAccumulationOperator )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value = 0u;
			params[0]->get( value );
			parsingContext->pass->setBWAccumulationOperator( uint8_t( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else
		{
			parsingContext->pass.reset();
			parsingContext->phongPass.reset();
			parsingContext->pbrMRPass.reset();
			parsingContext->pbrSGPass.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserUnitImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !params.empty() )
		{
			Path folder;
			Path relative;
			params[0]->get( relative );

			if ( File::fileExists( context->m_file.getPath() / relative ) )
			{
				folder = context->m_file.getPath();
			}
			else if ( !File::fileExists( relative ) )
			{
				CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				parsingContext->textureUnit->setAutoMipmaps( true );
				parsingContext->folder = folder;
				parsingContext->relative = relative;
				parsingContext->strName.clear();
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitLevelsCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->imageInfo->mipLevels );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else
		{
			parsingContext->renderTarget = parsingContext->m_pParser->getEngine()->getRenderTargetCache().add( TargetType::eTexture );
		}
	}
	CU_EndAttributePush( CSCNSection::eRenderTarget )

	CU_ImplementAttributeParser( parserUnitChannel )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t flags;
			params[0]->get( flags );
			auto textures = TextureFlags( uint16_t( flags ) );

			if ( checkFlag( textures, TextureFlag::eDiffuse ) )
			{
				parsingContext->textureConfiguration.colourMask[0] = 0x00FFFFFF;
			}

			if ( checkFlag( textures, TextureFlag::eNormal ) )
			{
				parsingContext->textureConfiguration.normalMask[0] = 0x00FFFFFF;
			}

			if ( checkFlag( textures, TextureFlag::eOpacity ) )
			{
				parsingContext->textureConfiguration.opacityMask[0] = 0xFF000000;
			}

			if ( checkFlag( textures, TextureFlag::eSpecular ) )
			{
				switch ( parsingContext->pass->getType() )
				{
				case MaterialType::ePhong:
				case MaterialType::eSpecularGlossiness:
					parsingContext->textureConfiguration.specularMask[0] = 0x00FFFFFF;
					break;
				case MaterialType::eMetallicRoughness:
					parsingContext->textureConfiguration.specularMask[0] = 0x000000FF;
					break;
				default:
					break;
				}
			}

			if ( checkFlag( textures, TextureFlag::eHeight ) )
			{
				parsingContext->textureConfiguration.heightMask[0] = 0x000000FF;
			}

			if ( checkFlag( textures, TextureFlag::eGlossiness ) )
			{
				parsingContext->textureConfiguration.glossinessMask[0] = 0x000000FF;
			}

			if ( checkFlag( textures, TextureFlag::eEmissive ) )
			{
				parsingContext->textureConfiguration.emissiveMask[0] = 0x00FFFFFF;
			}

			if ( checkFlag( textures, TextureFlag::eReflection ) )
			{
				parsingContext->textureConfiguration.environment |= TextureConfiguration::ReflectionMask;
			}

			if ( checkFlag( textures, TextureFlag::eRefraction ) )
			{
				parsingContext->textureConfiguration.environment |= TextureConfiguration::RefractionMask;
			}

			if ( checkFlag( textures, TextureFlag::eOcclusion ) )
			{
				parsingContext->textureConfiguration.occlusionMask[0] = 0x000000FF;
			}

			if ( checkFlag( textures, TextureFlag::eTransmittance ) )
			{
				parsingContext->textureConfiguration.transmittanceMask[0] = 0xFF000000;
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitAlbedoMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() == MaterialType::ePhong )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.colourMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitDiffuseMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() != MaterialType::ePhong )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.colourMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitNormalMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.normalMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitOpacityMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.opacityMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitMetalnessMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() != MaterialType::eMetallicRoughness )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.specularMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitSpecularMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() == MaterialType::eMetallicRoughness )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.specularMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitRoughnessMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() != MaterialType::eMetallicRoughness )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.glossinessMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitGlossinessMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() != MaterialType::eSpecularGlossiness )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.glossinessMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitShininessMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( parsingContext->pass->getType() != MaterialType::ePhong )
		{
			CU_ParsingError( cuT( "Wrong mask for pass type." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.glossinessMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitEmissiveMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.emissiveMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitHeightMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.heightMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitOcclusionMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.occlusionMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitTransmittanceMask )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.transmittanceMask[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitNormalFactor )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.normalFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitHeightFactor )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext->textureConfiguration.heightFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitNormalDirectX )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool isDirectX;
			params[0]->get( isDirectX );
			parsingContext->textureConfiguration.normalGMultiplier = isDirectX
				? -1.0f
				: 1.0f;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitReflection )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			params[0]->get( value );
			parsingContext->textureConfiguration.environment = value
				? TextureConfiguration::ReflectionMask
				: 0x00u;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitRefraction )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			params[0]->get( value );
			parsingContext->textureConfiguration.environment = value
				? TextureConfiguration::RefractionMask
				: 0x00u;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			SamplerSPtr sampler = parsingContext->m_pParser->getEngine()->getSamplerCache().find( params[0]->get( name ) );

			if ( sampler )
			{
				parsingContext->textureUnit->setSampler( sampler );
			}
			else
			{
				CU_ParsingError( cuT( "Unknown sampler : [" ) + name + cuT( "]" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->pass )
		{
			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "TextureUnit not initialised" ) );
			}
			else
			{
				if ( parsingContext->textureUnit->getRenderTarget()
					|| parsingContext->textureConfiguration.environment != 0u )
				{
					parsingContext->textureUnit->setConfiguration( parsingContext->textureConfiguration );
					parsingContext->pass->addTextureUnit( parsingContext->textureUnit );
				}
				else if ( parsingContext->folder.empty() && parsingContext->relative.empty() )
				{
					CU_ParsingError( cuT( "TextureUnit's image not initialised" ) );
				}
				else
				{
					if ( parsingContext->imageInfo->mipLevels == ~uint32_t( 0u ) )
					{
						parsingContext->imageInfo->mipLevels = 20;
					}

					if ( parsingContext->createUnit
						&& getUsedImageComponents( parsingContext->textureConfiguration ) != TextureFlag::eNone )
					{
						auto texture = std::make_shared< TextureLayout >( *parsingContext->m_pParser->getEngine()->getRenderSystem()
							, parsingContext->imageInfo
							, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
							, parsingContext->relative );
						texture->setSource( parsingContext->folder
							, parsingContext->relative );
						parsingContext->textureUnit->setTexture( texture );
						parsingContext->textureUnit->setConfiguration( parsingContext->textureConfiguration );
						parsingContext->pass->addTextureUnit( parsingContext->textureUnit );
					}
				}

				parsingContext->imageInfo =
				{
					0u,
					VK_IMAGE_TYPE_2D,
					VK_FORMAT_UNDEFINED,
					{ 1u, 1u, 1u },
					1u,
					1u,
					VK_SAMPLE_COUNT_1_BIT,
					VK_IMAGE_TILING_OPTIMAL,
					VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
				};
				parsingContext->textureConfiguration = TextureConfiguration{};
			}
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserVertexShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserPixelShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserGeometryShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserHullShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserdomainShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserComputeShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserConstantsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( parsingContext->strName );

			if ( parsingContext->strName.empty() )
			{
				CU_ParsingError( cuT( "Invalid empty name" ) );
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderUBO )

	CU_ImplementAttributeParser( parserShaderEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else
		{
			if ( parsingContext->particleSystem )
			{
				parsingContext->particleSystem->setCSUpdateProgram( parsingContext->shaderProgram );
				parsingContext->bBool1 = false;
			}

			parsingContext->shaderProgram.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserShaderProgramFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->shaderStage != VkShaderStageFlagBits( 0u ) )
			{
				Path path;
				params[0]->get( path );
				parsingContext->shaderProgram->setFile( parsingContext->shaderStage
					, context->m_file.getPath() / path );
			}
			else
			{
				CU_ParsingError( cuT( "Shader not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderGroupSizes )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext->shaderStage != VkShaderStageFlagBits( 0u ) )
			{
				Point3i sizes;
				params[0]->get( sizes );
				parsingContext->particleSystem->setCSGroupSizes( sizes );
			}
			else
			{
				CU_ParsingError( cuT( "Shader not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderUboShaders )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );

			if ( value )
			{
				//parsingContext->uniformBuffer = std::make_unique< UniformBuffer >( parsingContext->strName
				//	, *parsingContext->shaderProgram->getRenderSystem()
				//	, 1u );
			}
			else
			{
				CU_ParsingError( cuT( "Unsupported shader type" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderUboVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		params[0]->get( parsingContext->strName2 );

		if ( !parsingContext->uniformBuffer )
		{
			CU_ParsingError( cuT( "Shader constants buffer not initialised" ) );
		}
		else if ( parsingContext->strName2.empty() )
		{
			CU_ParsingError( cuT( "Invalid empty name" ) );
		}
		else
		{
			parsingContext->uiUInt32 = 1;
		}
	}
	CU_EndAttributePush( CSCNSection::eUBOVariable )

	CU_ImplementAttributeParser( parserShaderVariableCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		uint32_t param;
		params[0]->get( param );

		if ( parsingContext->uniformBuffer )
		{
			parsingContext->uiUInt32 = param;
		}
		else
		{
			CU_ParsingError( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderVariableType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		uint32_t uiType;
		params[0]->get( uiType );

		if ( parsingContext->uniformBuffer )
		{
			//if ( !parsingContext->uniform )
			//{
			//	parsingContext->uniform = parsingContext->uniformBuffer->createUniform( UniformType( uiType ), parsingContext->strName2, parsingContext->uiUInt32 );
			//}
			//else
			//{
			//	CU_ParsingError( cuT( "Variable type already set" ) );
			//}
		}
		else
		{
			CU_ParsingError( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderVariableValue )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String strParams;
		params[0]->get( strParams );

		//if ( parsingContext->uniform )
		//{
		//	parsingContext->uniform->setStrValue( strParams );
		//}
		//else
		//{
		//	CU_ParsingError( cuT( "Variable not initialised" ) );
		//}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		params[0]->get( parsingContext->path );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontHeight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		params[0]->get( parsingContext->iInt16 );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->strName.empty() && !parsingContext->path.empty() )
		{
			if ( parsingContext->scene )
			{
				parsingContext->scene->getFontView().add( parsingContext->strName, parsingContext->iInt16, context->m_file.getPath() / parsingContext->path );
			}
			else
			{
				parsingContext->m_pParser->getEngine()->getFontCache().add( parsingContext->strName, parsingContext->iInt16, context->m_file.getPath() / parsingContext->path );
			}
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserOverlayPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->overlay )
		{
			Point2d ptPosition;
			params[0]->get( ptPosition );
			parsingContext->overlay->setPosition( ptPosition );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlaySize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->overlay )
		{
			Point2d ptSize;
			params[0]->get( ptSize );
			parsingContext->overlay->setSize( ptSize );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPixelSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->overlay )
		{
			Size size;
			params[0]->get( size );
			parsingContext->overlay->setPixelSize( size );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPixelPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->overlay )
		{
			Position position;
			params[0]->get( position );
			parsingContext->overlay->setPixelPosition( position );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->overlay )
		{
			String name;
			params[0]->get( name );
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			parsingContext->overlay->setMaterial( cache.find( name ) );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( params[0]->get( name ), OverlayType::ePanel, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserOverlayBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserOverlayTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( params[0]->get( name ), OverlayType::eText, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserOverlayEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->overlay->getType() == OverlayType::eText )
		{
			auto textOverlay = parsingContext->overlay->getTextOverlay();

			if ( textOverlay->getFontTexture() )
			{
				parsingContext->overlay->setVisible( true );
			}
			else
			{
				parsingContext->overlay->setVisible( false );
				CU_ParsingError( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			parsingContext->overlay->setVisible( true );
		}

		parsingContext->overlay = parsingContext->overlay->getParent();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPanelOverlayUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::ePanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getPanelOverlay()->setUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlaySizes )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d ptSize;
			params[0]->get( ptSize );
			overlay->getBorderPanelOverlay()->setBorderSize( ptSize );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayPixelSizes )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Rectangle size;
			params[0]->get( size );
			overlay->getBorderPanelOverlay()->setBorderPixelSize( size );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			String name;
			params[0]->get( name );
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			overlay->getBorderPanelOverlay()->setBorderMaterial( cache.find( name ) );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			uint32_t position;
			params[0]->get( position );
			overlay->getBorderPanelOverlay()->setBorderPosition( BorderPosition( position ) );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayCenterUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getBorderPanelOverlay()->setUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayOuterUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getBorderPanelOverlay()->setBorderOuterUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayInnerUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getBorderPanelOverlay()->setBorderInnerUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getFontCache();
			String name;
			params[0]->get( name );

			if ( cache.find( name ) )
			{
				overlay->getTextOverlay()->setFont( name );
			}
			else
			{
				CU_ParsingError( cuT( "Unknown font" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayTextWrapping )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setTextWrappingMode( TextWrappingMode( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayVerticalAlign )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setVAlign( VAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayHorizontalAlign )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setHAlign( HAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayTexturingMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setTexturingMode( TextTexturingMode( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayLineSpacingMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setLineSpacingMode( TextLineSpacingMode( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayText )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		OverlaySPtr overlay = parsingContext->overlay;
		String strParams;
		params[0]->get( strParams );

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			string::replace( strParams, cuT( "\\n" ), cuT( "\n" ) );
			overlay->getTextOverlay()->setCaption( strParams );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		SceneNodeSPtr parent = parsingContext->scene->getSceneNodeCache().find( params[0]->get( name ) );

		if ( parent )
		{
			parsingContext->sceneNode = parent;

			while ( parent->getParent()
				&& parent->getParent() != parsingContext->scene->getObjectRootNode().get()
				&& parent->getParent() != parsingContext->scene->getCameraRootNode().get() )
			{
				parent = parent->getParent()->shared_from_this();
			}

			if ( !parent->getParent()
				|| parent->getParent() == parsingContext->scene->getObjectRootNode().get() )
			{
				parent->attachTo( *parsingContext->scene->getCameraRootNode() );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraViewport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->viewport = std::make_shared< Viewport >( *parsingContext->m_pParser->getEngine() );
		parsingContext->viewport->setPerspective( 0.0_degrees, 1, 0, 1 );
	}
	CU_EndAttributePush( CSCNSection::eViewport )

	CU_ImplementAttributeParser( parserCameraPrimitive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !params.empty() )
		{
			uint32_t uiType;
			parsingContext->primitiveType = VkPrimitiveTopology( params[0]->get( uiType ) );
		}
		else
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->sceneNode && parsingContext->viewport )
		{
			parsingContext->scene->getCameraCache().add( parsingContext->strName
				, *parsingContext->sceneNode
				, std::move( *parsingContext->viewport ) );
			parsingContext->viewport.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserViewportType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !params.empty() )
		{
			uint32_t uiType;
			parsingContext->viewport->updateType( ViewportType( params[0]->get( uiType ) ) );
		}
		else
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext->viewport->updateLeft( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext->viewport->updateRight( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext->viewport->updateTop( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext->viewport->updateBottom( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportNear )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext->viewport->updateNear( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFar )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext->viewport->updateFar( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		Size size;
		params[0]->get( size );
		parsingContext->viewport->resize( size );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFovY )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float fFovY;
		params[0]->get( fFovY );
		parsingContext->viewport->updateFovY( Angle::fromDegrees( fFovY ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportAspectRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float fRatio;
		params[0]->get( fRatio );
		parsingContext->viewport->updateRatio( fRatio );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->billboards )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr pParent = parsingContext->scene->getSceneNodeCache().find( name );

			if ( pParent )
			{
				pParent->attachObject( *parsingContext->billboards );
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Geometry not initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->billboards )
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );

			parsingContext->billboards->setBillboardType( BillboardType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->billboards )
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );

			parsingContext->billboards->setBillboardSize( BillboardSize( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardPositions )
	{
	}
	CU_EndAttributePush( CSCNSection::eBillboardList )

	CU_ImplementAttributeParser( parserBillboardMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->billboards )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			params[0]->get( name );

			if ( cache.has( name ) )
			{
				parsingContext->billboards->setMaterial( cache.find( name ) );
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardDimensions )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		Point2f dimensions;
		params[0]->get( dimensions );
		parsingContext->billboards->setDimensions( dimensions );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		parsingContext->scene->getBillboardListCache().add( parsingContext->billboards );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserBillboardPoint )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		Point3f position;
		params[0]->get( position );
		parsingContext->billboards->addPoint( position );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext->pAnimGroup )
		{
			GeometrySPtr geometry = parsingContext->scene->getGeometryCache().find( name );

			if ( geometry )
			{
				if ( !geometry->getAnimations().empty() )
				{
					parsingContext->pAnimMovable = parsingContext->pAnimGroup->addObject( *geometry
						, geometry->getName() + cuT( "_Movable" ) );
				}

				if ( geometry->getMesh() )
				{
					auto mesh = geometry->getMesh();

					if ( !mesh->getAnimations().empty() )
					{
						parsingContext->pAnimMesh = parsingContext->pAnimGroup->addObject( *mesh
							, *geometry
							, geometry->getName() + cuT( "_Mesh" ) );
					}

					auto skeleton = mesh->getSkeleton();

					if ( skeleton )
					{
						if ( !skeleton->getAnimations().empty() )
						{
							parsingContext->pAnimSkeleton = parsingContext->pAnimGroup->addObject( *skeleton
								, *mesh
								, *geometry
								, geometry->getName() + cuT( "_Skeleton" ) );
						}
					}
				}
			}
			else
			{
				CU_ParsingError( cuT( "No geometry with name " ) + name );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		params[0]->get( parsingContext->strName2 );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->addAnimation( parsingContext->strName2 );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eAnimation )

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimationStart )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->startAnimation( name );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimationPause )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->pauseAnimation( name );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup.reset();
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserAnimationLooped )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		bool value;
		params[0]->get( value );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->setAnimationLooped( parsingContext->strName2, value );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimationScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );
		float value;
		params[0]->get( value );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->setAnimationscale( parsingContext->strName2, value );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimationEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSkyboxEqui )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.size() <= 1 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext->skybox )
		{
			CU_ParsingError( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path path;
			Path filePath = context->m_file.getPath();
			params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				uint32_t size;
				params[1]->get( size );
				parsingContext->skybox->loadEquiTexture( filePath, path, size );
			}
			else
			{
				CU_ParsingError( cuT( "Couldn't load the image" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxCross )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( params.size() < 1 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext->skybox )
		{
			CU_ParsingError( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path path;
			Path filePath = context->m_file.getPath();
			params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				parsingContext->skybox->loadCrossTexture( filePath, path );
			}
			else
			{
				CU_ParsingError( cuT( "Couldn't load the image" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadLeftImage( context->m_file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadRightImage( context->m_file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadTopImage( context->m_file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadBottomImage( context->m_file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxFront )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadFrontImage( context->m_file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBack )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadBackImage( context->m_file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->skybox )
		{
			parsingContext->scene->setBackground( std::move( parsingContext->skybox ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSsaoEnabled )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext->ssaoConfig.enabled = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoHighQuality )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext->ssaoConfig.highQuality = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoUseNormalsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext->ssaoConfig.useNormalsBuffer = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoRadius )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext->ssaoConfig.radius = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext->ssaoConfig.bias = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext->ssaoConfig.intensity = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoNumSamples )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext->ssaoConfig.numSamples = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoEdgeSharpness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext->ssaoConfig.edgeSharpness = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBlurStepSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext->ssaoConfig.blurStepSize = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBlurHighQuality )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext->ssaoConfig.blurHighQuality = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBlurRadius )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext->ssaoConfig.blurRadius = makeRangedValue( int32_t( value ), 1, 6 );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( parsingContext->renderTarget )
		{
			parsingContext->renderTarget->setSsaoConfig( parsingContext->ssaoConfig );
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringStrength )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->subsurfaceScattering->setStrength( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringGaussianWidth )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->subsurfaceScattering->setGaussianWidth( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringTransmittanceProfile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eTransmittanceProfile )

	CU_ImplementAttributeParser( parserSubsurfaceScatteringEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else
		{
			parsingContext->pass->setSubsurfaceScattering( std::move( parsingContext->subsurfaceScattering ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserTransmittanceProfileFactor )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			Point4f value;
			params[0]->get( value );
			parsingContext->subsurfaceScattering->addProfileFactor( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHdrExponent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No render target initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->renderTarget->setExposure( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHdrGamma )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No render target initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext->renderTarget->setGamma( value );
		}
	}
	CU_EndAttribute()
}
