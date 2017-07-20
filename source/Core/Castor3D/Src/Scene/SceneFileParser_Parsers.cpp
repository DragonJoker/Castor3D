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

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		InterleavedVertexArray Convert( VertexPtrArray const & p_points )
		{
			InterleavedVertexArray result;
			result.reserve( p_points.size() );

			for ( auto point : p_points )
			{
				InterleavedVertex vertex;
				Vertex::GetPosition( *point, vertex.m_pos );
				Vertex::GetNormal( *point, vertex.m_nml );
				Vertex::GetTangent( *point, vertex.m_tan );
				Vertex::GetBitangent( *point, vertex.m_bin );
				Vertex::GetTexCoord( *point, vertex.m_tex );
				result.push_back( vertex );
			}

			return result;
		}
	}

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMtlFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			Path path;
			path = p_context->m_file->GetFilePath() / p_params[0]->Get( path );

			if ( File::FileExists( path ) )
			{
				TextFile fileMat( path, File::OpenMode::eRead, File::EncodingMode::eASCII );
				Logger::LogInfo( cuT( "Loading materials file : " ) + path );

				if ( parsingContext->m_pParser->GetEngine()->GetMaterialCache().Read( fileMat ) )
				{
					Logger::LogInfo( cuT( "Materials read" ) );
				}
				else
				{
					Logger::LogInfo( cuT( "Can't read materials" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			p_params[0]->Get( name );
			parsingContext->pScene = parsingContext->m_pParser->GetEngine()->GetSceneCache().Add( name );
			parsingContext->mapScenes.insert( std::make_pair( name, parsingContext->pScene ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->path.clear();

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->Get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eFont )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->Get( parsingContext->strName );
			parsingContext->pMaterial = parsingContext->m_pParser->GetEngine()->GetMaterialCache().Add( parsingContext->strName
				, MaterialType::eLegacy );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->pOverlay = parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( name ), OverlayType::ePanel, nullptr, parsingContext->pOverlay );
			parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->pOverlay = parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( name ), OverlayType::eBorderPanel, nullptr, parsingContext->pOverlay );
			parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->pOverlay = parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( name ), OverlayType::eText, nullptr, parsingContext->pOverlay );
			parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext->pSampler = parsingContext->m_pParser->GetEngine()->GetSamplerCache().Add( p_params[0]->Get( name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootDebugOverlays )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			parsingContext->m_pParser->GetEngine()->GetRenderLoop().ShowDebugOverlays( p_params[0]->Get( value ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootWindow )
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
				p_params[0]->Get( name );
				parsingContext->pWindow = parsingContext->pScene->GetEngine()->GetRenderWindowCache().Add( name );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eWindow )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_WindowRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pWindow )
		{
			parsingContext->pRenderTarget = parsingContext->m_pParser->GetEngine()->GetRenderTargetCache().Add( TargetType::eWindow );
			parsingContext->iInt16 = 0;
		}
		else
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_WindowVSync )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pWindow )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool bValue;
			p_params[0]->Get( bValue );
			parsingContext->pWindow->SetVSync( bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_WindowFullscreen )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pWindow )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{

			bool bValue;
			p_params[0]->Get( bValue );
			parsingContext->pWindow->SetFullscreen( bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetScene )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			ScenePtrStrMap::iterator it = parsingContext->mapScenes.find( p_params[0]->Get( name ) );

			if ( it != parsingContext->mapScenes.end() )
			{
				parsingContext->pRenderTarget->SetScene( it->second );
			}
			else
			{
				PARSING_ERROR( cuT( "No scene found with name : [" ) + name + cuT( "]." ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetCamera )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pRenderTarget->GetScene() )
			{
				parsingContext->pRenderTarget->SetCamera( parsingContext->pRenderTarget->GetScene()->GetCameraCache().Find( p_params[0]->Get( name ) ) );
			}
			else
			{
				PARSING_ERROR( cuT( "No scene initialised for this window, set scene before camera." ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Size size;
			parsingContext->pRenderTarget->SetSize( p_params[0]->Get( size ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetFormat )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			PixelFormat ePF;
			p_params[0]->Get( ePF );

			if ( ePF < PixelFormat::eD16 )
			{
				parsingContext->pRenderTarget->SetPixelFormat( ePF );
			}
			else
			{
				PARSING_ERROR( cuT( "Wrong format for colour" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetStereo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rIntraOcularDistance;
			p_params[0]->Get( rIntraOcularDistance );

			if ( rIntraOcularDistance > 0 )
			{
				//l_parsingContext->pRenderTarget->SetStereo( true );
				//l_parsingContext->pRenderTarget->SetIntraOcularDistance( rIntraOcularDistance );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetPostEffect )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );
			Parameters parameters;

			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.Parse( p_params[1]->Get( tmp ) );
			}

			Engine * engine = parsingContext->m_pParser->GetEngine();

			if ( !engine->GetRenderTargetCache().GetPostEffectFactory().IsRegistered( string::lower_case( name ) ) )
			{
				PARSING_ERROR( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				PostEffectSPtr effect = engine->GetRenderTargetCache().GetPostEffectFactory().Create( name, *parsingContext->pRenderTarget, *engine->GetRenderSystem(), parameters );
				parsingContext->pRenderTarget->AddPostEffect( effect );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetToneMapping )
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
			p_params[0]->Get( name );
			Parameters parameters;

			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.Parse( p_params[1]->Get( tmp ) );
			}

			parsingContext->pRenderTarget->SetToneMappingType( name, parameters );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetSsao )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No render target initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSsao )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pRenderTarget->GetTargetType() == TargetType::eTexture )
		{
			parsingContext->pTextureUnit->SetRenderTarget( parsingContext->pRenderTarget );
		}
		else
		{
			parsingContext->pWindow->SetRenderTarget( parsingContext->pRenderTarget );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMinFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMagFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMipFilter )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::eMip, InterpolationMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMinLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = -1000;
			p_params[0]->Get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->pSampler->SetMinLod( rValue );
			}
			else
			{
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::to_string( rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMaxLod )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->Get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->pSampler->SetMaxLod( rValue );
			}
			else
			{
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::to_string( rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerLodBias )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->Get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				parsingContext->pSampler->SetLodBias( rValue );
			}
			else
			{
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::to_string( rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerUWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetWrappingMode( TextureUVW::eU, WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerVWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetWrappingMode( TextureUVW::eV, WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerWWrapMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetWrappingMode( TextureUVW::eW, WrapMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerBorderColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour colour;
			p_params[0]->Get( colour );
			parsingContext->pSampler->SetBorderColour( colour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMaxAnisotropy )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real rValue = 1000;
			p_params[0]->Get( rValue );
			parsingContext->pSampler->SetMaxAnisotropy( rValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerComparisonMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetComparisonMode( ComparisonMode( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerComparisonFunc )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiMode;
			p_params[0]->Get( uiMode );
			parsingContext->pSampler->SetComparisonFunc( ComparisonFunc( uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneInclude )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path path;
			p_params[0]->Get( path );
			SceneFileParser parser{ *parsingContext->m_pParser->GetEngine() };
			parser.ParseFile( parsingContext->m_file->GetFilePath() / path, parsingContext );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneMaterials )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->Get( value );
			parsingContext->pScene->SetMaterialsType( MaterialType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBkColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour clrBackground;
			p_params[0]->Get( clrBackground );
			parsingContext->pScene->SetBackgroundColour( clrBackground );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBkImage )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path path;
			parsingContext->pScene->SetBackground( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneFont )
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
			p_params[0]->Get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eFont )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneMaterial )
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
			p_params[0]->Get( parsingContext->strName );
			parsingContext->pMaterial = parsingContext->pScene->GetMaterialView().Add( parsingContext->strName, parsingContext->pScene->GetMaterialsType() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneSamplerState )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pSampler = parsingContext->pScene->GetSamplerView().Add( p_params[0]->Get( name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneCamera )
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
			p_params[0]->Get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eCamera )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneLight )
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
			p_params[0]->Get( parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eLight )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneCameraNode )
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
			p_params[0]->Get( name );
			parsingContext->pSceneNode = parsingContext->pScene->GetSceneNodeCache().Add( name, parsingContext->pScene->GetCameraRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneSceneNode )
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
			p_params[0]->Get( name );
			parsingContext->pSceneNode = parsingContext->pScene->GetSceneNodeCache().Add( name, parsingContext->pScene->GetObjectRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneObject )
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
			p_params[0]->Get( name );
			parsingContext->pGeometry = parsingContext->pScene->GetGeometryCache().Add( name, nullptr, nullptr );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneAmbientLight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour clColour;
			p_params[0]->Get( clColour );
			parsingContext->pScene->SetAmbientLight( clColour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Path path;
		Path pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( path );

		Engine * engine = parsingContext->m_pParser->GetEngine();
		auto extension = string::lower_case( pathFile.GetExtension() );

		if ( !engine->GetImporterFactory().IsRegistered( extension ) )
		{
			PARSING_ERROR( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
		}
		else
		{
			auto importer = engine->GetImporterFactory().Create( extension, *engine );
			parsingContext->pScene->ImportExternal( pathFile, *importer );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBillboard )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );
			parsingContext->pBillboards = parsingContext->pScene->GetBillboardListCache().Add( name, SceneNodeSPtr{} );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboard )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneAnimatedObjectGroup )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );
			parsingContext->pAnimGroup = parsingContext->pScene->GetAnimatedObjectGroupCache().Add( name );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimGroup )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ScenePanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pOverlay = parsingContext->pScene->GetOverlayView().Add( p_params[0]->Get( name )
				, OverlayType::ePanel
				, parsingContext->pScene
				, parsingContext->pOverlay );
			parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pOverlay = parsingContext->pScene->GetOverlayView().Add( p_params[0]->Get( name ), OverlayType::eBorderPanel, parsingContext->pScene, parsingContext->pOverlay );
			parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			parsingContext->pOverlay = parsingContext->pScene->GetOverlayView().Add( p_params[0]->Get( name ), OverlayType::eText, parsingContext->pScene, parsingContext->pOverlay );
			parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneSkybox )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext->pSkybox = std::make_unique< Skybox >( *parsingContext->m_pParser->GetEngine() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSkybox )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneFogType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t value;
			p_params[0]->Get( value );
			parsingContext->pScene->GetFog().SetType( GLSL::FogType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneFogDensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			float value;
			p_params[0]->Get( value );
			parsingContext->pScene->GetFog().SetDensity( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneParticleSystem )
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
			p_params[0]->Get( value );
			parsingContext->strName = value;
			parsingContext->uiUInt32 = 0;
			parsingContext->pSceneNode.reset();
			parsingContext->pMaterial.reset();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticleSystem )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemParent )
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
			p_params[0]->Get( value );

			if ( !parsingContext->pScene->GetSceneNodeCache().Has( value ) )
			{
				PARSING_ERROR( cuT( "No scene node named " ) + value );
			}
			else
			{
				parsingContext->pSceneNode = parsingContext->pScene->GetSceneNodeCache().Find( value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemCount )
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
			p_params[0]->Get( value );
			parsingContext->uiUInt32 = value;
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemMaterial )
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
			auto & cache = parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			String name;
			p_params[0]->Get( name );

			if ( cache.Has( name ) )
			{
				parsingContext->pMaterial = cache.Find( name );
			}
			else
			{
				PARSING_ERROR( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	END_ATTRIBUTE()

		IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemDimensions )
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
			p_params[0]->Get( parsingContext->size );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemParticle )
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
		else if ( parsingContext->size.width() == 0 || parsingContext->size.height() == 0 )
		{
			PARSING_ERROR( cuT( "one component of the particles dimensions is 0." ) );
		}
		else
		{
			if ( !parsingContext->pMaterial )
			{
				parsingContext->pMaterial = parsingContext->m_pParser->GetEngine()->GetMaterialCache().GetDefaultMaterial();
			}

			parsingContext->particleSystem = parsingContext->pScene->GetParticleSystemCache().Add( parsingContext->strName, parsingContext->pSceneNode, parsingContext->uiUInt32 );
			parsingContext->particleSystem->SetMaterial( parsingContext->pMaterial );
			parsingContext->particleSystem->SetDimensions( parsingContext->size );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticle )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemTFShader )
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
			parsingContext->pShaderProgram = parsingContext->m_pParser->GetEngine()->GetShaderProgramCache().GetNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemCSShader )
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
			parsingContext->pShaderProgram = parsingContext->m_pParser->GetEngine()->GetShaderProgramCache().GetNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleType )
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
			p_params[0]->Get( value );
			Engine * engine = parsingContext->m_pParser->GetEngine();

			if ( !engine->GetParticleFactory().IsRegistered( string::lower_case( value ) ) )
			{
				PARSING_ERROR( cuT( "Particle type [" ) + value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext->particleSystem->SetParticleType( value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pUniform.reset();
		p_params[0]->Get( parsingContext->strName2 );

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
			p_params[0]->Get( name );
			p_params[1]->Get( type );

			if ( p_params.size() > 2 )
			{
				p_params[2]->Get( value );
			}

			parsingContext->particleSystem->AddParticleVariable( name, ElementType( type ), value );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eUBOVariable )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );
			SceneNodeSPtr pParent = parsingContext->pScene->GetSceneNodeCache().Find( name );

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
				parsingContext->pLight->Detach();
				parsingContext->pSceneNode->AttachObject( *parsingContext->pLight );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->Get( uiType );
			parsingContext->eLightType = LightType( uiType );
			parsingContext->pLight = parsingContext->pScene->GetLightCache().Add( parsingContext->strName, parsingContext->pSceneNode, parsingContext->eLightType );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightColour )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );
			parsingContext->pLight->SetColour( value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightIntensity )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f value;
			p_params[0]->Get( value );
			parsingContext->pLight->SetIntensity( value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightAttenuation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );

			if ( parsingContext->eLightType == LightType::ePoint )
			{
				parsingContext->pLight->GetPointLight()->SetAttenuation( value );
			}
			else if ( parsingContext->eLightType == LightType::eSpot )
			{
				parsingContext->pLight->GetSpotLight()->SetAttenuation( value );
			}
			else
			{
				PARSING_ERROR( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightCutOff )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->Get( fFloat );

			if ( parsingContext->eLightType == LightType::eSpot )
			{
				parsingContext->pLight->GetSpotLight()->SetCutOff( Angle::from_degrees( fFloat ) );
			}
			else
			{
				PARSING_ERROR( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightExponent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->Get( fFloat );

			if ( parsingContext->eLightType == LightType::eSpot )
			{
				parsingContext->pLight->GetSpotLight()->SetExponent( fFloat );
			}
			else
			{
				PARSING_ERROR( cuT( "Wrong type of light to apply an exponent, needs spotlight" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightShadowProducer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->Get( value );
			parsingContext->pLight->SetShadowProducer( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );
			SceneNodeSPtr parent;

			if ( name == Scene::ObjectRootNode )
			{
				parent = parsingContext->pScene->GetObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext->pScene->GetCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext->pScene->GetRootNode();
			}
			else
			{
				parent = parsingContext->pScene->GetSceneNodeCache().Find( name );
			}

			if ( parent )
			{
				parsingContext->pSceneNode->AttachTo( parent );
			}
			else
			{
				PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodePosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );
			parsingContext->pSceneNode->SetPosition( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeOrientation )
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
			p_params[0]->Get( axis );
			p_params[1]->Get( angle );
			parsingContext->pSceneNode->SetOrientation( Quaternion::from_axis_angle( axis, Angle::from_degrees( angle ) ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r value;
			p_params[0]->Get( value );
			parsingContext->pSceneNode->SetScale( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );
			SceneNodeSPtr parent;

			if ( name == Scene::ObjectRootNode )
			{
				parent = parsingContext->pScene->GetObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext->pScene->GetCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext->pScene->GetRootNode();
			}
			else
			{
				parent = parsingContext->pScene->GetSceneNodeCache().Find( name );
			}

			if ( parent )
			{
				parent->AttachObject( *parsingContext->pGeometry );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMesh )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->bBool1 = false;
		p_params[0]->Get( parsingContext->strName2 );

		if ( parsingContext->pScene )
		{
			auto const & cache = parsingContext->pScene->GetMeshCache();

			if ( cache.Has( parsingContext->strName2 ) )
			{
				parsingContext->pMesh = cache.Find( parsingContext->strName2 );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pGeometry->GetMesh() )
			{
				auto & cache = parsingContext->m_pParser->GetEngine()->GetMaterialCache();
				String name;
				p_params[0]->Get( name );

				if ( cache.Has( name ) )
				{
					for ( auto submesh : *parsingContext->pGeometry->GetMesh() )
					{
						MaterialSPtr material = cache.Find( name );
						parsingContext->pGeometry->SetMaterial( *submesh, material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMaterials )
	{
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eObjectMaterials )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectCastShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->Get( value );
			parsingContext->pGeometry->SetShadowCaster( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectReceivesShadows )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->Get( value );
			parsingContext->pGeometry->SetShadowReceiver( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pGeometry.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMaterialsMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( parsingContext->pGeometry->GetMesh() )
			{
				auto & cache = parsingContext->m_pParser->GetEngine()->GetMaterialCache();
				String name;
				uint16_t index;
				p_params[0]->Get( index );
				p_params[1]->Get( name );

				if ( cache.Has( name ) )
				{
					SubmeshSPtr submesh = parsingContext->pGeometry->GetMesh()->GetSubmesh( index );
					MaterialSPtr material = cache.Find( name );
					parsingContext->pGeometry->SetMaterial( *submesh, material );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMaterialsEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String type;
		p_params[0]->Get( type );

		if ( !parsingContext->pMesh )
		{
			Parameters parameters;
			if ( p_params.size() > 1 )
			{
				String tmp;
				parameters.Parse( p_params[1]->Get( tmp ) );
			}

			if ( parsingContext->pScene )
			{
				parsingContext->pMesh = parsingContext->pScene->GetMeshCache().Add( parsingContext->strName2 );
				parsingContext->pScene->GetEngine()->GetMeshFactory().Create( type )->Generate( *parsingContext->pMesh, parameters );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshSubmesh )
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
			parsingContext->pSubmesh = parsingContext->pMesh->CreateSubmesh();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSubmesh )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			Path path;
			Path pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			Parameters parameters;

			if ( p_params.size() > 1 )
			{
				String tmp;
				StringArray arrayStrParams = string::split( p_params[1]->Get( tmp ), cuT( "-" ), 20, false );

				if ( arrayStrParams.size() )
				{
					for ( StringArrayConstIt it = arrayStrParams.begin(); it != arrayStrParams.end(); ++it )
					{
						if ( it->find( cuT( "smooth_normals" ) ) == 0 )
						{
							String strNml = cuT( "smooth" );
							parameters.Add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
						}
						else if ( it->find( cuT( "flat_normals" ) ) == 0 )
						{
							String strNml = cuT( "flat" );
							parameters.Add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
						}
						else if ( it->find( cuT( "tangent_space" ) ) == 0 )
						{
							parameters.Add( cuT( "tangent_space" ), true );
						}
					}
				}
			}

			Engine * engine = parsingContext->m_pParser->GetEngine();
			auto extension = string::lower_case( pathFile.GetExtension() );

			if ( !engine->GetImporterFactory().IsRegistered( extension ) )
			{
				PARSING_ERROR( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext->pMesh = parsingContext->pScene->GetMeshCache().Add( parsingContext->strName2 );
				auto importer = engine->GetImporterFactory().Create( extension, *engine );

				if ( !importer->ImportMesh( *parsingContext->pMesh, pathFile, parameters, true ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshMorphImport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pMesh )
		{
			PARSING_ERROR( cuT( "No mesh initialised." ) );
		}
		else
		{
			real timeIndex;
			p_params[1]->Get( timeIndex );
			Path path;
			Path pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			Parameters parameters;

			if ( p_params.size() > 2 )
			{
				String tmp;
				StringArray arrayStrParams = string::split( p_params[2]->Get( tmp ), cuT( "-" ), 20, false );

				if ( arrayStrParams.size() )
				{
					for ( StringArrayConstIt it = arrayStrParams.begin(); it != arrayStrParams.end(); ++it )
					{
						if ( it->find( cuT( "smooth_normals" ) ) == 0 )
						{
							String strNml = cuT( "smooth" );
							parameters.Add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
						}
						else if ( it->find( cuT( "flat_normals" ) ) == 0 )
						{
							String strNml = cuT( "flat" );
							parameters.Add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
						}
						else if ( it->find( cuT( "tangent_space" ) ) == 0 )
						{
							parameters.Add( cuT( "tangent_space" ), true );
						}
					}
				}
			}

			Engine * engine = parsingContext->m_pParser->GetEngine();
			auto extension = string::lower_case( pathFile.GetExtension() );

			if ( !engine->GetImporterFactory().IsRegistered( extension ) )
			{
				PARSING_ERROR( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto importer = engine->GetImporterFactory().Create( extension, *engine );
				Mesh mesh{ cuT( "MorphImport" ), *parsingContext->pScene };

				if ( !importer->ImportMesh( mesh, pathFile, parameters, false ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
				}
				else if ( mesh.GetSubmeshCount() == parsingContext->pMesh->GetSubmeshCount() )
				{
					String animName{ "Morph" };

					if ( !parsingContext->pMesh->HasAnimation( animName ) )
					{
						auto & animation = parsingContext->pMesh->CreateAnimation( animName );

						for ( auto submesh : *parsingContext->pMesh )
						{
							submesh->SetAnimated( true );
							animation.AddChild( MeshAnimationSubmesh{ animation, *submesh } );
						}
					}

					MeshAnimation & animation{ static_cast< MeshAnimation & >( parsingContext->pMesh->GetAnimation( animName ) ) };
					uint32_t index = 0u;

					for ( auto submesh : mesh )
					{
						auto & submeshAnim = animation.GetSubmesh( index );
						std::clog << "Source: " << submeshAnim.GetSubmesh().GetPointsCount() << " - Anim: " << submesh->GetPointsCount() << std::endl;

						if ( submesh->GetPointsCount() == submeshAnim.GetSubmesh().GetPointsCount() )
						{
							submeshAnim.AddBuffer( std::chrono::milliseconds{ int64_t( timeIndex * 1000 ) }, Convert( submesh->GetPoints() ) );
						}

						++index;
					}

					animation.UpdateLength();
				}
				else
				{
					PARSING_ERROR( cuT( "The new mesh doesn't match the original mesh" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshDivide )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Engine * engine = parsingContext->m_pParser->GetEngine();

		if ( !parsingContext->pMesh )
		{
			PARSING_ERROR( cuT( "No Mesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			uint16_t count;
			p_params[0]->Get( name );
			p_params[1]->Get( count );

			if ( !engine->GetSubdividerFactory().IsRegistered( string::lower_case( name ) ) )
			{
				PARSING_ERROR( cuT( "Subdivider [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto divider = engine->GetSubdividerFactory().Create( name );
				parsingContext->pMesh->ComputeContainers();
				Point3r ptCenter = parsingContext->pMesh->GetCollisionBox().GetCenter();

				for ( auto submesh : *parsingContext->pMesh )
				{
					divider->Subdivide( submesh, count, false );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pMesh )
		{
			parsingContext->pGeometry->SetMesh( parsingContext->pMesh );
			parsingContext->pMesh.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshVertex )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );
			parsingContext->vertexPos.push_back( value[0] );
			parsingContext->vertexPos.push_back( value[1] );
			parsingContext->vertexPos.push_back( value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f value;
			p_params[0]->Get( value );
			parsingContext->vertexTex.push_back( value[0] );
			parsingContext->vertexTex.push_back( value[1] );
			parsingContext->vertexTex.push_back( 0.0 );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );
			parsingContext->vertexTex.push_back( value[0] );
			parsingContext->vertexTex.push_back( value[1] );
			parsingContext->vertexTex.push_back( value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshNormal )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );
			parsingContext->vertexNml.push_back( value[0] );
			parsingContext->vertexNml.push_back( value[1] );
			parsingContext->vertexNml.push_back( value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshTangent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f value;
			p_params[0]->Get( value );
			parsingContext->vertexTan.push_back( value[0] );
			parsingContext->vertexTan.push_back( value[1] );
			parsingContext->vertexTan.push_back( value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->Get( strParams );
			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ) );
			parsingContext->iFace1 = -1;
			parsingContext->iFace2 = -1;

			if ( arrayValues.size() >= 4 )
			{
				Point4i pt4Indices;

				if ( Castor::ParseValues( strParams, pt4Indices ) )
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
			else if ( Castor::ParseValues( strParams, pt3Indices ) )
			{
				parsingContext->iFace1 = int( parsingContext->faces.size() );
				parsingContext->faces.push_back( pt3Indices[0] );
				parsingContext->faces.push_back( pt3Indices[1] );
				parsingContext->faces.push_back( pt3Indices[2] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceUV )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->Get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexTex.size() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 6 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[2] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[3] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[5] );
			}

			if ( arrayValues.size() >= 8 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[1] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[4] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[7] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceUVW )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->Get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexTex.size() )
			{
				parsingContext->vertexTex.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[3] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[4] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[5] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[0] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[1] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[2] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[6] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[7] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[8] );
				parsingContext->vertexTex[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[ 9] );
				parsingContext->vertexTex[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[10] );
				parsingContext->vertexTex[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceNormals )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->Get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexNml.size() )
			{
				parsingContext->vertexNml.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[3] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[4] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[5] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[ 0] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[ 1] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[ 2] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[ 6] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[ 7] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[ 8] );
				parsingContext->vertexNml[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[ 9] );
				parsingContext->vertexNml[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[10] );
				parsingContext->vertexNml[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceTangents )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String strParams;
			p_params[0]->Get( strParams );
			SubmeshSPtr submesh = parsingContext->pSubmesh;

			if ( !parsingContext->vertexTan.size() )
			{
				parsingContext->vertexTan.resize( parsingContext->vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext->iFace1 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace1 + 0] * 3] = string::to_real( arrayValues[2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[3] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[4] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace1 + 1] * 3] = string::to_real( arrayValues[5] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace1 + 2] * 3] = string::to_real( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext->iFace2 != -1 )
			{
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[ 0] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[ 1] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace2 + 0] * 3] = string::to_real( arrayValues[ 2] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[ 6] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[ 7] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace2 + 1] * 3] = string::to_real( arrayValues[ 8] );
				parsingContext->vertexTan[0 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[ 9] );
				parsingContext->vertexTan[1 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[10] );
				parsingContext->vertexTan[2 + parsingContext->faces[parsingContext->iFace2 + 2] * 3] = string::to_real( arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshEnd )
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

			parsingContext->pSubmesh->AddPoints( vertices );

			if ( parsingContext->faces.size() )
			{
				auto indices = reinterpret_cast< FaceIndices * >( &parsingContext->faces[0] );
				parsingContext->pSubmesh->AddFaceGroup( indices, indices + ( parsingContext->faces.size() / 3 ) );

				if ( !parsingContext->vertexNml.empty() )
				{
					if ( !parsingContext->vertexTan.empty() )
					{
						parsingContext->pSubmesh->ComputeBitangents();
					}
					else
					{
						parsingContext->pSubmesh->ComputeTangentsFromNormals();
					}
				}
				else
				{
					parsingContext->pSubmesh->ComputeNormals();
				}
			}

			parsingContext->vertexPos.clear();
			parsingContext->vertexNml.clear();
			parsingContext->vertexTan.clear();
			parsingContext->vertexTex.clear();
			parsingContext->faces.clear();
			parsingContext->pSubmesh->GetParent().GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *parsingContext->pSubmesh ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialPass )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->strName.clear();

		if ( parsingContext->pMaterial )
		{
			parsingContext->pass = parsingContext->pMaterial->CreatePass();

			switch ( parsingContext->pass->GetType() )
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassDiffuse )
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
				Colour crColour;
				p_params[0]->Get( crColour );
				parsingContext->legacyPass->SetDiffuse( crColour );
			}
			else
			{
				Colour crColour;
				p_params[0]->Get( crColour );
				parsingContext->pbrSGPass->SetDiffuse( crColour );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassSpecular )
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
				Colour crColour;
				p_params[0]->Get( crColour );
				parsingContext->legacyPass->SetSpecular( crColour );
			}
			else
			{
				Colour crColour;
				p_params[0]->Get( crColour );
				parsingContext->pbrSGPass->SetSpecular( crColour );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAmbient )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->Get( value );
			parsingContext->legacyPass->SetAmbient( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEmissive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->Get( value );
			parsingContext->pass->SetEmissive( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShininess )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->Get( fFloat );
			parsingContext->legacyPass->SetShininess( fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlbedo )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour value;
			p_params[0]->Get( value );
			parsingContext->pbrMRPass->SetAlbedo( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassRoughness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->Get( value );
			parsingContext->pbrMRPass->SetRoughness( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassMetallic )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrMRPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->Get( value );
			parsingContext->pbrMRPass->SetMetallic( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassGlossiness )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pbrSGPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value;
			p_params[0]->Get( value );
			parsingContext->pbrSGPass->SetGlossiness( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlpha )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float fFloat;
			p_params[0]->Get( fFloat );
			parsingContext->pass->SetOpacity( fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassDoubleFace )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool value;
			p_params[0]->Get( value );
			parsingContext->pass->SetTwoSided( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassTextureUnit )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pTextureUnit.reset();

		if ( parsingContext->pass )
		{
			parsingContext->pTextureUnit = std::make_shared< TextureUnit >( *parsingContext->m_pParser->GetEngine() );
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextureUnit )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pShaderProgram.reset();
		parsingContext->eShaderObject = ShaderType::eCount;

		if ( parsingContext->pass )
		{
			parsingContext->pShaderProgram = parsingContext->m_pParser->GetEngine()->GetShaderProgramCache().GetNewProgram( true );
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlphaBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode = 0;
			p_params[0]->Get( mode );
			parsingContext->pass->SetAlphaBlendMode( BlendMode( mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassColourBlendMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t mode = 0;
			p_params[0]->Get( mode );
			parsingContext->pass->SetColourBlendMode( BlendMode( mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlphaFunc )
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
			p_params[0]->Get( uiFunc );
			p_params[1]->Get( fFloat );
			parsingContext->pass->SetAlphaFunc( ComparisonFunc( uiFunc ) );
			parsingContext->pass->SetAlphaValue( fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassRefractionRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float value = 0;
			p_params[0]->Get( value );
			parsingContext->pass->SetRefractionRatio( value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEnd )
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitImage )
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
			p_params[0]->Get( relative );

			if ( File::FileExists( p_context->m_file->GetFilePath() / relative ) )
			{
				folder = p_context->m_file->GetFilePath();
			}
			else if ( !File::FileExists( relative ) )
			{
				PARSING_ERROR( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				parsingContext->pTextureUnit->SetAutoMipmaps( true );
				auto texture = parsingContext->m_pParser->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead );
				texture->SetSource( folder, relative );

				if ( p_params.size() >= 2 )
				{
					String channels;
					p_params[1]->Get( channels );
					auto buffer = texture->GetImage().GetBuffer();

					if ( channels == cuT( "rgb" ) )
					{
						buffer = PxBufferBase::create( buffer->dimensions()
							, PF::GetPFWithoutAlpha( buffer->format() )
							, buffer->const_ptr()
							, buffer->format() );
					}
					else if ( channels == cuT( "a" ) )
					{
						auto tmp = PF::ExtractAlpha( buffer );
						buffer = tmp;
					}

					texture->GetImage().SetBuffer( buffer );
				}

				parsingContext->pTextureUnit->SetTexture( texture );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitRenderTarget )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else
		{
			parsingContext->pRenderTarget = parsingContext->m_pParser->GetEngine()->GetRenderTargetCache().Add( TargetType::eTexture );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitChannel )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiChannel;
			p_params[0]->Get( uiChannel );
			parsingContext->pTextureUnit->SetChannel( TextureChannel( uiChannel ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			SamplerSPtr sampler = parsingContext->m_pParser->GetEngine()->GetSamplerCache().Find( p_params[0]->Get( name ) );

			if ( sampler )
			{
				parsingContext->pTextureUnit->SetSampler( sampler );
			}
			else
			{
				PARSING_ERROR( cuT( "Unknown sampler : [" ) + name + cuT( "]" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pass )
		{
			if ( parsingContext->pTextureUnit )
			{
				parsingContext->pass->AddTextureUnit( parsingContext->pTextureUnit );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_VertexShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->CreateObject( ShaderType::eVertex );
			parsingContext->eShaderObject = ShaderType::eVertex;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PixelShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->CreateObject( ShaderType::ePixel );
			parsingContext->eShaderObject = ShaderType::ePixel;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_GeometryShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->CreateObject( ShaderType::eGeometry );
			parsingContext->eShaderObject = ShaderType::eGeometry;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_HullShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->CreateObject( ShaderType::eHull );
			parsingContext->eShaderObject = ShaderType::eHull;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_DomainShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->CreateObject( ShaderType::eDomain );
			parsingContext->eShaderObject = ShaderType::eDomain;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComputeShader )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			parsingContext->pShaderProgram->CreateObject( ShaderType::eCompute );
			parsingContext->eShaderObject = ShaderType::eCompute;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ConstantsBuffer )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->Get( parsingContext->strName );

			if ( parsingContext->strName.empty() )
			{
				PARSING_ERROR( cuT( "Invalid empty name" ) );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderUBO )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pShaderProgram )
		{
			if ( parsingContext->particleSystem )
			{
				if ( parsingContext->bBool1 )
				{
					parsingContext->particleSystem->SetTFUpdateProgram( parsingContext->pShaderProgram );
				}
				else
				{
					parsingContext->particleSystem->SetCSUpdateProgram( parsingContext->pShaderProgram );
				}

				parsingContext->bBool1 = false;
			}
			//l_parsingContext->pPass->SetShader( parsingContext->pShaderProgram );
			parsingContext->pShaderProgram.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderProgramFile )
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
				p_params[0]->Get( uiModel );
				p_params[1]->Get( path );
				parsingContext->pShaderProgram->SetFile( parsingContext->eShaderObject, p_context->m_file->GetFilePath() / path );
			}
			else
			{
				PARSING_ERROR( cuT( "Shader not initialised" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderProgramSampler )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String name;
			p_params[0]->Get( name );

			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				parsingContext->pSamplerUniform = parsingContext->pShaderProgram->CreateUniform< UniformType::eSampler >( name, parsingContext->eShaderObject );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderUboShaders )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !p_params.empty() )
		{
			uint32_t value;
			p_params[0]->Get( value );

			if ( value )
			{
				parsingContext->pUniformBuffer = std::make_unique< UniformBuffer >( parsingContext->strName, *parsingContext->pShaderProgram->GetRenderSystem() );
			}
			else
			{
				PARSING_ERROR( cuT( "Unsupported shader type" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_GeometryInputType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->Get( uiType );

			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				if ( parsingContext->eShaderObject == ShaderType::eGeometry )
				{
					parsingContext->pShaderProgram->SetInputType( parsingContext->eShaderObject, Topology( uiType ) );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_GeometryOutputType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t uiType;
			p_params[0]->Get( uiType );

			if ( parsingContext->eShaderObject != ShaderType::eCount )
			{
				if ( parsingContext->eShaderObject == ShaderType::eGeometry )
				{
					parsingContext->pShaderProgram->SetOutputType( parsingContext->eShaderObject, Topology( uiType ) );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_GeometryOutputVtxCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->eShaderObject != ShaderType::eCount )
		{
			if ( parsingContext->eShaderObject == ShaderType::eGeometry )
			{
				uint8_t count;
				p_params[0]->Get( count );
				parsingContext->pShaderProgram->SetOutputVtxCount( parsingContext->eShaderObject, count );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderUboVariable )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pUniform.reset();
		p_params[0]->Get( parsingContext->strName2 );

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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableCount )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t param;
		p_params[0]->Get( param );

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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t uiType;
		p_params[0]->Get( uiType );

		if ( parsingContext->pUniformBuffer )
		{
			if ( !parsingContext->pUniform )
			{
				parsingContext->pUniform = parsingContext->pUniformBuffer->CreateUniform( UniformType( uiType ), parsingContext->strName2, parsingContext->uiUInt32 );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableValue )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String strParams;
		p_params[0]->Get( strParams );

		if ( parsingContext->pUniform )
		{
			parsingContext->pUniform->SetStrValue( strParams );
		}
		else
		{
			PARSING_ERROR( cuT( "Variable not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontFile )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->Get( parsingContext->path );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontHeight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->Get( parsingContext->iInt16 );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !parsingContext->strName.empty() && !parsingContext->path.empty() )
		{
			if ( parsingContext->pScene )
			{
				parsingContext->pScene->GetFontView().Add( parsingContext->strName, parsingContext->iInt16, p_context->m_file->GetFilePath() / parsingContext->path );
			}
			else
			{
				parsingContext->m_pParser->GetEngine()->GetFontCache().Add( parsingContext->strName, parsingContext->iInt16, p_context->m_file->GetFilePath() / parsingContext->path );
			}
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay )
		{
			Point2d ptPosition;
			p_params[0]->Get( ptPosition );
			parsingContext->pOverlay->SetPosition( ptPosition );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlaySize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay )
		{
			Point2d ptSize;
			p_params[0]->Get( ptSize );
			parsingContext->pOverlay->SetSize( ptSize );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPixelSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay )
		{
			Size size;
			p_params[0]->Get( size );
			parsingContext->pOverlay->SetPixelSize( size );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPixelPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay )
		{
			Position position;
			p_params[0]->Get( position );
			parsingContext->pOverlay->SetPixelPosition( position );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay )
		{
			String name;
			p_params[0]->Get( name );
			auto & cache = parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			parsingContext->pOverlay->SetMaterial( cache.Find( name ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->pOverlay = parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( name ), OverlayType::ePanel, parsingContext->pOverlay->GetScene(), parsingContext->pOverlay );
		parsingContext->pOverlay->SetVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayBorderPanelOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->pOverlay = parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( name ), OverlayType::eBorderPanel, parsingContext->pOverlay->GetScene(), parsingContext->pOverlay );
		parsingContext->pOverlay->SetVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayTextOverlay )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		parsingContext->pOverlay = parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( name ), OverlayType::eText, parsingContext->pOverlay->GetScene(), parsingContext->pOverlay );
		parsingContext->pOverlay->SetVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pOverlay->GetType() == OverlayType::eText )
		{
			auto textOverlay = parsingContext->pOverlay->GetTextOverlay();

			if ( textOverlay->GetFontTexture() )
			{
				parsingContext->pOverlay->SetVisible( true );
			}
			else
			{
				parsingContext->pOverlay->SetVisible( false );
				PARSING_ERROR( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			parsingContext->pOverlay->SetVisible( true );
		}

		parsingContext->pOverlay = parsingContext->pOverlay->GetParent();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PanelOverlayUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::ePanel )
		{
			Point4d uvs;
			p_params[0]->Get( uvs );
			overlay->GetPanelOverlay()->SetUV( uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlaySizes )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d ptSize;
			p_params[0]->Get( ptSize );
			overlay->GetBorderPanelOverlay()->SetBorderSize( ptSize );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayPixelSizes )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			Rectangle size;
			p_params[0]->Get( size );
			overlay->GetBorderPanelOverlay()->SetBorderPixelSize( size );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			String name;
			p_params[0]->Get( name );
			auto & cache = parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			overlay->GetBorderPanelOverlay()->SetBorderMaterial( cache.Find( name ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayPosition )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			uint32_t position;
			p_params[0]->Get( position );
			overlay->GetBorderPanelOverlay()->SetBorderPosition( BorderPosition( position ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayCenterUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			p_params[0]->Get( uvs );
			overlay->GetBorderPanelOverlay()->SetUV( uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayOuterUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			p_params[0]->Get( uvs );
			overlay->GetBorderPanelOverlay()->SetBorderOuterUV( uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayInnerUvs )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			p_params[0]->Get( uvs );
			overlay->GetBorderPanelOverlay()->SetBorderInnerUV( uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayFont )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			auto & cache = parsingContext->m_pParser->GetEngine()->GetFontCache();
			String name;
			p_params[0]->Get( name );

			if ( cache.Find( name ) )
			{
				overlay->GetTextOverlay()->SetFont( name );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayTextWrapping )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->Get( value );

			overlay->GetTextOverlay()->SetTextWrappingMode( TextWrappingMode( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayVerticalAlign )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->Get( value );

			overlay->GetTextOverlay()->SetVAlign( VAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayHorizontalAlign )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->Get( value );

			overlay->GetTextOverlay()->SetHAlign( HAlign( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayTexturingMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->Get( value );

			overlay->GetTextOverlay()->SetTexturingMode( TextTexturingMode( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayLineSpacingMode )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			uint32_t value;
			p_params[0]->Get( value );

			overlay->GetTextOverlay()->SetLineSpacingMode( TextLineSpacingMode( value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayText )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr overlay = parsingContext->pOverlay;
		String strParams;
		p_params[0]->Get( strParams );

		if ( overlay && overlay->GetType() == OverlayType::eText )
		{
			string::replace( strParams, cuT( "\\n" ), cuT( "\n" ) );
			overlay->GetTextOverlay()->SetCaption( strParams );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		SceneNodeSPtr pParent = parsingContext->pScene->GetSceneNodeCache().Find( p_params[0]->Get( name ) );

		if ( pParent )
		{
			parsingContext->pSceneNode = pParent;

			while ( pParent->GetParent() && pParent->GetParent() != parsingContext->pScene->GetObjectRootNode() && pParent->GetParent() != parsingContext->pScene->GetCameraRootNode() )
			{
				pParent = pParent->GetParent();
			}

			if ( !pParent->GetParent() || pParent->GetParent() == parsingContext->pScene->GetObjectRootNode() )
			{
				pParent->AttachTo( parsingContext->pScene->GetCameraRootNode() );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Node " ) + name + cuT( " does not exist" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraViewport )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pViewport = std::make_shared< Viewport >( *parsingContext->m_pParser->GetEngine() );
		parsingContext->pViewport->SetPerspective( Angle::from_degrees( 0 ), 1, 0, 1 );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eViewport )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraPrimitive )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t uiType;
		parsingContext->ePrimitiveType = Topology( p_params[0]->Get( uiType ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSceneNode && parsingContext->pViewport )
		{
			parsingContext->pScene->GetCameraCache().Add( parsingContext->strName, parsingContext->pSceneNode, std::move( *parsingContext->pViewport ) );
			parsingContext->pViewport.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportType )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() > 0 )
		{
			uint32_t uiType;
			parsingContext->pViewport->UpdateType( ViewportType( p_params[0]->Get( uiType ) ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->Get( rValue );
		parsingContext->pViewport->UpdateLeft( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->Get( rValue );
		parsingContext->pViewport->UpdateRight( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->Get( rValue );
		parsingContext->pViewport->UpdateTop( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->Get( rValue );
		parsingContext->pViewport->UpdateBottom( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportNear )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->Get( rValue );
		parsingContext->pViewport->UpdateNear( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportFar )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real rValue;
		p_params[0]->Get( rValue );
		parsingContext->pViewport->UpdateFar( rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportSize )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size size;
		p_params[0]->Get( size );
		parsingContext->pViewport->Resize( size );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportFovY )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fFovY;
		p_params[0]->Get( fFovY );
		parsingContext->pViewport->UpdateFovY( Angle::from_degrees( fFovY ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportAspectRatio )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float fRatio;
		p_params[0]->Get( fRatio );
		parsingContext->pViewport->UpdateRatio( fRatio );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardParent )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pBillboards )
		{
			String name;
			p_params[0]->Get( name );
			SceneNodeSPtr pParent = parsingContext->pScene->GetSceneNodeCache().Find( name );

			if ( pParent )
			{
				pParent->AttachObject( *parsingContext->pBillboards );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardType )
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
			p_params[0]->Get( value );

			parsingContext->pBillboards->SetBillboardType( BillboardType( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardSize )
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
			p_params[0]->Get( value );

			parsingContext->pBillboards->SetBillboardSize( BillboardSize( value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardPositions )
	{
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboardList )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardMaterial )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pBillboards )
		{
			auto & cache = parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			String name;
			p_params[0]->Get( name );

			if ( cache.Has( name ) )
			{
				parsingContext->pBillboards->SetMaterial( cache.Find( name ) );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardDimensions )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size dimensions;
		p_params[0]->Get( dimensions );
		parsingContext->pBillboards->SetDimensions( dimensions );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		parsingContext->pBillboards = nullptr;
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardPoint )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point3r position;
		p_params[0]->Get( position );
		parsingContext->pBillboards->AddPoint( position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimatedObject )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		p_params[0]->Get( name );

		if ( parsingContext->pAnimGroup )
		{
			GeometrySPtr geometry = parsingContext->pScene->GetGeometryCache().Find( name );

			if ( geometry )
			{
				if ( !geometry->GetAnimations().empty() )
				{
					parsingContext->pAnimMovable = parsingContext->pAnimGroup->AddObject( *geometry, geometry->GetName() + cuT( "_Movable" ) );
				}

				if ( geometry->GetMesh() )
				{
					auto mesh = geometry->GetMesh();

					if ( !mesh->GetAnimations().empty() )
					{
						parsingContext->pAnimMesh = parsingContext->pAnimGroup->AddObject( *mesh, geometry->GetName() + cuT( "_Mesh" ) );
					}

					auto skeleton = mesh->GetSkeleton();

					if ( skeleton )
					{
						if ( !skeleton->GetAnimations().empty() )
						{
							parsingContext->pAnimSkeleton = parsingContext->pAnimGroup->AddObject( *skeleton, geometry->GetName() + cuT( "_Skeleton" ) );
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimation )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->Get( parsingContext->strName2 );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->AddAnimation( parsingContext->strName2 );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimation )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimationStart )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String name;
		p_params[0]->Get( name );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->StartAnimation( name );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupEnd )
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

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimationLooped )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		bool value;
		p_params[0]->Get( value );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->SetAnimationLooped( parsingContext->strName2, value );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimationScale )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float value;
		p_params[0]->Get( value );

		if ( parsingContext->pAnimGroup )
		{
			parsingContext->pAnimGroup->SetAnimationScale( parsingContext->strName2, value );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimationEnd )
	{
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxEqui )
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
			Path filePath = p_context->m_file->GetFilePath();
			p_params[0]->Get( path );

			if ( File::FileExists( filePath / path ) )
			{
				Size size;
				p_params[1]->Get( size );
				auto texture = parsingContext->pScene->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead );
				texture->GetImage().InitialiseSource( filePath, path );
				parsingContext->pSkybox->SetEquiTexture( texture
					, size );
			}
			else
			{
				PARSING_ERROR( cuT( "Couldn't load the image" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxLeft )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			Path path;
			parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxRight )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxTop )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxBottom )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxFront )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxBack )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			Path path;
			path = p_context->m_file->GetFilePath() / p_params[0]->Get( path );
			parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pSkybox )
		{
			parsingContext->pScene->SetForeground( std::move( parsingContext->pSkybox ) );
			parsingContext->pSkybox.reset();
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SsaoEnabled )
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
				p_params[0]->Get( value );
				parsingContext->ssaoConfig.m_enabled = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SsaoRadius )
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
				p_params[0]->Get( value );
				parsingContext->ssaoConfig.m_radius = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SsaoBias )
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
				p_params[0]->Get( value );
				parsingContext->ssaoConfig.m_bias = value;
			}
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SsaoEnd )
	{
		SceneFileContextSPtr parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( parsingContext->pRenderTarget )
		{
			parsingContext->pRenderTarget->SetSsaoConfig( parsingContext->ssaoConfig );
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()
}
