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
#include "Material/PbrPass.hpp"
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
			InterleavedVertexArray l_return;
			l_return.reserve( p_points.size() );

			for ( auto l_point : p_points )
			{
				InterleavedVertex l_vertex;
				Vertex::GetPosition( *l_point, l_vertex.m_pos );
				Vertex::GetNormal( *l_point, l_vertex.m_nml );
				Vertex::GetTangent( *l_point, l_vertex.m_tan );
				Vertex::GetBitangent( *l_point, l_vertex.m_bin );
				Vertex::GetTexCoord( *l_point, l_vertex.m_tex );
				l_return.push_back( l_vertex );
			}

			return l_return;
		}
	}

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMtlFile )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			Path l_path;
			l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );

			if ( File::FileExists( l_path ) )
			{
				TextFile l_fileMat( l_path, File::OpenMode::eRead, File::EncodingMode::eASCII );
				Logger::LogInfo( cuT( "Loading materials file : " ) + l_path );

				if ( l_parsingContext->m_pParser->GetEngine()->GetMaterialCache().Read( l_fileMat ) )
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_parsingContext->pScene = l_parsingContext->m_pParser->GetEngine()->GetSceneCache().Add( l_name );
			l_parsingContext->mapScenes.insert( std::make_pair( l_name, l_parsingContext->pScene ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootFont )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->path.clear();

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->Get( l_parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eFont )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->Get( l_parsingContext->strName );
			l_parsingContext->pMaterial = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache().Add( l_parsingContext->strName
				, MaterialType::eLegacy );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootPanelOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( l_name ), OverlayType::ePanel, nullptr, l_parsingContext->pOverlay );
			l_parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootBorderPanelOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( l_name ), OverlayType::eBorderPanel, nullptr, l_parsingContext->pOverlay );
			l_parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootTextOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( l_name ), OverlayType::eText, nullptr, l_parsingContext->pOverlay );
			l_parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootSamplerState )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			l_parsingContext->pSampler = l_parsingContext->m_pParser->GetEngine()->GetSamplerCache().Add( p_params[0]->Get( l_name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootDebugOverlays )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			bool l_value;
			l_parsingContext->m_pParser->GetEngine()->GetRenderLoop().ShowDebugOverlays( p_params[0]->Get( l_value ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eScene )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RootWindow )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( l_parsingContext->pWindow )
			{
				PARSING_ERROR( cuT( "Can't create more than one render window" ) );
			}
			else
			{
				String l_name;
				p_params[0]->Get( l_name );
				l_parsingContext->pWindow = l_parsingContext->pScene->GetEngine()->GetRenderWindowCache().Add( l_name );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eWindow )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_WindowRenderTarget )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pWindow )
		{
			l_parsingContext->pRenderTarget = l_parsingContext->m_pParser->GetEngine()->GetRenderTargetCache().Add( TargetType::eWindow );
			l_parsingContext->iInt16 = 0;
		}
		else
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_WindowVSync )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pWindow )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool l_bValue;
			p_params[0]->Get( l_bValue );
			l_parsingContext->pWindow->SetVSync( l_bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_WindowFullscreen )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pWindow )
		{
			PARSING_ERROR( cuT( "No window initialised." ) );
		}
		else if ( !p_params.empty() )
		{

			bool l_bValue;
			p_params[0]->Get( l_bValue );
			l_parsingContext->pWindow->SetFullscreen( l_bValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetScene )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			ScenePtrStrMap::iterator l_it = l_parsingContext->mapScenes.find( p_params[0]->Get( l_name ) );

			if ( l_it != l_parsingContext->mapScenes.end() )
			{
				l_parsingContext->pRenderTarget->SetScene( l_it->second );
			}
			else
			{
				PARSING_ERROR( cuT( "No scene found with name : [" ) + l_name + cuT( "]." ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetCamera )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( l_parsingContext->pRenderTarget->GetScene() )
			{
				l_parsingContext->pRenderTarget->SetCamera( l_parsingContext->pRenderTarget->GetScene()->GetCameraCache().Find( p_params[0]->Get( l_name ) ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Size l_size;
			l_parsingContext->pRenderTarget->SetSize( p_params[0]->Get( l_size ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetFormat )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			PixelFormat l_ePF;
			p_params[0]->Get( l_ePF );

			if ( l_ePF < PixelFormat::eD16 )
			{
				l_parsingContext->pRenderTarget->SetPixelFormat( l_ePF );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real l_rIntraOcularDistance;
			p_params[0]->Get( l_rIntraOcularDistance );

			if ( l_rIntraOcularDistance > 0 )
			{
				//l_parsingContext->pRenderTarget->SetStereo( true );
				//l_parsingContext->pRenderTarget->SetIntraOcularDistance( l_rIntraOcularDistance );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetPostEffect )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );
			Parameters l_parameters;

			if ( p_params.size() > 1 )
			{
				String l_tmp;
				l_parameters.Parse( p_params[1]->Get( l_tmp ) );
			}

			Engine * l_engine = l_parsingContext->m_pParser->GetEngine();

			if ( !l_engine->GetRenderTargetCache().GetPostEffectFactory().IsRegistered( string::lower_case( l_name ) ) )
			{
				PARSING_ERROR( cuT( "PostEffect [" ) + l_name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				PostEffectSPtr l_effect = l_engine->GetRenderTargetCache().GetPostEffectFactory().Create( l_name, *l_parsingContext->pRenderTarget, *l_engine->GetRenderSystem(), l_parameters );
				l_parsingContext->pRenderTarget->AddPostEffect( l_effect );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetToneMapping )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No target initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			p_params[0]->Get( l_name );
			Parameters l_parameters;

			if ( p_params.size() > 1 )
			{
				String l_tmp;
				l_parameters.Parse( p_params[1]->Get( l_tmp ) );
			}

			l_parsingContext->pRenderTarget->SetToneMappingType( l_name, l_parameters );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetSsao )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pRenderTarget )
		{
			PARSING_ERROR( cuT( "No render target initialised." ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSsao )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_RenderTargetEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pRenderTarget->GetTargetType() == TargetType::eTexture )
		{
			l_parsingContext->pTextureUnit->SetRenderTarget( l_parsingContext->pRenderTarget );
		}
		else
		{
			l_parsingContext->pWindow->SetRenderTarget( l_parsingContext->pRenderTarget );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMinFilter )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::eMin, InterpolationMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMagFilter )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::eMag, InterpolationMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMipFilter )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::eMip, InterpolationMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMinLod )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real l_rValue = -1000;
			p_params[0]->Get( l_rValue );

			if ( l_rValue >= -1000 && l_rValue <= 1000 )
			{
				l_parsingContext->pSampler->SetMinLod( l_rValue );
			}
			else
			{
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::to_string( l_rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMaxLod )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real l_rValue = 1000;
			p_params[0]->Get( l_rValue );

			if ( l_rValue >= -1000 && l_rValue <= 1000 )
			{
				l_parsingContext->pSampler->SetMaxLod( l_rValue );
			}
			else
			{
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::to_string( l_rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerLodBias )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real l_rValue = 1000;
			p_params[0]->Get( l_rValue );

			if ( l_rValue >= -1000 && l_rValue <= 1000 )
			{
				l_parsingContext->pSampler->SetLodBias( l_rValue );
			}
			else
			{
				PARSING_ERROR( cuT( "LOD out of bounds [-1000,1000] : " ) + string::to_string( l_rValue ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerUWrapMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetWrappingMode( TextureUVW::eU, WrapMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerVWrapMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetWrappingMode( TextureUVW::eV, WrapMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerWWrapMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetWrappingMode( TextureUVW::eW, WrapMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerBorderColour )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour l_colour;
			p_params[0]->Get( l_colour );
			l_parsingContext->pSampler->SetBorderColour( l_colour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerMaxAnisotropy )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			real l_rValue = 1000;
			p_params[0]->Get( l_rValue );
			l_parsingContext->pSampler->SetMaxAnisotropy( l_rValue );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerComparisonMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetComparisonMode( ComparisonMode( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SamplerComparisonFunc )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSampler )
		{
			PARSING_ERROR( cuT( "No sampler initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiMode;
			p_params[0]->Get( l_uiMode );
			l_parsingContext->pSampler->SetComparisonFunc( ComparisonFunc( l_uiMode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneInclude )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path l_path;
			p_params[0]->Get( l_path );
			SceneFileParser l_parser{ *l_parsingContext->m_pParser->GetEngine() };
			l_parser.ParseFile( l_parsingContext->m_file->GetFilePath() / l_path, l_parsingContext );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneMaterials )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pScene->SetMaterialsType( MaterialType( l_value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBkColour )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour l_clrBackground;
			p_params[0]->Get( l_clrBackground );
			l_parsingContext->pScene->SetBackgroundColour( l_clrBackground );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBkImage )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path l_path;
			l_parsingContext->pScene->SetBackground( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneFont )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
			l_parsingContext->path.clear();
			p_params[0]->Get( l_parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eFont )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->Get( l_parsingContext->strName );
			l_parsingContext->pMaterial = l_parsingContext->pScene->GetMaterialView().Add( l_parsingContext->strName, l_parsingContext->pScene->GetMaterialsType() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eMaterial )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneSamplerState )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			l_parsingContext->pSampler = l_parsingContext->pScene->GetSamplerView().Add( p_params[0]->Get( l_name ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSampler )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneCamera )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pViewport.reset();
		l_parsingContext->pSceneNode.reset();

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			l_parsingContext->pSceneNode.reset();
			p_params[0]->Get( l_parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eCamera )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneLight )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pLight.reset();
		l_parsingContext->strName.clear();
		l_parsingContext->pSceneNode.reset();

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->Get( l_parsingContext->strName );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eLight )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneCameraNode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pSceneNode.reset();
		String l_name;

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->Get( l_name );
			l_parsingContext->pSceneNode = l_parsingContext->pScene->GetSceneNodeCache().Add( l_name, l_parsingContext->pScene->GetCameraRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneSceneNode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pSceneNode.reset();
		String l_name;

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->Get( l_name );
			l_parsingContext->pSceneNode = l_parsingContext->pScene->GetSceneNodeCache().Add( l_name, l_parsingContext->pScene->GetObjectRootNode() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eNode )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneObject )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pGeometry.reset();
		String l_name;

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->Get( l_name );
			l_parsingContext->pGeometry = l_parsingContext->pScene->GetGeometryCache().Add( l_name, nullptr, nullptr );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneAmbientLight )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour l_clColour;
			p_params[0]->Get( l_clColour );
			l_parsingContext->pScene->SetAmbientLight( l_clColour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneImport )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Path l_path;
		Path l_pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );

		Engine * l_engine = l_parsingContext->m_pParser->GetEngine();
		auto l_extension = string::lower_case( l_pathFile.GetExtension() );

		if ( !l_engine->GetImporterFactory().IsRegistered( l_extension ) )
		{
			PARSING_ERROR( cuT( "Importer for [" ) + l_extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
		}
		else
		{
			auto l_importer = l_engine->GetImporterFactory().Create( l_extension, *l_engine );
			l_parsingContext->pScene->ImportExternal( l_pathFile, *l_importer );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBillboard )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_parsingContext->pBillboards = l_parsingContext->pScene->GetBillboardListCache().Add( l_name, SceneNodeSPtr{} );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboard )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneAnimatedObjectGroup )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );
			l_parsingContext->pAnimGroup = l_parsingContext->pScene->GetAnimatedObjectGroupCache().Add( l_name );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimGroup )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ScenePanelOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			l_parsingContext->pOverlay = l_parsingContext->pScene->GetOverlayView().Add( p_params[0]->Get( l_name )
				, OverlayType::ePanel
				, l_parsingContext->pScene
				, l_parsingContext->pOverlay );
			l_parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneBorderPanelOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			l_parsingContext->pOverlay = l_parsingContext->pScene->GetOverlayView().Add( p_params[0]->Get( l_name ), OverlayType::eBorderPanel, l_parsingContext->pScene, l_parsingContext->pOverlay );
			l_parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneTextOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			l_parsingContext->pOverlay = l_parsingContext->pScene->GetOverlayView().Add( p_params[0]->Get( l_name ), OverlayType::eText, l_parsingContext->pScene, l_parsingContext->pOverlay );
			l_parsingContext->pOverlay->SetVisible( false );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneSkybox )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			l_parsingContext->pSkybox = std::make_unique< Skybox >( *l_parsingContext->m_pParser->GetEngine() );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSkybox )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneFogType )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pScene->GetFog().SetType( GLSL::FogType( l_value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneFogDensity )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			float l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pScene->GetFog().SetDensity( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SceneParticleSystem )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->strName = l_value;
			l_parsingContext->uiUInt32 = 0;
			l_parsingContext->pSceneNode.reset();
			l_parsingContext->pMaterial.reset();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticleSystem )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemParent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_value;
			p_params[0]->Get( l_value );

			if ( !l_parsingContext->pScene->GetSceneNodeCache().Has( l_value ) )
			{
				PARSING_ERROR( cuT( "No scene node named " ) + l_value );
			}
			else
			{
				l_parsingContext->pSceneNode = l_parsingContext->pScene->GetSceneNodeCache().Find( l_value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemCount )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->uiUInt32 = l_value;
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			String l_name;
			p_params[0]->Get( l_name );

			if ( l_cache.Has( l_name ) )
			{
				l_parsingContext->pMaterial = l_cache.Find( l_name );
			}
			else
			{
				PARSING_ERROR( cuT( "Material " ) + l_name + cuT( " does not exist" ) );
			}
		}
	}
	END_ATTRIBUTE()

		IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemDimensions )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			p_params[0]->Get( l_parsingContext->size );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemParticle )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		
		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( l_parsingContext->uiUInt32 == 0 )
		{
			PARSING_ERROR( cuT( "particles_count has not been specified." ) );
		}
		else if ( l_parsingContext->size.width() == 0 || l_parsingContext->size.height() == 0 )
		{
			PARSING_ERROR( cuT( "one component of the particles dimensions is 0." ) );
		}
		else
		{
			if ( !l_parsingContext->pMaterial )
			{
				l_parsingContext->pMaterial = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache().GetDefaultMaterial();
			}

			l_parsingContext->particleSystem = l_parsingContext->pScene->GetParticleSystemCache().Add( l_parsingContext->strName, l_parsingContext->pSceneNode, l_parsingContext->uiUInt32 );
			l_parsingContext->particleSystem->SetMaterial( l_parsingContext->pMaterial );
			l_parsingContext->particleSystem->SetDimensions( l_parsingContext->size );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eParticle )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemTFShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pShaderProgram.reset();
		l_parsingContext->eShaderObject = ShaderType::eCount;
		l_parsingContext->bBool1 = true;
		
		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			l_parsingContext->pShaderProgram = l_parsingContext->m_pParser->GetEngine()->GetShaderProgramCache().GetNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleSystemCSShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pShaderProgram.reset();
		l_parsingContext->eShaderObject = ShaderType::eCount;
		l_parsingContext->bBool1 = false;
		
		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			l_parsingContext->pShaderProgram = l_parsingContext->m_pParser->GetEngine()->GetShaderProgramCache().GetNewProgram( true );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleType )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pUniform.reset();

		if ( !l_parsingContext->particleSystem )
		{
			PARSING_ERROR( cuT( "Particle system not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_value;
			p_params[0]->Get( l_value );
			Engine * l_engine = l_parsingContext->m_pParser->GetEngine();

			if ( !l_engine->GetParticleFactory().IsRegistered( string::lower_case( l_value ) ) )
			{
				PARSING_ERROR( cuT( "Particle type [" ) + l_value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				l_parsingContext->particleSystem->SetParticleType( l_value );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ParticleVariable )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pUniform.reset();
		p_params[0]->Get( l_parsingContext->strName2 );

		if ( !l_parsingContext->particleSystem )
		{
			PARSING_ERROR( cuT( "Particle system not initialised" ) );
		}
		else if ( p_params.size() < 2 )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else
		{
			String l_name;
			uint32_t l_type;
			String l_value;
			p_params[0]->Get( l_name );
			p_params[1]->Get( l_type );

			if ( p_params.size() > 2 )
			{
				p_params[2]->Get( l_value );
			}

			l_parsingContext->particleSystem->AddParticleVariable( l_name, ElementType( l_type ), l_value );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eUBOVariable )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightParent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );
			SceneNodeSPtr l_pParent = l_parsingContext->pScene->GetSceneNodeCache().Find( l_name );

			if ( l_pParent )
			{
				l_parsingContext->pSceneNode = l_pParent;
			}
			else
			{
				PARSING_ERROR( cuT( "Node " ) + l_name + cuT( " does not exist" ) );
			}

			if ( l_parsingContext->pLight )
			{
				l_parsingContext->pLight->Detach();
				l_parsingContext->pSceneNode->AttachObject( *l_parsingContext->pLight );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightType )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiType;
			p_params[0]->Get( l_uiType );
			l_parsingContext->eLightType = LightType( l_uiType );
			l_parsingContext->pLight = l_parsingContext->pScene->GetLightCache().Add( l_parsingContext->strName, l_parsingContext->pSceneNode, l_parsingContext->eLightType );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightColour )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pLight->SetColour( l_value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightIntensity )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pLight->SetIntensity( l_value.ptr() );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_LightAttenuation )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );

			if ( l_parsingContext->eLightType == LightType::ePoint )
			{
				l_parsingContext->pLight->GetPointLight()->SetAttenuation( l_value );
			}
			else if ( l_parsingContext->eLightType == LightType::eSpot )
			{
				l_parsingContext->pLight->GetSpotLight()->SetAttenuation( l_value );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float l_fFloat;
			p_params[0]->Get( l_fFloat );

			if ( l_parsingContext->eLightType == LightType::eSpot )
			{
				l_parsingContext->pLight->GetSpotLight()->SetCutOff( Angle::from_degrees( l_fFloat ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			float l_fFloat;
			p_params[0]->Get( l_fFloat );

			if ( l_parsingContext->eLightType == LightType::eSpot )
			{
				l_parsingContext->pLight->GetSpotLight()->SetExponent( l_fFloat );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pLight )
		{
			PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !p_params.empty() )
		{
			bool l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pLight->SetShadowProducer( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeParent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );
			SceneNodeSPtr l_parent;

			if ( l_name == Scene::ObjectRootNode )
			{
				l_parent = l_parsingContext->pScene->GetObjectRootNode();
			}
			else if ( l_name == Scene::CameraRootNode )
			{
				l_parent = l_parsingContext->pScene->GetCameraRootNode();
			}
			else if ( l_name == Scene::RootNode )
			{
				l_parent = l_parsingContext->pScene->GetRootNode();
			}
			else
			{
				l_parent = l_parsingContext->pScene->GetSceneNodeCache().Find( l_name );
			}

			if ( l_parent )
			{
				l_parsingContext->pSceneNode->AttachTo( l_parent );
			}
			else
			{
				PARSING_ERROR( cuT( "Node " ) + l_name + cuT( " does not exist" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodePosition )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pSceneNode->SetPosition( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeOrientation )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r l_axis;
			float l_angle;
			p_params[0]->Get( l_axis );
			p_params[1]->Get( l_angle );
			l_parsingContext->pSceneNode->SetOrientation( Quaternion::from_axis_angle( l_axis, Angle::from_degrees( l_angle ) ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_NodeScale )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSceneNode )
		{
			PARSING_ERROR( cuT( "No Scene node initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3r l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pSceneNode->SetScale( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectParent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );
			SceneNodeSPtr l_parent;

			if ( l_name == Scene::ObjectRootNode )
			{
				l_parent = l_parsingContext->pScene->GetObjectRootNode();
			}
			else if ( l_name == Scene::CameraRootNode )
			{
				l_parent = l_parsingContext->pScene->GetCameraRootNode();
			}
			else if ( l_name == Scene::RootNode )
			{
				l_parent = l_parsingContext->pScene->GetRootNode();
			}
			else
			{
				l_parent = l_parsingContext->pScene->GetSceneNodeCache().Find( l_name );
			}

			if ( l_parent )
			{
				l_parent->AttachObject( *l_parsingContext->pGeometry );
			}
			else
			{
				PARSING_ERROR( cuT( "Node " ) + l_name + cuT( " does not exist" ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->bBool1 = false;
		p_params[0]->Get( l_parsingContext->strName2 );

		if ( l_parsingContext->pScene )
		{
			auto const & l_cache = l_parsingContext->pScene->GetMeshCache();

			if ( l_cache.Has( l_parsingContext->strName2 ) )
			{
				l_parsingContext->pMesh = l_cache.Find( l_parsingContext->strName2 );
			}
			else
			{
				l_parsingContext->pMesh.reset();
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( l_parsingContext->pGeometry->GetMesh() )
			{
				auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache();
				String l_name;
				p_params[0]->Get( l_name );

				if ( l_cache.Has( l_name ) )
				{
					for ( auto l_submesh : *l_parsingContext->pGeometry->GetMesh() )
					{
						MaterialSPtr l_material = l_cache.Find( l_name );
						l_parsingContext->pGeometry->SetMaterial( *l_submesh, l_material );
					}
				}
				else
				{
					PARSING_ERROR( cuT( "Material " ) + l_name + cuT( " does not exist" ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pGeometry->SetShadowCaster( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectReceivesShadows )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pGeometry->SetShadowReceiver( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pGeometry.reset();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ObjectMaterialsMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pGeometry )
		{
			PARSING_ERROR( cuT( "No Geometry initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( l_parsingContext->pGeometry->GetMesh() )
			{
				auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache();
				String l_name;
				uint16_t l_index;
				p_params[0]->Get( l_index );
				p_params[1]->Get( l_name );

				if ( l_cache.Has( l_name ) )
				{
					SubmeshSPtr l_submesh = l_parsingContext->pGeometry->GetMesh()->GetSubmesh( l_index );
					MaterialSPtr l_material = l_cache.Find( l_name );
					l_parsingContext->pGeometry->SetMaterial( *l_submesh, l_material );
				}
				else
				{
					PARSING_ERROR( cuT( "Material " ) + l_name + cuT( " does not exist" ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_type;
		p_params[0]->Get( l_type );

		if ( !l_parsingContext->pMesh )
		{
			Parameters l_parameters;
			if ( p_params.size() > 1 )
			{
				String l_tmp;
				l_parameters.Parse( p_params[1]->Get( l_tmp ) );
			}

			if ( l_parsingContext->pScene )
			{
				l_parsingContext->pMesh = l_parsingContext->pScene->GetMeshCache().Add( l_parsingContext->strName2 );
				l_parsingContext->pScene->GetEngine()->GetMeshFactory().Create( l_type )->Generate( *l_parsingContext->pMesh, l_parameters );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->bBool2 = false;
		l_parsingContext->bBool1 = true;
		l_parsingContext->iFace1 = -1;
		l_parsingContext->iFace2 = -1;
		l_parsingContext->pSubmesh.reset();

		if ( !l_parsingContext->pMesh )
		{
			PARSING_ERROR( cuT( "No Mesh initialised." ) );
		}
		else
		{
			l_parsingContext->pSubmesh = l_parsingContext->pMesh->CreateSubmesh();
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eSubmesh )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshImport )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pScene )
		{
			PARSING_ERROR( cuT( "No scene initialised." ) );
		}
		else
		{
			Path l_path;
			Path l_pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			Parameters l_parameters;

			if ( p_params.size() > 1 )
			{
				String l_tmp;
				StringArray l_arrayStrParams = string::split( p_params[1]->Get( l_tmp ), cuT( "-" ), 20, false );

				if ( l_arrayStrParams.size() )
				{
					for ( StringArrayConstIt l_it = l_arrayStrParams.begin(); l_it != l_arrayStrParams.end(); ++l_it )
					{
						if ( l_it->find( cuT( "smooth_normals" ) ) == 0 )
						{
							String l_strNml = cuT( "smooth" );
							l_parameters.Add( cuT( "normals" ), l_strNml.c_str(), uint32_t( l_strNml.size() ) );
						}
						else if ( l_it->find( cuT( "flat_normals" ) ) == 0 )
						{
							String l_strNml = cuT( "flat" );
							l_parameters.Add( cuT( "normals" ), l_strNml.c_str(), uint32_t( l_strNml.size() ) );
						}
						else if ( l_it->find( cuT( "tangent_space" ) ) == 0 )
						{
							l_parameters.Add( cuT( "tangent_space" ), true );
						}
					}
				}
			}

			Engine * l_engine = l_parsingContext->m_pParser->GetEngine();
			auto l_extension = string::lower_case( l_pathFile.GetExtension() );

			if ( !l_engine->GetImporterFactory().IsRegistered( l_extension ) )
			{
				PARSING_ERROR( cuT( "Importer for [" ) + l_extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				l_parsingContext->pMesh = l_parsingContext->pScene->GetMeshCache().Add( l_parsingContext->strName2 );
				auto l_importer = l_engine->GetImporterFactory().Create( l_extension, *l_engine );

				if ( !l_importer->ImportMesh( *l_parsingContext->pMesh, l_pathFile, l_parameters, true ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshMorphImport )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pMesh )
		{
			PARSING_ERROR( cuT( "No mesh initialised." ) );
		}
		else
		{
			real l_timeIndex;
			p_params[1]->Get( l_timeIndex );
			Path l_path;
			Path l_pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			Parameters l_parameters;

			if ( p_params.size() > 2 )
			{
				String l_tmp;
				StringArray l_arrayStrParams = string::split( p_params[2]->Get( l_tmp ), cuT( "-" ), 20, false );

				if ( l_arrayStrParams.size() )
				{
					for ( StringArrayConstIt l_it = l_arrayStrParams.begin(); l_it != l_arrayStrParams.end(); ++l_it )
					{
						if ( l_it->find( cuT( "smooth_normals" ) ) == 0 )
						{
							String l_strNml = cuT( "smooth" );
							l_parameters.Add( cuT( "normals" ), l_strNml.c_str(), uint32_t( l_strNml.size() ) );
						}
						else if ( l_it->find( cuT( "flat_normals" ) ) == 0 )
						{
							String l_strNml = cuT( "flat" );
							l_parameters.Add( cuT( "normals" ), l_strNml.c_str(), uint32_t( l_strNml.size() ) );
						}
						else if ( l_it->find( cuT( "tangent_space" ) ) == 0 )
						{
							l_parameters.Add( cuT( "tangent_space" ), true );
						}
					}
				}
			}

			Engine * l_engine = l_parsingContext->m_pParser->GetEngine();
			auto l_extension = string::lower_case( l_pathFile.GetExtension() );

			if ( !l_engine->GetImporterFactory().IsRegistered( l_extension ) )
			{
				PARSING_ERROR( cuT( "Importer for [" ) + l_extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto l_importer = l_engine->GetImporterFactory().Create( l_extension, *l_engine );
				Mesh l_mesh{ cuT( "MorphImport" ), *l_parsingContext->pScene };

				if ( !l_importer->ImportMesh( l_mesh, l_pathFile, l_parameters, false ) )
				{
					PARSING_ERROR( cuT( "Mesh Import failed" ) );
				}
				else if ( l_mesh.GetSubmeshCount() == l_parsingContext->pMesh->GetSubmeshCount() )
				{
					String l_animName{ "Morph" };

					if ( !l_parsingContext->pMesh->HasAnimation( l_animName ) )
					{
						auto & l_animation = l_parsingContext->pMesh->CreateAnimation( l_animName );

						for ( auto l_submesh : *l_parsingContext->pMesh )
						{
							l_submesh->SetAnimated( true );
							l_animation.AddChild( MeshAnimationSubmesh{ l_animation, *l_submesh } );
						}
					}

					MeshAnimation & l_animation{ static_cast< MeshAnimation & >( l_parsingContext->pMesh->GetAnimation( l_animName ) ) };
					uint32_t l_index = 0u;

					for ( auto l_submesh : l_mesh )
					{
						auto & l_submeshAnim = l_animation.GetSubmesh( l_index );
						std::clog << "Source: " << l_submeshAnim.GetSubmesh().GetPointsCount() << " - Anim: " << l_submesh->GetPointsCount() << std::endl;

						if ( l_submesh->GetPointsCount() == l_submeshAnim.GetSubmesh().GetPointsCount() )
						{
							l_submeshAnim.AddBuffer( std::chrono::milliseconds{ int64_t( l_timeIndex * 1000 ) }, Convert( l_submesh->GetPoints() ) );
						}

						++l_index;
					}

					l_animation.UpdateLength();
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Engine * l_engine = l_parsingContext->m_pParser->GetEngine();

		if ( !l_parsingContext->pMesh )
		{
			PARSING_ERROR( cuT( "No Mesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			uint16_t l_count;
			p_params[0]->Get( l_name );
			p_params[1]->Get( l_count );

			if ( !l_engine->GetSubdividerFactory().IsRegistered( string::lower_case( l_name ) ) )
			{
				PARSING_ERROR( cuT( "Subdivider [" ) + l_name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto l_divider = l_engine->GetSubdividerFactory().Create( l_name );
				l_parsingContext->pMesh->ComputeContainers();
				Point3r l_ptCenter = l_parsingContext->pMesh->GetCollisionBox().GetCenter();

				for ( auto l_submesh : *l_parsingContext->pMesh )
				{
					l_divider->Subdivide( l_submesh, l_count, false );
				}
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MeshEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pMesh )
		{
			l_parsingContext->pGeometry->SetMesh( l_parsingContext->pMesh );
			l_parsingContext->pMesh.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshVertex )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->vertexPos.push_back( l_value[0] );
			l_parsingContext->vertexPos.push_back( l_value[1] );
			l_parsingContext->vertexPos.push_back( l_value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshUV )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point2f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->vertexTex.push_back( l_value[0] );
			l_parsingContext->vertexTex.push_back( l_value[1] );
			l_parsingContext->vertexTex.push_back( 0.0 );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshUVW )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->vertexTex.push_back( l_value[0] );
			l_parsingContext->vertexTex.push_back( l_value[1] );
			l_parsingContext->vertexTex.push_back( l_value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshNormal )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->vertexNml.push_back( l_value[0] );
			l_parsingContext->vertexNml.push_back( l_value[1] );
			l_parsingContext->vertexNml.push_back( l_value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshTangent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Point3f l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->vertexTan.push_back( l_value[0] );
			l_parsingContext->vertexTan.push_back( l_value[1] );
			l_parsingContext->vertexTan.push_back( l_value[2] );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFace )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_strParams;
			p_params[0]->Get( l_strParams );
			Point3i l_pt3Indices;
			StringArray l_arrayValues = string::split( l_strParams, cuT( " " ) );
			l_parsingContext->iFace1 = -1;
			l_parsingContext->iFace2 = -1;

			if ( l_arrayValues.size() >= 4 )
			{
				Point4i l_pt4Indices;

				if ( Castor::ParseValues( l_strParams, l_pt4Indices ) )
				{
					l_parsingContext->iFace1 = int( l_parsingContext->faces.size() );
					l_parsingContext->faces.push_back( l_pt4Indices[0] );
					l_parsingContext->faces.push_back( l_pt4Indices[1] );
					l_parsingContext->faces.push_back( l_pt4Indices[2] );
					l_parsingContext->iFace2 = int( l_parsingContext->faces.size() );
					l_parsingContext->faces.push_back( l_pt4Indices[0] );
					l_parsingContext->faces.push_back( l_pt4Indices[2] );
					l_parsingContext->faces.push_back( l_pt4Indices[3] );
				}
			}
			else if ( Castor::ParseValues( l_strParams, l_pt3Indices ) )
			{
				l_parsingContext->iFace1 = int( l_parsingContext->faces.size() );
				l_parsingContext->faces.push_back( l_pt3Indices[0] );
				l_parsingContext->faces.push_back( l_pt3Indices[1] );
				l_parsingContext->faces.push_back( l_pt3Indices[2] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceUV )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_strParams;
			p_params[0]->Get( l_strParams );
			SubmeshSPtr l_submesh = l_parsingContext->pSubmesh;

			if ( !l_parsingContext->vertexTex.size() )
			{
				l_parsingContext->vertexTex.resize( l_parsingContext->vertexPos.size() );
			}

			Point3i l_pt3Indices;
			StringArray l_arrayValues = string::split( l_strParams, cuT( " " ), 20 );

			if ( l_arrayValues.size() >= 6 && l_parsingContext->iFace1 != -1 )
			{
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[0] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[1] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[2] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[3] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[4] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[5] );
			}

			if ( l_arrayValues.size() >= 8 && l_parsingContext->iFace2 != -1 )
			{
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[0] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[1] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[4] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[5] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[6] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[7] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceUVW )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_strParams;
			p_params[0]->Get( l_strParams );
			SubmeshSPtr l_submesh = l_parsingContext->pSubmesh;

			if ( !l_parsingContext->vertexTex.size() )
			{
				l_parsingContext->vertexTex.resize( l_parsingContext->vertexPos.size() );
			}

			Point3i l_pt3Indices;
			StringArray l_arrayValues = string::split( l_strParams, cuT( " " ), 20 );

			if ( l_arrayValues.size() >= 9 && l_parsingContext->iFace1 != -1 )
			{
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[0] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[1] );
				l_parsingContext->vertexTex[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[2] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[3] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[4] );
				l_parsingContext->vertexTex[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[5] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[6] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[7] );
				l_parsingContext->vertexTex[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[8] );
			}

			if ( l_arrayValues.size() >= 12 && l_parsingContext->iFace2 != -1 )
			{
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[0] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[1] );
				l_parsingContext->vertexTex[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[2] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[6] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[7] );
				l_parsingContext->vertexTex[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[8] );
				l_parsingContext->vertexTex[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[ 9] );
				l_parsingContext->vertexTex[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[10] );
				l_parsingContext->vertexTex[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceNormals )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_strParams;
			p_params[0]->Get( l_strParams );
			SubmeshSPtr l_submesh = l_parsingContext->pSubmesh;

			if ( !l_parsingContext->vertexNml.size() )
			{
				l_parsingContext->vertexNml.resize( l_parsingContext->vertexPos.size() );
			}

			Point3i l_pt3Indices;
			StringArray l_arrayValues = string::split( l_strParams, cuT( " " ), 20 );

			if ( l_arrayValues.size() >= 9 && l_parsingContext->iFace1 != -1 )
			{
				l_parsingContext->vertexNml[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[0] );
				l_parsingContext->vertexNml[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[1] );
				l_parsingContext->vertexNml[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[2] );
				l_parsingContext->vertexNml[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[3] );
				l_parsingContext->vertexNml[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[4] );
				l_parsingContext->vertexNml[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[5] );
				l_parsingContext->vertexNml[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[6] );
				l_parsingContext->vertexNml[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[7] );
				l_parsingContext->vertexNml[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[8] );
			}

			if ( l_arrayValues.size() >= 12 && l_parsingContext->iFace2 != -1 )
			{
				l_parsingContext->vertexNml[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[ 0] );
				l_parsingContext->vertexNml[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[ 1] );
				l_parsingContext->vertexNml[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[ 2] );
				l_parsingContext->vertexNml[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[ 6] );
				l_parsingContext->vertexNml[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[ 7] );
				l_parsingContext->vertexNml[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[ 8] );
				l_parsingContext->vertexNml[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[ 9] );
				l_parsingContext->vertexNml[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[10] );
				l_parsingContext->vertexNml[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshFaceTangents )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pSubmesh )
		{
			PARSING_ERROR( cuT( "No Submesh initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_strParams;
			p_params[0]->Get( l_strParams );
			SubmeshSPtr l_submesh = l_parsingContext->pSubmesh;

			if ( !l_parsingContext->vertexTan.size() )
			{
				l_parsingContext->vertexTan.resize( l_parsingContext->vertexPos.size() );
			}

			Point3i l_pt3Indices;
			StringArray l_arrayValues = string::split( l_strParams, cuT( " " ), 20 );

			if ( l_arrayValues.size() >= 9 && l_parsingContext->iFace1 != -1 )
			{
				l_parsingContext->vertexTan[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[0] );
				l_parsingContext->vertexTan[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[1] );
				l_parsingContext->vertexTan[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 0] * 3] = string::to_real( l_arrayValues[2] );
				l_parsingContext->vertexTan[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[3] );
				l_parsingContext->vertexTan[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[4] );
				l_parsingContext->vertexTan[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 1] * 3] = string::to_real( l_arrayValues[5] );
				l_parsingContext->vertexTan[0 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[6] );
				l_parsingContext->vertexTan[1 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[7] );
				l_parsingContext->vertexTan[2 + l_parsingContext->faces[l_parsingContext->iFace1 + 2] * 3] = string::to_real( l_arrayValues[8] );
			}

			if ( l_arrayValues.size() >= 12 && l_parsingContext->iFace2 != -1 )
			{
				l_parsingContext->vertexTan[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[ 0] );
				l_parsingContext->vertexTan[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[ 1] );
				l_parsingContext->vertexTan[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 0] * 3] = string::to_real( l_arrayValues[ 2] );
				l_parsingContext->vertexTan[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[ 6] );
				l_parsingContext->vertexTan[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[ 7] );
				l_parsingContext->vertexTan[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 1] * 3] = string::to_real( l_arrayValues[ 8] );
				l_parsingContext->vertexTan[0 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[ 9] );
				l_parsingContext->vertexTan[1 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[10] );
				l_parsingContext->vertexTan[2 + l_parsingContext->faces[l_parsingContext->iFace2 + 2] * 3] = string::to_real( l_arrayValues[11] );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SubmeshEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->vertexPos.empty() )
		{
			std::vector< InterleavedVertex > l_vertices{ l_parsingContext->vertexPos.size() / 3 };
			uint32_t l_index{ 0u };

			for ( auto & l_vertex : l_vertices )
			{
				std::memcpy( l_vertex.m_pos.data(), &l_parsingContext->vertexPos[l_index], sizeof( l_vertex.m_pos ) );

				if ( !l_parsingContext->vertexNml.empty() )
				{
					std::memcpy( l_vertex.m_nml.data(), &l_parsingContext->vertexNml[l_index], sizeof( l_vertex.m_nml ) );
				}

				if ( !l_parsingContext->vertexTan.empty() )
				{
					std::memcpy( l_vertex.m_tan.data(), &l_parsingContext->vertexTan[l_index], sizeof( l_vertex.m_tan ) );
				}

				if ( !l_parsingContext->vertexTex.empty() )
				{
					std::memcpy( l_vertex.m_tex.data(), &l_parsingContext->vertexTex[l_index], sizeof( l_vertex.m_tex ) );
				}

				l_index += 3;
			}

			l_parsingContext->pSubmesh->AddPoints( l_vertices );

			if ( l_parsingContext->faces.size() )
			{
				auto l_indices = reinterpret_cast< FaceIndices * >( &l_parsingContext->faces[0] );
				l_parsingContext->pSubmesh->AddFaceGroup( l_indices, l_indices + ( l_parsingContext->faces.size() / 3 ) );

				if ( !l_parsingContext->vertexNml.empty() )
				{
					if ( !l_parsingContext->vertexTan.empty() )
					{
						l_parsingContext->pSubmesh->ComputeBitangents();
					}
					else
					{
						l_parsingContext->pSubmesh->ComputeTangentsFromNormals();
					}
				}
				else
				{
					l_parsingContext->pSubmesh->ComputeNormals();
				}
			}

			l_parsingContext->vertexPos.clear();
			l_parsingContext->vertexNml.clear();
			l_parsingContext->vertexTan.clear();
			l_parsingContext->vertexTex.clear();
			l_parsingContext->faces.clear();
			l_parsingContext->pSubmesh->GetParent().GetScene()->GetListener().PostEvent( MakeInitialiseEvent( *l_parsingContext->pSubmesh ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_MaterialPass )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->strName.clear();

		if ( l_parsingContext->pMaterial )
		{
			l_parsingContext->pass = l_parsingContext->pMaterial->CreatePass();

			switch ( l_parsingContext->pass->GetType() )
			{
			case MaterialType::eLegacy:
				l_parsingContext->legacyPass = std::static_pointer_cast< LegacyPass >( l_parsingContext->pass );
				break;

			case MaterialType::ePbr:
				l_parsingContext->pbrPass = std::static_pointer_cast< PbrPass >( l_parsingContext->pass );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour l_crColour;
			p_params[0]->Get( l_crColour );
			l_parsingContext->legacyPass->SetDiffuse( l_crColour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassSpecular )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour l_crColour;
			p_params[0]->Get( l_crColour );
			l_parsingContext->legacyPass->SetSpecular( l_crColour );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAmbient )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->legacyPass->SetAmbient( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEmissive )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->legacyPass->SetEmissive( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShininess )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->legacyPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_fFloat;
			p_params[0]->Get( l_fFloat );
			l_parsingContext->legacyPass->SetShininess( l_fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlbedo )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pbrPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Colour l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pbrPass->SetAlbedo( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassRoughness )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pbrPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pbrPass->SetRoughness( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassMetallic )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pbrPass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pbrPass->SetMetallic( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlpha )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_fFloat;
			p_params[0]->Get( l_fFloat );
			l_parsingContext->pass->SetOpacity( l_fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassDoubleFace )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			bool l_value;
			p_params[0]->Get( l_value );
			l_parsingContext->pass->SetTwoSided( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassTextureUnit )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pTextureUnit.reset();

		if ( l_parsingContext->pass )
		{
			l_parsingContext->pTextureUnit = std::make_shared< TextureUnit >( *l_parsingContext->m_pParser->GetEngine() );
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextureUnit )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pShaderProgram.reset();
		l_parsingContext->eShaderObject = ShaderType::eCount;

		if ( l_parsingContext->pass )
		{
			l_parsingContext->pShaderProgram = l_parsingContext->m_pParser->GetEngine()->GetShaderProgramCache().GetNewProgram( true );
		}
		else
		{
			PARSING_ERROR( cuT( "Pass not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderProgram )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlphaBlendMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_mode = 0;
			p_params[0]->Get( l_mode );
			l_parsingContext->pass->SetAlphaBlendMode( BlendMode( l_mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassColourBlendMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_mode = 0;
			p_params[0]->Get( l_mode );
			l_parsingContext->pass->SetColourBlendMode( BlendMode( l_mode ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassAlphaFunc )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiFunc;
			float l_fFloat;
			p_params[0]->Get( l_uiFunc );
			p_params[1]->Get( l_fFloat );
			l_parsingContext->pass->SetAlphaFunc( ComparisonFunc( l_uiFunc ) );
			l_parsingContext->pass->SetAlphaValue( l_fFloat );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassRefractionRatio )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			float l_value = 0;
			p_params[0]->Get( l_value );
			l_parsingContext->pass->SetRefractionRatio( l_value );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PassEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pass )
		{
			PARSING_ERROR( cuT( "No Pass initialised." ) );
		}
		else
		{
			l_parsingContext->pass.reset();
			l_parsingContext->legacyPass.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitImage )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			Path l_folder;
			Path l_relative;
			p_params[0]->Get( l_relative );

			if ( File::FileExists( p_context->m_file->GetFilePath() / l_relative ) )
			{
				l_folder = p_context->m_file->GetFilePath();
			}
			else if ( !File::FileExists( l_relative ) )
			{
				PARSING_ERROR( cuT( "File [" ) + l_relative + cuT( "] not found, check the relativeness of the path" ) );
				l_relative.clear();
			}

			if ( !l_relative.empty() )
			{
				l_parsingContext->pTextureUnit->SetAutoMipmaps( true );
				auto l_texture = l_parsingContext->m_pParser->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions, AccessType::eRead, AccessType::eRead );
				l_texture->SetSource( l_folder, l_relative );

				if ( p_params.size() >= 2 )
				{
					String l_channels;
					p_params[1]->Get( l_channels );
					auto l_buffer = l_texture->GetImage().GetBuffer();

					if ( l_channels == cuT( "rgb" ) )
					{
						l_buffer = PxBufferBase::create( l_buffer->dimensions()
							, PF::GetPFWithoutAlpha( l_buffer->format() )
							, l_buffer->const_ptr()
							, l_buffer->format() );
					}
					else if ( l_channels == cuT( "a" ) )
					{
						auto l_tmp = PF::ExtractAlpha( l_buffer );
						l_buffer = l_tmp;
					}

					l_texture->SetSource( l_buffer );
				}

				l_parsingContext->pTextureUnit->SetTexture( l_texture );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitRenderTarget )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else
		{
			l_parsingContext->pRenderTarget = l_parsingContext->m_pParser->GetEngine()->GetRenderTargetCache().Add( TargetType::eTexture );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eRenderTarget )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitChannel )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiChannel;
			p_params[0]->Get( l_uiChannel );
			l_parsingContext->pTextureUnit->SetChannel( TextureChannel( l_uiChannel ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitSampler )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pTextureUnit )
		{
			PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			SamplerSPtr l_sampler = l_parsingContext->m_pParser->GetEngine()->GetSamplerCache().Find( p_params[0]->Get( l_name ) );

			if ( l_sampler )
			{
				l_parsingContext->pTextureUnit->SetSampler( l_sampler );
			}
			else
			{
				PARSING_ERROR( cuT( "Unknown sampler : [" ) + l_name + cuT( "]" ) );
			}
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_UnitEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pass )
		{
			if ( l_parsingContext->pTextureUnit )
			{
				l_parsingContext->pass->AddTextureUnit( l_parsingContext->pTextureUnit );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			l_parsingContext->pShaderProgram->CreateObject( ShaderType::eVertex );
			l_parsingContext->eShaderObject = ShaderType::eVertex;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PixelShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			l_parsingContext->pShaderProgram->CreateObject( ShaderType::ePixel );
			l_parsingContext->eShaderObject = ShaderType::ePixel;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_GeometryShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			l_parsingContext->pShaderProgram->CreateObject( ShaderType::eGeometry );
			l_parsingContext->eShaderObject = ShaderType::eGeometry;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_HullShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			l_parsingContext->pShaderProgram->CreateObject( ShaderType::eHull );
			l_parsingContext->eShaderObject = ShaderType::eHull;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_DomainShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			l_parsingContext->pShaderProgram->CreateObject( ShaderType::eDomain );
			l_parsingContext->eShaderObject = ShaderType::eDomain;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ComputeShader )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			l_parsingContext->pShaderProgram->CreateObject( ShaderType::eCompute );
			l_parsingContext->eShaderObject = ShaderType::eCompute;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderObject )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ConstantsBuffer )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			p_params[0]->Get( l_parsingContext->strName );

			if ( l_parsingContext->strName.empty() )
			{
				PARSING_ERROR( cuT( "Invalid empty name" ) );
			}
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eShaderUBO )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pShaderProgram )
		{
			if ( l_parsingContext->particleSystem )
			{
				if ( l_parsingContext->bBool1 )
				{
					l_parsingContext->particleSystem->SetTFUpdateProgram( l_parsingContext->pShaderProgram );
				}
				else
				{
					l_parsingContext->particleSystem->SetCSUpdateProgram( l_parsingContext->pShaderProgram );
				}

				l_parsingContext->bBool1 = false;
			}
			//l_parsingContext->pPass->SetShader( l_parsingContext->pShaderProgram );
			l_parsingContext->pShaderProgram.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderProgramFile )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			if ( l_parsingContext->eShaderObject != ShaderType::eCount )
			{
				uint32_t l_uiModel;
				Path l_path;
				p_params[0]->Get( l_uiModel );
				p_params[1]->Get( l_path );
				l_parsingContext->pShaderProgram->SetFile( l_parsingContext->eShaderObject, p_context->m_file->GetFilePath() / l_path );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			String l_name;
			p_params[0]->Get( l_name );

			if ( l_parsingContext->eShaderObject != ShaderType::eCount )
			{
				l_parsingContext->pSamplerUniform = l_parsingContext->pShaderProgram->CreateUniform< UniformType::eSampler >( l_name, l_parsingContext->eShaderObject );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !p_params.empty() )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			if ( l_value )
			{
				l_parsingContext->pUniformBuffer = std::make_unique< UniformBuffer >( l_parsingContext->strName, *l_parsingContext->pShaderProgram->GetRenderSystem() );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiType;
			p_params[0]->Get( l_uiType );

			if ( l_parsingContext->eShaderObject != ShaderType::eCount )
			{
				if ( l_parsingContext->eShaderObject == ShaderType::eGeometry )
				{
					l_parsingContext->pShaderProgram->SetInputType( l_parsingContext->eShaderObject, Topology( l_uiType ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pShaderProgram )
		{
			PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !p_params.empty() )
		{
			uint32_t l_uiType;
			p_params[0]->Get( l_uiType );

			if ( l_parsingContext->eShaderObject != ShaderType::eCount )
			{
				if ( l_parsingContext->eShaderObject == ShaderType::eGeometry )
				{
					l_parsingContext->pShaderProgram->SetOutputType( l_parsingContext->eShaderObject, Topology( l_uiType ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->eShaderObject != ShaderType::eCount )
		{
			if ( l_parsingContext->eShaderObject == ShaderType::eGeometry )
			{
				uint8_t l_count;
				p_params[0]->Get( l_count );
				l_parsingContext->pShaderProgram->SetOutputVtxCount( l_parsingContext->eShaderObject, l_count );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pUniform.reset();
		p_params[0]->Get( l_parsingContext->strName2 );

		if ( !l_parsingContext->pUniformBuffer )
		{
			PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
		}
		else if ( l_parsingContext->strName2.empty() )
		{
			PARSING_ERROR( cuT( "Invalid empty name" ) );
		}
		else
		{
			l_parsingContext->uiUInt32 = 1;
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eUBOVariable )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableCount )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t l_param;
		p_params[0]->Get( l_param );

		if ( l_parsingContext->pUniformBuffer )
		{
			l_parsingContext->uiUInt32 = l_param;
		}
		else
		{
			PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ShaderVariableType )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t l_uiType;
		p_params[0]->Get( l_uiType );

		if ( l_parsingContext->pUniformBuffer )
		{
			if ( !l_parsingContext->pUniform )
			{
				l_parsingContext->pUniform = l_parsingContext->pUniformBuffer->CreateUniform( UniformType( l_uiType ), l_parsingContext->strName2, l_parsingContext->uiUInt32 );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_strParams;
		p_params[0]->Get( l_strParams );

		if ( l_parsingContext->pUniform )
		{
			l_parsingContext->pUniform->SetStrValue( l_strParams );
		}
		else
		{
			PARSING_ERROR( cuT( "Variable not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontFile )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->Get( l_parsingContext->path );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontHeight )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->Get( l_parsingContext->iInt16 );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_FontEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->strName.empty() && !l_parsingContext->path.empty() )
		{
			if ( l_parsingContext->pScene )
			{
				l_parsingContext->pScene->GetFontView().Add( l_parsingContext->strName, l_parsingContext->iInt16, p_context->m_file->GetFilePath() / l_parsingContext->path );
			}
			else
			{
				l_parsingContext->m_pParser->GetEngine()->GetFontCache().Add( l_parsingContext->strName, l_parsingContext->iInt16, p_context->m_file->GetFilePath() / l_parsingContext->path );
			}
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPosition )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pOverlay )
		{
			Point2d l_ptPosition;
			p_params[0]->Get( l_ptPosition );
			l_parsingContext->pOverlay->SetPosition( l_ptPosition );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlaySize )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pOverlay )
		{
			Point2d l_ptSize;
			p_params[0]->Get( l_ptSize );
			l_parsingContext->pOverlay->SetSize( l_ptSize );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPixelSize )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pOverlay )
		{
			Size l_size;
			p_params[0]->Get( l_size );
			l_parsingContext->pOverlay->SetPixelSize( l_size );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPixelPosition )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pOverlay )
		{
			Position l_position;
			p_params[0]->Get( l_position );
			l_parsingContext->pOverlay->SetPixelPosition( l_position );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pOverlay )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			l_parsingContext->pOverlay->SetMaterial( l_cache.Find( l_name ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayPanelOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;
		l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( l_name ), OverlayType::ePanel, l_parsingContext->pOverlay->GetScene(), l_parsingContext->pOverlay );
		l_parsingContext->pOverlay->SetVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::ePanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayBorderPanelOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;
		l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( l_name ), OverlayType::eBorderPanel, l_parsingContext->pOverlay->GetScene(), l_parsingContext->pOverlay );
		l_parsingContext->pOverlay->SetVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBorderPanelOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayTextOverlay )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;
		l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayCache().Add( p_params[0]->Get( l_name ), OverlayType::eText, l_parsingContext->pOverlay->GetScene(), l_parsingContext->pOverlay );
		l_parsingContext->pOverlay->SetVisible( false );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eTextOverlay )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_OverlayEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pOverlay->GetType() == OverlayType::eText )
		{
			auto l_textOverlay = l_parsingContext->pOverlay->GetTextOverlay();

			if ( l_textOverlay->GetFontTexture() )
			{
				l_parsingContext->pOverlay->SetVisible( true );
			}
			else
			{
				l_parsingContext->pOverlay->SetVisible( false );
				PARSING_ERROR( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			l_parsingContext->pOverlay->SetVisible( true );
		}

		l_parsingContext->pOverlay = l_parsingContext->pOverlay->GetParent();
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_PanelOverlayUvs )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::ePanel )
		{
			Point4d l_uvs;
			p_params[0]->Get( l_uvs );
			l_overlay->GetPanelOverlay()->SetUV( l_uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlaySizes )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d l_ptSize;
			p_params[0]->Get( l_ptSize );
			l_overlay->GetBorderPanelOverlay()->SetBorderSize( l_ptSize );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayPixelSizes )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			Rectangle l_size;
			p_params[0]->Get( l_size );
			l_overlay->GetBorderPanelOverlay()->SetBorderPixelSize( l_size );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			String l_name;
			p_params[0]->Get( l_name );
			auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			l_overlay->GetBorderPanelOverlay()->SetBorderMaterial( l_cache.Find( l_name ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayPosition )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			uint32_t l_position;
			p_params[0]->Get( l_position );
			l_overlay->GetBorderPanelOverlay()->SetBorderPosition( BorderPosition( l_position ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayCenterUvs )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d l_uvs;
			p_params[0]->Get( l_uvs );
			l_overlay->GetBorderPanelOverlay()->SetUV( l_uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayOuterUvs )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d l_uvs;
			p_params[0]->Get( l_uvs );
			l_overlay->GetBorderPanelOverlay()->SetBorderOuterUV( l_uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BorderPanelOverlayInnerUvs )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eBorderPanel )
		{
			Point4d l_uvs;
			p_params[0]->Get( l_uvs );
			l_overlay->GetBorderPanelOverlay()->SetBorderInnerUV( l_uvs );
		}
		else
		{
			PARSING_ERROR( cuT( "Overlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayFont )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetFontCache();
			String l_name;
			p_params[0]->Get( l_name );

			if ( l_cache.Find( l_name ) )
			{
				l_overlay->GetTextOverlay()->SetFont( l_name );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_overlay->GetTextOverlay()->SetTextWrappingMode( TextWrappingMode( l_value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayVerticalAlign )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_overlay->GetTextOverlay()->SetVAlign( VAlign( l_value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayHorizontalAlign )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_overlay->GetTextOverlay()->SetHAlign( HAlign( l_value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayTexturingMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_overlay->GetTextOverlay()->SetTexturingMode( TextTexturingMode( l_value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayLineSpacingMode )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_overlay->GetTextOverlay()->SetLineSpacingMode( TextLineSpacingMode( l_value ) );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_TextOverlayText )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		OverlaySPtr l_overlay = l_parsingContext->pOverlay;
		String l_strParams;
		p_params[0]->Get( l_strParams );

		if ( l_overlay && l_overlay->GetType() == OverlayType::eText )
		{
			string::replace( l_strParams, cuT( "\\n" ), cuT( "\n" ) );
			l_overlay->GetTextOverlay()->SetCaption( l_strParams );
		}
		else
		{
			PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraParent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;
		SceneNodeSPtr l_pParent = l_parsingContext->pScene->GetSceneNodeCache().Find( p_params[0]->Get( l_name ) );

		if ( l_pParent )
		{
			l_parsingContext->pSceneNode = l_pParent;

			while ( l_pParent->GetParent() && l_pParent->GetParent() != l_parsingContext->pScene->GetObjectRootNode() && l_pParent->GetParent() != l_parsingContext->pScene->GetCameraRootNode() )
			{
				l_pParent = l_pParent->GetParent();
			}

			if ( !l_pParent->GetParent() || l_pParent->GetParent() == l_parsingContext->pScene->GetObjectRootNode() )
			{
				l_pParent->AttachTo( l_parsingContext->pScene->GetCameraRootNode() );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Node " ) + l_name + cuT( " does not exist" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraViewport )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pViewport = std::make_shared< Viewport >( *l_parsingContext->m_pParser->GetEngine() );
		l_parsingContext->pViewport->SetPerspective( Angle::from_degrees( 0 ), 1, 0, 1 );
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eViewport )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraPrimitive )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		uint32_t l_uiType;
		l_parsingContext->ePrimitiveType = Topology( p_params[0]->Get( l_uiType ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_CameraEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSceneNode && l_parsingContext->pViewport )
		{
			l_parsingContext->pScene->GetCameraCache().Add( l_parsingContext->strName, l_parsingContext->pSceneNode, std::move( *l_parsingContext->pViewport ) );
			l_parsingContext->pViewport.reset();
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportType )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() > 0 )
		{
			uint32_t l_uiType;
			l_parsingContext->pViewport->UpdateType( ViewportType( p_params[0]->Get( l_uiType ) ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportLeft )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real l_rValue;
		p_params[0]->Get( l_rValue );
		l_parsingContext->pViewport->UpdateLeft( l_rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportRight )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real l_rValue;
		p_params[0]->Get( l_rValue );
		l_parsingContext->pViewport->UpdateRight( l_rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportTop )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real l_rValue;
		p_params[0]->Get( l_rValue );
		l_parsingContext->pViewport->UpdateTop( l_rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportBottom )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real l_rValue;
		p_params[0]->Get( l_rValue );
		l_parsingContext->pViewport->UpdateBottom( l_rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportNear )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real l_rValue;
		p_params[0]->Get( l_rValue );
		l_parsingContext->pViewport->UpdateNear( l_rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportFar )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		real l_rValue;
		p_params[0]->Get( l_rValue );
		l_parsingContext->pViewport->UpdateFar( l_rValue );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportSize )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size l_size;
		p_params[0]->Get( l_size );
		l_parsingContext->pViewport->Resize( l_size );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportFovY )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float l_fFovY;
		p_params[0]->Get( l_fFovY );
		l_parsingContext->pViewport->UpdateFovY( Angle::from_degrees( l_fFovY ) );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_ViewportAspectRatio )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float l_fRatio;
		p_params[0]->Get( l_fRatio );
		l_parsingContext->pViewport->UpdateRatio( l_fRatio );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardParent )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pBillboards )
		{
			String l_name;
			p_params[0]->Get( l_name );
			SceneNodeSPtr l_pParent = l_parsingContext->pScene->GetSceneNodeCache().Find( l_name );

			if ( l_pParent )
			{
				l_pParent->AttachObject( *l_parsingContext->pBillboards );
			}
			else
			{
				PARSING_ERROR( cuT( "Node " ) + l_name + cuT( " does not exist" ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pBillboards )
		{
			PARSING_ERROR( cuT( "Billboard not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_parsingContext->pBillboards->SetBillboardType( BillboardType( l_value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardSize )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( !l_parsingContext->pBillboards )
		{
			PARSING_ERROR( cuT( "Billboard not initialised" ) );
		}
		else if ( p_params.empty() )
		{
			PARSING_ERROR( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t l_value;
			p_params[0]->Get( l_value );

			l_parsingContext->pBillboards->SetBillboardSize( BillboardSize( l_value ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardPositions )
	{
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eBillboardList )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardMaterial )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pBillboards )
		{
			auto & l_cache = l_parsingContext->m_pParser->GetEngine()->GetMaterialCache();
			String l_name;
			p_params[0]->Get( l_name );

			if ( l_cache.Has( l_name ) )
			{
				l_parsingContext->pBillboards->SetMaterial( l_cache.Find( l_name ) );
			}
			else
			{
				PARSING_ERROR( cuT( "Material " ) + l_name + cuT( " does not exist" ) );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Size l_dimensions;
		p_params[0]->Get( l_dimensions );
		l_parsingContext->pBillboards->SetDimensions( l_dimensions );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		l_parsingContext->pBillboards = nullptr;
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_BillboardPoint )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		Point3r l_position;
		p_params[0]->Get( l_position );
		l_parsingContext->pBillboards->AddPoint( l_position );
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimatedObject )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;
		p_params[0]->Get( l_name );

		if ( l_parsingContext->pAnimGroup )
		{
			GeometrySPtr l_geometry = l_parsingContext->pScene->GetGeometryCache().Find( l_name );

			if ( l_geometry )
			{
				if ( !l_geometry->GetAnimations().empty() )
				{
					l_parsingContext->pAnimMovable = l_parsingContext->pAnimGroup->AddObject( *l_geometry, l_geometry->GetName() + cuT( "_Movable" ) );
				}

				if ( l_geometry->GetMesh() )
				{
					auto l_mesh = l_geometry->GetMesh();

					if ( !l_mesh->GetAnimations().empty() )
					{
						l_parsingContext->pAnimMesh = l_parsingContext->pAnimGroup->AddObject( *l_mesh, l_geometry->GetName() + cuT( "_Mesh" ) );
					}

					auto l_skeleton = l_mesh->GetSkeleton();

					if ( l_skeleton )
					{
						if ( !l_skeleton->GetAnimations().empty() )
						{
							l_parsingContext->pAnimSkeleton = l_parsingContext->pAnimGroup->AddObject( *l_skeleton, l_geometry->GetName() + cuT( "_Skeleton" ) );
						}
					}
				}
			}
			else
			{
				PARSING_ERROR( cuT( "No geometry with name " ) + l_name );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		p_params[0]->Get( l_parsingContext->strName2 );

		if ( l_parsingContext->pAnimGroup )
		{
			l_parsingContext->pAnimGroup->AddAnimation( l_parsingContext->strName2 );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE_PUSH( CSCNSection::eAnimation )

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupAnimationStart )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		String l_name;
		p_params[0]->Get( l_name );

		if ( l_parsingContext->pAnimGroup )
		{
			l_parsingContext->pAnimGroup->StartAnimation( l_name );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimatedObjectGroupEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pAnimGroup )
		{
			l_parsingContext->pAnimGroup.reset();
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimationLooped )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		bool l_value;
		p_params[0]->Get( l_value );

		if ( l_parsingContext->pAnimGroup )
		{
			l_parsingContext->pAnimGroup->SetAnimationLooped( l_parsingContext->strName2, l_value );
		}
		else
		{
			PARSING_ERROR( cuT( "No animated object group initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_AnimationScale )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
		float l_value;
		p_params[0]->Get( l_value );

		if ( l_parsingContext->pAnimGroup )
		{
			l_parsingContext->pAnimGroup->SetAnimationScale( l_parsingContext->strName2, l_value );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( p_params.size() <= 1 )
		{
			PARSING_ERROR( cuT( "Missing parameter." ) );
		}
		else if ( !l_parsingContext->pSkybox )
		{
			PARSING_ERROR( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path l_path;
			Path l_filePath = p_context->m_file->GetFilePath();
			p_params[0]->Get( l_path );
			PxBufferBaseSPtr l_buffer;

			if ( File::FileExists( l_filePath / l_path ) )
			{
				Image l_image{ l_path.GetFileName(), l_filePath / l_path };
				l_buffer = l_image.GetPixels();
			}

			if ( l_buffer )
			{
				Size l_size;
				p_params[1]->Get( l_size );
				auto l_texture = l_parsingContext->pScene->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::eTwoDimensions
					, AccessType::eNone
					, AccessType::eRead );
				l_texture->GetImage().InitialiseSource( l_buffer );
				l_parsingContext->pSkybox->SetEquiTexture( l_texture
					, l_size );
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			Path l_path;
			l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::eNegativeX ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxRight )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			Path l_path;
			l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::ePositiveX ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxTop )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			Path l_path;
			l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::eNegativeY ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxBottom )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			Path l_path;
			l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::ePositiveY ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxFront )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			Path l_path;
			l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::eNegativeZ ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxBack )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			Path l_path;
			l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
			l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::ePositiveZ ) ).InitialiseSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SkyboxEnd )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pSkybox )
		{
			l_parsingContext->pScene->SetForeground( std::move( l_parsingContext->pSkybox ) );
			l_parsingContext->pSkybox.reset();
		}
		else
		{
			PARSING_ERROR( cuT( "No skybox initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()

	IMPLEMENT_ATTRIBUTE_PARSER( Parser_SsaoEnabled )
	{
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pRenderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				bool l_value;
				p_params[0]->Get( l_value );
				l_parsingContext->ssaoConfig.m_enabled = l_value;
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pRenderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				float l_value;
				p_params[0]->Get( l_value );
				l_parsingContext->ssaoConfig.m_radius = l_value;
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pRenderTarget )
		{
			if ( p_params.empty() )
			{
				PARSING_ERROR( cuT( "Missing parameter." ) );
			}
			else
			{
				float l_value;
				p_params[0]->Get( l_value );
				l_parsingContext->ssaoConfig.m_bias = l_value;
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
		SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

		if ( l_parsingContext->pRenderTarget )
		{
			l_parsingContext->pRenderTarget->SetSsaoConfig( l_parsingContext->ssaoConfig );
		}
		else
		{
			PARSING_ERROR( cuT( "No render target initialised" ) );
		}
	}
	END_ATTRIBUTE_POP()
}
