#include "SceneFileParser_Parsers.hpp"

#include "Engine.hpp"
#include "Cache/BillboardCache.hpp"
#include "Cache/GeometryCache.hpp"
#include "Cache/LightCache.hpp"
#include "Cache/SceneNodeCache.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Animation/Mesh/MeshAnimationKeyFrame.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Cache/CacheView.hpp"
#include "Material/Material.hpp"
#include "Material/LegacyPass.hpp"
#include "Material/MetallicRoughnessPbrPass.hpp"
#include "Material/SpecularGlossinessPbrPass.hpp"
#include "Material/SubsurfaceScattering.hpp"
#include "Mesh/SubmeshComponent/Face.hpp"
#include "Mesh/Importer.hpp"
#include "Mesh/Mesh.hpp"
#include "Mesh/Subdivider.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Overlay/BorderPanelOverlay.hpp"
#include "Overlay/Overlay.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Plugin/Plugin.hpp"
#include "Plugin/DividerPlugin.hpp"
#include "Plugin/ImporterPlugin.hpp"
#include "Plugin/PostFxPlugin.hpp"
#include "Plugin/TechniquePlugin.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderTarget.hpp"
#include "Render/RenderWindow.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Geometry.hpp"
#include "Scene/Scene.hpp"
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Background/Image.hpp"
#include "Scene/Background/Skybox.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/Program.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	CU_ImplementAttributeParser( parserRootScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			parsingContext->scene = parsingContext->m_pParser->getEngine()->getSceneCache().add( name );
			parsingContext->mapScenes.insert( std::make_pair( name, parsingContext->scene ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->path.clear();

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eFont )

	CU_ImplementAttributeParser( parserRootMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->strName );
			parsingContext->material = parsingContext->m_pParser->getEngine()->getMaterialCache().add( parsingContext->strName
				, MaterialType::eLegacy );
		}
	}
	CU_EndAttributePush( CSCNSection::eMaterial )

	CU_ImplementAttributeParser( parserRootMaterials )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->m_pParser->getEngine()->setMaterialsType( MaterialType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRootPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::ePanel, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserRootBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eBorderPanel, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserRootTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eText, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserRootSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->sampler = parsingContext->m_pParser->getEngine()->getSamplerCache().add( p_params[0]->get( name ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSampler )

	CU_ImplementAttributeParser( parserRootDebugOverlays )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			parsingContext->m_pParser->getEngine()->getRenderLoop().showDebugOverlays( p_params[0]->get( value ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootWindow )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->window )
			{
				CU_ParsingError( cuT( "Can't create more than one render window" ) );
			}
			else
			{
				String name;
				p_params[0]->get( name );
				parsingContext->window = parsingContext->scene->getEngine()->getRenderWindowCache().add( name );
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eWindow )

	CU_ImplementAttributeParser( parserWindowRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->window )
		{
			CU_ParsingError( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->window->enableVSync( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowFullscreen )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->window )
		{
			CU_ParsingError( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{

			bool value;
			p_params[0]->get( value );
			parsingContext->window->setFullscreen( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			ScenePtrStrMap::iterator it = parsingContext->mapScenes.find( p_params[0]->get( name ) );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->renderTarget->getScene() )
			{
				parsingContext->renderTarget->setCamera( parsingContext->renderTarget->getScene()->getCameraCache().find( p_params[0]->get( name ) ) );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Size size;
			parsingContext->renderTarget->setSize( p_params[0]->get( size ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetFormat )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			PixelFormat ePF;
			p_params[0]->get( ePF );

			if ( ePF < PixelFormat::eD16 )
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rIntraOcularDistance;
			p_params[0]->get( rIntraOcularDistance );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			Parameters parameters;

			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.parse( p_params[1]->get( tmp ) );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			Parameters parameters;

			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.parse( p_params[1]->get( tmp ) );
			}

			parsingContext->renderTarget->setToneMappingType( name, parameters );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetSsao )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No render target initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSsao )
		
	CU_ImplementAttributeParser( parserRenderTargetHdrConfig )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eHdrConfig )

	CU_ImplementAttributeParser( parserRenderTargetEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setMinFilter( ashes::Filter( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMagFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setMagFilter( ashes::Filter( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMipFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setMipFilter( ashes::MipmapMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMinLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = -1000;
			p_params[0]->get( rValue );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setWrapS( ashes::WrapMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerVWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setWrapT( ashes::WrapMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerWWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setWrapR( ashes::WrapMode( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerBorderColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t colour;
			p_params[0]->get( colour );
			parsingContext->sampler->setBorderColour( ashes::BorderColour( colour ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerAnisotropicFiltering )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->sampler->enableAnisotropicFiltering( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMaxAnisotropy )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );
			parsingContext->sampler->setMaxAnisotropy( rValue );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerComparisonMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode;
			p_params[0]->get( mode );
			parsingContext->sampler->enableCompare( bool( mode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerComparisonFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setCompareOp( ashes::CompareOp( uiMode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneInclude )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path path;
			p_params[0]->get( path );
			SceneFileParser parser{ *parsingContext->m_pParser->getEngine() };
			parser.parseFile( parsingContext->m_file.getPath() / path, parsingContext );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBkColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour clrBackground;
			p_params[0]->get( clrBackground );
			parsingContext->scene->setBackgroundColour( clrBackground );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBkImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			auto imgBackground = std::make_shared< ImageBackground >( *parsingContext->m_pParser->getEngine()
				, *parsingContext->scene );
			Path path;
			imgBackground->loadImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
			parsingContext->scene->setBackground( imgBackground );
			parsingContext->skybox.reset();
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
			parsingContext->path.clear();
			p_params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eFont )

	CU_ImplementAttributeParser( parserSceneMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->strName );
			auto & view = parsingContext->scene->getMaterialView();
			parsingContext->createMaterial = !view.has( parsingContext->strName );
			parsingContext->material = view.add( parsingContext->strName, parsingContext->scene->getMaterialsType() );
			parsingContext->passIndex = 0u;
		}
	}
	CU_EndAttributePush( CSCNSection::eMaterial )

	CU_ImplementAttributeParser( parserSceneSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->sampler = parsingContext->scene->getSamplerView().add( p_params[0]->get( name ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSampler )

	CU_ImplementAttributeParser( parserSceneCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->viewport.reset();
		parsingContext->sceneNode.reset();

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			parsingContext->sceneNode.reset();
			p_params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eCamera )

	CU_ImplementAttributeParser( parserSceneLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->light.reset();
		parsingContext->strName.clear();
		parsingContext->sceneNode.reset();

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( parsingContext->strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eLight )

	CU_ImplementAttributeParser( parserSceneCameraNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->sceneNode.reset();
		String name;

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().add( name, parsingContext->scene->getCameraRootNode() );
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneSceneNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->sceneNode.reset();
		String name;

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().add( name, parsingContext->scene->getObjectRootNode() );
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->geometry.reset();
		String name;

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->geometry = std::make_shared< Geometry >( name
				, *parsingContext->scene
				, nullptr
				, nullptr );
		}
	}
	CU_EndAttributePush( CSCNSection::eObject )

	CU_ImplementAttributeParser( parserSceneAmbientLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour clColour;
			p_params[0]->get( clColour );
			parsingContext->scene->setAmbientLight( clColour );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Path path;
		Path pathFile = p_context->m_file.getPath() / p_params[0]->get( path );

		Engine * engine = parsingContext->m_pParser->getEngine();
		auto extension = string::lowerCase( pathFile.getExtension() );

		if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
		{
			CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
		}
		else
		{
			auto importer = engine->getImporterFactory().create( extension, *engine );
			parsingContext->scene->importExternal( pathFile, *importer );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBillboard )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			parsingContext->billboards = std::make_shared< BillboardList >( name, *parsingContext->scene, nullptr );
		}
	}
	CU_EndAttributePush( CSCNSection::eBillboard )

	CU_ImplementAttributeParser( parserSceneAnimatedObjectGroup )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			parsingContext->pAnimGroup = parsingContext->scene->getAnimatedObjectGroupCache().add( name );
		}
	}
	CU_EndAttributePush( CSCNSection::eAnimGroup )

	CU_ImplementAttributeParser( parserScenePanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( p_params[0]->get( name )
				, OverlayType::ePanel
				, parsingContext->scene
				, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserSceneBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( p_params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->scene, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserSceneTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( p_params[0]->get( name ), OverlayType::eText, parsingContext->scene, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserSceneSkybox )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->scene->getFog().setType( FogType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFogDensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->scene->getFog().setDensity( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneParticleSystem )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			p_params[0]->get( value );
			parsingContext->strName = value;
			parsingContext->uiUInt32 = 0u;
			parsingContext->sceneNode.reset();
			parsingContext->material.reset();
		}
	}
	CU_EndAttributePush( CSCNSection::eParticleSystem )

	CU_ImplementAttributeParser( parserMesh )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->bBool1 = false;
		p_params[0]->get( parsingContext->strName2 );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->scene->setDirectionalShadowCascades( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->uiUInt32 = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->point2f );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemParticle )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		
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

			parsingContext->particleSystem = parsingContext->scene->getParticleSystemCache().add( parsingContext->strName, parsingContext->sceneNode, parsingContext->uiUInt32 );
			parsingContext->particleSystem->setMaterial( parsingContext->material );
			parsingContext->particleSystem->setDimensions( parsingContext->point2f );
		}
	}
	CU_EndAttributePush( CSCNSection::eParticle )

	CU_ImplementAttributeParser( parserParticleSystemCSShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderProgram.reset();
		parsingContext->shaderStage = ashes::ShaderStageFlag( 0u );
		parsingContext->bBool1 = false;
		
		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->shaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderProgram )

	CU_ImplementAttributeParser( parserParticleType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->particleSystem )
		{
			CU_ParsingError( cuT( "Particle system not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->strName2 );

		if ( !parsingContext->particleSystem )
		{
			CU_ParsingError( cuT( "Particle system not initialised" ) );
		}
		else if ( p_params.size() < 2 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			uint32_t type;
			String value;
			p_params[0]->get( name );
			p_params[1]->get( type );

			if ( p_params.size() > 2 )
			{
				p_params[2]->get( value );
			}

			parsingContext->particleSystem->addParticleVariable( name, ParticleFormat( type ), value );
		}
	}
	CU_EndAttributePush( CSCNSection::eUBOVariable )

	CU_ImplementAttributeParser( parserLightParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->get( uiType );
			parsingContext->lightType = LightType( uiType );
			auto & cache = parsingContext->scene->getLightCache();
			parsingContext->light = cache.add( parsingContext->strName
				, parsingContext->sceneNode
				, parsingContext->lightType );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->light->setColour( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f value;
			p_params[0]->get( value );
			parsingContext->light->setIntensity( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightAttenuation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShadows )

	CU_ImplementAttributeParser( parserShadowsProducer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->light->setShadowProducer( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->light->setShadowType( ShadowType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVolumetricSteps )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->light->setVolumetricSteps( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVolumetricScatteringFactor )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->light->setVolumetricScatteringFactor( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsMinOffset )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->light->setShadowMinOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsMaxSlopeOffset )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->light->setShadowMaxSlopeOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVarianceMax )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->light->setShadowMaxVariance( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVarianceBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->light->setShadowVarianceBias( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
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
				parsingContext->sceneNode->attachTo( parent );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->sceneNode->setPosition( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeOrientation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r axis;
			float angle;
			p_params[0]->get( axis );
			p_params[1]->get( angle );
			parsingContext->sceneNode->setOrientation( Quaternion::fromAxisAngle( axis, Angle::fromDegrees( angle ) ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeDirection )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r direction;
			p_params[0]->get( direction );
			Point3r up{ 0, 1, 0 };
			Point3r right{ point::cross( direction, up ) };
			parsingContext->sceneNode->setOrientation( Quaternion::fromAxes( right, up, direction ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r value;
			p_params[0]->get( value );
			parsingContext->sceneNode->setScale( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->geometry->getMesh() )
			{
				auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
				String name;
				p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->geometry->setShadowCaster( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectReceivesShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->geometry->setShadowReceiver( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->scene->getGeometryCache().add( parsingContext->geometry );
		parsingContext->geometry.reset();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserObjectMaterialsMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->geometry->getMesh() )
			{
				auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
				String name;
				uint16_t index;
				p_params[0]->get( index );
				p_params[1]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String type;
		p_params[0]->get( type );

		if ( !parsingContext->mesh )
		{
			Parameters parameters;
			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.parse( p_params[1]->get( tmp ) );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			Path path;
			Path pathFile = p_context->m_file.getPath() / p_params[0]->get( path );
			Parameters parameters;

			if ( p_params.size() > 1 )
			{
				String params;
				StringArray paramArray = string::split( p_params[1]->get( params ), cuT( "-" ), 20, false );

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

				if ( !importer->importMesh( *parsingContext->mesh, pathFile, parameters, true ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshMorphImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No mesh initialised." ) );
		}
		else
		{
			real timeIndex;
			p_params[1]->get( timeIndex );
			Path path;
			Path pathFile = p_context->m_file.getPath() / p_params[0]->get( path );
			Parameters parameters;

			if ( p_params.size() > 2 )
			{
				String tmp;
				StringArray arrayStrParams = string::split( p_params[2]->get( tmp ), cuT( "-" ), 20, false );

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

				if ( !importer->importMesh( mesh, pathFile, parameters, false ) )
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
						, Milliseconds{ int64_t( timeIndex * 1000 ) } );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Engine * engine = parsingContext->m_pParser->getEngine();

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			uint16_t count;
			p_params[0]->get( name );
			p_params[1]->get( count );

			if ( !engine->getSubdividerFactory().isTypeRegistered( string::lowerCase( name ) ) )
			{
				CU_ParsingError( cuT( "Subdivider [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto divider = engine->getSubdividerFactory().create( name );
				parsingContext->mesh->computeContainers();
				Point3r ptCenter = parsingContext->mesh->getBoundingBox().getCenter();

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			uint16_t index;
			p_params[0]->get( index );
			p_params[1]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->vertexPos.push_back( value[0] );
			parsingContext->vertexPos.push_back( value[1] );
			parsingContext->vertexPos.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f value;
			p_params[0]->get( value );
			parsingContext->vertexTex.push_back( value[0] );
			parsingContext->vertexTex.push_back( value[1] );
			parsingContext->vertexTex.push_back( 0.0 );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->vertexTex.push_back( value[0] );
			parsingContext->vertexTex.push_back( value[1] );
			parsingContext->vertexTex.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshNormal )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->vertexNml.push_back( value[0] );
			parsingContext->vertexNml.push_back( value[1] );
			parsingContext->vertexNml.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshTangent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->vertexTan.push_back( value[0] );
			parsingContext->vertexTan.push_back( value[1] );
			parsingContext->vertexTan.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ) );
			parsingContext->face1 = -1;
			parsingContext->face2 = -1;

			if ( arrayValues.size() >= 4 )
			{
				Point4i pt4Indices;

				if ( castor::parseValues( strParams, pt4Indices ) )
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
			else if ( castor::parseValues( strParams, pt3Indices ) )
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexTex.empty() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 6 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[5] );
			}

			if ( arrayValues.size() >= 8 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[7] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexTex.empty() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[8] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[ 9] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[10] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceNormals )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexNml.empty() )
			{
				parsingContext->vertexNml.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[ 0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[ 1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[ 2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[ 6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[ 7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[ 8] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[ 9] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[10] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceTangents )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->submesh;

			if ( parsingContext->vertexTan.empty() )
			{
				parsingContext->vertexTan.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->face1 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face1 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face1 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face1 + 2] * 3] = string::toReal( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->face2 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[ 0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[ 1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face2 + 0] * 3] = string::toReal( arrayValues[ 2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[ 6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[ 7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face2 + 1] * 3] = string::toReal( arrayValues[ 8] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[ 9] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[10] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->face2 + 2] * 3] = string::toReal( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
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
			case MaterialType::eLegacy:
				parsingContext->legacyPass = std::static_pointer_cast< LegacyPass >( parsingContext->pass );
				break;

			case MaterialType::ePbrMetallicRoughness:
				parsingContext->pbrMRPass = std::static_pointer_cast< MetallicRoughnessPbrPass >( parsingContext->pass );
				break;

			case MaterialType::ePbrSpecularGlossiness:
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
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPassDiffuse )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass
			&& !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->legacyPass )
			{
				RgbColour crColour;
				p_params[0]->get( crColour );
				parsingContext->legacyPass->setDiffuse( crColour );
			}
			else
			{
				RgbColour crColour;
				p_params[0]->get( crColour );
				parsingContext->pbrSGPass->setDiffuse( crColour );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassSpecular )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass
			&& !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->legacyPass )
			{
				RgbColour crColour;
				p_params[0]->get( crColour );
				parsingContext->legacyPass->setSpecular( crColour );
			}
			else
			{
				RgbColour crColour;
				p_params[0]->get( crColour );
				parsingContext->pbrSGPass->setSpecular( crColour );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAmbient )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->legacyPass->setAmbient( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassEmissive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pass->setEmissive( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassShininess )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );
			parsingContext->legacyPass->setShininess( fFloat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlbedo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour value;
			p_params[0]->get( value );
			parsingContext->pbrMRPass->setAlbedo( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassRoughness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pbrMRPass->setRoughness( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassMetallic )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pbrMRPass->setMetallic( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassGlossiness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrSGPass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pbrSGPass->setGlossiness( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlpha )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );
			parsingContext->pass->setOpacity( fFloat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassdoubleFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->pass->setTwoSided( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassTextureUnit )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->textureUnit.reset();

		if ( parsingContext->pass )
		{
			if ( parsingContext->createPass
				|| parsingContext->pass->getTextureUnitsCount() < parsingContext->unitIndex )
			{
				parsingContext->textureUnit = std::make_shared< TextureUnit >( *parsingContext->m_pParser->getEngine() );
				parsingContext->createUnit = true;
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderProgram.reset();
		parsingContext->shaderStage = ashes::ShaderStageFlag( 0u );

		if ( parsingContext->pass )
		{
			parsingContext->shaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderProgram )

	CU_ImplementAttributeParser( parserPassAlphaBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode = 0;
			p_params[0]->get( mode );
			parsingContext->pass->setAlphaBlendMode( BlendMode( mode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassColourBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode = 0;
			p_params[0]->get( mode );
			parsingContext->pass->setColourBlendMode( BlendMode( mode ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassAlphaFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiFunc;
			float fFloat;
			p_params[0]->get( uiFunc );
			p_params[1]->get( fFloat );
			parsingContext->pass->setAlphaFunc( ashes::CompareOp( uiFunc ) );
			parsingContext->pass->setAlphaValue( fFloat );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassRefractionRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value = 0;
			p_params[0]->get( value );
			parsingContext->pass->setRefractionRatio( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassSubsurfaceScattering )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value = false;
			p_params[0]->get( value );
			parsingContext->pass->setParallaxOcclusion( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassBWAccumulationOperator )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value = 0u;
			p_params[0]->get( value );
			parsingContext->pass->setBWAccumulationOperator( uint8_t( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserPassEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else
		{
			parsingContext->pass.reset();
			parsingContext->legacyPass.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserUnitImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path folder;
			Path relative;
			p_params[0]->get( relative );

			if ( File::fileExists( p_context->m_file.getPath() / relative ) )
			{
				folder = p_context->m_file.getPath();
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
				parsingContext->components = ImageComponents::eAll;

				if ( p_params.size() >= 2 )
				{
					String components;
					p_params[1]->get( components );

					if ( components == "r" )
					{
						parsingContext->components = ImageComponents::eR;
					}
					else if ( components == "rg" )
					{
						parsingContext->components = ImageComponents::eRG;
					}
					else if ( components == "rgb" )
					{
						parsingContext->components = ImageComponents::eRGB;
					}
					else if ( components == "a" )
					{
						parsingContext->components = ImageComponents::eA;
					}
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitLevelsCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->imageInfo.mipLevels );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiChannel;
			p_params[0]->get( uiChannel );
			parsingContext->textureUnit->setChannel( TextureChannel( uiChannel ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			CU_ParsingError( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			SamplerSPtr sampler = parsingContext->m_pParser->getEngine()->getSamplerCache().find( p_params[0]->get( name ) );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pass )
		{
			if ( !parsingContext->textureUnit )
			{
				CU_ParsingError( cuT( "TextureUnit not initialised" ) );
			}
			else
			{
				if ( parsingContext->textureUnit->getRenderTarget()
					|| parsingContext->textureUnit->getChannel() == TextureChannel::eReflection
					|| parsingContext->textureUnit->getChannel() == TextureChannel::eRefraction )
				{
					parsingContext->pass->addTextureUnit( parsingContext->textureUnit );
				}
				else if ( parsingContext->folder.empty() && parsingContext->relative.empty() )
				{
					CU_ParsingError( cuT( "TextureUnit's image not initialised" ) );
				}
				else
				{
					if ( !parsingContext->imageInfo.mipLevels )
					{
						parsingContext->imageInfo.mipLevels = 20;
					}

					if ( parsingContext->createUnit )
					{
						auto texture = std::make_shared< TextureLayout >( *parsingContext->m_pParser->getEngine()->getRenderSystem()
							, parsingContext->imageInfo
							, ashes::MemoryPropertyFlag::eDeviceLocal );
						texture->setSource( parsingContext->folder
							, parsingContext->relative
							, parsingContext->components );
						parsingContext->buffer = texture->getDefaultImage().getBuffer();
						parsingContext->imageInfo =
						{
							0u,
							ashes::TextureType::e2D,
							ashes::Format::eUndefined,
							{ 1u, 1u, 1u },
							0u,
							1u,
							ashes::SampleCountFlag::e1,
							ashes::ImageTiling::eOptimal,
							ashes::ImageUsageFlag::eSampled | ashes::ImageUsageFlag::eTransferDst
						};

						if ( parsingContext->components == ImageComponents::eR )
						{
							auto srcFormat = parsingContext->buffer->format();
							auto dstFormat = ( ( srcFormat == PixelFormat::eR8G8B8
								|| srcFormat == PixelFormat::eB8G8R8
								|| srcFormat == PixelFormat::eR8G8B8_SRGB
								|| srcFormat == PixelFormat::eB8G8R8_SRGB
								|| srcFormat == PixelFormat::eA8R8G8B8
								|| srcFormat == PixelFormat::eA8B8G8R8
								|| srcFormat == PixelFormat::eA8R8G8B8_SRGB
								|| srcFormat == PixelFormat::eA8B8G8R8_SRGB )
								? PixelFormat::eL8
								: ( ( srcFormat == PixelFormat::eRGB16F
									|| srcFormat == PixelFormat::eRGBA16F )
									? PixelFormat::eL16F
									: ( ( srcFormat == PixelFormat::eRGB32F
										|| srcFormat == PixelFormat::eRGBA32F )
										? PixelFormat::eL32F
										: srcFormat ) ) );
							parsingContext->buffer = PxBufferBase::create( parsingContext->buffer->dimensions()
								, dstFormat
								, parsingContext->buffer->constPtr()
								, srcFormat );
						}
						else if ( parsingContext->components == ImageComponents::eA )
						{
							auto tmp = PF::extractAlpha( parsingContext->buffer );
							parsingContext->buffer = tmp;
						}
						else
						{
							auto srcFormat = parsingContext->buffer->format();
							auto dstFormat = ( srcFormat == PixelFormat::eR8G8B8
								? PixelFormat::eA8R8G8B8
								: ( srcFormat == PixelFormat::eB8G8R8
									? PixelFormat::eA8B8G8R8
									: ( srcFormat == PixelFormat::eR8G8B8_SRGB
										? PixelFormat::eA8R8G8B8_SRGB
										: ( srcFormat == PixelFormat::eB8G8R8_SRGB
											? PixelFormat::eA8B8G8R8_SRGB
											: ( srcFormat == PixelFormat::eRGB16F
												? PixelFormat::eRGBA16F
												: ( srcFormat == PixelFormat::eRGB32F
													? PixelFormat::eRGBA32F
													: srcFormat ) ) ) ) ) );

							if ( srcFormat != dstFormat )
							{
								parsingContext->buffer = PxBufferBase::create( parsingContext->buffer->dimensions()
									, dstFormat
									, parsingContext->buffer->constPtr()
									, srcFormat );
							}
						}

						texture->getDefaultImage().setBuffer( parsingContext->buffer );
						parsingContext->textureUnit->setTexture( texture );
						parsingContext->pass->addTextureUnit( parsingContext->textureUnit );
					}
				}
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = ashes::ShaderStageFlag::eVertex;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserPixelShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = ashes::ShaderStageFlag::eFragment;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserGeometryShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = ashes::ShaderStageFlag::eGeometry;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserHullShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = ashes::ShaderStageFlag::eTessellationControl;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserdomainShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = ashes::ShaderStageFlag::eTessellationEvaluation;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserComputeShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = ashes::ShaderStageFlag::eCompute;
	}
	CU_EndAttributePush( CSCNSection::shaderStage )

	CU_ImplementAttributeParser( parserConstantsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( parsingContext->strName );

			if ( parsingContext->strName.empty() )
			{
				CU_ParsingError( cuT( "Invalid empty name" ) );
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderUBO )

	CU_ImplementAttributeParser( parserShaderEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->shaderStage != ashes::ShaderStageFlag( 0u ) )
			{
				Path path;
				p_params[0]->get( path );
				parsingContext->shaderProgram->setFile( parsingContext->shaderStage
					, p_context->m_file.getPath() / path );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->shaderStage != ashes::ShaderStageFlag( 0u ) )
			{
				Point3i sizes;
				p_params[0]->get( sizes );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->strName2 );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t param;
		p_params[0]->get( param );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t uiType;
		p_params[0]->get( uiType );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String strParams;
		p_params[0]->get( strParams );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->path );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontHeight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->iInt16 );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->strName.empty() && !parsingContext->path.empty() )
		{
			if ( parsingContext->scene )
			{
				parsingContext->scene->getFontView().add( parsingContext->strName, parsingContext->iInt16, p_context->m_file.getPath() / parsingContext->path );
			}
			else
			{
				parsingContext->m_pParser->getEngine()->getFontCache().add( parsingContext->strName, parsingContext->iInt16, p_context->m_file.getPath() / parsingContext->path );
			}
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserOverlayPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->overlay )
		{
			Point2d ptPosition;
			p_params[0]->get( ptPosition );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->overlay )
		{
			Point2d ptSize;
			p_params[0]->get( ptSize );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->overlay )
		{
			Size size;
			p_params[0]->get( size );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->overlay )
		{
			Position position;
			p_params[0]->get( position );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->overlay )
		{
			String name;
			p_params[0]->get( name );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::ePanel, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserOverlayBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserOverlayTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eText, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserOverlayEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::ePanel )
		{
			Point4d uvs;
			p_params[0]->get( uvs );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d ptSize;
			p_params[0]->get( ptSize );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Rectangle size;
			p_params[0]->get( size );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			String name;
			p_params[0]->get( name );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			uint32_t position;
			p_params[0]->get( position );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			p_params[0]->get( uvs );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			p_params[0]->get( uvs );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			p_params[0]->get( uvs );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getFontCache();
			String name;
			p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->overlay;
		String strParams;
		p_params[0]->get( strParams );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		SceneNodeSPtr pParent = parsingContext->scene->getSceneNodeCache().find( p_params[0]->get( name ) );

		if ( pParent )
		{
			parsingContext->sceneNode = pParent;

			while ( pParent->getParent() && pParent->getParent() != parsingContext->scene->getObjectRootNode() && pParent->getParent() != parsingContext->scene->getCameraRootNode() )
			{
				pParent = pParent->getParent();
			}

			if ( !pParent->getParent() || pParent->getParent() == parsingContext->scene->getObjectRootNode() )
			{
				pParent->attachTo( parsingContext->scene->getCameraRootNode() );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->viewport = std::make_shared< Viewport >( *parsingContext->m_pParser->getEngine() );
		parsingContext->viewport->setPerspective( 0.0_degrees, 1, 0, 1 );
	}
	CU_EndAttributePush( CSCNSection::eViewport )

	CU_ImplementAttributeParser( parserCameraPrimitive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t uiType;
		parsingContext->primitiveType = ashes::PrimitiveTopology( p_params[0]->get( uiType ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->sceneNode && parsingContext->viewport )
		{
			parsingContext->scene->getCameraCache().add( parsingContext->strName, parsingContext->sceneNode, std::move( *parsingContext->viewport ) );
			parsingContext->viewport.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserViewportType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !p_params.empty() )
		{
			uint32_t uiType;
			parsingContext->viewport->updateType( ViewportType( p_params[0]->get( uiType ) ) );
		}
		else
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateLeft( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateRight( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateTop( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateBottom( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportNear )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateNear( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFar )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateFar( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size size;
		p_params[0]->get( size );
		parsingContext->viewport->resize( size );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFovY )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fFovY;
		p_params[0]->get( fFovY );
		parsingContext->viewport->updateFovY( Angle::fromDegrees( fFovY ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportAspectRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fRatio;
		p_params[0]->get( fRatio );
		parsingContext->viewport->updateRatio( fRatio );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->billboards )
		{
			String name;
			p_params[0]->get( name );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->billboards )
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );

			parsingContext->billboards->setBillboardType( BillboardType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->billboards )
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->billboards )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point2f dimensions;
		p_params[0]->get( dimensions );
		parsingContext->billboards->setDimensions( dimensions );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->scene->getBillboardListCache().add( parsingContext->billboards );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserBillboardPoint )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point3r position;
		p_params[0]->get( position );
		parsingContext->billboards->addPoint( position );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->strName2 );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		p_params[0]->get( name );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		bool value;
		p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float value;
		p_params[0]->get( value );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() <= 1 )
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
			Path filePath = p_context->m_file.getPath();
			p_params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				uint32_t size;
				p_params[1]->get( size );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() < 1 )
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
			Path filePath = p_context->m_file.getPath();
			p_params[0]->get( path );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadLeftImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadRightImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadTopImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadBottomImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxFront )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadFrontImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBack )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->loadBackImage( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				p_params[0]->get( value );
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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->subsurfaceScattering->setStrength( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringGaussianWidth )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->subsurfaceScattering->setGaussianWidth( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringTransmittanceProfile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eTransmittanceProfile )

	CU_ImplementAttributeParser( parserSubsurfaceScatteringEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

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
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			Point4f value;
			p_params[0]->get( value );
			parsingContext->subsurfaceScattering->addProfileFactor( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHdrExponent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No render target initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->renderTarget->setExposure( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHdrGamma )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			CU_ParsingError( cuT( "No render target initialised." ) );
		}
		else if ( p_params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->renderTarget->setGamma( value );
		}
	}
	CU_EndAttribute()
}
