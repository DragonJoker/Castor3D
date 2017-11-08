#include "SceneFileParser_Parsers.hpp"

#include "Engine.hpp"
#include "Cache/BillboardCache.hpp"
#include "Cache/GeometryCache.hpp"
#include "Cache/LightCache.hpp"
#include "Cache/SceneNodeCache.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Cache/CacheView.hpp"
#include "Material/Material.hpp"
#include "Material/LegacyPass.hpp"
#include "Material/MetallicRoughnessPbrPass.hpp"
#include "Material/SpecularGlossinessPbrPass.hpp"
#include "Material/SubsurfaceScattering.hpp"
#include "Mesh/Face.hpp"
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
#include "Scene/Light/PointLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureLayout.hpp"

#include <Graphics/Font.hpp>
#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		InterleavedVertexArray convert( VertexPtrArray const & p_points )
		{
			InterleavedVertexArray result;
			result.reserve( p_points.size() );

			for ( auto point : p_points )
			{
				InterleavedVertex vertex;
				Vertex::getPosition( *point, vertex.m_pos );
				Vertex::getNormal( *point, vertex.m_nml );
				Vertex::getTangent( *point, vertex.m_tan );
				Vertex::getBitangent( *point, vertex.m_bin );
				Vertex::getTexCoord( *point, vertex.m_tex );
				result.push_back( vertex );
			}

			return result;
		}
	}

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
			parsingContext->pScene = parsingContext->m_pParser->getEngine()->getSceneCache().add( name );
			parsingContext->mapScenes.insert( std::make_pair( name, parsingContext->pScene ) );
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
			parsingContext->pMaterial = parsingContext->m_pParser->getEngine()->getMaterialCache().add( parsingContext->strName
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
			parsingContext->pOverlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::ePanel, nullptr, parsingContext->pOverlay );
			parsingContext->pOverlay->setVisible( false );
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
			parsingContext->pOverlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eBorderPanel, nullptr, parsingContext->pOverlay );
			parsingContext->pOverlay->setVisible( false );
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
			parsingContext->pOverlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eText, nullptr, parsingContext->pOverlay );
			parsingContext->pOverlay->setVisible( false );
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
			parsingContext->pSampler = parsingContext->m_pParser->getEngine()->getSamplerCache().add( p_params[0]->get( name ) );
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

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pWindow )
			{
				PARSING_ERROR( cuT( "Can't create more than one render window" ) );
			}
			else
			{
				String name;
				p_params[0]->get( name );
				parsingContext->pWindow = parsingContext->pScene->getEngine()->getRenderWindowCache().add( name );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eWindow )

	IMPLEMENT_ATTRIBUTE_PARSER( parserWindowRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pWindow )
		{
			parsingContext->pRenderTarget = parsingContext->m_pParser->getEngine()->getRenderTargetCache().add( TargetType::eWindow );
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

		if ( !parsingContext->pWindow )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool bValue;
			p_params[0]->get( bValue );
			parsingContext->pWindow->setVSync( bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserWindowFullscreen )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pWindow )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{

			bool bValue;
			p_params[0]->get( bValue );
			parsingContext->pWindow->setFullscreen( bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			ScenePtrStrMap::iterator it = parsingContext->mapScenes.find( p_params[0]->get( name ) );

			if ( it != parsingContext->mapScenes.end() )
			{
				parsingContext->pRenderTarget->setScene( it->second );
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

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pRenderTarget->getScene() )
			{
				parsingContext->pRenderTarget->setCamera( parsingContext->pRenderTarget->getScene()->getCameraCache().find( p_params[0]->get( name ) ) );
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

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Size size;
			parsingContext->pRenderTarget->setSize( p_params[0]->get( size ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetFormat )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			PixelFormat ePF;
			p_params[0]->get( ePF );

			if ( ePF < PixelFormat::eD16 )
			{
				parsingContext->pRenderTarget->setPixelFormat( ePF );
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

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rIntraOcularDistance;
			p_params[0]->get( rIntraOcularDistance );

			if ( rIntraOcularDistance > 0 )
			{
				//l_parsingContext->pRenderTarget->setStereo( true );
				//l_parsingContext->pRenderTarget->setIntraOcularDistance( rIntraOcularDistance );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetPostEffect )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
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
				PostEffectSPtr effect = engine->getRenderTargetCache().getPostEffectFactory().create( name, *parsingContext->pRenderTarget, *engine->getRenderSystem(), parameters );
				parsingContext->pRenderTarget->addPostEffect( effect );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetToneMapping )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
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

			parsingContext->pRenderTarget->setToneMappingType( name, parameters );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetSsao )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No render target initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSsao )

	IMPLEMENT_ATTRIBUTE_PARSER( parserRenderTargetEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pRenderTarget->getTargetType() == TargetType::eTexture )
		{
			parsingContext->pTextureUnit->setRenderTarget( parsingContext->pRenderTarget );
		}
		else
		{
			parsingContext->pWindow->setRenderTarget( parsingContext->pRenderTarget );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMinFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMagFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMipFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setInterpolationMode( InterpolationFilter::eMip, InterpolationMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMinLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = -1000;
			p_params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->pSampler->setMinLod( rValue );
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

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->pSampler->setMaxLod( rValue );
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

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->pSampler->setLodBias( rValue );
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

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setWrappingMode( TextureUVW::eU, WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerVWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setWrappingMode( TextureUVW::eV, WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerWWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setWrappingMode( TextureUVW::eW, WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerBorderColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbaColour colour;
			p_params[0]->get( colour );
			parsingContext->pSampler->setBorderColour( colour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerMaxAnisotropy )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->get( rValue );
			parsingContext->pSampler->setMaxAnisotropy( rValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerComparisonMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setComparisonMode( ComparisonMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSamplerComparisonFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->get( uiMode );
			parsingContext->pSampler->setComparisonFunc( ComparisonFunc( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneInclude )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
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

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour clrBackground;
			p_params[0]->get( clrBackground );
			parsingContext->pScene->setBackgroundColour( clrBackground );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBkImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path path;
			parsingContext->pScene->setBackground( p_context->m_file.getPath(), p_params[0]->get( path ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
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

		if ( !parsingContext->pScene )
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
			parsingContext->pMaterial = parsingContext->pScene->getMaterialView().add( parsingContext->strName, parsingContext->pScene->getMaterialsType() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pSampler = parsingContext->pScene->getSamplerView().add( p_params[0]->get( name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pViewport.reset();
		parsingContext->pSceneNode.reset();

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			parsingContext->pSceneNode.reset();
			p_params[0]->get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eCamera )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pLight.reset();
		parsingContext->strName.clear();
		parsingContext->pSceneNode.reset();

		if ( !parsingContext->pScene )
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
		parsingContext->pSceneNode.reset();
		String name;

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->pSceneNode = parsingContext->pScene->getSceneNodeCache().add( name, parsingContext->pScene->getCameraRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneSceneNode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pSceneNode.reset();
		String name;

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->pSceneNode = parsingContext->pScene->getSceneNodeCache().add( name, parsingContext->pScene->getObjectRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pGeometry.reset();
		String name;

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->get( name );
			parsingContext->pGeometry = parsingContext->pScene->getGeometryCache().add( name, nullptr, nullptr );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneAmbientLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			RgbColour clColour;
			p_params[0]->get( clColour );
			parsingContext->pScene->setAmbientLight( clColour );
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
			parsingContext->pScene->importExternal( pathFile, *importer );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBillboard )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			parsingContext->pBillboards = parsingContext->pScene->getBillboardListCache().add( name, SceneNodeSPtr{} );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboard )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneAnimatedObjectGroup )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			parsingContext->pAnimGroup = parsingContext->pScene->getAnimatedObjectGroupCache().add( name );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimGroup )

	IMPLEMENT_ATTRIBUTE_PARSER( parserScenePanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pOverlay = parsingContext->pScene->getOverlayView().add( p_params[0]->get( name )
				, OverlayType::ePanel
				, parsingContext->pScene
				, parsingContext->pOverlay );
			parsingContext->pOverlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pOverlay = parsingContext->pScene->getOverlayView().add( p_params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->pScene, parsingContext->pOverlay );
			parsingContext->pOverlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pOverlay = parsingContext->pScene->getOverlayView().add( p_params[0]->get( name ), OverlayType::eText, parsingContext->pScene, parsingContext->pOverlay );
			parsingContext->pOverlay->setVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneSkybox )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->pSkybox = std::make_unique< Skybox >( *parsingContext->m_pParser->getEngine() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSkybox )

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneFogType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->get( value );
			parsingContext->pScene->getFog().setType( FogType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneFogDensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			float value;
			p_params[0]->get( value );
			parsingContext->pScene->getFog().setDensity( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSceneParticleSystem )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
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
			parsingContext->pSceneNode.reset();
			parsingContext->pMaterial.reset();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticleSystem )

	IMPLEMENT_ATTRIBUTE_PARSER( parserMesh )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->bBool1 = false;
		p_params[0]->get( parsingContext->strName2 );

		if ( parsingContext->pScene )
		{
			auto const & cache = parsingContext->pScene->getMeshCache();

			if ( cache.has( parsingContext->strName2 ) )
			{
				parsingContext->pMesh = cache.find( parsingContext->strName2 );
			}
			else
			{
				parsingContext->pMesh.reset();
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

		if ( !parsingContext->pScene )
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

			if ( !parsingContext->pScene->getSceneNodeCache().has( value ) )
			{
				PARSING_ERROR( cuT( "No scene node named " ) + value );
			}
			else
			{
				parsingContext->pSceneNode = parsingContext->pScene->getSceneNodeCache().find( value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
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

		if ( !parsingContext->pScene )
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
				parsingContext->pMaterial = cache.find( name );
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

		if ( !parsingContext->pScene )
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
		
		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( parsingContext->uiUInt32 == 0 )
		{
			PARSING_ERROR( cuT( "particles_count has not been specified." ) );
		}
		else if ( parsingContext->point2f[0] == 0 || parsingContext->point2f[1] == 0 )
		{
			PARSING_ERROR( cuT( "one PixelComponents of the particles dimensions is 0." ) );
		}
		else
		{
			if ( !parsingContext->pMaterial )
			{
				parsingContext->pMaterial = parsingContext->m_pParser->getEngine()->getMaterialCache().getDefaultMaterial();
			}

			parsingContext->particleSystem = parsingContext->pScene->getParticleSystemCache().add( parsingContext->strName, parsingContext->pSceneNode, parsingContext->uiUInt32 );
			parsingContext->particleSystem->setMaterial( parsingContext->pMaterial );
			parsingContext->particleSystem->setDimensions( parsingContext->point2f );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticle )

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemTFShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pShaderProgram.reset();
		parsingContext->eShaderObject = ShaderType::eCount;
		parsingContext->bBool1 = true;
		
		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->pShaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleSystemCSShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pShaderProgram.reset();
		parsingContext->eShaderObject = ShaderType::eCount;
		parsingContext->bBool1 = false;
		
		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->pShaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( parserParticleType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pUniform.reset();

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
		parsingContext->pUniform.reset();
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

			parsingContext->particleSystem->addParticleVariable( name, ElementType( type ), value );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eUBOVariable )

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );
			SceneNodeSPtr pParent = parsingContext->pScene->getSceneNodeCache().find( name );

			if ( pParent )
			{
				parsingContext->pSceneNode = pParent;
			}
			else
			{
				PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}

			if ( parsingContext->pLight )
			{
				parsingContext->pLight->detach();
				parsingContext->pSceneNode->attachObject( *parsingContext->pLight );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->get( uiType );
			parsingContext->eLightType = LightType( uiType );
			parsingContext->pLight = parsingContext->pScene->getLightCache().add( parsingContext->strName, parsingContext->pSceneNode, parsingContext->eLightType );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->pLight->setColour( value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f value;
			p_params[0]->get( value );
			parsingContext->pLight->setIntensity( value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserLightAttenuation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );

			if ( parsingContext->eLightType == LightType::ePoint )
			{
				parsingContext->pLight->getPointLight()->setAttenuation( value );
			}
			else if ( parsingContext->eLightType == LightType::eSpot )
			{
				parsingContext->pLight->getSpotLight()->setAttenuation( value );
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

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );

			if ( parsingContext->eLightType == LightType::eSpot )
			{
				parsingContext->pLight->getSpotLight()->setCutOff( Angle::fromDegrees( fFloat ) );
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

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->get( fFloat );

			if ( parsingContext->eLightType == LightType::eSpot )
			{
				parsingContext->pLight->getSpotLight()->setExponent( fFloat );
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

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->pLight->setShadowProducer( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
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
				parent = parsingContext->pScene->getObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext->pScene->getCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext->pScene->getRootNode();
			}
			else
			{
				parent = parsingContext->pScene->getSceneNodeCache().find( name );
			}

			if ( parent )
			{
				parsingContext->pSceneNode->attachTo( parent );
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

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->get( value );
			parsingContext->pSceneNode->setPosition( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeOrientation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r axis;
			float angle;
			p_params[0]->get( axis );
			p_params[1]->get( angle );
			parsingContext->pSceneNode->setOrientation( Quaternion::fromAxisAngle( axis, Angle::fromDegrees( angle ) ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeDirection )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r direction;
			p_params[0]->get( direction );
			Point3r up{ 0, 1, 0 };
			Point3r right{ point::cross( direction, up ) };
			parsingContext->pSceneNode->setOrientation( Quaternion::fromAxes( right, up, direction ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserNodeScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r value;
			p_params[0]->get( value );
			parsingContext->pSceneNode->setScale( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
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
				parent = parsingContext->pScene->getObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext->pScene->getCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext->pScene->getRootNode();
			}
			else
			{
				parent = parsingContext->pScene->getSceneNodeCache().find( name );
			}

			if ( parent )
			{
				parent->attachObject( *parsingContext->pGeometry );
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

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pGeometry->getMesh() )
			{
				auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
				String name;
				p_params[0]->get( name );

				if ( cache.has( name ) )
				{
					for ( auto submesh : *parsingContext->pGeometry->getMesh() )
					{
						MaterialSPtr material = cache.find( name );
						parsingContext->pGeometry->setMaterial( *submesh, material );
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

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->pGeometry->setShadowCaster( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectReceivesShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->get( value );
			parsingContext->pGeometry->setShadowReceiver( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pGeometry.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserObjectMaterialsMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pGeometry->getMesh() )
			{
				auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
				String name;
				uint16_t index;
				p_params[0]->get( index );
				p_params[1]->get( name );

				if ( cache.has( name ) )
				{
					if ( parsingContext->pGeometry->getMesh()->getSubmeshCount() > index )
					{
						SubmeshSPtr submesh = parsingContext->pGeometry->getMesh()->getSubmesh( index );
						MaterialSPtr material = cache.find( name );
						parsingContext->pGeometry->setMaterial( *submesh, material );
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

		if ( !parsingContext->pMesh )
		{
			Parameters parameters;
			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.parse( p_params[1]->get( tmp ) );
			}

			if ( parsingContext->pScene )
			{
				parsingContext->pMesh = parsingContext->pScene->getMeshCache().add( parsingContext->strName2 );
				parsingContext->pScene->getEngine()->getMeshFactory().create( type )->generate( *parsingContext->pMesh, parameters );
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
		parsingContext->iFace1 = -1;
		parsingContext->iFace2 = -1;
		parsingContext->pSubmesh.reset();

		if ( !parsingContext->pMesh )
		{
			PARSING_ERROR( cuT( "No Mesh initialised." ) );
		}
		else
		{
			parsingContext->pSubmesh = parsingContext->pMesh->createSubmesh();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSubmesh )

	IMPLEMENT_ATTRIBUTE_PARSER( parserMeshImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
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
				String tmp;
				StringArray arrayStrParams = string::split( p_params[1]->get( tmp ), cuT( "-" ), 20, false );

				for ( auto param : arrayStrParams )
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
				parsingContext->pMesh = parsingContext->pScene->getMeshCache().add( parsingContext->strName2 );
				auto importer = engine->getImporterFactory().create( extension, *engine );

				if ( !importer->importMesh( *parsingContext->pMesh, pathFile, parameters, true ) )
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

		if ( !parsingContext->pMesh )
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

				if ( arrayStrParams.size() )
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
				Mesh mesh{ cuT( "MorphImport" ), *parsingContext->pScene };

				if ( !importer->importMesh( mesh, pathFile, parameters, false ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
				}
				else if ( mesh.getSubmeshCount() == parsingContext->pMesh->getSubmeshCount() )
				{
					String animName{ "Morph" };

					if ( !parsingContext->pMesh->hasAnimation( animName ) )
					{
						auto & animation = parsingContext->pMesh->createAnimation( animName );

						for ( auto submesh : *parsingContext->pMesh )
						{
							submesh->setAnimated( true );
							animation.addChild( MeshAnimationSubmesh{ animation, *submesh } );
						}
					}

					MeshAnimation & animation{ static_cast< MeshAnimation & >( parsingContext->pMesh->getAnimation( animName ) ) };
					uint32_t index = 0u;

					for ( auto submesh : mesh )
					{
						auto & submeshAnim = animation.getSubmesh( index );
						std::clog << "Source: " << submeshAnim.getSubmesh().getPointsCount() << " - Anim: " << submesh->getPointsCount() << std::endl;

						if ( submesh->getPointsCount() == submeshAnim.getSubmesh().getPointsCount() )
						{
							submeshAnim.addBuffer( Milliseconds{ int64_t( timeIndex * 1000 ) }, convert( submesh->getPoints() ) );
						}

						++index;
					}

					animation.updateLength();
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

		if ( !parsingContext->pMesh )
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
				parsingContext->pMesh->computeContainers();
				Point3r ptCenter = parsingContext->pMesh->getCollisionBox().getCenter();

				for ( auto submesh : *parsingContext->pMesh )
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

		if ( parsingContext->pMesh )
		{
			if ( parsingContext->pGeometry )
			{
				parsingContext->pGeometry->setMesh( parsingContext->pMesh );
			}

			parsingContext->pMesh.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshVertex )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
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

		if ( !parsingContext->pSubmesh )
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

		if ( !parsingContext->pSubmesh )
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

		if ( !parsingContext->pSubmesh )
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

		if ( !parsingContext->pSubmesh )
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

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ) );
			parsingContext->iFace1 = -1;
			parsingContext->iFace2 = -1;

			if ( arrayValues.size() >= 4 )
			{
				Point4i pt4Indices;

				if ( castor::parseValues( strParams, pt4Indices ) )
				{
					parsingContext->iFace1 = int( parsingContext->faces.size() );
					parsingContext->faces.push_back( pt4Indices[0] );
					parsingContext->faces.push_back( pt4Indices[1] );
					parsingContext->faces.push_back( pt4Indices[2] );
					parsingContext->iFace2 = int( parsingContext->faces.size() );
					parsingContext->faces.push_back( pt4Indices[0] );
					parsingContext->faces.push_back( pt4Indices[2] );
					parsingContext->faces.push_back( pt4Indices[3] );
				}
			}
			else if ( castor::parseValues( strParams, pt3Indices ) )
			{
				parsingContext->iFace1 = int( parsingContext->faces.size() );
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

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexTex.size() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 6 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[5] );
			}

			if ( arrayValues.size() >= 8 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[7] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexTex.size() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[8] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[ 9] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[10] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceNormals )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexNml.size() )
			{
				parsingContext->vertexNml.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[ 0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[ 1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[ 2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[ 6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[ 7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[ 8] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[ 9] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[10] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserSubmeshFaceTangents )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexTan.size() )
			{
				parsingContext->vertexTan.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::toReal( arrayValues[2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[3] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[4] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::toReal( arrayValues[5] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::toReal( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[ 0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[ 1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::toReal( arrayValues[ 2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[ 6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[ 7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::toReal( arrayValues[ 8] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[ 9] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[10] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::toReal( arrayValues[11] );
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
				std::memcpy( vertex.m_pos.data(), &parsingContext->vertexPos[index], sizeof( vertex.m_pos ) );

				if ( !parsingContext->vertexNml.empty() )
				{
					std::memcpy( vertex.m_nml.data(), &parsingContext->vertexNml[index], sizeof( vertex.m_nml ) );
				}

				if ( !parsingContext->vertexTan.empty() )
				{
					std::memcpy( vertex.m_tan.data(), &parsingContext->vertexTan[index], sizeof( vertex.m_tan ) );
				}

				if ( !parsingContext->vertexTex.empty() )
				{
					std::memcpy( vertex.m_tex.data(), &parsingContext->vertexTex[index], sizeof( vertex.m_tex ) );
				}

				index += 3;
			}

			parsingContext->pSubmesh->addPoints( vertices );

			if ( parsingContext->faces.size() )
			{
				auto indices = reinterpret_cast< FaceIndices * >( &parsingContext->faces[0] );
				parsingContext->pSubmesh->addFaceGroup( indices, indices + ( parsingContext->faces.size() / 3 ) );

				if ( !parsingContext->vertexNml.empty() )
				{
					if ( !parsingContext->vertexTan.empty() )
					{
						parsingContext->pSubmesh->computeBitangents();
					}
					else
					{
						parsingContext->pSubmesh->computeTangentsFromNormals();
					}
				}
				else
				{
					parsingContext->pSubmesh->computeNormals();
				}
			}

			parsingContext->vertexPos.clear();
			parsingContext->vertexNml.clear();
			parsingContext->vertexTan.clear();
			parsingContext->vertexTex.clear();
			parsingContext->faces.clear();
			parsingContext->pSubmesh->getParent().getScene()->getListener().postEvent( makeInitialiseEvent( *parsingContext->pSubmesh ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserMaterialPass )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->strName.clear();

		if ( parsingContext->pMaterial )
		{
			parsingContext->pass = parsingContext->pMaterial->createPass();

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
		parsingContext->pTextureUnit.reset();

		if ( parsingContext->pass )
		{
			parsingContext->pTextureUnit = std::make_shared< TextureUnit >( *parsingContext->m_pParser->getEngine() );
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
		parsingContext->pShaderProgram.reset();
		parsingContext->eShaderObject = ShaderType::eCount;

		if ( parsingContext->pass )
		{
			parsingContext->pShaderProgram = parsingContext->m_pParser->getEngine()->getShaderProgramCache().getNewProgram( true );
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
			parsingContext->pass->setAlphaFunc( ComparisonFunc( uiFunc ) );
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

		if ( !parsingContext->pTextureUnit )
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
				parsingContext->pTextureUnit->setAutoMipmaps( true );
				auto texture = parsingContext->m_pParser->getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead );
				texture->setSource( folder, relative );

				if ( p_params.size() >= 2 )
				{
					String channels;
					p_params[1]->get( channels );
					auto buffer = texture->getImage().getBuffer();

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
								|| buffer->format() == PixelFormat::eRGBA16F
								|| buffer->format() == PixelFormat::eRGB16F32F
								|| buffer->format() == PixelFormat::eRGBA16F32F )
								? PixelFormat::eL16F32F
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

					texture->getImage().setBuffer( buffer );
				}

				parsingContext->pTextureUnit->setTexture( texture );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else
		{
			parsingContext->pRenderTarget = parsingContext->m_pParser->getEngine()->getRenderTargetCache().add( TargetType::eTexture );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitChannel )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiChannel;
			p_params[0]->get( uiChannel );
			parsingContext->pTextureUnit->setChannel( TextureChannel( uiChannel ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserUnitSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			SamplerSPtr sampler = parsingContext->m_pParser->getEngine()->getSamplerCache().find( p_params[0]->get( name ) );

			if ( sampler )
			{
				parsingContext->pTextureUnit->setSampler( sampler );
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
			if ( parsingContext->pTextureUnit )
			{
				parsingContext->pass->addTextureUnit( parsingContext->pTextureUnit );
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

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->createObject( ShaderType::eVertex );
			parsingContext->eShaderObject = ShaderType::eVertex;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserPixelShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->createObject( ShaderType::ePixel );
			parsingContext->eShaderObject = ShaderType::ePixel;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserGeometryShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->createObject( ShaderType::eGeometry );
			parsingContext->eShaderObject = ShaderType::eGeometry;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserHullShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->createObject( ShaderType::eHull );
			parsingContext->eShaderObject = ShaderType::eHull;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserdomainShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->createObject( ShaderType::eDomain );
			parsingContext->eShaderObject = ShaderType::eDomain;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserComputeShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->createObject( ShaderType::eCompute );
			parsingContext->eShaderObject = ShaderType::eCompute;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( parserConstantsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
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

		if ( parsingContext->pShaderProgram )
		{
			if ( parsingContext->particleSystem )
			{
				if ( parsingContext->bBool1 )
				{
					parsingContext->particleSystem->setTFUpdateProgram( parsingContext->pShaderProgram );
				}
				else
				{
					parsingContext->particleSystem->setCSUpdateProgram( parsingContext->pShaderProgram );
				}

				parsingContext->bBool1 = false;
			}
			//l_parsingContext->pPass->setShader( parsingContext->pShaderProgram );
			parsingContext->pShaderProgram.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderProgramFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				uint32_t uiModel;
				Path path;
				p_params[0]->get( uiModel );
				p_params[1]->get( path );
				parsingContext->pShaderProgram->setFile( parsingContext->eShaderObject, p_context->m_file.getPath() / path );
			}
			else
			{
				PARSING_ERROR( cuT( "Shader not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderProgramSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->get( name );

			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				parsingContext->pSamplerUniform = parsingContext->pShaderProgram->createUniform< UniformType::eSampler >( name, parsingContext->eShaderObject );
			}
			else
			{
				PARSING_ERROR( cuT( "Shader program not initialised" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
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
				parsingContext->pUniformBuffer = std::make_unique< UniformBuffer >( parsingContext->strName
					, *parsingContext->pShaderProgram->getRenderSystem()
					, 1u );
			}
			else
			{
				PARSING_ERROR( cuT( "Unsupported shader type" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserGeometryInputType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->get( uiType );

			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				if ( parsingContext->eShaderObject == ShaderType::eGeometry )
				{
					parsingContext->pShaderProgram->setInputType( parsingContext->eShaderObject, Topology( uiType ) );
				}
				else
				{
					PARSING_ERROR( cuT( "Only valid for geometry shader" ) );
				}
			}
			else
			{
				PARSING_ERROR( cuT( "Shader not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserGeometryOutputType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->get( uiType );

			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				if ( parsingContext->eShaderObject == ShaderType::eGeometry )
				{
					parsingContext->pShaderProgram->setOutputType( parsingContext->eShaderObject, Topology( uiType ) );
				}
				else
				{
					PARSING_ERROR( cuT( "Only valid for geometry shader" ) );
				}
			}
			else
			{
				PARSING_ERROR( cuT( "Shader not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserGeometryOutputVtxCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->eShaderObject != ShaderType::eCount )
		{
			if ( parsingContext->eShaderObject == ShaderType::eGeometry )
			{
				uint8_t count;
				p_params[0]->get( count );
				parsingContext->pShaderProgram->setOutputVtxCount( parsingContext->eShaderObject, count );
			}
			else
			{
				PARSING_ERROR( cuT( "Only valid for geometry shader" ) );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserShaderUboVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pUniform.reset();
		p_params[0]->get( parsingContext->strName2 );

		if ( !parsingContext->pUniformBuffer )
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

		if ( parsingContext->pUniformBuffer )
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

		if ( parsingContext->pUniformBuffer )
		{
			if ( !parsingContext->pUniform )
			{
				parsingContext->pUniform = parsingContext->pUniformBuffer->createUniform( UniformType( uiType ), parsingContext->strName2, parsingContext->uiUInt32 );
			}
			else
			{
				PARSING_ERROR( cuT( "Variable type already set" ) );
			}
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

		if ( parsingContext->pUniform )
		{
			parsingContext->pUniform->setStrValue( strParams );
		}
		else
		{
			PARSING_ERROR( cuT( "Variable not initialised" ) );
		}
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
			if ( parsingContext->pScene )
			{
				parsingContext->pScene->getFontView().add( parsingContext->strName, parsingContext->iInt16, p_context->m_file.getPath() / parsingContext->path );
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

		if ( parsingContext->pOverlay )
		{
			Point2d ptPosition;
			p_params[0]->get( ptPosition );
			parsingContext->pOverlay->setPosition( ptPosition );
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

		if ( parsingContext->pOverlay )
		{
			Point2d ptSize;
			p_params[0]->get( ptSize );
			parsingContext->pOverlay->setSize( ptSize );
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

		if ( parsingContext->pOverlay )
		{
			Size size;
			p_params[0]->get( size );
			parsingContext->pOverlay->setPixelSize( size );
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

		if ( parsingContext->pOverlay )
		{
			Position position;
			p_params[0]->get( position );
			parsingContext->pOverlay->setPixelPosition( position );
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

		if ( parsingContext->pOverlay )
		{
			String name;
			p_params[0]->get( name );
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			parsingContext->pOverlay->setMaterial( cache.find( name ) );
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
		parsingContext->pOverlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::ePanel, parsingContext->pOverlay->getScene(), parsingContext->pOverlay );
		parsingContext->pOverlay->setVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->pOverlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eBorderPanel, parsingContext->pOverlay->getScene(), parsingContext->pOverlay );
		parsingContext->pOverlay->setVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->pOverlay = parsingContext->m_pParser->getEngine()->getOverlayCache().add( p_params[0]->get( name ), OverlayType::eText, parsingContext->pOverlay->getScene(), parsingContext->pOverlay );
		parsingContext->pOverlay->setVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( parserOverlayEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay->getType() == OverlayType::eText )
		{
			auto textOverlay = parsingContext->pOverlay->getTextOverlay();

			if ( textOverlay->getFontTexture() )
			{
				parsingContext->pOverlay->setVisible( true );
			}
			else
			{
				parsingContext->pOverlay->setVisible( false );
				PARSING_ERROR( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			parsingContext->pOverlay->setVisible( true );
		}

		parsingContext->pOverlay = parsingContext->pOverlay->getParent();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserPanelOverlayUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;

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
		OverlaySPtr overlay = parsingContext->pOverlay;
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
		SceneNodeSPtr pParent = parsingContext->pScene->getSceneNodeCache().find( p_params[0]->get( name ) );

		if ( pParent )
		{
			parsingContext->pSceneNode = pParent;

			while ( pParent->getParent() && pParent->getParent() != parsingContext->pScene->getObjectRootNode() && pParent->getParent() != parsingContext->pScene->getCameraRootNode() )
			{
				pParent = pParent->getParent();
			}

			if ( !pParent->getParent() || pParent->getParent() == parsingContext->pScene->getObjectRootNode() )
			{
				pParent->attachTo( parsingContext->pScene->getCameraRootNode() );
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
		parsingContext->pViewport = std::make_shared< Viewport >( *parsingContext->m_pParser->getEngine() );
		parsingContext->pViewport->setPerspective( Angle::fromDegrees( 0 ), 1, 0, 1 );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eViewport )

	IMPLEMENT_ATTRIBUTE_PARSER( parserCameraPrimitive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t uiType;
		parsingContext->ePrimitiveType = Topology( p_params[0]->get( uiType ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserCameraEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSceneNode && parsingContext->pViewport )
		{
			parsingContext->pScene->getCameraCache().add( parsingContext->strName, parsingContext->pSceneNode, std::move( *parsingContext->pViewport ) );
			parsingContext->pViewport.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() > 0 )
		{
			uint32_t uiType;
			parsingContext->pViewport->updateType( ViewportType( p_params[0]->get( uiType ) ) );
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
		parsingContext->pViewport->updateLeft( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->pViewport->updateRight( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->pViewport->updateTop( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->pViewport->updateBottom( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportNear )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->pViewport->updateNear( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportFar )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->get( rValue );
		parsingContext->pViewport->updateFar( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size size;
		p_params[0]->get( size );
		parsingContext->pViewport->resize( size );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportFovY )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fFovY;
		p_params[0]->get( fFovY );
		parsingContext->pViewport->updateFovY( Angle::fromDegrees( fFovY ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserViewportAspectRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fRatio;
		p_params[0]->get( fRatio );
		parsingContext->pViewport->updateRatio( fRatio );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pBillboards )
		{
			String name;
			p_params[0]->get( name );
			SceneNodeSPtr pParent = parsingContext->pScene->getSceneNodeCache().find( name );

			if ( pParent )
			{
				pParent->attachObject( *parsingContext->pBillboards );
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

		if ( !parsingContext->pBillboards )
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

			parsingContext->pBillboards->setBillboardType( BillboardType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pBillboards )
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

			parsingContext->pBillboards->setBillboardSize( BillboardSize( value ) );
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

		if ( parsingContext->pBillboards )
		{
			auto & cache = parsingContext->m_pParser->getEngine()->getMaterialCache();
			String name;
			p_params[0]->get( name );

			if ( cache.has( name ) )
			{
				parsingContext->pBillboards->setMaterial( cache.find( name ) );
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
		parsingContext->pBillboards->setDimensions( dimensions );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pBillboards = nullptr;
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( parserBillboardPoint )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point3r position;
		p_params[0]->get( position );
		parsingContext->pBillboards->addPoint( position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( parserAnimatedObjectGroupAnimatedObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		p_params[0]->get( name );

		if ( parsingContext->pAnimGroup )
		{
			GeometrySPtr geometry = parsingContext->pScene->getGeometryCache().find( name );

			if ( geometry )
			{
				if ( !geometry->getAnimations().empty() )
				{
					parsingContext->pAnimMovable = parsingContext->pAnimGroup->addObject( *geometry, geometry->getName() + cuT( "_Movable" ) );
				}

				if ( geometry->getMesh() )
				{
					auto mesh = geometry->getMesh();

					if ( !mesh->getAnimations().empty() )
					{
						parsingContext->pAnimMesh = parsingContext->pAnimGroup->addObject( *mesh, geometry->getName() + cuT( "_Mesh" ) );
					}

					auto skeleton = mesh->getSkeleton();

					if ( skeleton )
					{
						if ( !skeleton->getAnimations().empty() )
						{
							parsingContext->pAnimSkeleton = parsingContext->pAnimGroup->addObject( *skeleton, geometry->getName() + cuT( "_Skeleton" ) );
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
		else if ( !parsingContext->pSkybox )
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
				auto texture = parsingContext->pScene->getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead );
				texture->getImage().initialiseSource( filePath, path );
				parsingContext->pSkybox->setEquiTexture( texture
					, size );
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

		if ( parsingContext->pSkybox )
		{
			Path path;
			parsingContext->pSkybox->getTexture().getImage( uint32_t( CubeMapFace::eNegativeX ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
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

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->pSkybox->getTexture().getImage( uint32_t( CubeMapFace::ePositiveX ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
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

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->pSkybox->getTexture().getImage( uint32_t( CubeMapFace::eNegativeY ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
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

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->pSkybox->getTexture().getImage( uint32_t( CubeMapFace::ePositiveY ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
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

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->pSkybox->getTexture().getImage( uint32_t( CubeMapFace::eNegativeZ ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
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

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file.getPath() / p_params[0]->get( path );
			parsingContext->pSkybox->getTexture().getImage( uint32_t( CubeMapFace::ePositiveZ ) ).initialiseSource( p_context->m_file.getPath(), p_params[0]->get( path ) );
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

		if ( parsingContext->pSkybox )
		{
			parsingContext->pScene->setForeground( std::move( parsingContext->pSkybox ) );
			parsingContext->pSkybox.reset();
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

		if ( parsingContext->pRenderTarget )
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoRadius )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pRenderTarget )
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

		if ( parsingContext->pRenderTarget )
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

	IMPLEMENT_ATTRIBUTE_PARSER( parserSsaoEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pRenderTarget )
		{
			parsingContext->pRenderTarget->setSsaoConfig( parsingContext->ssaoConfig );
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
}
