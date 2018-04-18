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
#include "Scene/ParticleSystem/ParticleSystem.hpp"
#include "Scene/Animation/AnimatedObjectGroup.hpp"
#include "Scene/Background/Image.hpp"
#include "Scene/Background/Skybox.hpp"
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
	IMPLEMENT_ATTRIBUTE_PARSER( parserRootMtlFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );

			if ( File::fileExists( path ) )
			{
				Logger::logInfo( cuT( "Loading materials file : " ) + path );

				if ( parsingContext->m_pParser->getEngine()->getMaterialCache().read( path ) )
				{
					Logger::logInfo( cuT( "Materials read" ) );
				}
				else
				{
					Logger::logInfo( cuT( "Can't read materials" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->get( name );
			parsingContext->scene = parsingContext->m_pParser->getEngine()->getSceneCache().add( name );
			parsingContext->mapScenes.insert( std::make_pair( name, parsingContext->scene ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->path.clear();

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eFont )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->strName );
			parsingContext->material = parsingContext->m_pParser->getEngine()->getMaterialCache().add( parsingContext->strName
				, MaterialType::eLegacy );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootMaterials )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->m_pParser->getEngine()->setMaterialsType( MaterialType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::ePanel, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eBorderPanel, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eText, nullptr, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->sampler = parsingContext->m_pParser->getEngine()->getSamplerCache().add( p_params[0]->get( name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootDebugOverlays )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			parsingContext->m_pParser->getEngine()->getRenderLoop().showDebugOverlays( p_params[0]->get( value ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRootWindow )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->window )
			{
				PARSING_ERROR( cuT( "Can't create more than one render window" ) );
			}
			else
			{
				String name;
				p_params[0]->get( name );
				parsingContext->window = parsingContext->scene->getEngine()->getRenderWindowCache().add( name );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eWindow )

	IMPLEMENT_ATTRIBUTE_PARSER( parserWindowRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->window )
		{
			parsingContext->renderTarget = parsingContext->m_pParser->getEngine()->getRenderTargetCache().add( TargetType::eWindow );
			parsingContext->iInt16 = 0;
		}
		else
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( parserWindowVSync )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->window )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool bValue;
			p_params[0]->get( bValue );
			parsingContext->window->setVSync( bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserWindowFullscreen )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->window )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{

			bool bValue;
			p_params[0]->get( bValue );
			parsingContext->window->setFullscreen( bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
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
				PARSING_ERROR( cuT( "No scene found with name : [" ) + name + cuT( "]." ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->renderTarget->getScene() )
			{
				parsingContext->renderTarget->setCamera( parsingContext->renderTarget->getScene()->getCameraCache().find( p_params[0]->get( name ) ) );
			}
			else
			{
				PARSING_ERROR( cuT( "No scene initialised for this window, set scene before camera." ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Size size;
			parsingContext->renderTarget->setSize( p_params[0]->get( size ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetFormat )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
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
				PARSING_ERROR( cuT( "Wrong format for colour" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetStereo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetPostEffect )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
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
				PARSING_ERROR( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				PostEffectSPtr effect = engine->getRenderTargetCache().getPostEffectFactory().create( name, *parsingContext->renderTarget, *engine->getRenderSystem(), parameters );
				parsingContext->renderTarget->addPostEffect( effect );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetToneMapping )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetSsao )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->renderTarget )
		{
			PARSING_ERROR( cuT( "No render target initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSsao )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetEnd )
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
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMinFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setMinFilter( renderer::Filter( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMagFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setMagFilter( renderer::Filter( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMipFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setMipFilter( renderer::MipmapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMinLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
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
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMaxLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
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
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerLodBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
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
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerUWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setWrapS( renderer::WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerVWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setWrapT( renderer::WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerWWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setWrapR( renderer::WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerBorderColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t colour;
			p_params[0]->get( colour );
			parsingContext->sampler->setBorderColour( renderer::BorderColour( colour ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMaxAnisotropy )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );
			parsingContext->sampler->setMaxAnisotropy( rValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerComparisonMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode;
			p_params[0]->get( mode );
			parsingContext->sampler->enableCompare( bool( mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerComparisonFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->sampler->setCompareOp( renderer::CompareOp( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneInclude )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path path;
			p_params[0]->get( path );
			SceneFileParser parser{ *parsingContext->m_pParser->getEngine() };
			parser.parseFile( parsingContext->m_file.getPath() / path, parsingContext );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBkColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour clrBackground;
			p_params[0]->get( clrBackground );
			parsingContext->scene->setBackgroundColour( clrBackground );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBkImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
			parsingContext->path.clear();
			p_params[0]->get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eFont )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->strName );
			parsingContext->material = parsingContext->scene->getMaterialView().add( parsingContext->strName, parsingContext->scene->getMaterialsType() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->sampler = parsingContext->scene->getSamplerView().add( p_params[0]->get( name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->viewport.reset();
		parsingContext->sceneNode.reset();

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			parsingContext->sceneNode.reset();
			p_params[0]->get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eCamera )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->light.reset();
		parsingContext->strName.clear();
		parsingContext->sceneNode.reset();

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eLight )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneCameraNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->sceneNode.reset();
		String name;

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().add( name, parsingContext->scene->getCameraRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneSceneNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->sceneNode.reset();
		String name;

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().add( name, parsingContext->scene->getObjectRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->geometry.reset();
		String name;

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->geometry = parsingContext->scene->getGeometryCache().add( name, nullptr, nullptr );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneAmbientLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour clColour;
			p_params[0]->get( clColour );
			parsingContext->scene->setAmbientLight( clColour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Path path;
		Path pathFile = p_context->m_file.getPath() / p_params[0]->get( path );

		Engine * engine = parsingContext->m_pParser->getEngine();
		auto extension = string::lowerCase( pathFile.getExtension() );

		if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
		{
			PARSING_ERROR( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
		}
		else
		{
			auto importer = engine->getImporterFactory().create( extension, *engine );
			parsingContext->scene->importExternal( pathFile, *importer );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBillboard )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			parsingContext->billboards = parsingContext->scene->getBillboardListCache().add( name, SceneNodeSPtr{} );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboard )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneAnimatedObjectGroup )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			parsingContext->pAnimGroup = parsingContext->scene->getAnimatedObjectGroupCache().add( name );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimGroup )

	IMPLEMENT_ATTRIBUTE_PARSER( parserScenePanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
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
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( p_params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->scene, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->overlay = parsingContext->scene->getOverlayView().add( p_params[0]->get( name ), OverlayType::eText, parsingContext->scene, parsingContext->overlay );
			parsingContext->overlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneSkybox )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->skybox = std::make_shared< SkyboxBackground >( *parsingContext->m_pParser->getEngine()
					, *parsingContext->scene );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSkybox )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneFogType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->scene->getFog().setType( FogType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneFogDensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->scene->getFog().setDensity( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneParticleSystem )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			p_params[0]->get( value );
			parsingContext->strName = value;
			parsingContext->uiUInt32 = 0;
			parsingContext->sceneNode.reset();
			parsingContext->material.reset();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticleSystem )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneHdrConfig )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eHdrConfig )

	IMPLEMENT_ATTRIBUTE_PARSER( parserMesh )
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
			PARSING_ERROR( cuT( "No scene initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMesh )

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			p_params[0]->get( value );

			if ( !parsingContext->scene->getSceneNodeCache().has( value ) )
			{
				PARSING_ERROR( cuT( "No scene node named " ) + value );
			}
			else
			{
				parsingContext->sceneNode = parsingContext->scene->getSceneNodeCache().find( value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->uiUInt32 = value;
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
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
				PARSING_ERROR( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	END_ATTRIBUTE()

		IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemDimensions )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->get( parsingContext->point2f );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemParticle )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		
		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( parsingContext->uiUInt32 == 0 )
		{
			PARSING_ERROR( cuT( "particles_count has not been specified." ) );
		}
		else if ( parsingContext->point2f[0] == 0 || parsingContext->point2f[1] == 0 )
		{
			PARSING_ERROR( cuT( "one component of the particles dimensions is 0." ) );
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
	END_ATTRIBUTE_PUSH( CSCNSection::eParticle )

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemCSShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderProgram.reset();
		parsingContext->shaderStage = renderer::ShaderStageFlag( 0u );
		parsingContext->bBool1 = false;
		
		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->shaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->particleSystem )
		{
			PARSING_ERROR( cuT( "Particle system not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			p_params[0]->get( value );
			Engine * engine = parsingContext->m_pParser->getEngine();

			if ( !engine->getParticleFactory().isTypeRegistered( string::lowerCase( value ) ) )
			{
				PARSING_ERROR( cuT( "Particle type [" ) + value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext->particleSystem->setParticleType( value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->strName2 );

		if ( !parsingContext->particleSystem )
		{
			PARSING_ERROR( cuT( "Particle system not initialised" ) );
		}
		else if ( p_params.size() < 2 )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
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
	END_ATTRIBUTE_PUSH( CSCNSection::eUBOVariable )

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
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
				PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}

			if ( parsingContext->light )
			{
				parsingContext->light->detach();
				parsingContext->sceneNode->attachObject( *parsingContext->light );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->get( uiType );
			parsingContext->lightType = LightType( uiType );
			parsingContext->light = parsingContext->scene->getLightCache().add( parsingContext->strName, parsingContext->sceneNode, parsingContext->lightType );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->light->setColour( value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f value;
			p_params[0]->get( value );
			parsingContext->light->setIntensity( value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightAttenuation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
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
				PARSING_ERROR( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightCutOff )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
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
				PARSING_ERROR( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightExponent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
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
				PARSING_ERROR( cuT( "Wrong type of light to apply an exponent, needs spotlight" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightShadowProducer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->light )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->light->setShadowProducer( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
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
				PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodePosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->sceneNode->setPosition( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeOrientation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeDirection )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->sceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r value;
			p_params[0]->get( value );
			parsingContext->sceneNode->setScale( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
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
				PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Geometry not initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
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
					for ( auto submesh : *parsingContext->geometry->getMesh() )
					{
						MaterialSPtr material = cache.find( name );
						parsingContext->geometry->setMaterial( *submesh, material );
					}
				}
				else
				{
					PARSING_ERROR( cuT( "Material " ) + name + cuT( " does not exist" ) );
				}
			}
			else
			{
				PARSING_ERROR( cuT( "Geometry's mesh not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectMaterials )
	{
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eObjectMaterials )

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectCastShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->geometry->setShadowCaster( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectReceivesShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->geometry->setShadowReceiver( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->geometry.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectMaterialsMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->geometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
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
						PARSING_ERROR( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					PARSING_ERROR( cuT( "Material " ) + name + cuT( " does not exist" ) );
				}
			}
			else
			{
				PARSING_ERROR( cuT( "Geometry's mesh not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectMaterialsEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshType )
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
				PARSING_ERROR( cuT( "No scene initialised" ) );
			}
		}
		else
		{
			PARSING_WARNING( cuT( "Mesh already initialised => ignored" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshSubmesh )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->bBool2 = false;
		parsingContext->bBool1 = true;
		parsingContext->face1 = -1;
		parsingContext->face2 = -1;
		parsingContext->submesh.reset();

		if ( !parsingContext->mesh )
		{
			PARSING_ERROR( cuT( "No Mesh initialised." ) );
		}
		else
		{
			parsingContext->submesh = parsingContext->mesh->createSubmesh();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSubmesh )

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
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
							PARSING_ERROR( cuT( "Malformed parameter -rescale." ) );
						}
					}
				}
			}

			Engine * engine = parsingContext->m_pParser->getEngine();
			auto extension = string::lowerCase( pathFile.getExtension() );

			if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
			{
				PARSING_ERROR( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext->mesh = parsingContext->scene->getMeshCache().add( parsingContext->strName2 );
				auto importer = engine->getImporterFactory().create( extension, *engine );

				if ( !importer->importMesh( *parsingContext->mesh, pathFile, parameters, true ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshMorphImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->mesh )
		{
			PARSING_ERROR( cuT( "No mesh initialised." ) );
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
				PARSING_ERROR( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto importer = engine->getImporterFactory().create( extension, *engine );
				Mesh mesh{ cuT( "MorphImport" ), *parsingContext->scene };

				if ( !importer->importMesh( mesh, pathFile, parameters, false ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
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
					PARSING_ERROR( cuT( "The new mesh doesn't match the original mesh" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshDivide )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Engine * engine = parsingContext->m_pParser->getEngine();

		if ( !parsingContext->mesh )
		{
			PARSING_ERROR( cuT( "No Mesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			uint16_t count;
			p_params[0]->get( name );
			p_params[1]->get( count );

			if ( !engine->getSubdividerFactory().isTypeRegistered( string::lowerCase( name ) ) )
			{
				PARSING_ERROR( cuT( "Subdivider [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->mesh )
		{
			if ( parsingContext->geometry )
			{
				parsingContext->geometry->setMesh( parsingContext->mesh );
			}

			parsingContext->mesh.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshVertex )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshNormal )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshTangent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceNormals )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceTangents )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->submesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshEnd )
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
					if ( !parsingContext->vertexTan.empty() )
					{
						mapping->computeBitangents();
					}
					else
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
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMaterialPass )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->strName.clear();

		if ( parsingContext->material )
		{
			parsingContext->pass = parsingContext->material->createPass();

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
				PARSING_ERROR( "Unsupported pass type." );
				break;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Material not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePass )

	IMPLEMENT_ATTRIBUTE_PARSER( parserMaterialEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassDiffuse )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass
			&& !parsingContext->pbrSGPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassSpecular )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass
			&& !parsingContext->pbrSGPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
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
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassAmbient )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->legacyPass->setAmbient( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassEmissive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pass->setEmissive( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassShininess )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );
			parsingContext->legacyPass->setShininess( fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassAlbedo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour value;
			p_params[0]->get( value );
			parsingContext->pbrMRPass->setAlbedo( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassRoughness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pbrMRPass->setRoughness( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassMetallic )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pbrMRPass->setMetallic( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassGlossiness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrSGPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pbrSGPass->setGlossiness( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassAlpha )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );
			parsingContext->pass->setOpacity( fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassdoubleFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->pass->setTwoSided( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassTextureUnit )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->textureUnit.reset();

		if ( parsingContext->pass )
		{
			parsingContext->textureUnit = std::make_shared< TextureUnit >( *parsingContext->m_pParser->getEngine() );
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextureUnit )

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderProgram.reset();
		parsingContext->shaderStage = renderer::ShaderStageFlag( 0u );

		if ( parsingContext->pass )
		{
			parsingContext->shaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassAlphaBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode = 0;
			p_params[0]->get( mode );
			parsingContext->pass->setAlphaBlendMode( BlendMode( mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassColourBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode = 0;
			p_params[0]->get( mode );
			parsingContext->pass->setColourBlendMode( BlendMode( mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassAlphaFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiFunc;
			float fFloat;
			p_params[0]->get( uiFunc );
			p_params[1]->get( fFloat );
			parsingContext->pass->setAlphaFunc( renderer::CompareOp( uiFunc ) );
			parsingContext->pass->setAlphaValue( fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassRefractionRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value = 0;
			p_params[0]->get( value );
			parsingContext->pass->setRefractionRatio( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassSubsurfaceScattering )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else
		{
			parsingContext->subsurfaceScattering = std::make_unique< SubsurfaceScattering >();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSubsurfaceScattering )

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassParallaxOcclusion )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value = false;
			p_params[0]->get( value );
			parsingContext->pass->setParallaxOcclusion( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPassEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else
		{
			parsingContext->pass.reset();
			parsingContext->legacyPass.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
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
				PARSING_ERROR( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				parsingContext->textureUnit->setAutoMipmaps( true );
				renderer::ImageCreateInfo image{};
				image.arrayLayers = 1u;
				image.extent.depth = 1u;
				image.imageType = renderer::TextureType::e2D;
				image.initialLayout = renderer::ImageLayout::eUndefined;
				image.mipLevels = 1u;
				image.samples = renderer::SampleCountFlag::e1;
				image.sharingMode = renderer::SharingMode::eExclusive;
				image.tiling = renderer::ImageTiling::eOptimal;
				image.usage = renderer::ImageUsageFlag::eSampled | renderer::ImageUsageFlag::eTransferDst;
				auto texture = std::make_shared< TextureLayout >( *parsingContext->m_pParser->getEngine()->getRenderSystem()
					, image
					, renderer::MemoryPropertyFlag::eDeviceLocal );
				texture->setSource( folder, relative );

				if ( p_params.size() >= 2 )
				{
					String channels;
					p_params[1]->get( channels );
					auto buffer = texture->getDefaultImage().getBuffer();

					if ( channels == cuT( "rgb" ) )
					{
						buffer = PxBufferBase::create( buffer->dimensions()
							, PF::getPFWithoutAlpha( buffer->format() )
							, buffer->constPtr()
							, buffer->format() );
					}
					else if ( channels == cuT( "r" ) )
					{
						auto format = ( buffer->format() == PixelFormat::eR8G8B8
							|| buffer->format() == PixelFormat::eB8G8R8
							|| buffer->format() == PixelFormat::eR8G8B8_SRGB
							|| buffer->format() == PixelFormat::eB8G8R8_SRGB
							|| buffer->format() == PixelFormat::eA8R8G8B8
							|| buffer->format() == PixelFormat::eA8B8G8R8
							|| buffer->format() == PixelFormat::eA8R8G8B8_SRGB
							|| buffer->format() == PixelFormat::eA8B8G8R8_SRGB )
							? PixelFormat::eL8
							: ( buffer->format() == PixelFormat::eRGB16F
								|| buffer->format() == PixelFormat::eRGBA16F )
								? PixelFormat::eL16F
								: ( buffer->format() == PixelFormat::eRGB32F
									|| buffer->format() == PixelFormat::eRGBA32F )
									? PixelFormat::eL32F
									: buffer->format();
						buffer = PxBufferBase::create( buffer->dimensions()
							, format
							, buffer->constPtr()
							, buffer->format() );
					}
					else if ( channels == cuT( "a" ) )
					{
						auto tmp = PF::extractAlpha( buffer );
						buffer = tmp;
					}

					texture->getDefaultImage().setBuffer( buffer );
				}

				parsingContext->textureUnit->setTexture( texture );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else
		{
			parsingContext->renderTarget = parsingContext->m_pParser->getEngine()->getRenderTargetCache().add( TargetType::eTexture );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitChannel )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiChannel;
			p_params[0]->get( uiChannel );
			parsingContext->textureUnit->setChannel( TextureChannel( uiChannel ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->textureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
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
				PARSING_ERROR( cuT( "Unknown sampler : [" ) + name + cuT( "]" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pass )
		{
			if ( parsingContext->textureUnit )
			{
				parsingContext->pass->addTextureUnit( parsingContext->textureUnit );
			}
			else
			{
				PARSING_ERROR( cuT( "TextureUnit not initialised" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserVertexShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = renderer::ShaderStageFlag::eVertex;
	}
	END_ATTRIBUTE_PUSH( CSCNSection::shaderStage )

	IMPLEMENT_ATTRIBUTE_PARSER( parserPixelShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = renderer::ShaderStageFlag::eFragment;
	}
	END_ATTRIBUTE_PUSH( CSCNSection::shaderStage )

	IMPLEMENT_ATTRIBUTE_PARSER( parserGeometryShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = renderer::ShaderStageFlag::eGeometry;
	}
	END_ATTRIBUTE_PUSH( CSCNSection::shaderStage )

	IMPLEMENT_ATTRIBUTE_PARSER( parserHullShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = renderer::ShaderStageFlag::eTessellationControl;
	}
	END_ATTRIBUTE_PUSH( CSCNSection::shaderStage )

	IMPLEMENT_ATTRIBUTE_PARSER( parserdomainShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = renderer::ShaderStageFlag::eTessellationEvaluation;
	}
	END_ATTRIBUTE_PUSH( CSCNSection::shaderStage )

	IMPLEMENT_ATTRIBUTE_PARSER( parserComputeShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->shaderStage = renderer::ShaderStageFlag::eCompute;
	}
	END_ATTRIBUTE_PUSH( CSCNSection::shaderStage )

	IMPLEMENT_ATTRIBUTE_PARSER( parserConstantsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->shaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( parsingContext->strName );

			if ( parsingContext->strName.empty() )
			{
				PARSING_ERROR( cuT( "Invalid empty name" ) );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderUBO )

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->shaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else
		{
			if ( parsingContext->particleSystem )
			{
				parsingContext->particleSystem->setCSUpdateProgram( parsingContext->shaderProgram->getStates()[0] );
				parsingContext->bBool1 = false;
			}

			parsingContext->shaderProgram.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderProgramFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->shaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->shaderStage != renderer::ShaderStageFlag( 0u ) )
			{
				uint32_t uiModel;
				Path path;
				p_params[0]->get( uiModel );
				p_params[1]->get( path );
				parsingContext->shaderProgram->setFile( parsingContext->shaderStage
					, p_context->m_file.getPath() / path );
			}
			else
			{
				PARSING_ERROR( cuT( "Shader not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderUboShaders )
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
				PARSING_ERROR( cuT( "Unsupported shader type" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderUboVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->strName2 );

		if ( !parsingContext->uniformBuffer )
		{
			PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
		}
		else if ( parsingContext->strName2.empty() )
		{
			PARSING_ERROR( cuT( "Invalid empty name" ) );
		}
		else
		{
			parsingContext->uiUInt32 = 1;
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eUBOVariable )

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderVariableCount )
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
			PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderVariableType )
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
			//	PARSING_ERROR( cuT( "Variable type already set" ) );
			//}
		}
		else
		{
			PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderVariableValue )
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
		//	PARSING_ERROR( cuT( "Variable not initialised" ) );
		//}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserFontFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->path );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserFontHeight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->iInt16 );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserFontEnd )
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
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayPosition )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlaySize )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayPixelSize )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayPixelPosition )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayMaterial )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::ePanel, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->overlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eText, parsingContext->overlay->getScene(), parsingContext->overlay );
		parsingContext->overlay->setVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayEnd )
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
				PARSING_ERROR( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			parsingContext->overlay->setVisible( true );
		}

		parsingContext->overlay = parsingContext->overlay->getParent();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPanelOverlayUvs )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlaySizes )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlayPixelSizes )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlayMaterial )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlayPosition )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlayCenterUvs )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlayOuterUvs )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBorderPanelOverlayInnerUvs )
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
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayFont )
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
				PARSING_ERROR( cuT( "Unknown font" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayTextWrapping )
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
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayVerticalAlign )
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
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayHorizontalAlign )
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
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayTexturingMode )
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
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayLineSpacingMode )
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
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTextOverlayText )
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
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserCameraParent )
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
			PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserCameraViewport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->viewport = std::make_shared< Viewport >();
		parsingContext->viewport->setPerspective( Angle::fromDegrees( 0 ), 1, 0, 1 );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eViewport )

	IMPLEMENT_ATTRIBUTE_PARSER( parserCameraPrimitive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t uiType;
		parsingContext->primitiveType = renderer::PrimitiveTopology( p_params[0]->get( uiType ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserCameraEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->sceneNode && parsingContext->viewport )
		{
			parsingContext->scene->getCameraCache().add( parsingContext->strName, parsingContext->sceneNode, std::move( *parsingContext->viewport ) );
			parsingContext->viewport.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !p_params.empty() )
		{
			uint32_t uiType;
			parsingContext->viewport->updateType( ViewportType( p_params[0]->get( uiType ) ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateLeft( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateRight( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateTop( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateBottom( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportNear )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateNear( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportFar )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->viewport->updateFar( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size size;
		p_params[0]->get( size );
		parsingContext->viewport->resize( size );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportFovY )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fFovY;
		p_params[0]->get( fFovY );
		parsingContext->viewport->updateFovY( Angle::fromDegrees( fFovY ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportAspectRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fRatio;
		p_params[0]->get( fRatio );
		parsingContext->viewport->updateRatio( fRatio );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardParent )
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
				PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Geometry not initialised." ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->billboards )
		{
			PARSING_ERROR( cuT( "Billboard not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );

			parsingContext->billboards->setBillboardType( BillboardType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->billboards )
		{
			PARSING_ERROR( cuT( "Billboard not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );

			parsingContext->billboards->setBillboardSize( BillboardSize( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardPositions )
	{
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboardList )

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardMaterial )
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
				PARSING_ERROR( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Billboard not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardDimensions )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point2f dimensions;
		p_params[0]->get( dimensions );
		parsingContext->billboards->setDimensions( dimensions );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->billboards = nullptr;
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardPoint )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point3r position;
		p_params[0]->get( position );
		parsingContext->billboards->addPoint( position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimatedObject )
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
				PARSING_ERROR( cuT( "No geometry with name " ) + name );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->get( parsingContext->strName2 );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->addAnimation( parsingContext->strName2 );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimation )

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimationStart )
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
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimationPause )
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
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimatedObjectGroupEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup.reset();
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimationLooped )
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
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimationScale )
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
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimationEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxEqui )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() <= 1 )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext->skybox )
		{
			PARSING_ERROR( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path path;
			Path filePath = p_context->m_file.getPath();
			p_params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				Size size;
				p_params[1]->get( size );
				renderer::ImageCreateInfo image{};
				image.arrayLayers = 1u;
				image.extent.width = size.getWidth();
				image.extent.height = size.getHeight();
				image.extent.depth = 1u;
				image.imageType = renderer::TextureType::e2D;
				image.initialLayout = renderer::ImageLayout::eUndefined;
				image.mipLevels = 1u;
				image.samples = renderer::SampleCountFlag::e1;
				image.sharingMode = renderer::SharingMode::eExclusive;
				image.tiling = renderer::ImageTiling::eOptimal;
				image.usage = renderer::ImageUsageFlag::eSampled
					| renderer::ImageUsageFlag::eTransferDst;
				auto texture = std::make_shared< TextureLayout >( *parsingContext->scene->getEngine()->getRenderSystem()
					, image
					, renderer::MemoryPropertyFlag::eDeviceLocal );
				texture->getDefaultImage().initialiseSource( filePath, path );
				parsingContext->skybox->setEquiTexture( texture
					, size );
			}
			else
			{
				PARSING_ERROR( cuT( "Couldn't load the image" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxCross )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() < 1 )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext->skybox )
		{
			PARSING_ERROR( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path path;
			Path filePath = p_context->m_file.getPath();
			p_params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				renderer::ImageCreateInfo image{};
				image.arrayLayers = 1u;
				image.extent.depth = 1u;
				image.imageType = renderer::TextureType::e2D;
				image.initialLayout = renderer::ImageLayout::eUndefined;
				image.mipLevels = 1u;
				image.samples = renderer::SampleCountFlag::e1;
				image.sharingMode = renderer::SharingMode::eExclusive;
				image.tiling = renderer::ImageTiling::eOptimal;
				image.usage = renderer::ImageUsageFlag::eSampled
					| renderer::ImageUsageFlag::eTransferSrc
					| renderer::ImageUsageFlag::eTransferDst;
				auto texture = std::make_shared< TextureLayout >( *parsingContext->scene->getEngine()->getRenderSystem()
					, image
					, renderer::MemoryPropertyFlag::eDeviceLocal );
				texture->getDefaultImage().initialiseSource( filePath, path );
				parsingContext->skybox->setCrossTexture( texture );
			}
			else
			{
				PARSING_ERROR( cuT( "Couldn't load the image" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			parsingContext->skybox->getTexture().getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->skybox->getTexture().getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->skybox->getTexture().getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->skybox->getTexture().getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxFront )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->skybox->getTexture().getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxBack )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->skybox->getTexture().getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSkyboxEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->skybox )
		{
			parsingContext->scene->setBackground( std::move( parsingContext->skybox ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoEnabled )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_enabled = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoHighQuality )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_highQuality = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoUseNormalsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_useNormalsBuffer = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoRadius )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_radius = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_bias = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_intensity = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoNumSamples )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_numSamples = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoEdgeSharpness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_edgeSharpness = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoBlurStepSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_blurStepSize = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoBlurHighQuality )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_blurHighQuality = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoBlurRadius )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				p_params[0]->get( value );
				parsingContext->ssaoConfig.m_blurRadius = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->renderTarget )
		{
			parsingContext->renderTarget->setSsaoConfig( parsingContext->ssaoConfig );
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubsurfaceScatteringStrength )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			PARSING_ERROR( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->subsurfaceScattering->setStrength( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubsurfaceScatteringGaussianWidth )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			PARSING_ERROR( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->subsurfaceScattering->setGaussianWidth( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubsurfaceScatteringTransmittanceProfile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			PARSING_ERROR( cuT( "No SubsurfaceScattering initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTransmittanceProfile )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubsurfaceScatteringEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			PARSING_ERROR( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else
		{
			parsingContext->pass->setSubsurfaceScattering( std::move( parsingContext->subsurfaceScattering ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserTransmittanceProfileFactor )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->subsurfaceScattering )
		{
			PARSING_ERROR( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			Point4f value;
			p_params[0]->get( value );
			parsingContext->subsurfaceScattering->addProfileFactor( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserHdrExponent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->scene->setExposure( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserHdrGamma )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->scene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->scene->setGamma( value );
		}
	}
	END_ATTRIBUTE()
}
