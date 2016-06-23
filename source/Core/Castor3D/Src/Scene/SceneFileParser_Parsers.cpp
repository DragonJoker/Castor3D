#include "SceneFileParser_Parsers.hpp"

#include "AnimatedObjectGroupManager.hpp"
#include "BillboardManager.hpp"
#include "BlendStateManager.hpp"
#include "CameraManager.hpp"
#include "Engine.hpp"
#include "GeometryManager.hpp"
#include "LightManager.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "OverlayManager.hpp"
#include "PluginManager.hpp"
#include "SamplerManager.hpp"
#include "SceneManager.hpp"
#include "SceneNodeManager.hpp"
#include "ShaderManager.hpp"
#include "TargetManager.hpp"
#include "TechniqueManager.hpp"
#include "WindowManager.hpp"

#include "Animation/AnimatedObject.hpp"
#include "Animation/Animation.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Animation/Mesh/MeshAnimationSubmesh.hpp"
#include "Event/Frame/InitialiseEvent.hpp"
#include "Manager/ManagerView.hpp"
#include "Material/Pass.hpp"
#include "Mesh/CmshImporter.hpp"
#include "Mesh/Face.hpp"
#include "Mesh/Importer.hpp"
#include "Mesh/Subdivider.hpp"
#include "Mesh/Submesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Mesh/Skeleton/Skeleton.hpp"
#include "Miscellaneous/PostEffect.hpp"
#include "Overlay/BorderPanelOverlay.hpp"
#include "Overlay/PanelOverlay.hpp"
#include "Overlay/TextOverlay.hpp"
#include "Plugin/DividerPlugin.hpp"
#include "Plugin/ImporterPlugin.hpp"
#include "Plugin/PostFxPlugin.hpp"
#include "Plugin/TechniquePlugin.hpp"
#include "Render/RenderLoop.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderWindow.hpp"
#include "Render/Viewport.hpp"
#include "Scene/Skybox.hpp"
#include "Scene/Animation/AnimatedSkeleton.hpp"
#include "Scene/Animation/AnimationInstance.hpp"
#include "Scene/Animation/Skeleton/SkeletonAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstanceSubmesh.hpp"
#include "Scene/Light/DirectionalLight.hpp"
#include "Scene/Light/PointLight.hpp"
#include "Scene/Light/SpotLight.hpp"
#include "Shader/FrameVariable.hpp"
#include "Shader/FrameVariableBuffer.hpp"
#include "Shader/OneFrameVariable.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Font.hpp>
#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootMtlFile )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	Path l_path;
	l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );

	if ( File::FileExists( l_path ) )
	{
		TextFile l_fileMat( l_path, File::eOPEN_MODE_READ, File::eENCODING_MODE_ASCII );
		Logger::LogInfo( cuT( "Loading materials file : " ) + l_path );

		if ( l_parsingContext->m_pParser->GetEngine()->GetMaterialManager().Read( l_fileMat ) )
		{
			Logger::LogInfo( cuT( "Materials read" ) );
		}
		else
		{
			Logger::LogInfo( cuT( "Can't read materials" ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootScene )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	p_params[0]->Get( l_name );
	l_parsingContext->pScene = l_parsingContext->m_pParser->GetEngine()->GetSceneManager().Create( l_name, *l_parsingContext->m_pParser->GetEngine() );
	l_parsingContext->mapScenes.insert( std::make_pair( l_name, l_parsingContext->pScene ) );
}
END_ATTRIBUTE_PUSH( eSECTION_SCENE )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootFont )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->path.clear();
	p_params[0]->Get( l_parsingContext->strName );
}
END_ATTRIBUTE_PUSH( eSECTION_FONT )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootMaterial )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	p_params[0]->Get( l_name );
	l_parsingContext->pMaterial = l_parsingContext->m_pParser->GetEngine()->GetMaterialManager().Create( l_name, *l_parsingContext->m_pParser->GetEngine() );
}
END_ATTRIBUTE_PUSH( eSECTION_MATERIAL )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootPanelOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayManager().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_PANEL, l_parsingContext->pOverlay, nullptr );
	l_parsingContext->pOverlay->SetVisible( false );
}
END_ATTRIBUTE_PUSH( eSECTION_PANEL_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootBorderPanelOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayManager().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_BORDER_PANEL, l_parsingContext->pOverlay, nullptr );
	l_parsingContext->pOverlay->SetVisible( false );
}
END_ATTRIBUTE_PUSH( eSECTION_BORDER_PANEL_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootTextOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayManager().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_TEXT, l_parsingContext->pOverlay, nullptr );
	l_parsingContext->pOverlay->SetVisible( false );
}
END_ATTRIBUTE_PUSH( eSECTION_TEXT_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootSamplerState )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pSampler = l_parsingContext->m_pParser->GetEngine()->GetSamplerManager().Create( p_params[0]->Get( l_name ) );
}
END_ATTRIBUTE_PUSH( eSECTION_SAMPLER )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootDebugOverlays )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	bool l_value;
	l_parsingContext->m_pParser->GetEngine()->GetRenderLoop().ShowDebugOverlays( p_params[0]->Get( l_value ) );
}
END_ATTRIBUTE_PUSH( eSECTION_SCENE )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_WindowRenderTarget )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pWindow )
	{
		l_parsingContext->pRenderTarget = l_parsingContext->m_pParser->GetEngine()->GetTargetManager().Create( eTARGET_TYPE_WINDOW );
	}
	else
	{
		PARSING_ERROR( cuT( "No window initialised." ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_RENDER_TARGET )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_WindowVSync )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_WindowFullscreen )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetScene )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetCamera )
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
			l_parsingContext->pRenderTarget->SetCamera( l_parsingContext->pRenderTarget->GetScene()->GetCameraManager().Find( p_params[0]->Get( l_name ) ) );
		}
		else
		{
			PARSING_ERROR( cuT( "No scene initialised for this window, set scene before camera." ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetSize )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetFormat )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pRenderTarget )
	{
		PARSING_ERROR( cuT( "No target initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		ePIXEL_FORMAT l_ePF;
		p_params[0]->Get( l_ePF );

		if ( l_ePF < ePIXEL_FORMAT_DEPTH16 )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetTechnique )
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
			StringArray l_arrayStrParams = string::split( p_params[1]->Get( l_tmp ), cuT( "-" ), 20, false );

			for ( auto l_value : l_arrayStrParams )
			{
				StringArray l_param = string::split( l_value, cuT( "= " ), 2, false );

				if ( l_param.size() > 1 )
				{
					l_parameters.Add( l_param[0], l_param[1] );
				}
			}
		}

		Engine * l_engine = l_parsingContext->m_pParser->GetEngine();

		if ( !l_engine->GetRenderTechniqueManager().GetTechniqueFactory().IsRegistered( string::lower_case( l_name ) ) )
		{
			PARSING_ERROR( cuT( "Technique [" ) + l_name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			l_name = cuT( "direct" );
		}

		l_parsingContext->pRenderTarget->SetTechnique( l_name, l_parameters );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetStereo )
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
			l_parsingContext->pRenderTarget->SetStereo( true );
			l_parsingContext->pRenderTarget->SetIntraOcularDistance( l_rIntraOcularDistance );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetPostEffect )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pRenderTarget )
	{
		PARSING_ERROR( cuT( "No target initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Engine * l_engine = l_parsingContext->m_pParser->GetEngine();
		PostFxPluginSPtr l_plugin;
		PostEffectSPtr l_effect;
		String l_name;
		p_params[0]->Get( l_name );
		Parameters l_parameters;

		if ( p_params.size() > 1 )
		{
			String l_tmp;
			StringArray l_arrayStrParams = string::split( p_params[1]->Get( l_tmp ), cuT( "-" ), 20, false );

			for ( auto l_value : l_arrayStrParams )
			{
				StringArray l_param = string::split( l_value, cuT( "= " ), 2, false );

				if ( l_param.size() > 1 )
				{
					l_parameters.Add( l_param[0], l_param[1] );
				}
			}
		}

		for ( auto l_it : l_engine->GetPluginManager().GetPlugins( ePLUGIN_TYPE_POSTFX ) )
		{
			l_plugin = std::static_pointer_cast< PostFxPlugin >( l_it.second );

			if ( !l_effect && string::lower_case( l_plugin->GetPostEffectType() ) == string::lower_case( l_name ) )
			{
				l_effect = l_plugin->CreateEffect( l_engine->GetRenderSystem(), *l_parsingContext->pRenderTarget, l_parameters );
			}
		}

		if ( !l_effect )
		{
			PARSING_ERROR( cuT( "PostEffect [" ) + l_name + cuT( "] not found, make sure the corresponding plug-in is installed" ) );
		}
		else
		{
			l_parsingContext->pRenderTarget->AddPostEffect( l_effect );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetToneMapping )
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
			StringArray l_arrayStrParams = string::split( p_params[1]->Get( l_tmp ), cuT( "-" ), 20, false );

			for ( auto l_value : l_arrayStrParams )
			{
				StringArray l_param = string::split( l_value, cuT( "= " ), 2, false );

				if ( l_param.size() > 1 )
				{
					l_parameters.Add( l_param[0], l_param[1] );
				}
			}
		}

		l_parsingContext->pRenderTarget->SetToneMappingType( l_name, l_parameters );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pRenderTarget->GetTargetType() == eTARGET_TYPE_TEXTURE )
	{
		l_parsingContext->pTextureUnit->SetRenderTarget( l_parsingContext->pRenderTarget );
	}
	else
	{
		l_parsingContext->pWindow->SetRenderTarget( l_parsingContext->pRenderTarget );
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMinFilter )
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
		l_parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::Min, InterpolationMode( l_uiMode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMagFilter )
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
		l_parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::Mag, InterpolationMode( l_uiMode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMipFilter )
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
		l_parsingContext->pSampler->SetInterpolationMode( InterpolationFilter::Mip, InterpolationMode( l_uiMode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMinLod )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMaxLod )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerLodBias )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerUWrapMode )
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
		l_parsingContext->pSampler->SetWrappingMode( TextureUVW::U, WrapMode( l_uiMode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerVWrapMode )
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
		l_parsingContext->pSampler->SetWrappingMode( TextureUVW::V, WrapMode( l_uiMode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerWWrapMode )
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
		l_parsingContext->pSampler->SetWrappingMode( TextureUVW::W, WrapMode( l_uiMode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerBorderColour )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMaxAnisotropy )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBkColour )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBkImage )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneFont )
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
END_ATTRIBUTE_PUSH( eSECTION_FONT )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneMaterial )
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
		l_parsingContext->pMaterial = l_parsingContext->pScene->GetMaterialView().Create( l_name, *l_parsingContext->m_pParser->GetEngine() );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_MATERIAL )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneSamplerState )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pScene )
	{
		PARSING_ERROR( cuT( "No scene initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		String l_name;
		l_parsingContext->pSampler = l_parsingContext->pScene->GetSamplerView().Create( p_params[0]->Get( l_name ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SAMPLER )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneCamera )
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
END_ATTRIBUTE_PUSH( eSECTION_CAMERA )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneLight )
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
END_ATTRIBUTE_PUSH( eSECTION_LIGHT )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneCameraNode )
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
		l_parsingContext->pSceneNode = l_parsingContext->pScene->GetSceneNodeManager().Create( l_name, l_parsingContext->pScene->GetCameraRootNode() );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_NODE )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneSceneNode )
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
		l_parsingContext->pSceneNode = l_parsingContext->pScene->GetSceneNodeManager().Create( l_name, l_parsingContext->pScene->GetObjectRootNode() );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_NODE )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneObject )
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
		l_parsingContext->pGeometry = l_parsingContext->pScene->GetGeometryManager().Create( l_name, nullptr );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_OBJECT )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneAmbientLight )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneImport )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	Path l_path;
	Path l_pathFile = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );

	if ( string::lower_case( l_pathFile.GetExtension() ) == cuT( "cmsh" ) )
	{
		BinaryFile l_file( l_pathFile, File::eOPEN_MODE_READ );
		//Mesh::BinaryLoader()( *l_parsingContext->pMesh, l_file );
	}
	else
	{
		Engine * l_pEngine = l_parsingContext->m_pParser->GetEngine();
		ImporterPluginSPtr l_pPlugin;
		ImporterSPtr l_pImporter;
		ImporterPlugin::ExtensionArray l_arrayExtensions;

		for ( auto l_it : l_pEngine->GetPluginManager().GetPlugins( ePLUGIN_TYPE_IMPORTER ) )
		{
			l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it.second );

			if ( !l_pImporter && l_pPlugin )
			{
				l_arrayExtensions = l_pPlugin->GetExtensions();

				for ( auto l_itExt : l_arrayExtensions )
				{
					if ( !l_pImporter && string::lower_case( l_pathFile.GetExtension() ) == string::lower_case( l_itExt.first ) )
					{
						l_pImporter = l_pPlugin->GetImporter();
					}
				}
			}
		}

		if ( l_pImporter )
		{
			l_parsingContext->pScene->ImportExternal( l_pathFile, *l_pImporter );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBillboard )
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
		l_parsingContext->pBillboards = l_parsingContext->pScene->GetBillboardManager().Create( l_name, nullptr, *l_parsingContext->m_pParser->GetEngine()->GetRenderSystem() );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_BILLBOARD )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneWindow )
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
			l_parsingContext->pWindow = l_parsingContext->pScene->GetWindowManager().Create( l_name );
		}
	}
}
END_ATTRIBUTE_PUSH( eSECTION_WINDOW )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneAnimatedObjectGroup )
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
		l_parsingContext->pAnimGroup = l_parsingContext->pScene->GetAnimatedObjectGroupManager().Create( l_name );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_ANIMGROUP )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ScenePanelOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pScene )
	{
		PARSING_ERROR( cuT( "No scene initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		String l_name;
		l_parsingContext->pOverlay = l_parsingContext->pScene->GetOverlayView().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_PANEL, l_parsingContext->pOverlay, l_parsingContext->pScene );
		l_parsingContext->pOverlay->SetVisible( false );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_PANEL_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBorderPanelOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pScene )
	{
		PARSING_ERROR( cuT( "No scene initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		String l_name;
		l_parsingContext->pOverlay = l_parsingContext->pScene->GetOverlayView().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_BORDER_PANEL, l_parsingContext->pOverlay, l_parsingContext->pScene );
		l_parsingContext->pOverlay->SetVisible( false );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_BORDER_PANEL_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneTextOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pScene )
	{
		PARSING_ERROR( cuT( "No scene initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		String l_name;
		l_parsingContext->pOverlay = l_parsingContext->pScene->GetOverlayView().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_TEXT, l_parsingContext->pOverlay, l_parsingContext->pScene );
		l_parsingContext->pOverlay->SetVisible( false );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_TEXT_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneSkybox )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pScene )
	{
		PARSING_ERROR( cuT( "No scene initialised." ) );
	}
	else
	{
		l_parsingContext->pSkybox = std::make_shared< Skybox >( *l_parsingContext->m_pParser->GetEngine() );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SKYBOX )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightParent )
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
		SceneNodeSPtr l_pParent = l_parsingContext->pScene->GetSceneNodeManager().Find( l_name );

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
			l_parsingContext->pSceneNode->AttachObject( l_parsingContext->pLight );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightType )
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
		l_parsingContext->eLightType = eLIGHT_TYPE( l_uiType );
		l_parsingContext->pLight = l_parsingContext->pScene->GetLightManager().Create( l_parsingContext->strName, l_parsingContext->pSceneNode, l_parsingContext->eLightType );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightColour )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pLight )
	{
		PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->pLight->SetColour( l_vVector.ptr() );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightIntensity )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pLight )
	{
		PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->pLight->SetIntensity( l_vVector.ptr() );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightAttenuation )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pLight )
	{
		PARSING_ERROR( cuT( "No Light initialised. Have you set it's type?" ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );

		if ( l_parsingContext->eLightType == eLIGHT_TYPE_POINT )
		{
			l_parsingContext->pLight->GetPointLight()->SetAttenuation( l_vVector );
		}
		else if ( l_parsingContext->eLightType == eLIGHT_TYPE_SPOT )
		{
			l_parsingContext->pLight->GetSpotLight()->SetAttenuation( l_vVector );
		}
		else
		{
			PARSING_ERROR( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightCutOff )
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

		if ( l_parsingContext->eLightType == eLIGHT_TYPE_SPOT )
		{
			l_parsingContext->pLight->GetSpotLight()->SetCutOff( l_fFloat );
		}
		else
		{
			PARSING_ERROR( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightExponent )
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

		if ( l_parsingContext->eLightType == eLIGHT_TYPE_SPOT )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodeParent )
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
			l_parent = l_parsingContext->pScene->GetSceneNodeManager().Find( l_name );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodePosition )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSceneNode )
	{
		PARSING_ERROR( cuT( "No Scene node initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->pSceneNode->SetPosition( l_vVector );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodeOrientation )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSceneNode )
	{
		PARSING_ERROR( cuT( "No Scene node initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3r l_ptVector;
		float l_fAngle;
		p_params[0]->Get( l_ptVector );
		p_params[1]->Get( l_fAngle );
		l_parsingContext->pSceneNode->SetOrientation( Quaternion( l_ptVector, Angle::from_degrees( l_fAngle ) ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodeScale )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSceneNode )
	{
		PARSING_ERROR( cuT( "No Scene node initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3r l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->pSceneNode->SetScale( l_vVector );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectParent )
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
			l_parent = l_parsingContext->pScene->GetSceneNodeManager().Find( l_name );
		}

		if ( l_parent )
		{
			l_parent->AttachObject( l_parsingContext->pGeometry );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectMesh )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->bBool1 = false;
	p_params[0]->Get( l_parsingContext->strName2 );

	if ( l_parsingContext->pScene )
	{
		auto const & l_manager = l_parsingContext->pScene->GetMeshManager();

		if ( l_manager.Has( l_parsingContext->strName2 ) )
		{
			l_parsingContext->pMesh = l_manager.Find( l_parsingContext->strName2 );
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
END_ATTRIBUTE_PUSH( eSECTION_MESH )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectMaterial )
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
			MaterialManager & l_manager = l_parsingContext->m_pParser->GetEngine()->GetMaterialManager();
			String l_name;
			p_params[0]->Get( l_name );

			if ( l_manager.Has( l_name ) )
			{
				for ( auto l_submesh : *l_parsingContext->pGeometry->GetMesh() )
				{
					MaterialSPtr l_material = l_manager.Find( l_name );
					l_parsingContext->pGeometry->SetMaterial( l_submesh, l_material );
					l_submesh->Ref( l_material );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectMaterials )
{
}
END_ATTRIBUTE_PUSH( eSECTION_OBJECT_MATERIALS )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->pGeometry.reset();
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectMaterialsMaterial )
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
			MaterialManager & l_manager = l_parsingContext->m_pParser->GetEngine()->GetMaterialManager();
			String l_name;
			uint16_t l_index;
			p_params[0]->Get( l_index );
			p_params[1]->Get( l_name );

			if ( l_manager.Has( l_name ) )
			{
				SubmeshSPtr l_submesh = l_parsingContext->pGeometry->GetMesh()->GetSubmesh( l_index );
				MaterialSPtr l_material = l_manager.Find( l_name );
				l_parsingContext->pGeometry->SetMaterial( l_submesh, l_material );
				l_submesh->Ref( l_material );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectMaterialsEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshType )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_strType;
	p_params[0]->Get( l_strType );
	eMESH_TYPE l_type = eMESH_TYPE_CUSTOM;
	UIntArray l_arrayFaces;
	RealArray l_arraySizes;
	String l_strParams;
	p_params[0]->Get( l_strParams );

	if ( !l_parsingContext->pMesh )
	{
		if ( l_strType != cuT( "custom" ) )
		{
			StringArray l_arrayMeshInfos = string::split( l_strParams, cuT( " " ) );
			l_strType = l_arrayMeshInfos[0];

			if ( l_strType == cuT( "cube" ) )
			{
				l_type = eMESH_TYPE_CUBE;
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[1] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[2] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[3] ) );
			}
			else if ( l_strType == cuT( "cone" ) )
			{
				l_type = eMESH_TYPE_CONE;
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[1] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[2] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[3] ) );
			}
			else if ( l_strType == cuT( "cylinder" ) )
			{
				l_type = eMESH_TYPE_CYLINDER;
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[1] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[2] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[3] ) );
			}
			else if ( l_strType == cuT( "sphere" ) )
			{
				l_type = eMESH_TYPE_SPHERE;
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[1] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[2] ) );
			}
			else if ( l_strType == cuT( "icosahedron" ) )
			{
				l_type = eMESH_TYPE_ICOSAHEDRON;
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[1] ) );
			}
			else if ( l_strType == cuT( "plane" ) )
			{
				l_type = eMESH_TYPE_PLANE;
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[1] ) );
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[2] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[3] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[4] ) );
			}
			else if ( l_strType == cuT( "torus" ) )
			{
				l_type = eMESH_TYPE_TORUS;
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[1] ) );
				l_arrayFaces.push_back( string::to_int( l_arrayMeshInfos[2] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[3] ) );
				l_arraySizes.push_back( string::to_real( l_arrayMeshInfos[4] ) );
			}
			else
			{
				PARSING_ERROR( cuT( "Unknown mesh type : " ) + l_strType );
			}
		}

		if ( l_parsingContext->pScene )
		{
			l_parsingContext->pMesh = l_parsingContext->pScene->GetMeshManager().Create( l_parsingContext->strName2, l_type, l_arrayFaces, l_arraySizes );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshNormals )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pMesh )
	{
		PARSING_ERROR( cuT( "No Mesh initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		uint32_t l_uiMode;
		p_params[0]->Get( l_uiMode );
		l_parsingContext->pMesh->ComputeNormals();
		l_parsingContext->bBool1 = true;
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshSubmesh )
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
END_ATTRIBUTE_PUSH( eSECTION_SUBMESH )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshImport )
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
						bool l_bValue = true;
						l_parameters.Add( cuT( "tangent_space" ), l_bValue );
					}
				}
			}
		}

		Engine * l_pEngine = l_parsingContext->m_pParser->GetEngine();
		ImporterSPtr l_importer;

		if ( string::lower_case( l_pathFile.GetExtension() ) == cuT( "cmsh" ) )
		{
			l_importer = std::make_shared< CmshImporter >( *l_pEngine );
		}
		else
		{
			ImporterPluginSPtr l_pPlugin;
			ImporterPlugin::ExtensionArray l_arrayExtensions;

			for ( auto l_it : l_pEngine->GetPluginManager().GetPlugins( ePLUGIN_TYPE_IMPORTER ) )
			{
				l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it.second );

				if ( !l_importer && l_pPlugin )
				{
					l_arrayExtensions = l_pPlugin->GetExtensions();

					for ( auto l_itExt : l_arrayExtensions )
					{
						if ( !l_importer && string::lower_case( l_pathFile.GetExtension() ) == string::lower_case( l_itExt.first ) )
						{
							l_importer = l_pPlugin->GetImporter();
						}
					}
				}
			}
		}

		if ( l_importer )
		{
			l_parsingContext->pMesh = l_importer->ImportMesh( *l_parsingContext->pScene, l_pathFile, l_parameters, true );
		}
		else
		{
			PARSING_WARNING( cuT( "No importer for mesh type file extension : " ) + l_pathFile.GetExtension() );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshMorphImport )
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

		if ( p_params.size() > 1 )
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
						bool l_bValue = true;
						l_parameters.Add( cuT( "tangent_space" ), l_bValue );
					}
				}
			}
		}

		Engine * l_pEngine = l_parsingContext->m_pParser->GetEngine();
		ImporterSPtr l_importer;

		if ( string::lower_case( l_pathFile.GetExtension() ) == cuT( "cmsh" ) )
		{
			l_importer = std::make_shared< CmshImporter >( *l_pEngine );
		}
		else
		{
			ImporterPluginSPtr l_pPlugin;
			ImporterPlugin::ExtensionArray l_arrayExtensions;

			for ( auto l_it : l_pEngine->GetPluginManager().GetPlugins( ePLUGIN_TYPE_IMPORTER ) )
			{
				l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it.second );

				if ( !l_importer && l_pPlugin )
				{
					l_arrayExtensions = l_pPlugin->GetExtensions();

					for ( auto l_itExt : l_arrayExtensions )
					{
						if ( !l_importer && string::lower_case( l_pathFile.GetExtension() ) == string::lower_case( l_itExt.first ) )
						{
							l_importer = l_pPlugin->GetImporter();
						}
					}
				}
			}
		}

		if ( l_importer )
		{
			Scene l_scene{ cuT( "MorphImport" ), *l_importer->GetEngine() };
			MeshSPtr l_mesh = l_importer->ImportMesh( l_scene, l_pathFile, l_parameters, false );

			if ( l_mesh && l_mesh->GetSubmeshCount() == l_parsingContext->pMesh->GetSubmeshCount() )
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

				for ( auto l_submesh : *l_mesh )
				{
					auto & l_submeshAnim = l_animation.GetSubmesh( l_index );

					if ( l_submesh->GetPointsCount() == l_submeshAnim.GetSubmesh().GetPointsCount() )
					{
						l_submeshAnim.AddBuffer( l_timeIndex, Convert( l_submesh->GetPoints() ) );
					}

					++l_index;
				}

				l_animation.UpdateLength();
			}
		}
		else
		{
			PARSING_WARNING( cuT( "No importer for mesh type file extension : " ) + l_pathFile.GetExtension() );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshDivide )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	Engine * l_pEngine = l_parsingContext->m_pParser->GetEngine();
	DividerPluginSPtr l_pPlugin;
	Subdivider * l_pDivider = nullptr;

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

		for ( auto l_it : l_pEngine->GetPluginManager().GetPlugins( ePLUGIN_TYPE_DIVIDER ) )
		{
			l_pPlugin = std::static_pointer_cast< DividerPlugin, PluginBase >( l_it.second );

			if ( !l_pDivider && string::lower_case( l_pPlugin->GetDividerType() ) == string::lower_case( l_name ) )
			{
				l_pDivider = l_pPlugin->CreateDivider();
			}
		}

		if ( !l_pDivider )
		{
			PARSING_ERROR( cuT( "Divider [" ) + l_name + cuT( "] not found, make sure the corresponding plug-in is installed" ) );
		}
		else
		{
			l_parsingContext->pMesh->ComputeContainers();
			Point3r l_ptCenter = l_parsingContext->pMesh->GetCollisionBox().GetCenter();

			for ( auto l_submesh : *l_parsingContext->pMesh )
			{
				l_pDivider->Subdivide( l_submesh, l_count, false );
			}

			l_pPlugin->DestroyDivider( l_pDivider );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pMesh )
	{
		l_parsingContext->pGeometry->SetMesh( l_parsingContext->pMesh );
		l_parsingContext->pMesh.reset();
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshVertex )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSubmesh )
	{
		PARSING_ERROR( cuT( "No Submesh initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->vertexPos.push_back( l_vVector[0] );
		l_parsingContext->vertexPos.push_back( l_vVector[1] );
		l_parsingContext->vertexPos.push_back( l_vVector[2] );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshUV )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSubmesh )
	{
		PARSING_ERROR( cuT( "No Submesh initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point2f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->vertexTex.push_back( l_vVector[0] );
		l_parsingContext->vertexTex.push_back( l_vVector[1] );
		l_parsingContext->vertexTex.push_back( 0.0 );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshUVW )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSubmesh )
	{
		PARSING_ERROR( cuT( "No Submesh initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->vertexTex.push_back( l_vVector[0] );
		l_parsingContext->vertexTex.push_back( l_vVector[1] );
		l_parsingContext->vertexTex.push_back( l_vVector[2] );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshNormal )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSubmesh )
	{
		PARSING_ERROR( cuT( "No Submesh initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->vertexNml.push_back( l_vVector[0] );
		l_parsingContext->vertexNml.push_back( l_vVector[1] );
		l_parsingContext->vertexNml.push_back( l_vVector[2] );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshTangent )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pSubmesh )
	{
		PARSING_ERROR( cuT( "No Submesh initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Point3f l_vVector;
		p_params[0]->Get( l_vVector );
		l_parsingContext->vertexTan.push_back( l_vVector[0] );
		l_parsingContext->vertexTan.push_back( l_vVector[1] );
		l_parsingContext->vertexTan.push_back( l_vVector[2] );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFace )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceUV )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceUVW )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceNormals )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceTangents )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshEnd )
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
		l_parsingContext->m_pParser->GetEngine()->PostEvent( MakeInitialiseEvent( *l_parsingContext->pSubmesh ) );
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MaterialPass )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->strName.clear();

	if ( l_parsingContext->pMaterial )
	{
		l_parsingContext->pPass = l_parsingContext->pMaterial->CreatePass();
	}
	else
	{
		PARSING_ERROR( cuT( "Material not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_PASS )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MaterialEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassAmbient )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Colour l_crColour;
		p_params[0]->Get( l_crColour );
		l_parsingContext->pPass->SetAmbient( l_crColour );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassDiffuse )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Colour l_crColour;
		p_params[0]->Get( l_crColour );
		l_parsingContext->pPass->SetDiffuse( l_crColour );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassSpecular )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Colour l_crColour;
		p_params[0]->Get( l_crColour );
		l_parsingContext->pPass->SetSpecular( l_crColour );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassEmissive )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Colour l_crColour;
		p_params[0]->Get( l_crColour );
		l_parsingContext->pPass->SetEmissive( l_crColour );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassShininess )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		float l_fFloat;
		p_params[0]->Get( l_fFloat );
		l_parsingContext->pPass->SetShininess( l_fFloat );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassAlpha )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		float l_fFloat;
		p_params[0]->Get( l_fFloat );
		l_parsingContext->pPass->SetAlpha( l_fFloat );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassDoubleFace )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		bool l_bDouble;
		p_params[0]->Get( l_bDouble );
		l_parsingContext->pPass->SetTwoSided( l_bDouble );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassBlendFunc )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		PassSPtr l_pPass = l_parsingContext->pPass;
		uint32_t l_uiSrcBlend;
		uint32_t l_uiDstBlend;
		p_params[0]->Get( l_uiSrcBlend );
		p_params[1]->Get( l_uiDstBlend );
		l_pPass->GetBlendState()->SetAlphaSrcBlend( BlendOperand( l_uiSrcBlend ) );
		l_pPass->GetBlendState()->SetAlphaDstBlend( BlendOperand( l_uiDstBlend ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassTextureUnit )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->pTextureUnit.reset();

	if ( l_parsingContext->pPass )
	{
		l_parsingContext->pTextureUnit = std::make_shared< TextureUnit >( *l_parsingContext->m_pParser->GetEngine() );
	}
	else
	{
		PARSING_ERROR( cuT( "Pass not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_TEXTURE_UNIT )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassShader )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->pShaderProgram.reset();
	l_parsingContext->eShaderObject = eSHADER_TYPE_COUNT;

	if ( l_parsingContext->pPass )
	{
		l_parsingContext->pShaderProgram = l_parsingContext->m_pParser->GetEngine()->GetShaderManager().GetNewProgram();
	}
	else
	{
		PARSING_ERROR( cuT( "Pass not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_GLSL_SHADER )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassAlphaBlendMode )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		uint32_t l_mode = 0;
		p_params[0]->Get( l_mode );
		l_parsingContext->pPass->SetAlphaBlendMode( BlendMode( l_mode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassColourBlendMode )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pPass )
	{
		PARSING_ERROR( cuT( "No Pass initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		uint32_t l_mode = 0;
		p_params[0]->Get( l_mode );
		l_parsingContext->pPass->SetColourBlendMode( BlendMode( l_mode ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitImage )
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
			auto l_texture = l_parsingContext->m_pParser->GetEngine()->GetRenderSystem()->CreateTexture( TextureType::TwoDimensions, eACCESS_TYPE_READ, eACCESS_TYPE_READ );
			l_texture->GetImage().SetSource( l_folder, l_relative );
			l_parsingContext->pTextureUnit->SetTexture( l_texture );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitRenderTarget )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pTextureUnit )
	{
		PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
	}
	else
	{
		l_parsingContext->pRenderTarget = l_parsingContext->m_pParser->GetEngine()->GetTargetManager().Create( eTARGET_TYPE_TEXTURE );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_RENDER_TARGET )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitAlphaFunc )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pTextureUnit )
	{
		PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		uint32_t l_uiFunc;
		float l_fFloat;
		p_params[0]->Get( l_uiFunc );
		p_params[1]->Get( l_fFloat );
		l_parsingContext->pTextureUnit->SetAlphaFunc( AlphaFunc( l_uiFunc ) );
		l_parsingContext->pTextureUnit->SetAlphaValue( l_fFloat );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitRgbBlend )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pTextureUnit )
	{
		PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		uint32_t l_uiArg1;
		uint32_t l_uiArg2;
		uint32_t l_uiMode;
		p_params[0]->Get( l_uiMode );
		p_params[1]->Get( l_uiArg1 );
		p_params[2]->Get( l_uiArg2 );
		l_parsingContext->pTextureUnit->SetRgbFunction( RGBBlendFunc( l_uiMode ) );
		l_parsingContext->pTextureUnit->SetRgbArgument( BlendSrcIndex::Index0, BlendSource( l_uiArg1 ) );
		l_parsingContext->pTextureUnit->SetRgbArgument( BlendSrcIndex::Index1, BlendSource( l_uiArg2 ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitAlphaBlend )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pTextureUnit )
	{
		PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		uint32_t l_uiArg1;
		uint32_t l_uiArg2;
		uint32_t l_uiMode;
		p_params[0]->Get( l_uiMode );
		p_params[1]->Get( l_uiArg1 );
		p_params[2]->Get( l_uiArg2 );
		l_parsingContext->pTextureUnit->SetAlpFunction( AlphaBlendFunc( l_uiMode ) );
		l_parsingContext->pTextureUnit->SetAlpArgument( BlendSrcIndex::Index0, BlendSource( l_uiArg1 ) );
		l_parsingContext->pTextureUnit->SetAlpArgument( BlendSrcIndex::Index1, BlendSource( l_uiArg2 ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitChannel )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitSampler )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pTextureUnit )
	{
		PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		String l_name;
		SamplerSPtr l_pSampler = l_parsingContext->m_pParser->GetEngine()->GetSamplerManager().Find( p_params[0]->Get( l_name ) );

		if ( l_pSampler )
		{
			l_parsingContext->pTextureUnit->SetSampler( l_pSampler );
		}
		else
		{
			PARSING_ERROR( cuT( "Unknown sampler : [" ) + l_name + cuT( "]" ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitBlendColour )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pTextureUnit )
	{
		PARSING_ERROR( cuT( "No TextureUnit initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		Colour l_clrColour;
		p_params[0]->Get( l_clrColour );
		l_parsingContext->pTextureUnit->SetBlendColour( l_clrColour );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pPass )
	{
		if ( l_parsingContext->pTextureUnit )
		{
			l_parsingContext->pPass->AddTextureUnit( l_parsingContext->pTextureUnit );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_VertexShader )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pShaderProgram )
	{
		l_parsingContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX );
		l_parsingContext->eShaderObject = eSHADER_TYPE_VERTEX;
	}
	else
	{
		PARSING_ERROR( cuT( "Shader not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SHADER_PROGRAM )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PixelShader )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pShaderProgram )
	{
		l_parsingContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL );
		l_parsingContext->eShaderObject = eSHADER_TYPE_PIXEL;
	}
	else
	{
		PARSING_ERROR( cuT( "Shader not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SHADER_PROGRAM )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GeometryShader )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pShaderProgram )
	{
		l_parsingContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY );
		l_parsingContext->eShaderObject = eSHADER_TYPE_GEOMETRY;
	}
	else
	{
		PARSING_ERROR( cuT( "Shader not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SHADER_PROGRAM )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HullShader )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pShaderProgram )
	{
		l_parsingContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL );
		l_parsingContext->eShaderObject = eSHADER_TYPE_HULL;
	}
	else
	{
		PARSING_ERROR( cuT( "Shader not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SHADER_PROGRAM )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_DomainShader )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pShaderProgram )
	{
		l_parsingContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN );
		l_parsingContext->eShaderObject = eSHADER_TYPE_DOMAIN;
	}
	else
	{
		PARSING_ERROR( cuT( "Shader not initialised" ) );
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SHADER_PROGRAM )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ConstantsBuffer )
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

		if ( l_name.empty() )
		{
			PARSING_ERROR( cuT( "Invalid empty name" ) );
		}
		else
		{
			l_parsingContext->pFrameVariableBuffer = l_parsingContext->m_pParser->GetEngine()->GetRenderSystem()->CreateFrameVariableBuffer( l_name );
		}
	}
}
END_ATTRIBUTE_PUSH( eSECTION_SHADER_UBO )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pShaderProgram )
	{
		//l_parsingContext->pPass->SetShader( l_parsingContext->pShaderProgram );
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderProgramFile )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pShaderProgram )
	{
		PARSING_ERROR( cuT( "No ShaderProgram initialised." ) );
	}
	else if ( !p_params.empty() )
	{
		if ( l_parsingContext->eShaderObject != eSHADER_TYPE_COUNT )
		{
			uint32_t l_uiModel;
			Path l_path;
			p_params[0]->Get( l_uiModel );
			p_params[1]->Get( l_path );
			l_parsingContext->pShaderProgram->SetFile( l_parsingContext->eShaderObject, eSHADER_MODEL( l_uiModel ), p_context->m_file->GetFilePath() / l_path );
		}
		else
		{
			PARSING_ERROR( cuT( "Shader not initialised" ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderProgramSampler )
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

		if ( l_parsingContext->eShaderObject != eSHADER_TYPE_COUNT )
		{
			l_parsingContext->pSamplerFrameVariable = l_parsingContext->pShaderProgram->CreateFrameVariable< OneIntFrameVariable >( l_name, l_parsingContext->eShaderObject );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderUboShaders )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->pFrameVariableBuffer )
	{
		PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
	}
	else if ( !p_params.empty() )
	{
		uint64_t l_value;
		p_params[0]->Get( l_value );

		if ( l_value )
		{
			l_parsingContext->pShaderProgram->AddFrameVariableBuffer( l_parsingContext->pFrameVariableBuffer, l_value );
		}
		else
		{
			PARSING_ERROR( cuT( "Unsupported shader type" ) );
		}
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GeometryInputType )
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

		if ( l_parsingContext->eShaderObject != eSHADER_TYPE_COUNT )
		{
			if ( l_parsingContext->eShaderObject == eSHADER_TYPE_GEOMETRY )
			{
				l_parsingContext->pShaderProgram->SetInputType( l_parsingContext->eShaderObject, eTOPOLOGY( l_uiType ) );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GeometryOutputType )
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

		if ( l_parsingContext->eShaderObject != eSHADER_TYPE_COUNT )
		{
			if ( l_parsingContext->eShaderObject == eSHADER_TYPE_GEOMETRY )
			{
				l_parsingContext->pShaderProgram->SetOutputType( l_parsingContext->eShaderObject, eTOPOLOGY( l_uiType ) );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GeometryOutputVtxCount )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		if ( l_parsingContext->eShaderObject == eSHADER_TYPE_GEOMETRY )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderUboVariable )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->pFrameVariable.reset();
	p_params[0]->Get( l_parsingContext->strName2 );

	if ( !l_parsingContext->pFrameVariableBuffer )
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
END_ATTRIBUTE_PUSH( eSECTION_SHADER_UBO_VARIABLE )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderVariableCount )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	uint32_t l_param;
	p_params[0]->Get( l_param );

	if ( l_parsingContext->pFrameVariableBuffer )
	{
		l_parsingContext->uiUInt32 = l_param;
	}
	else
	{
		PARSING_ERROR( cuT( "Shader constants buffer not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderVariableType )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	uint32_t l_uiType;
	p_params[0]->Get( l_uiType );

	if ( l_parsingContext->pFrameVariableBuffer )
	{
		if ( !l_parsingContext->pFrameVariable )
		{
			l_parsingContext->pFrameVariable = l_parsingContext->pFrameVariableBuffer->CreateVariable( *l_parsingContext->pShaderProgram.get(), FrameVariableType( l_uiType ), l_parsingContext->strName2, l_parsingContext->uiUInt32 );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ShaderVariableValue )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_strParams;
	p_params[0]->Get( l_strParams );

	if ( l_parsingContext->pFrameVariable )
	{
		l_parsingContext->pFrameVariable->SetValueStr( l_strParams );
	}
	else
	{
		PARSING_ERROR( cuT( "Variable not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_FontFile )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	p_params[0]->Get( l_parsingContext->path );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_FontHeight )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	p_params[0]->Get( l_parsingContext->iInt16 );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_FontEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( !l_parsingContext->strName.empty() && !l_parsingContext->path.empty() )
	{
		if ( l_parsingContext->pScene )
		{
			l_parsingContext->pScene->GetFontView().Create( l_parsingContext->strName, l_parsingContext->iInt16, p_context->m_file->GetFilePath() / l_parsingContext->path );
		}
		else
		{
			l_parsingContext->m_pParser->GetEngine()->GetFontManager().Create( l_parsingContext->strName, l_parsingContext->iInt16, p_context->m_file->GetFilePath() / l_parsingContext->path );
		}
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayPosition )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlaySize )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayPixelSize )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayPixelPosition )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayMaterial )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pOverlay )
	{
		String l_name;
		p_params[0]->Get( l_name );
		MaterialManager & l_manager = l_parsingContext->m_pParser->GetEngine()->GetMaterialManager();
		l_parsingContext->pOverlay->SetMaterial( l_manager.Find( l_name ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Overlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayPanelOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayManager().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_PANEL, l_parsingContext->pOverlay, l_parsingContext->pOverlay->GetScene() );
	l_parsingContext->pOverlay->SetVisible( false );
}
END_ATTRIBUTE_PUSH( eSECTION_PANEL_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayBorderPanelOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayManager().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_BORDER_PANEL, l_parsingContext->pOverlay, l_parsingContext->pOverlay->GetScene() );
	l_parsingContext->pOverlay->SetVisible( false );
}
END_ATTRIBUTE_PUSH( eSECTION_BORDER_PANEL_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayTextOverlay )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	l_parsingContext->pOverlay = l_parsingContext->m_pParser->GetEngine()->GetOverlayManager().Create( p_params[0]->Get( l_name ), eOVERLAY_TYPE_TEXT, l_parsingContext->pOverlay, l_parsingContext->pOverlay->GetScene() );
	l_parsingContext->pOverlay->SetVisible( false );
}
END_ATTRIBUTE_PUSH( eSECTION_TEXT_OVERLAY )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pOverlay->GetType() == eOVERLAY_TYPE_TEXT )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PanelOverlayUvs )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_PANEL )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlaySizes )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayPixelSizes )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayMaterial )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
	{
		String l_name;
		p_params[0]->Get( l_name );
		MaterialManager & l_manager = l_parsingContext->m_pParser->GetEngine()->GetMaterialManager();
		l_overlay->GetBorderPanelOverlay()->SetBorderMaterial( l_manager.Find( l_name ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Overlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayPosition )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
	{
		uint32_t l_position;
		p_params[0]->Get( l_position );
		l_overlay->GetBorderPanelOverlay()->SetBorderPosition( eBORDER_POSITION( l_position ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Overlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayCenterUvs )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayOuterUvs )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayInnerUvs )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayFont )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		FontManager & l_fontManager = l_parsingContext->m_pParser->GetEngine()->GetFontManager();
		String l_name;
		p_params[0]->Get( l_name );

		if ( l_fontManager.Find( l_name ) )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayTextWrapping )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		uint32_t l_value;
		p_params[0]->Get( l_value );

		l_overlay->GetTextOverlay()->SetTextWrappingMode( eTEXT_WRAPPING_MODE( l_value ) );
	}
	else
	{
		PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayVerticalAlign )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		uint32_t l_value;
		p_params[0]->Get( l_value );

		l_overlay->GetTextOverlay()->SetVAlign( eVALIGN( l_value ) );
	}
	else
	{
		PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayHorizontalAlign )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		uint32_t l_value;
		p_params[0]->Get( l_value );

		l_overlay->GetTextOverlay()->SetHAlign( eHALIGN( l_value ) );
	}
	else
	{
		PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayTexturingMode )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		uint32_t l_value;
		p_params[0]->Get( l_value );

		l_overlay->GetTextOverlay()->SetTexturingMode( eTEXT_TEXTURING_MODE( l_value ) );
	}
	else
	{
		PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayLineSpacingMode )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		uint32_t l_value;
		p_params[0]->Get( l_value );

		l_overlay->GetTextOverlay()->SetLineSpacingMode( eTEXT_LINE_SPACING_MODE( l_value ) );
	}
	else
	{
		PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayText )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	OverlaySPtr l_overlay = l_parsingContext->pOverlay;
	String l_strParams;
	p_params[0]->Get( l_strParams );

	if ( l_overlay && l_overlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		l_overlay->GetTextOverlay()->SetCaption( l_strParams );
	}
	else
	{
		PARSING_ERROR( cuT( "TextOverlay not initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraParent )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	SceneNodeSPtr l_pParent = l_parsingContext->pScene->GetSceneNodeManager().Find( p_params[0]->Get( l_name ) );

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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraViewport )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->pViewport = std::make_shared< Viewport >( *l_parsingContext->m_pParser->GetEngine() );
	l_parsingContext->pViewport->SetPerspective( Angle::from_degrees( 0 ), 1, 0, 1 );
}
END_ATTRIBUTE_PUSH( eSECTION_VIEWPORT )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraPrimitive )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	uint32_t l_uiType;
	l_parsingContext->ePrimitiveType = eTOPOLOGY( p_params[0]->Get( l_uiType ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSceneNode && l_parsingContext->pViewport )
	{
		l_parsingContext->pScene->GetCameraManager().Create( l_parsingContext->strName, l_parsingContext->pSceneNode, *l_parsingContext->pViewport );
	}
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportType )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	uint32_t l_uiType;
	l_parsingContext->pViewport->UpdateType( eVIEWPORT_TYPE( p_params[0]->Get( l_uiType ) ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportLeft )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	real l_rValue;
	p_params[0]->Get( l_rValue );
	l_parsingContext->pViewport->UpdateLeft( l_rValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportRight )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	real l_rValue;
	p_params[0]->Get( l_rValue );
	l_parsingContext->pViewport->UpdateRight( l_rValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportTop )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	real l_rValue;
	p_params[0]->Get( l_rValue );
	l_parsingContext->pViewport->UpdateTop( l_rValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportBottom )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	real l_rValue;
	p_params[0]->Get( l_rValue );
	l_parsingContext->pViewport->UpdateBottom( l_rValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportNear )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	real l_rValue;
	p_params[0]->Get( l_rValue );
	l_parsingContext->pViewport->UpdateNear( l_rValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportFar )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	real l_rValue;
	p_params[0]->Get( l_rValue );
	l_parsingContext->pViewport->UpdateFar( l_rValue );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportSize )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	Size l_size;
	p_params[0]->Get( l_size );
	l_parsingContext->pViewport->Resize( l_size );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportFovY )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	float l_fFovY;
	p_params[0]->Get( l_fFovY );
	l_parsingContext->pViewport->UpdateFovY( Angle::from_degrees( l_fFovY ) );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportAspectRatio )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	float l_fRatio;
	p_params[0]->Get( l_fRatio );
	l_parsingContext->pViewport->UpdateRatio( l_fRatio );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardParent )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pBillboards )
	{
		String l_name;
		p_params[0]->Get( l_name );
		SceneNodeSPtr l_pParent = l_parsingContext->pScene->GetSceneNodeManager().Find( l_name );

		if ( l_pParent )
		{
			l_pParent->AttachObject( l_parsingContext->pBillboards );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardPositions )
{
}
END_ATTRIBUTE_PUSH( eSECTION_BILLBOARD_LIST )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardMaterial )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pBillboards )
	{
		MaterialManager & l_manager = l_parsingContext->m_pParser->GetEngine()->GetMaterialManager();
		String l_name;
		p_params[0]->Get( l_name );

		if ( l_manager.Has( l_name ) )
		{
			l_parsingContext->pBillboards->SetMaterial( l_manager.Find( l_name ) );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardDimensions )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	Size l_dimensions;
	p_params[0]->Get( l_dimensions );
	l_parsingContext->pBillboards->SetDimensions( l_dimensions );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	l_parsingContext->m_pParser->GetEngine()->PostEvent( MakeInitialiseEvent( *l_parsingContext->pBillboards ) );
	l_parsingContext->pBillboards = nullptr;
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardPoint )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	Point3r l_position;
	p_params[0]->Get( l_position );
	l_parsingContext->pBillboards->AddPoint( l_position );
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimatedObjectGroupAnimatedObject )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );
	String l_name;
	p_params[0]->Get( l_name );

	if ( l_parsingContext->pAnimGroup )
	{
		GeometrySPtr l_geometry = l_parsingContext->pScene->GetGeometryManager().Find( l_name );

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

				if ( l_mesh->GetSkeleton() )
				{
					auto l_skeleton = l_mesh->GetSkeleton();

					if ( !l_skeleton->GetAnimations().empty() )
					{
						l_parsingContext->pAnimSkeleton = l_parsingContext->pAnimGroup->AddObject( *l_mesh->GetSkeleton(), l_geometry->GetName() + cuT( "_Skeleton" ) );
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimatedObjectGroupAnimation )
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
END_ATTRIBUTE_PUSH( eSECTION_ANIMATION )

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimatedObjectGroupAnimationStart )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimatedObjectGroupEnd )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimationLooped )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimationScale )
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

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_AnimationEnd )
{
}
END_ATTRIBUTE_POP()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxLeft )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		Path l_path;
		l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::NegativeX ) ).SetSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxRight )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		Path l_path;
		l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
		l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::PositiveX ) ).SetSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxTop )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		Path l_path;
		l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
		l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::NegativeY ) ).SetSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxBottom )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		Path l_path;
		l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
		l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::PositiveY ) ).SetSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxFront )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		Path l_path;
		l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
		l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::NegativeZ ) ).SetSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxBack )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		Path l_path;
		l_path = p_context->m_file->GetFilePath() / p_params[0]->Get( l_path );
		l_parsingContext->pSkybox->GetTexture().GetImage( uint32_t( CubeMapFace::PositiveZ ) ).SetSource( p_context->m_file->GetFilePath(), p_params[0]->Get( l_path ) );
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE()

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SkyboxEnd )
{
	SceneFileContextSPtr l_parsingContext = std::static_pointer_cast< SceneFileContext >( p_context );

	if ( l_parsingContext->pSkybox )
	{
		l_parsingContext->pScene->SetForeground( l_parsingContext->pSkybox );
		l_parsingContext->pSkybox.reset();
	}
	else
	{
		PARSING_ERROR( cuT( "No skybox initialised" ) );
	}
}
END_ATTRIBUTE_POP()
