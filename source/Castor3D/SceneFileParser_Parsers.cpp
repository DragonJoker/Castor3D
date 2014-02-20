#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/SceneFileParser_Parsers.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Overlay.hpp"
#include "Castor3D/RenderWindow.hpp"
#include "Castor3D/RenderTarget.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Plugin.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Light.hpp"
#include "Castor3D/DirectionalLight.hpp"
#include "Castor3D/PointLight.hpp"
#include "Castor3D/SpotLight.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Importer.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Subdivider.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Texture.hpp"
#include "Castor3D/TextureUnit.hpp"
#include "Castor3D/ShaderProgram.hpp"
#include "Castor3D/FrameVariable.hpp"
#include "Castor3D/Viewport.hpp"
#include "Castor3D/Camera.hpp"
#include "Castor3D/Sampler.hpp"
#include "Castor3D/BlendState.hpp"
#include "Castor3D/BillboardList.hpp"
#include "Castor3D/AnimatedObjectGroup.hpp"
#include "Castor3D/AnimatedObject.hpp"
#include "Castor3D/Animation.hpp"
#include "Castor3D/ShaderManager.hpp"

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4100 )
#endif

using namespace Castor3D;
using namespace Castor;

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootMtlFile )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Path l_path;
	l_path = p_pContext->pFile->GetFilePath() / p_arrayParams[0]->Get( l_path );

	if( File::FileExists( l_path ) )
	{
		TextFile l_fileMat( l_path, File::eOPEN_MODE_READ, File::eENCODING_MODE_ASCII );
		Logger::LogMessage( cuT( "Loading materials file : " ) + l_path );

		if( l_pContext->m_pParser->GetEngine()->GetMaterialManager().Read( l_fileMat ) )
		{
			Logger::LogMessage( cuT( "Materials read" ) );
		}
		else
		{
			Logger::LogMessage( cuT( "Can't read materials" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootScene )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->spScene = l_pContext->m_pParser->GetEngine()->CreateScene( p_arrayParams[0]->Get( l_strName ) );
	l_pContext->pScene = l_pContext->spScene.get();
	l_pContext->mapScenes.insert( std::make_pair( l_strName, l_pContext->spScene ) );
	ATTRIBUTE_END_PUSH( eSECTION_SCENE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootFont )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->path.clear();
	p_arrayParams[0]->Get( l_pContext->strName );
	ATTRIBUTE_END_PUSH( eSECTION_FONT );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootMaterial )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pMaterial = l_pContext->m_pParser->GetEngine()->GetMaterialManager().find( p_arrayParams[0]->Get( l_strName ) );

	if( !l_pContext->pMaterial )
	{
		l_pContext->pMaterial = std::make_shared< Material >( l_pContext->m_pParser->GetEngine(), l_strName, 0 );
		l_pContext->m_pParser->GetEngine()->GetMaterialManager().insert( l_strName, l_pContext->pMaterial );
	}

	ATTRIBUTE_END_PUSH( eSECTION_MATERIAL );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootWindow )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pWindow )
	{
		PARSING_ERROR( cuT( "Directive <window> : Can't create more than one render window" ) );
	}
	else
	{
		l_pContext->pWindow = l_pContext->m_pParser->GetEngine()->CreateRenderWindow();
	}

	ATTRIBUTE_END_PUSH( eSECTION_WINDOW );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootPanelOverlay )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_PANEL, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, nullptr );
	l_pContext->pOverlay->SetVisible( false );
	ATTRIBUTE_END_PUSH( eSECTION_PANEL_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootBorderPanelOverlay)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, nullptr );
	l_pContext->pOverlay->SetVisible( false );
	ATTRIBUTE_END_PUSH( eSECTION_BORDER_PANEL_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootTextOverlay)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_TEXT, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, nullptr );
	l_pContext->pOverlay->SetVisible( false );
	ATTRIBUTE_END_PUSH( eSECTION_TEXT_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RootSamplerState )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pSampler = l_pContext->m_pParser->GetEngine()->CreateSampler( p_arrayParams[0]->Get( l_strName ) );
	ATTRIBUTE_END_PUSH( eSECTION_SAMPLER );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_WindowRenderTarget )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pWindow )
	{
		l_pContext->pRenderTarget = l_pContext->m_pParser->GetEngine()->CreateRenderTarget( eTARGET_TYPE_WINDOW );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <window::render_target> : No window initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_RENDER_TARGET );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_WindowVSync )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pWindow )
	{
		bool l_bValue;
		p_arrayParams[0]->Get( l_bValue );
		l_pContext->pWindow->SetVSync( l_bValue );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <window::vsync> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_WindowFullscreen )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pWindow )
	{
		bool l_bValue;
		p_arrayParams[0]->Get( l_bValue );
		l_pContext->pWindow->SetFullscreen( l_bValue );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <window::fullscreen> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetScene )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;

	if( l_pContext->pRenderTarget )
	{
		ScenePtrStrMap::iterator l_it = l_pContext->mapScenes.find( p_arrayParams[0]->Get( l_strName ) );

		if( l_it != l_pContext->mapScenes.end() )
		{
			l_pContext->pRenderTarget->SetScene( l_it->second );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <render_target::scene> : No scene found with name : [") + l_strName + cuT( "]."));
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::scene> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetCamera)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;

	if (l_pContext->pRenderTarget)
	{
		if (l_pContext->pRenderTarget->GetScene())
		{
			l_pContext->pRenderTarget->SetCamera( l_pContext->pRenderTarget->GetScene()->GetCamera( p_arrayParams[0]->Get( l_strName ) ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <render_target::camera> : No scene initialised for this window, set scene before camera." ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::camera> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetSize )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		Size l_size;
		l_pContext->pRenderTarget->SetSize( p_arrayParams[0]->Get( l_size ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::size> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetFormat )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		ePIXEL_FORMAT l_ePF;
		p_arrayParams[0]->Get( l_ePF );

		if( l_ePF < ePIXEL_FORMAT_DEPTH16 )
		{
			l_pContext->pRenderTarget->SetPixelFormat( l_ePF );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <render_target::format> : Wrong format for colour" ) );
		}
	}
	else
	{
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetDepth )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		ePIXEL_FORMAT l_ePF;
		p_arrayParams[0]->Get( l_ePF );

		if( l_ePF >= ePIXEL_FORMAT_DEPTH16 )
		{
			l_pContext->pRenderTarget->SetDepthFormat( l_ePF );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <render_target::depth> : Wrong format for depth/stencil" ) );
		}
	}
	else
	{
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetMsaa )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		uint16_t l_usSamples;
		p_arrayParams[0]->Get( l_usSamples );

		if( l_usSamples > 1 )
		{
			l_pContext->pRenderTarget->SetTechnique( cuT( "msaa" ) );
			l_pContext->pRenderTarget->SetSamplesCount( l_usSamples );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::msaa> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetSsaa )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		uint16_t l_usSamples;
		p_arrayParams[0]->Get( l_usSamples );

		if( l_usSamples > 1 )
		{
			l_pContext->pRenderTarget->SetTechnique( cuT( "ssaa" ) );
			l_pContext->pRenderTarget->SetSamplesCount( l_usSamples );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::msaa> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetStereo )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		real l_rIntraOcularDistance;
		p_arrayParams[0]->Get( l_rIntraOcularDistance );

		if( l_rIntraOcularDistance > 0 )
		{
			l_pContext->pRenderTarget->SetStereo( true );
			l_pContext->pRenderTarget->SetIntraOcularDistance( l_rIntraOcularDistance );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::stereo> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetDeferred )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget )
	{
		bool l_bValue;
		p_arrayParams[0]->Get( l_bValue );

		if( l_bValue )
		{
			l_pContext->pRenderTarget->SetTechnique( cuT( "deferred" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <render_target::deferred> : No window initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_RenderTargetEnd )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pRenderTarget->GetTargetType() == eTARGET_TYPE_TEXTURE )
	{
		l_pContext->pTextureUnit->SetRenderTarget( l_pContext->pRenderTarget );
	}
	else
	{
		l_pContext->pWindow->SetRenderTarget( l_pContext->pRenderTarget );
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMinFilter )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::min_filter> : No sampler initialised." ) );
	}
	else
	{
		uint32_t l_uiMode;
		p_arrayParams[0]->Get( l_uiMode );
		l_pContext->pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MIN, eINTERPOLATION_MODE( l_uiMode ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMagFilter )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::mag_filter> : No sampler initialised." ) );
	}
	else
	{
		uint32_t l_uiMode;
		p_arrayParams[0]->Get( l_uiMode );
		l_pContext->pSampler->SetInterpolationMode( eINTERPOLATION_FILTER_MAG, eINTERPOLATION_MODE( l_uiMode ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMinLod )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::min_lod> : No sampler initialised." ) );
	}
	else
	{
		real l_rValue = -1000;
		p_arrayParams[0]->Get( l_rValue );

		if( l_rValue >= -1000 && l_rValue <= 1000 )
		{
			l_pContext->pSampler->SetMinLod( l_rValue );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <sampler_state::min_lod> : LOD out of bounds [-1000,1000] : " ) + str_utils::to_string( l_rValue ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMaxLod )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::max_lod> : No sampler initialised." ) );
	}
	else
	{
		real l_rValue = 1000;
		p_arrayParams[0]->Get( l_rValue );

		if( l_rValue >= -1000 && l_rValue <= 1000 )
		{
			l_pContext->pSampler->SetMaxLod( l_rValue );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <sampler_state::max_lod> : LOD out of bounds [-1000,1000] : " ) + str_utils::to_string( l_rValue ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerLodBias )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::lod_bias> : No sampler initialised." ) );
	}
	else
	{
		real l_rValue = 1000;
		p_arrayParams[0]->Get( l_rValue );

		if( l_rValue >= -1000 && l_rValue <= 1000 )
		{
			l_pContext->pSampler->SetLodBias( l_rValue );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <sampler_state::lod_bias> : LOD out of bounds [-1000,1000] : " ) + str_utils::to_string( l_rValue ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerUWrapMode )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::u_wrap_mode> : No sampler initialised." ) );
	}
	else
	{
		uint32_t l_uiMode;
		p_arrayParams[0]->Get( l_uiMode );
		l_pContext->pSampler->SetWrappingMode( eTEXTURE_UVW_U, eWRAP_MODE( l_uiMode ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerVWrapMode )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::v_wrap_mode> : No sampler initialised." ) );
	}
	else
	{
		uint32_t l_uiMode;
		p_arrayParams[0]->Get( l_uiMode );
		l_pContext->pSampler->SetWrappingMode( eTEXTURE_UVW_V, eWRAP_MODE( l_uiMode ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerWWrapMode )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::w_wrap_mode> : No sampler initialised." ) );
	}
	else
	{
		uint32_t l_uiMode;
		p_arrayParams[0]->Get( l_uiMode );
		l_pContext->pSampler->SetWrappingMode( eTEXTURE_UVW_W, eWRAP_MODE( l_uiMode ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerBorderColour )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::border_colour> : No sampler initialised." ) );
	}
	else
	{
		Colour l_colour;
		p_arrayParams[0]->Get( l_colour );
		l_pContext->pSampler->SetBorderColour( l_colour );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SamplerMaxAnisotropy )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->pSampler )
	{
		PARSING_ERROR( cuT( "Directive <sampler_state::max_anisotropy> : No sampler initialised." ) );
	}
	else
	{
		real l_rValue = 1000;
		p_arrayParams[0]->Get( l_rValue );
		l_pContext->pSampler->SetMaxAnisotropy( l_rValue );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBkColour )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pScene )
	{
		Colour l_clrBackground;
		p_arrayParams[0]->Get( l_clrBackground );
		l_pContext->pScene->SetBackgroundColour( l_clrBackground );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <background_colour> : No scene initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBkImage )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pScene )
	{
		Path l_path;
		Path l_pathFile = p_pContext->pFile->GetFilePath() / p_arrayParams[0]->Get( l_path );

		if( File::FileExists( l_pathFile ) )
		{
			l_pContext->pScene->SetBackgroundImage( l_pathFile );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <background_image> : File [" ) + l_pathFile + cuT( "] does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <background_image> : No scene initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneCamera)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pViewport.reset();
	l_pContext->pSceneNode.reset();

	if (l_pContext->pScene)
	{
		l_pContext->pSceneNode.reset();
		p_arrayParams[0]->Get( l_pContext->strName );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <camera> : No scene initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CAMERA );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneLight)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pLight.reset();
	l_pContext->strName.clear();
	l_pContext->pSceneNode.reset();

	if (l_pContext->pScene)
	{
		p_arrayParams[0]->Get( l_pContext->strName );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light> : No scene initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_LIGHT );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneCameraNode)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pSceneNode.reset();
	String l_strName;

	if (l_pContext->pScene)
	{
		p_arrayParams[0]->Get( l_strName );
		l_pContext->pSceneNode = l_pContext->pScene->CreateSceneNode( l_strName, l_pContext->pScene->GetCameraRootNode() );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <camera_node> : No scene initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_NODE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneSceneNode)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pSceneNode.reset();
	String l_strName;

	if (l_pContext->pScene)
	{
		p_arrayParams[0]->Get( l_strName );
		l_pContext->pSceneNode = l_pContext->pScene->CreateSceneNode( l_strName, l_pContext->pScene->GetObjectRootNode() );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene_node> : No scene initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_NODE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneObject)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pGeometry.reset();
	String l_strName;

	if (l_pContext->pScene)
	{
		p_arrayParams[0]->Get( l_strName );
		l_pContext->pGeometry = l_pContext->pScene->CreateGeometry( l_strName );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <object> : No scene initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_OBJECT );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneAmbientLight)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pScene)
	{
		Colour l_clColour;
		p_arrayParams[0]->Get( l_clColour );
		l_pContext->pScene->SetAmbientLight( l_clColour );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <ambient_light> : No scene initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneImport )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Path l_path;
	Path l_pathFile = p_pContext->pFile->GetFilePath() / p_arrayParams[0]->Get( l_path );

	if( str_utils::lower_case( l_pathFile.GetExtension() ) == cuT( "cmsh" ) )
	{
		BinaryFile l_file( l_pathFile, File::eOPEN_MODE_READ );
//		Mesh::BinaryLoader()( *l_pContext->pMesh, l_file );
	}
	else
	{
		Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
		ImporterPluginSPtr l_pPlugin;
		ImporterSPtr l_pImporter;
		ImporterPlugin::ExtensionArray l_arrayExtensions;

		for( PluginStrMap::iterator l_it = l_pEngine->PluginsBegin( ePLUGIN_TYPE_IMPORTER ); l_it != l_pEngine->PluginsEnd( ePLUGIN_TYPE_IMPORTER ) && !l_pImporter; ++l_it )
		{
			l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it->second );

			if( l_pPlugin )
			{
				l_arrayExtensions = l_pPlugin->GetExtensions();

				for( ImporterPlugin::ExtensionArrayIt l_itExt = l_arrayExtensions.begin(); l_itExt != l_arrayExtensions.end() && !l_pImporter; ++l_itExt )
				{
					if( str_utils::lower_case( l_pathFile.GetExtension() ) == str_utils::lower_case( l_itExt->first ) )
					{
						l_pImporter = l_pPlugin->GetImporter();
					}
				}
			}
		}

		if( l_pImporter )
		{
			l_pContext->pScene->ImportExternal( l_pathFile, *l_pImporter );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBillboard )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );

	if( l_pContext->pScene )
	{
		l_pContext->pBillboards = l_pContext->m_pParser->GetEngine()->GetRenderSystem()->CreateBillboardsList( l_pContext->pScene );

		if( l_pContext->pBillboards )
		{
			l_pContext->pBillboards->SetName( l_strName );
			l_pContext->pScene->AddBillboards( l_pContext->pBillboards );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <scene::billboard> : Can't create billboard" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene::billboard> : Scene not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_BILLBOARD );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneAnimatedObjectGroup )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );

	if( l_pContext->pScene )
	{
		l_pContext->pGroup = l_pContext->pScene->CreateAnimatedObjectGroup( l_strName );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene::animated_object_group> : Scene not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_ANIMGROUP );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ScenePanelOverlay )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pScene )
	{
		String l_strName;
		l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_PANEL, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, l_pContext->spScene );
		l_pContext->pOverlay->SetVisible( false );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene::panel_overlay> : Scene not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_PANEL_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneBorderPanelOverlay)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pScene )
	{
		String l_strName;
		l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, l_pContext->spScene );
		l_pContext->pOverlay->SetVisible( false );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene::border_panel_overlay> : Scene not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_BORDER_PANEL_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SceneTextOverlay)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pScene )
	{
		String l_strName;
		l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_TEXT, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, l_pContext->spScene );
		l_pContext->pOverlay->SetVisible( false );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene::text_overlay> : Scene not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_TEXT_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightParent )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );
	SceneNodeSPtr l_pParent = l_pContext->pScene->GetNode( l_strName );

	if (l_pParent)
	{
		l_pContext->pSceneNode = l_pParent;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::parent> : Node " ) + l_strName + cuT( " does not exist" ) );
	}

	if (l_pContext->pLight)
	{
		l_pContext->pLight->Detach();
		l_pContext->pSceneNode->AttachObject( l_pContext->pLight.get());
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );
	l_pContext->eLightType = eLIGHT_TYPE( l_uiType );
	l_pContext->pLight = l_pContext->pScene->CreateLight( l_pContext->strName, l_pContext->pSceneNode, l_pContext->eLightType );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightAmbient)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pLight)
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->pLight->SetAmbient( l_vVector.ptr() );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::ambient> : Light not initialised. Have you set it's type ?" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightDiffuse)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pLight)
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->pLight->SetDiffuse( l_vVector.ptr() );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::diffuse> : Light not initialised. Have you set it's type ?" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightSpecular)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pLight)
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->pLight->SetSpecular( l_vVector.ptr() );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::specular> : Light not initialised. Have you set it's type ?" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightAttenuation)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pLight)
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );

		if( l_pContext->eLightType == eLIGHT_TYPE_POINT || l_pContext->eLightType == eLIGHT_TYPE_SPOT )
		{
			std::static_pointer_cast<SpotLight>( l_pContext->pLight->GetLightCategory())->SetAttenuation( l_vVector );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <light::attenuation> : Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::attenuation> : Light not initialised. Have you set it's type ?" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightCutOff)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pLight)
	{
		float l_fFloat;
		p_arrayParams[0]->Get( l_fFloat );

		if (l_pContext->eLightType == eLIGHT_TYPE_SPOT)
		{
			std::static_pointer_cast<SpotLight>( l_pContext->pLight->GetLightCategory())->SetCutOff( l_fFloat);
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <light::cut_off> : Wrong type of light to apply a cut off, needs spotlight" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::cut_off> : Light not initialised. Have you set it's type ?" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_LightExponent)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pLight)
	{
		float l_fFloat;
		p_arrayParams[0]->Get( l_fFloat );

		if (l_pContext->eLightType == eLIGHT_TYPE_SPOT)
		{
			std::static_pointer_cast<SpotLight>( l_pContext->pLight->GetLightCategory())->SetExponent( l_fFloat);
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <light::exponent> : Wrong type of light to apply an exponent, needs spotlight" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <light::exponent> : Light not initialised. Have you set it's type ?" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodeParent)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pSceneNode)
	{
		String l_strName;
		p_arrayParams[0]->Get( l_strName );
		SceneNodeSPtr l_pParent = l_pContext->pScene->GetNode( l_strName );

		if (l_pParent)
		{
			l_pContext->pSceneNode->AttachTo( l_pParent.get());
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <scene_node::parent> : Node " ) + l_strName + cuT( " does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene_node::parent> : Scene node not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodePosition)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pSceneNode)
	{
		Point3r l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->pSceneNode->SetPosition( l_vVector );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene_node::position> : Scene node not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodeOrientation)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pSceneNode)
	{
		Point3r l_ptVector;
		float l_fAngle;
		p_arrayParams[0]->Get( l_ptVector );
		p_arrayParams[1]->Get( l_fAngle );
		l_pContext->pSceneNode->SetOrientation( Quaternion( l_ptVector, Angle::FromDegrees( l_fAngle ) ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene_node::orientation> : Scene node not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_NodeScale)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pSceneNode)
	{
		Point3r l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->pSceneNode->SetScale( l_vVector);
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <scene_node::scale> : Scene node not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectParent)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pGeometry)
	{
		String l_strName;
		p_arrayParams[0]->Get( l_strName );
		SceneNodeSPtr l_pParent = l_pContext->pScene->GetNode( l_strName );

		if (l_pParent)
		{
			l_pParent->AttachObject( l_pContext->pGeometry.get());
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <object::parent> : Node " ) + l_strName + cuT( " does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <object::parent> : Geometry not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectMesh)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->bBool1 = false;
	p_arrayParams[0]->Get( l_pContext->strName2 );
	l_pContext->pMesh = l_pContext->m_pParser->GetEngine()->GetMeshManager().find( l_pContext->strName2 );

	if( l_pContext->pMesh )
	{
		l_pContext->pMesh->Ref();
	}

	ATTRIBUTE_END_PUSH( eSECTION_MESH );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ObjectEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pScene->AddGeometry( l_pContext->pGeometry );
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strType;
	p_arrayParams[0]->Get( l_strType );
	eMESH_TYPE l_eType = eMESH_TYPE_CUSTOM;
	UIntArray l_arrayFaces;
	RealArray l_arraySizes;
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if ( ! l_pContext->pMesh)
	{
		if (l_strType != cuT( "custom" ) )
		{
			StringArray l_arrayMeshInfos = str_utils::split( l_strParams, cuT( " " ) );
			l_strType = l_arrayMeshInfos[0];

			if (l_strType == cuT( "cube" ) )
			{
				l_eType = eMESH_TYPE_CUBE;
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[1] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[2] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[3] ));
			}
			else if (l_strType == cuT( "cone" ) )
			{
				l_eType = eMESH_TYPE_CONE;
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[1] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[2] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[3] ));
			}
			else if (l_strType == cuT( "cylinder" ) )
			{
				l_eType = eMESH_TYPE_CYLINDER;
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[1] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[2] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[3] ));
			}
			else if (l_strType == cuT( "sphere" ) )
			{
				l_eType = eMESH_TYPE_SPHERE;
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[1] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[2] ));
			}
			else if (l_strType == cuT( "icosahedron" ) )
			{
				l_eType = eMESH_TYPE_ICOSAHEDRON;
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[1] ));
			}
			else if (l_strType == cuT( "plane" ) )
			{
				l_eType = eMESH_TYPE_PLANE;
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[1] ));
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[2] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[3] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[4] ));
			}
			else if (l_strType == cuT( "torus" ) )
			{
				l_eType = eMESH_TYPE_TORUS;
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[1] ));
				l_arrayFaces.push_back( str_utils::to_int(	l_arrayMeshInfos[2] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[3] ));
				l_arraySizes.push_back( str_utils::to_real(	l_arrayMeshInfos[4] ));
			}
			else
			{
				PARSING_ERROR( cuT( "Directive <mesh::type> : Unknown mesh type : " ) + l_strType );
			}
		}

		l_pContext->pMesh = l_pContext->m_pParser->GetEngine()->CreateMesh( l_eType, l_pContext->strName2, l_arrayFaces, l_arraySizes);
	}
	else
	{
		PARSING_WARNING( cuT( "Directive <mesh::type> : Mesh already initialised => ignored" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshNormals )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiMode;
	p_arrayParams[0]->Get( l_uiMode );
	l_pContext->pMesh->ComputeNormals();
	l_pContext->bBool1 = true;
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshFile)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pMesh = l_pContext->m_pParser->GetEngine()->CreateMesh(eMESH_TYPE_CUSTOM, cuEmptyString, UIntArray(), RealArray());
	Path l_path;
	p_arrayParams[0]->Get( l_path );

	if( !l_pContext->pMesh )
	{
		PARSING_ERROR( cuT( "Directive <mesh::file> : mesh isn't initialised" ) );
	}
	else
	{
		BinaryFile l_file( p_pContext->pFile->GetFilePath() / l_path, File::eOPEN_MODE_READ );

		if( !l_file.IsOk() )
		{
			PARSING_ERROR( cuT( "Directive <mesh::file> : file [" ) + l_path + cuT( "] doesn't exist" ) );
		}
		else
		{
			//if( !Mesh::BinaryLoader()( *l_pContext->pMesh, l_file ) )
			//{
			//	PARSING_ERROR( cuT( "Directive <mesh::file> : Can't load mesh file " ) + l_path );
			//}
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshMaterial)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pMesh)
	{
		MaterialManager & l_manager = l_pContext->m_pParser->GetEngine()->GetMaterialManager();
		String l_strName;
		p_arrayParams[0]->Get( l_strName );

		if (l_manager.has( l_strName ) )
		{
			std::for_each( l_pContext->pMesh->Begin(), l_pContext->pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				p_pSubmesh->SetMaterial( l_manager.find( l_strName ) );
			} );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <mesh::material> : Material " ) + l_strName + cuT( " does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive mesh::<material> : Mesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshSubmesh)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->bBool2 = false;
	l_pContext->bBool1 = true;
	l_pContext->iFace1 = -1;
	l_pContext->iFace2 = -1;
	l_pContext->pSubmesh.reset();

	if (l_pContext->pMesh)
	{
		l_pContext->pSubmesh = l_pContext->pMesh->CreateSubmesh();
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <mesh::submesh> : Mesh not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_SUBMESH );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshImport )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Path l_path;
	Path l_pathFile = p_pContext->pFile->GetFilePath() / p_arrayParams[0]->Get( l_path );
	Parameters l_parameters;

	if( p_arrayParams.size() > 1 )
	{
		String l_strTmp;
		StringArray l_arrayStrParams = str_utils::split( p_arrayParams[1]->Get( l_strTmp ), cuT( "-" ), 20, false );

		if( l_arrayStrParams.size() )
		{
			for( StringArrayConstIt l_it = l_arrayStrParams.begin(); l_it != l_arrayStrParams.end(); ++l_it )
			{
				if( l_it->find( cuT( "smooth_normals" ) ) == 0 )
				{
					String l_strNml = cuT( "smooth" );
					l_parameters.Add( cuT( "normals" ), l_strNml.c_str(), uint32_t( l_strNml.size() ) );
				}
				else if( l_it->find( cuT( "flat_normals" ) ) == 0 )
				{
					String l_strNml = cuT( "flat" );
					l_parameters.Add( cuT( "normals" ), l_strNml.c_str(), uint32_t( l_strNml.size() ) );
				}
				else if( l_it->find( cuT( "tangent_space" ) ) == 0 )
				{
					bool l_bValue = true;
					l_parameters.Add( cuT( "tangent_space" ), l_bValue );
				}
			}
		}
	}

	if( str_utils::lower_case( l_pathFile.GetExtension() ) == cuT( "cmsh" ) )
	{
		BinaryFile l_file( l_pathFile, File::eOPEN_MODE_READ );
//		Mesh::BinaryLoader()( *l_pContext->pMesh, l_file );
	}
	else
	{
		Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
		ImporterPluginSPtr l_pPlugin;
		ImporterSPtr l_pImporter;
		ImporterPlugin::ExtensionArray l_arrayExtensions;

		for( PluginStrMap::iterator l_it = l_pEngine->PluginsBegin( ePLUGIN_TYPE_IMPORTER ); l_it != l_pEngine->PluginsEnd( ePLUGIN_TYPE_IMPORTER ) && !l_pImporter; ++l_it )
		{
			l_pPlugin = std::static_pointer_cast< ImporterPlugin, PluginBase >( l_it->second );

			if( l_pPlugin )
			{
				l_arrayExtensions = l_pPlugin->GetExtensions();

				for( ImporterPlugin::ExtensionArrayIt l_itExt = l_arrayExtensions.begin(); l_itExt != l_arrayExtensions.end() && !l_pImporter; ++l_itExt )
				{
					if( str_utils::lower_case( l_pathFile.GetExtension() ) == str_utils::lower_case( l_itExt->first ) )
					{
						l_pImporter = l_pPlugin->GetImporter();
					}
				}
			}
		}

		if( l_pImporter )
		{
			l_pContext->pMesh = l_pImporter->ImportMesh( l_pathFile, l_parameters );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshDivide )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Engine * l_pEngine = l_pContext->m_pParser->GetEngine();
	DividerPluginSPtr l_pPlugin;
	Subdivider * l_pDivider = NULL;

	if (l_pContext->pMesh)
	{
		String l_strName;
		uint8_t l_uiCount;
		p_arrayParams[0]->Get( l_strName );
		p_arrayParams[1]->Get( l_uiCount );

		for( PluginStrMap::iterator l_it = l_pEngine->PluginsBegin( ePLUGIN_TYPE_DIVIDER ); l_it != l_pEngine->PluginsEnd( ePLUGIN_TYPE_DIVIDER ) && !l_pDivider; ++l_it )
		{
			l_pPlugin = std::static_pointer_cast< DividerPlugin, PluginBase >( l_it->second );

			if( str_utils::lower_case( l_pPlugin->GetDividerType() ) == str_utils::lower_case( l_strName ) )
			{
				l_pDivider = l_pPlugin->CreateDivider();
			}
		}

		if( !l_pDivider )
		{
			PARSING_ERROR( cuT( "Directive <mesh::division> : Divider [" ) + l_strName + cuT( "] not found, make sure the corresponding plugin is installed" ) );
		}
		else
		{
			l_pContext->pMesh->ComputeContainers();
			Point3r l_ptCenter = l_pContext->pMesh->GetCollisionBox().GetCenter();

			std::for_each( l_pContext->pMesh->Begin(), l_pContext->pMesh->End(), [&]( SubmeshSPtr p_pSubmesh )
			{
				for( uint8_t i = 0; i < l_uiCount; i++ )
				{
					l_pDivider->Subdivide( p_pSubmesh, &l_ptCenter, false );
				}
			} );

			l_pPlugin->DestroyDivider( l_pDivider );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <mesh::division> : Mesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MeshEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pMesh)
	{
		l_pContext->pMesh->GenerateBuffers();
		l_pContext->pGeometry->SetMesh( l_pContext->pMesh );
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshMaterial)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pSubmesh)
	{
		MaterialManager & l_manager = l_pContext->m_pParser->GetEngine()->GetMaterialManager();
		String l_strName;
		p_arrayParams[0]->Get( l_strName );

		if (l_manager.has( l_strName ) )
		{
			l_pContext->pSubmesh->SetMaterial( l_manager.find( l_strName ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <submesh::material> : Material " ) + l_strName + cuT( " does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::material> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshVertex )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pSubmesh )
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->vertexPos.push_back( l_vVector[0] );
		l_pContext->vertexPos.push_back( l_vVector[1] );
		l_pContext->vertexPos.push_back( l_vVector[2] );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::vertex> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshUV )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pSubmesh )
	{
		Point2f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->vertexTex.push_back( l_vVector[0] );
		l_pContext->vertexTex.push_back( l_vVector[1] );
		l_pContext->vertexTex.push_back( 0.0 );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::uv> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshUVW )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pSubmesh )
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->vertexTex.push_back( l_vVector[0] );
		l_pContext->vertexTex.push_back( l_vVector[1] );
		l_pContext->vertexTex.push_back( l_vVector[2] );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::uvw> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshNormal )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pSubmesh )
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->vertexNml.push_back( l_vVector[0] );
		l_pContext->vertexNml.push_back( l_vVector[1] );
		l_pContext->vertexNml.push_back( l_vVector[2] );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::normal> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshTangent )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pSubmesh )
	{
		Point3f l_vVector;
		p_arrayParams[0]->Get( l_vVector );
		l_pContext->vertexTan.push_back( l_vVector[0] );
		l_pContext->vertexTan.push_back( l_vVector[1] );
		l_pContext->vertexTan.push_back( l_vVector[2] );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::tangent> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFace )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if( l_pContext->pSubmesh )
	{
		Point3i l_pt3Indices;
		StringArray l_arrayValues = str_utils::split( l_strParams, cuT( " " ) );
		l_pContext->iFace1 = -1;
		l_pContext->iFace2 = -1;

		if (l_arrayValues.size() >= 4)
		{
			Point4i l_pt4Indices;

			if( Castor::ParseVector( l_strParams, l_pt4Indices ) )
			{
				l_pContext->iFace1 = int( l_pContext->faces.size() );
				l_pContext->faces.push_back( l_pt4Indices[0] );
				l_pContext->faces.push_back( l_pt4Indices[1] );
				l_pContext->faces.push_back( l_pt4Indices[2] );

				l_pContext->iFace2 = int( l_pContext->faces.size() );
				l_pContext->faces.push_back( l_pt4Indices[0] );
				l_pContext->faces.push_back( l_pt4Indices[2] );
				l_pContext->faces.push_back( l_pt4Indices[3] );
			}
		}
		else if( Castor::ParseVector( l_strParams, l_pt3Indices ) )
		{
			l_pContext->iFace1 = int( l_pContext->faces.size() );
			l_pContext->faces.push_back( l_pt3Indices[0] );
			l_pContext->faces.push_back( l_pt3Indices[1] );
			l_pContext->faces.push_back( l_pt3Indices[2] );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::face> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceUV )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	SubmeshSPtr l_pSubmesh = l_pContext->pSubmesh;

	if( l_pSubmesh )
	{
		if( !l_pContext->vertexTex.size() )
		{
			l_pContext->vertexTex.resize( l_pContext->vertexPos.size() );
		}

		Point3i l_pt3Indices;
		StringArray l_arrayValues = str_utils::split( l_strParams, cuT( " " ), 20);

		if( l_arrayValues.size() >= 6 && l_pContext->iFace1 != -1 )
		{
			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[0] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[1] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[2] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[3] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[4] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[5] );
		}

		if( l_arrayValues.size() >= 8 && l_pContext->iFace2 != -1 )
		{
			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[0] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[1] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[4] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[5] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[6] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[7] );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::uv> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceUVW )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	SubmeshSPtr l_pSubmesh = l_pContext->pSubmesh;

	if( l_pSubmesh )
	{
		if( !l_pContext->vertexTex.size() )
		{
			l_pContext->vertexTex.resize( l_pContext->vertexPos.size() );
		}

		Point3i l_pt3Indices;
		StringArray l_arrayValues = str_utils::split( l_strParams, cuT( " " ), 20);

		if( l_arrayValues.size() >= 9 && l_pContext->iFace1 != -1 )
		{
			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[0] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[1] );
			l_pContext->vertexTex[2 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[2] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[3] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[4] );
			l_pContext->vertexTex[2 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[5] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[6] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[7] );
			l_pContext->vertexTex[2 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[8] );
		}

		if( l_arrayValues.size() >= 12 && l_pContext->iFace2 != -1 )
		{
			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[0] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[1] );
			l_pContext->vertexTex[2 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[2] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[6] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[7] );
			l_pContext->vertexTex[2 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[8] );

			l_pContext->vertexTex[0 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[ 9] );
			l_pContext->vertexTex[1 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[10] );
			l_pContext->vertexTex[2 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[11] );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::uv> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceNormals )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	SubmeshSPtr l_pSubmesh = l_pContext->pSubmesh;

	if( l_pSubmesh )
	{
		if( !l_pContext->vertexNml.size() )
		{
			l_pContext->vertexNml.resize( l_pContext->vertexPos.size() );
		}

		Point3i l_pt3Indices;
		StringArray l_arrayValues = str_utils::split( l_strParams, cuT( " " ), 20 );

		if( l_arrayValues.size() >= 9 && l_pContext->iFace1 != -1 )
		{
			l_pContext->vertexNml[0 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[0] );
			l_pContext->vertexNml[1 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[1] );
			l_pContext->vertexNml[2 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[2] );

			l_pContext->vertexNml[0 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[3] );
			l_pContext->vertexNml[1 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[4] );
			l_pContext->vertexNml[2 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[5] );

			l_pContext->vertexNml[0 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[6] );
			l_pContext->vertexNml[1 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[7] );
			l_pContext->vertexNml[2 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[8] );
		}

		if( l_arrayValues.size() >= 12 && l_pContext->iFace2 != -1 )
		{
			l_pContext->vertexNml[0 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[ 0] );
			l_pContext->vertexNml[1 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[ 1] );
			l_pContext->vertexNml[2 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[ 2] );

			l_pContext->vertexNml[0 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[ 6] );
			l_pContext->vertexNml[1 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[ 7] );
			l_pContext->vertexNml[2 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[ 8] );

			l_pContext->vertexNml[0 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[ 9] );
			l_pContext->vertexNml[1 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[10] );
			l_pContext->vertexNml[2 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[11] );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::normals> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshFaceTangents)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );
	SubmeshSPtr l_pSubmesh = l_pContext->pSubmesh;

	if( l_pSubmesh )
	{
		if( !l_pContext->vertexTan.size() )
		{
			l_pContext->vertexTan.resize( l_pContext->vertexPos.size() );
		}

		Point3i l_pt3Indices;
		StringArray l_arrayValues = str_utils::split( l_strParams, cuT( " " ), 20 );

		if( l_arrayValues.size() >= 9 && l_pContext->iFace1 != -1 )
		{
			l_pContext->vertexTan[0 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[0] );
			l_pContext->vertexTan[1 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[1] );
			l_pContext->vertexTan[2 + l_pContext->faces[l_pContext->iFace1 + 0] * 3] = str_utils::to_real( l_arrayValues[2] );

			l_pContext->vertexTan[0 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[3] );
			l_pContext->vertexTan[1 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[4] );
			l_pContext->vertexTan[2 + l_pContext->faces[l_pContext->iFace1 + 1] * 3] = str_utils::to_real( l_arrayValues[5] );

			l_pContext->vertexTan[0 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[6] );
			l_pContext->vertexTan[1 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[7] );
			l_pContext->vertexTan[2 + l_pContext->faces[l_pContext->iFace1 + 2] * 3] = str_utils::to_real( l_arrayValues[8] );
		}

		if( l_arrayValues.size() >= 12 && l_pContext->iFace2 != -1 )
		{
			l_pContext->vertexTan[0 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[ 0] );
			l_pContext->vertexTan[1 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[ 1] );
			l_pContext->vertexTan[2 + l_pContext->faces[l_pContext->iFace2 + 0] * 3] = str_utils::to_real( l_arrayValues[ 2] );

			l_pContext->vertexTan[0 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[ 6] );
			l_pContext->vertexTan[1 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[ 7] );
			l_pContext->vertexTan[2 + l_pContext->faces[l_pContext->iFace2 + 1] * 3] = str_utils::to_real( l_arrayValues[ 8] );

			l_pContext->vertexTan[0 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[ 9] );
			l_pContext->vertexTan[1 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[10] );
			l_pContext->vertexTan[2 + l_pContext->faces[l_pContext->iFace2 + 2] * 3] = str_utils::to_real( l_arrayValues[11] );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <submesh::tangents> : Submesh not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_SubmeshEnd )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->vertexPos.size() )
	{
		stVERTEX_GROUP l_group = { 0 };
		l_group.m_uiCount = uint32_t( l_pContext->vertexPos.size() / 3 );
		l_group.m_pVtx = &l_pContext->vertexPos[0];

		if( l_pContext->vertexNml.size() )
		{
			l_group.m_pNml = &l_pContext->vertexNml[0];
		}

		if( l_pContext->vertexTan.size() )
		{
			l_group.m_pTan = &l_pContext->vertexTan[0];
		}

		if( l_pContext->vertexTex.size() )
		{
			l_group.m_pTex = &l_pContext->vertexTex[0];
		}

		l_pContext->pSubmesh->AddPoints( l_group );

		if( l_pContext->faces.size() )
		{
			l_pContext->pSubmesh->AddFaceGroup( reinterpret_cast< stFACE_INDICES * >( &l_pContext->faces[0] ), uint32_t( l_pContext->faces.size() / 3 ) );

			if( l_group.m_pNml )
			{
				if( l_group.m_pTan )
				{
					l_pContext->pSubmesh->ComputeBitangents();
				}
				else
				{
					l_pContext->pSubmesh->ComputeTangentsFromNormals();
				}
			}
			else
			{
				l_pContext->pSubmesh->ComputeNormals();
			}
		}

		l_pContext->vertexPos.clear();
		l_pContext->vertexNml.clear();
		l_pContext->vertexTan.clear();
		l_pContext->vertexTex.clear();
		l_pContext->faces.clear();
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MaterialPass)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->uiPass = 0xFFFFFFFF;
	l_pContext->strName.clear();

	if (l_pContext->pMaterial)
	{
		l_pContext->pMaterial->CreatePass();
		l_pContext->uiPass = l_pContext->pMaterial->GetPassCount() - 1;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <material::pass> : Material not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_PASS );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_MaterialEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->m_pParser->GetEngine()->PostEvent( std::make_shared< InitialiseEvent< Material > >( *l_pContext->pMaterial ) );
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassAmbient)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		Colour l_crColour;
		p_arrayParams[0]->Get( l_crColour );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass )->SetAmbient( l_crColour );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::ambient> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassDiffuse)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		Colour l_crColour;
		p_arrayParams[0]->Get( l_crColour );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetDiffuse( l_crColour );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::diffuse> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassSpecular)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		Colour l_crColour;
		p_arrayParams[0]->Get( l_crColour );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetSpecular( l_crColour );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::specular> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassEmissive)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		Colour l_crColour;
		p_arrayParams[0]->Get( l_crColour );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetEmissive( l_crColour );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::emissive> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassShininess)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		float l_fFloat;
		p_arrayParams[0]->Get( l_fFloat );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetShininess( l_fFloat);
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::shininess> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassAlpha)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		float l_fFloat;
		p_arrayParams[0]->Get( l_fFloat );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetAlpha( l_fFloat);
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::alpha> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassDoubleFace)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		bool l_bDouble;
		p_arrayParams[0]->Get( l_bDouble );
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetDoubleFace( l_bDouble);
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::double_face> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassBlendFunc)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->uiPass >= 0)
	{
		PassSPtr l_pPass = l_pContext->pMaterial->GetPass( l_pContext->uiPass);
		uint32_t l_uiSrcBlend;
		uint32_t l_uiDstBlend;
		p_arrayParams[0]->Get( l_uiSrcBlend );
		p_arrayParams[1]->Get( l_uiDstBlend );
		l_pPass->GetBlendState()->SetAlphaSrcBlend( eBLEND( l_uiSrcBlend ) );
		l_pPass->GetBlendState()->SetAlphaDstBlend( eBLEND( l_uiDstBlend ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::blend_func> : Pass not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassTextureUnit)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pTextureUnit.reset();

	if (l_pContext->uiPass >= 0)
	{
		l_pContext->pTextureUnit = l_pContext->pMaterial->GetPass( l_pContext->uiPass)->AddTextureUnit();
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::texture_unit> : Pass not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_TEXTURE_UNIT );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassGlShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pShaderProgram.reset();
	l_pContext->eShaderObject = eSHADER_TYPE_COUNT;

	if (l_pContext->uiPass >= 0)
	{
		if( l_pContext->m_pParser->GetEngine()->GetRenderSystem()->GetRendererType() == eRENDERER_TYPE_OPENGL )
		{
			l_pContext->pShaderProgram = l_pContext->m_pParser->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_GLSL );
		}
		else
		{
			p_pParser->Ignore();
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::gl_shader> : Pass not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassHlShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pShaderProgram.reset();
	l_pContext->eShaderObject = eSHADER_TYPE_COUNT;

	if (l_pContext->uiPass >= 0)
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->m_pParser->GetEngine()->GetRenderSystem()->GetRendererType();
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			l_pContext->pShaderProgram = l_pContext->m_pParser->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_HLSL );
		}
		else
		{
			p_pParser->Ignore();
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::hl_shader> : Pass not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_PassCgShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pShaderProgram.reset();
	l_pContext->eShaderObject = eSHADER_TYPE_COUNT;

	if (l_pContext->uiPass >= 0)
	{
		l_pContext->pShaderProgram = l_pContext->m_pParser->GetEngine()->GetShaderManager().GetNewProgram( eSHADER_LANGUAGE_CG );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <pass::cg_shader> : Pass not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitImage)
{
	SceneFileContextSPtr	l_pContext		= std::static_pointer_cast< SceneFileContext >( p_pContext );
	Engine *			l_pEngine		= l_pContext->m_pParser->GetEngine();
	ImageCollection &	l_imgCollection	= l_pEngine->GetImageManager();
	ImageSPtr			l_pImage;
	StaticTextureSPtr	l_pTexture;
	Path				l_path;

	if( l_pContext->pTextureUnit )
	{
		if( File::FileExists( p_pContext->pFile->GetFilePath() / p_arrayParams[0]->Get( l_path ) ) )
		{
			l_pContext->pTextureUnit->SetAutoMipmaps( true );
			l_pContext->pTextureUnit->LoadTexture( p_pContext->pFile->GetFilePath() / l_path );
		}
		else if( File::FileExists( l_path ) )
		{
			l_pContext->pTextureUnit->SetAutoMipmaps( true );
			l_pContext->pTextureUnit->LoadTexture( l_path );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <texture_unit::image> : File [" ) + l_path + cuT( "] not found, check the relativeness of the path" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::image> : Texture Unit not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitRenderTarget )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pTextureUnit )
	{
		l_pContext->pRenderTarget = l_pContext->m_pParser->GetEngine()->CreateRenderTarget( eTARGET_TYPE_TEXTURE );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::render_target> : No window initialised." ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_RENDER_TARGET );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitMapType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pTextureUnit)
	{
		uint32_t l_uiMode;
		p_arrayParams[0]->Get( l_uiMode );
		l_pContext->pTextureUnit->SetMappingMode( eTEXTURE_MAP_MODE( l_uiMode ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::map_type> : Texture Unit not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitAlphaFunc)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pTextureUnit )
	{
		uint32_t l_uiFunc;
		float l_fFloat;
		p_arrayParams[0]->Get( l_uiFunc );
		p_arrayParams[1]->Get( l_fFloat );
		l_pContext->pTextureUnit->SetAlphaFunc( eALPHA_FUNC( l_uiFunc ) );
		l_pContext->pTextureUnit->SetAlphaValue( l_fFloat );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::alpha_func> : Texture Unit not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitRgbBlend)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pTextureUnit )
	{
		uint32_t l_uiArg1;
		uint32_t l_uiArg2;
		uint32_t l_uiMode;

		p_arrayParams[0]->Get( l_uiMode );
		p_arrayParams[1]->Get( l_uiArg1 );
		p_arrayParams[2]->Get( l_uiArg2 );

		l_pContext->pTextureUnit->SetRgbFunction( eRGB_BLEND_FUNC( l_uiMode ) );
		l_pContext->pTextureUnit->SetRgbArgument( eBLEND_SRC_INDEX_0, eBLEND_SOURCE( l_uiArg1 ) );
		l_pContext->pTextureUnit->SetRgbArgument( eBLEND_SRC_INDEX_1, eBLEND_SOURCE( l_uiArg2 ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::rgb_blend> : Texture Unit not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitAlphaBlend)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pTextureUnit )
	{
		uint32_t l_uiArg1;
		uint32_t l_uiArg2;
		uint32_t l_uiMode;

		p_arrayParams[0]->Get( l_uiMode );
		p_arrayParams[1]->Get( l_uiArg1 );
		p_arrayParams[2]->Get( l_uiArg2 );

		l_pContext->pTextureUnit->SetAlpFunction( eALPHA_BLEND_FUNC( l_uiMode ) );
		l_pContext->pTextureUnit->SetAlpArgument( eBLEND_SRC_INDEX_0, eBLEND_SOURCE( l_uiArg1 ) );
		l_pContext->pTextureUnit->SetAlpArgument( eBLEND_SRC_INDEX_1, eBLEND_SOURCE( l_uiArg2 ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::alpha_blend> : Texture Unit not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitChannel )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	uint32_t l_uiChannel;
	p_arrayParams[0]->Get( l_uiChannel );
	l_pContext->pTextureUnit->SetChannel( eTEXTURE_CHANNEL( l_uiChannel ) );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitSampler )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;

	SamplerSPtr l_pSampler = l_pContext->m_pParser->GetEngine()->GetSamplerManager().find( p_arrayParams[0]->Get( l_strName ) );

	if( l_pSampler )
	{
		l_pContext->pTextureUnit->SetSampler( l_pSampler );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <texture_unit::sampler> Unknown sampler : [" ) + l_strName + cuT( "]" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_UnitBlendColour )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	Colour l_clrColour;
	p_arrayParams[0]->Get( l_clrColour );
	l_pContext->pTextureUnit->SetBlendColour( l_clrColour );

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlVertexShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX);
		l_pContext->eShaderObject = eSHADER_TYPE_VERTEX;
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::vertex_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::vertex_program> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlPixelShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL);
		l_pContext->eShaderObject = eSHADER_TYPE_PIXEL;
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::pixel_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::pixel_program> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlGeometryShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY);
		l_pContext->eShaderObject = eSHADER_TYPE_GEOMETRY;
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::geometry_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::geometry_program> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlGeometryInputType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		if( l_pContext->eShaderObject == eSHADER_TYPE_GEOMETRY )
		{
			l_pContext->pShaderProgram->SetInputType( l_pContext->eShaderObject, eTOPOLOGY( l_uiType ) );
		}
		else
		{
			if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
			{
				PARSING_ERROR( cuT( "Directive <gl_shader_program::input_type> : Only valid for geometry shader" ) );
			}
			else
			{
				PARSING_WARNING( cuT( "Directive <gl_shader_program::input_type> : Non OpenGL Renderer" ) );
			}
		}
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::input_type> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::input_type> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlGeometryOutputType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		if( l_pContext->eShaderObject == eSHADER_TYPE_GEOMETRY )
		{
			l_pContext->pShaderProgram->SetOutputType( l_pContext->eShaderObject, eTOPOLOGY( l_uiType ) );
		}
		else
		{
			if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
			{
				PARSING_ERROR( cuT( "Directive <gl_shader_program::output_type> : Only valid for geometry shader" ) );
			}
			else
			{
				PARSING_WARNING( cuT( "Directive <gl_shader_program::output_type> : Non OpenGL Renderer" ) );
			}
		}
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::output_type> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::output_type> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlGeometryOutputVtxCount )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		if( l_pContext->eShaderObject == eSHADER_TYPE_GEOMETRY )
		{
			uint8_t l_uiCount;
			p_arrayParams[0]->Get( l_uiCount );
			l_pContext->pShaderProgram->SetOutputVtxCount( l_pContext->eShaderObject, l_uiCount );
		}
		else
		{
			if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
			{
				PARSING_ERROR( cuT( "Directive <gl_shader_program::output_vtx_count> : Only valid for geometry shader" ) );
			}
			else
			{
				PARSING_WARNING( cuT( "Directive <gl_shader_program::output_vtx_count> : Non OpenGL Renderer" ) );
			}
		}
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::output_vtx_count> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::output_vtx_count> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlHullShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL);
		l_pContext->eShaderObject = eSHADER_TYPE_HULL;
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::hull_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::hull_program> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlDomainShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN);
		l_pContext->eShaderObject = eSHADER_TYPE_DOMAIN;
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_shader_program::domain_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_shader_program::domain_program> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlShaderEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetShader( l_pContext->pShaderProgram );
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlShaderProgramFile )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		uint32_t l_uiModel;
		Path l_path;
		p_arrayParams[0]->Get( l_uiModel );
		p_arrayParams[1]->Get( l_path );
		l_pContext->pShaderProgram->SetFile( l_pContext->eShaderObject, eSHADER_MODEL( l_uiModel ), p_pContext->pFile->GetFilePath() / l_path );
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_program::file> : Shader Program not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_program::file> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlShaderProgramVariable)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pFrameVariable.reset();
	p_arrayParams[0]->Get( l_pContext->strName2 );

	if( l_pContext->eShaderObject == eSHADER_TYPE_COUNT )
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_program::variable> : GPU Program not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_program::variable> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_GLSL_SHADER_VARIABLE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlShaderVariableType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );

	if (l_pContext->pShaderProgram && l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		if ( !l_pContext->pFrameVariable )
		{
			l_pContext->pFrameVariable = l_pContext->pShaderProgram->GetUserBuffer()->CreateVariable( *l_pContext->pShaderProgram.get(), eFRAME_VARIABLE_TYPE( l_uiType ), l_pContext->strName2 );
		}
		else
		{
			if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
			{
				PARSING_ERROR( cuT( "Directive <gl_variable::type> : Variable type already set" ) );
			}
			else
			{
				PARSING_WARNING( cuT( "Directive <gl_variable::type> : Non OpenGL Renderer" ) );
			}
		}
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_variable::type> : Shader program or object not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_variable::type> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GlShaderVariableValue)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if (l_pContext->pFrameVariable)
	{
		l_pContext->pFrameVariable->SetValueStr( l_strParams );
	}
	else
	{
		if( l_pContext->eRendererType == eRENDERER_TYPE_OPENGL )
		{
			PARSING_ERROR( cuT( "Directive <gl_variable::value> : Variable not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <gl_variable::value> : Non OpenGL Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlVertexShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX);
		l_pContext->eShaderObject = eSHADER_TYPE_VERTEX;
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_shader_program::vertex_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_shader_program::vertex_program> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlPixelShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL);
		l_pContext->eShaderObject = eSHADER_TYPE_PIXEL;
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_shader_program::pixel_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_shader_program::pixel_program> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlGeometryShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY);
		l_pContext->eShaderObject = eSHADER_TYPE_GEOMETRY;
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_shader_program::geometry_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_shader_program::geometry_program> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlHullShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL);
		l_pContext->eShaderObject = eSHADER_TYPE_HULL;
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_shader_program::hull_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_shader_program::hull_program> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlDomainShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN);
		l_pContext->eShaderObject = eSHADER_TYPE_DOMAIN;
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_shader_program::domain_program> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_shader_program::domain_program> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlFile)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pShaderProgram)
	{
		uint32_t l_uiModel;
		Path l_path;
		p_arrayParams[0]->Get( l_uiModel );
		p_arrayParams[1]->Get( l_path );
		l_pContext->pShaderProgram->SetFile( eSHADER_MODEL( l_uiModel ), p_pContext->pFile->GetFilePath() / l_path );
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_shader_program::file> : Shader not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_shader_program::file> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlShaderEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetShader( l_pContext->pShaderProgram);
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlShaderProgramFile)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		uint32_t l_uiModel;
		Path l_path;
		p_arrayParams[0]->Get( l_uiModel );
		p_arrayParams[1]->Get( l_path );
		l_pContext->pShaderProgram->SetFile( l_pContext->eShaderObject, eSHADER_MODEL( l_uiModel ), p_pContext->pFile->GetFilePath() / l_path );
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_program::file> : Shader Program not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_program::file> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlShaderProgramEntry)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	p_arrayParams[0]->Get( l_strName );

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		l_pContext->pShaderProgram->SetEntryPoint( l_pContext->eShaderObject, l_strName );
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_program::entry> : Shader Program not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_program::entry> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlShaderProgramVariable)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pFrameVariable.reset();
	p_arrayParams[0]->Get( l_pContext->strName2 );

	if( l_pContext->eShaderObject == eSHADER_TYPE_COUNT )
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_program::variable> : GPU Program not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_program::variable> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_HLSL_SHADER_VARIABLE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlShaderVariableType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );

	if (l_pContext->pShaderProgram && l_pContext->eShaderObject != eSHADER_TYPE_COUNT)
	{
		if ( ! l_pContext->pFrameVariable)
		{
			l_pContext->pFrameVariable = l_pContext->pShaderProgram->GetUserBuffer()->CreateVariable( *l_pContext->pShaderProgram.get(), eFRAME_VARIABLE_TYPE( l_uiType ), l_pContext->strName2 );
		}
		else
		{
			eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
			if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
			{
				PARSING_ERROR( cuT( "Directive <hl_variable::type> : Variable type already set" ) );
			}
			else
			{
				PARSING_WARNING( cuT( "Directive <hl_variable::type> : Non D3D Renderer" ) );
			}
		}
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_variable::type> : Shader program or object not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_variable::type> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_HlShaderVariableValue)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if (l_pContext->pFrameVariable)
	{
		l_pContext->pFrameVariable->SetValueStr( l_strParams );
	}
	else
	{
		eRENDERER_TYPE l_eRendererType = l_pContext->eRendererType;
		if( l_eRendererType == eRENDERER_TYPE_DIRECT3D9 || l_eRendererType == eRENDERER_TYPE_DIRECT3D10 || l_eRendererType == eRENDERER_TYPE_DIRECT3D11 )
		{
			PARSING_ERROR( cuT( "Directive <hl_variable::value> : Variable not initialised" ) );
		}
		else
		{
			PARSING_WARNING( cuT( "Directive <hl_variable::value> : Non D3D Renderer" ) );
		}
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgVertexShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_VERTEX);
		l_pContext->eShaderObject = eSHADER_TYPE_VERTEX;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_shader_program::vertex_program> : Cg Shader not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgPixelShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_PIXEL);
		l_pContext->eShaderObject = eSHADER_TYPE_PIXEL;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_shader_program::pixel_program> : Cg Shader not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgGeometryShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_GEOMETRY);
		l_pContext->eShaderObject = eSHADER_TYPE_GEOMETRY;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_shader_program::geometry_program> : Cg Shader not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgHullShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_HULL);
		l_pContext->eShaderObject = eSHADER_TYPE_HULL;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_shader_program::hull_program> : Shader not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgDomainShader)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pShaderProgram->CreateObject( eSHADER_TYPE_DOMAIN);
		l_pContext->eShaderObject = eSHADER_TYPE_DOMAIN;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_shader_program::domain_program> : Shader not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER_PROGRAM );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgFile)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pShaderProgram)
	{
		uint32_t l_uiModel;
		Path l_path;
		p_arrayParams[0]->Get( l_uiModel );
		p_arrayParams[1]->Get( l_path );
		l_pContext->pShaderProgram->SetFile( eSHADER_MODEL( l_uiModel ), p_pContext->pFile->GetFilePath() / l_path );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_shader_program::file> : Cg Shader not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgShaderEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pShaderProgram)
	{
		l_pContext->pMaterial->GetPass( l_pContext->uiPass)->SetShader( l_pContext->pShaderProgram);
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgShaderProgramFile)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		uint32_t l_uiModel;
		Path l_path;
		p_arrayParams[0]->Get( l_uiModel );
		p_arrayParams[1]->Get( l_path );
		l_pContext->pShaderProgram->SetFile( l_pContext->eShaderObject, eSHADER_MODEL( l_uiModel ), p_pContext->pFile->GetFilePath() / l_path );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_program::file> : Shader Program not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgShaderProgramVariable)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pFrameVariable.reset();
	p_arrayParams[0]->Get( l_pContext->strName2 );

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		PARSING_ERROR( cuT( "Directive <cg_program::variable> : GPU Program not initialised" ) );
	}

	ATTRIBUTE_END_PUSH( eSECTION_CG_SHADER_VARIABLE );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgShaderProgramEntry)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;

	if( l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		l_pContext->pShaderProgram->SetEntryPoint( l_pContext->eShaderObject, p_arrayParams[0]->Get( l_strName ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_program::entry> : Shader Program not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgShaderVariableType)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	p_arrayParams[0]->Get( l_uiType );

	if (l_pContext->pShaderProgram && l_pContext->eShaderObject != eSHADER_TYPE_COUNT )
	{
		if ( !l_pContext->pFrameVariable )
		{
			l_pContext->pFrameVariable = l_pContext->pShaderProgram->GetUserBuffer()->CreateVariable( *l_pContext->pShaderProgram.get(), eFRAME_VARIABLE_TYPE( l_uiType ), l_pContext->strName2 );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <cg_variable::type> : Cg Variable type already set" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_variable::type> : Shader program or object not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CgShaderVariableValue)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if (l_pContext->pFrameVariable)
	{
		l_pContext->pFrameVariable->SetValueStr( l_strParams );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <cg_variable::value> : Cg Variable not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_FontFile)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	p_arrayParams[0]->Get( l_pContext->path );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_FontHeight)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	p_arrayParams[0]->Get( l_pContext->iInt8 );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_FontEnd)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( !l_pContext->strName.empty() && !l_pContext->path.empty() )
	{
		FontCollection & l_fontCollection = l_pContext->m_pParser->GetEngine()->GetFontManager();
		FontSPtr l_pFont = l_fontCollection.find( l_pContext->strName);

		if( !l_pFont )
		{
			l_pFont = std::make_shared< Castor::Font >( l_pContext->strName, abs( l_pContext->iInt8 ) );
			l_fontCollection.insert( l_pContext->strName, l_pFont);
		}

		if( l_pFont )
		{
			Castor::Font::BinaryLoader()( *l_pFont, p_pContext->pFile->GetFilePath() / l_pContext->path, l_pContext->iInt8 );
		}
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayPosition)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pOverlay)
	{
		Point2r l_ptPosition;
		p_arrayParams[0]->Get( l_ptPosition );
		l_pContext->pOverlay->SetPosition( l_ptPosition );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <overlay::position> : Overlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlaySize)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pOverlay)
	{
		Point2r l_ptSize;
		p_arrayParams[0]->Get( l_ptSize );
		l_pContext->pOverlay->SetSize( l_ptSize );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <overlay::size> : Overlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayMaterial)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pOverlay )
	{
		String l_strName;
		p_arrayParams[0]->Get( l_strName );
		MaterialManager & l_manager = l_pContext->m_pParser->GetEngine()->GetMaterialManager();
		l_pContext->pOverlay->SetMaterial( l_manager.find( l_strName ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <overlay::material> : Overlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayPanelOverlay )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_PANEL, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, l_pContext->pOverlay->GetScene() );
	l_pContext->pOverlay->SetVisible( false );

	ATTRIBUTE_END_PUSH( eSECTION_PANEL_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayBorderPanelOverlay)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_BORDER_PANEL, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, l_pContext->pOverlay->GetScene() );
	l_pContext->pOverlay->SetVisible( false );

	ATTRIBUTE_END_PUSH( eSECTION_BORDER_PANEL_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayTextOverlay )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	l_pContext->pOverlay = l_pContext->m_pParser->GetEngine()->CreateOverlay( eOVERLAY_TYPE_TEXT, p_arrayParams[0]->Get( l_strName ), l_pContext->pOverlay, l_pContext->pOverlay->GetScene() );
	l_pContext->pOverlay->SetVisible( false );

	ATTRIBUTE_END_PUSH( eSECTION_TEXT_OVERLAY );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_OverlayEnd )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pOverlay->SetVisible( true );
	l_pContext->pOverlay = l_pContext->pOverlay->GetParent();

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlaySizes )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	OverlaySPtr l_pOverlay = l_pContext->pOverlay;

	if( l_pOverlay && l_pOverlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
	{
		Point4r l_ptSize;
		p_arrayParams[0]->Get( l_ptSize );
		std::static_pointer_cast< BorderPanelOverlay >( l_pOverlay->GetOverlayCategory() )->SetBorderSize( l_ptSize );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <border_panel_overlay::border_size> : Overlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BorderPanelOverlayMaterial )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	OverlaySPtr l_pOverlay = l_pContext->pOverlay;

	if( l_pOverlay && l_pOverlay->GetType() == eOVERLAY_TYPE_BORDER_PANEL )
	{
		String l_strName;
		p_arrayParams[0]->Get( l_strName );
		MaterialManager & l_manager = l_pContext->m_pParser->GetEngine()->GetMaterialManager();
		l_pContext->pOverlay->SetMaterial( l_manager.find( l_strName ) );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <border_panel_overlay::border_material> : Overlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayFont)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	OverlaySPtr l_pOverlay = l_pContext->pOverlay;

	if( l_pOverlay && l_pOverlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		FontCollection & l_fontManager = l_pContext->m_pParser->GetEngine()->GetFontManager();
		String l_strName;
		p_arrayParams[0]->Get( l_strName );

		if( l_fontManager.has( l_strName ) )
		{
			std::static_pointer_cast< TextOverlay >( l_pOverlay->GetOverlayCategory() )->SetFont( l_strName );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <text_overlay::font> : Unknown font" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <text_overlay::font> : TextOverlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_TextOverlayText)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	OverlaySPtr l_pOverlay = l_pContext->pOverlay;
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if( l_pOverlay && l_pOverlay->GetType() == eOVERLAY_TYPE_TEXT )
	{
		std::static_pointer_cast< TextOverlay >( l_pOverlay->GetOverlayCategory() )->SetCaption( l_strParams );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <text_overlay::text> : TextOverlay not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraParent)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_strName;
	SceneNodeSPtr l_pParent = l_pContext->pScene->GetNode( p_arrayParams[0]->Get( l_strName ) );

	if (l_pParent)
	{
		l_pContext->pSceneNode = l_pParent;
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <camera::parent> : Node " ) + l_strName + cuT( " does not exist" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraViewport)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->pViewport = std::make_shared< Viewport >( l_pContext->m_pParser->GetEngine(), Size( 100, 100 ), eVIEWPORT_TYPE_3D );
	String l_strParams;
	p_arrayParams[0]->Get( l_strParams );

	if( !l_strParams.empty() )
	{
		UIntStrMap::iterator l_it = l_pContext->m_mapViewportModes.find( l_strParams );

		if( l_it != l_pContext->m_mapViewportModes.end() )
		{
			l_pContext->pViewport->SetType( eVIEWPORT_TYPE( l_it->second ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <viewport::type> : Unknown viewport type : " ) + l_strParams );
		}
	}

	ATTRIBUTE_END_PUSH( eSECTION_VIEWPORT );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraPrimitive )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	l_pContext->ePrimitiveType = eTOPOLOGY( p_arrayParams[0]->Get( l_uiType ) );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_CameraEnd )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if( l_pContext->pSceneNode && l_pContext->pViewport )
	{
		l_pContext->pScene->CreateCamera( l_pContext->strName, l_pContext->pSceneNode, l_pContext->pViewport )->SetPrimitiveType( l_pContext->ePrimitiveType );
	}

	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportType )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	uint32_t l_uiType;
	l_pContext->pViewport->SetType( eVIEWPORT_TYPE( p_arrayParams[0]->Get( l_uiType ) ) );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportLeft )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	real l_rValue;
	p_arrayParams[0]->Get( l_rValue );
	l_pContext->pViewport->SetLeft( l_rValue );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportRight )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	real l_rValue;
	p_arrayParams[0]->Get( l_rValue );
	l_pContext->pViewport->SetRight( l_rValue );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportTop )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	real l_rValue;
	p_arrayParams[0]->Get( l_rValue );
	l_pContext->pViewport->SetTop( l_rValue );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportBottom )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	real l_rValue;
	p_arrayParams[0]->Get( l_rValue );
	l_pContext->pViewport->SetBottom( l_rValue );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportNear )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	real l_rValue;
	p_arrayParams[0]->Get( l_rValue );
	l_pContext->pViewport->SetNear( l_rValue );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportFar )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	real l_rValue;
	p_arrayParams[0]->Get( l_rValue );
	l_pContext->pViewport->SetFar( l_rValue );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportSize )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Size l_size;
	p_arrayParams[0]->Get( l_size );
	l_pContext->pViewport->SetSize( l_size );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportFovY )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	float l_fFovY;
	p_arrayParams[0]->Get( l_fFovY );
	l_pContext->pViewport->SetFovY( Angle::FromDegrees( l_fFovY ) );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_ViewportAspectRatio )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	float l_fRatio;
	p_arrayParams[0]->Get( l_fRatio );
	l_pContext->pViewport->SetRatio( l_fRatio );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardParent)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );

	if (l_pContext->pBillboards)
	{
		String l_strName;
		p_arrayParams[0]->Get( l_strName );
		SceneNodeSPtr l_pParent = l_pContext->pScene->GetNode( l_strName );

		if (l_pParent)
		{
			l_pParent->AttachObject( l_pContext->pBillboards.get());
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <billboard::parent> : Node " ) + l_strName + cuT( " does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <billboard::parent> : Geometry not initialised." ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardPositions )
{
	ATTRIBUTE_END_PUSH( eSECTION_BILLBOARD_LIST );
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardMaterial)
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	if (l_pContext->pBillboards)
	{
		MaterialManager & l_manager = l_pContext->m_pParser->GetEngine()->GetMaterialManager();
		String l_strName;
		p_arrayParams[0]->Get( l_strName );

		if (l_manager.has( l_strName ) )
		{
			l_pContext->pBillboards->SetMaterial( l_manager.find( l_strName ) );
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <billboard::material> : Material " ) + l_strName + cuT( " does not exist" ) );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <billboard::material> : Billboard not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardDimensions )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Size l_dimensions;
	p_arrayParams[0]->Get( l_dimensions );
	l_pContext->pBillboards->SetDimensions( l_dimensions );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardEnd )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	l_pContext->m_pParser->GetEngine()->PostEvent( std::make_shared< InitialiseEvent< BillboardList > >( *l_pContext->pBillboards ) );
	l_pContext->pBillboards = nullptr;
	ATTRIBUTE_END_POP();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_BillboardPoint )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	Point3r l_position;
	p_arrayParams[0]->Get( l_position );
	l_pContext->pBillboards->AddPoint( l_position );
	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GroupAnimatedObject )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_name;
	p_arrayParams[0]->Get( l_name );

	if( l_pContext->pScene )
	{
		GeometrySPtr l_pGeometry = l_pContext->pScene->GetGeometry( l_name );

		if( l_pGeometry )
		{
			AnimatedObjectSPtr l_pObject = l_pContext->pGroup->CreateObject( l_name );

			if( l_pObject )
			{
				l_pObject->SetGeometry( l_pGeometry );
				l_pGeometry->SetAnimatedObject( l_pObject );
			}
			else
			{
				PARSING_ERROR( cuT( "Directive <animated_object_group::animated_object> : An object with the given name already exists : " ) + l_name );
			}
		}
		else
		{
			PARSING_ERROR( cuT( "Directive <animated_object_group::animated_object> : No geometry with name " ) + l_name );
		}
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <animated_object_group::animated_object> : Scene not initialised" ) );
	}

	ATTRIBUTE_END();
}

IMPLEMENT_ATTRIBUTE_PARSER( Castor3D, Parser_GroupAnimation )
{
	SceneFileContextSPtr l_pContext = std::static_pointer_cast< SceneFileContext >( p_pContext );
	String l_name;
	p_arrayParams[0]->Get( l_name );

	if( l_pContext->pGroup )
	{
		l_pContext->pGroup->AddAnimation( l_name );
		l_pContext->pGroup->SetAnimationLooped( l_name, true );
		l_pContext->pGroup->StartAnimation( l_name );
	}
	else
	{
		PARSING_ERROR( cuT( "Directive <animated_object_group::animation> : No animated object group initialised" ) );
	}

	ATTRIBUTE_END();
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif
