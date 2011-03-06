#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9TextureRenderer.h"
#include "Dx9RenderSystem/Dx9RenderSystem.h"
#include <CastorUtils/PixelFormat.h>

using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

Dx9TextureRenderer :: Dx9TextureRenderer( SceneManager * p_pSceneManager)
	:	TextureRenderer( p_pSceneManager)
	,	m_pTextureObject( NULL)
{}

void Dx9TextureRenderer :: Cleanup()
{
	(this->*m_pfnCleanup)();
}

bool Dx9TextureRenderer :: Initialise()
{
	m_pfnCleanup	= & Dx9TextureRenderer::_cleanup;
	m_pfnInitialise	= & Dx9TextureRenderer::_initialise;
	m_pfnRender		= & Dx9TextureRenderer::_render;
	m_pfnEndRender	= & Dx9TextureRenderer::_endRender;

	return (this->*m_pfnInitialise)();
}

void Dx9TextureRenderer :: Render()
{
	(this->*m_pfnRender)();
}

void Dx9TextureRenderer :: EndRender()
{
	(this->*m_pfnEndRender)();
}

void  Dx9TextureRenderer :: _cleanup()
{
	if (m_pTextureObject != NULL)
	{
		m_pTextureObject->Release();
		m_pTextureObject = NULL;
	}
}

bool Dx9TextureRenderer :: _initialise()
{
	bool l_bHasMipmaps  = true;
	bool l_bAutoMipmaps = Dx9RenderSystem::GetSingletonPtr()->GetD3dObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, D3dEnum::Get( m_target->GetPixelFormat())) != D3D_OK;
	unsigned long l_ulUsage = l_bAutoMipmaps ? D3DUSAGE_AUTOGENMIPMAP : 0;

	CheckDxError( Dx9RenderSystem::GetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3dEnum::Get( m_target->GetWrapMode( 0))), CU_T( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_ADDRESSU"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3dEnum::Get( m_target->GetWrapMode( 1))), CU_T( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_ADDRESSV"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3dEnum::Get( m_target->GetInterpolationMode( 0))), CU_T( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_MINFILTER"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3dEnum::Get( m_target->GetInterpolationMode( 1))), CU_T( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_MAGFILTER"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE), CU_T( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_MIPFILTER"), false);
	CheckDxError( Dx9RenderSystem::GetDevice()->CreateTexture( m_target->GetWidth(), m_target->GetHeight(), 0, l_ulUsage, D3dEnum::Get( m_target->GetPixelFormat()), D3DPOOL_MANAGED, & m_pTextureObject, NULL), CU_T( "Dx9TextureRenderer :: Initialise - Can't create texture"), true);

	D3DLOCKED_RECT l_lockedRect;
	RECT l_rcLock = { 0, 0, m_target->GetWidth(), m_target->GetHeight() };
	CheckDxError( m_pTextureObject->LockRect( 0, & l_lockedRect, & l_rcLock, 0), CU_T( "Dx9TextureRenderer :: Initialise - LockRect"), false);
	unsigned char * l_pDestPix = reinterpret_cast<unsigned char *>( l_lockedRect.pBits);
	const unsigned char * l_pSrcPix = m_target->GetImagePixels();
	unsigned int l_uiBpp = Castor::Resources::GetBytesPerPixel( m_target->GetPixelFormat());
	std::copy( l_pSrcPix, l_pSrcPix + m_target->GetWidth() * l_uiBpp * m_target->GetHeight(), l_pDestPix);
	CheckDxError( m_pTextureObject->UnlockRect( 0), CU_T( "Dx9TextureRenderer :: Initialise - LockRect"), false);

	return true;
}

void Dx9TextureRenderer :: _render()
{
	IDirect3DDevice9 * l_pDevice = Dx9RenderSystem::GetDevice();
	CheckDxError( l_pDevice->SetTexture( m_target->GetIndex(), m_pTextureObject), CU_T( "Dx9TextureRenderer :: Render - SetTexture"), false);
	CheckDxError( l_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_target->GetPrimaryColour().ToLong()), "Dx9TextureRenderer :: Render - PrimaryColour", false);


	if (m_target->GetRgbMode() != TextureUnit::eRgbModeNone)
	{
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_COLOROP,   D3dEnum::Get( m_target->GetRgbMode())), "Dx9TextureRenderer :: Render - RgbMode", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_COLORARG1, D3dEnum::Get( m_target->GetRgbArgument( 0))), "Dx9TextureRenderer :: Render - RgbArgument", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_COLORARG2, D3dEnum::Get( m_target->GetRgbArgument( 1))), "Dx9TextureRenderer :: Render - RgbArgument", false);
	}

	if (m_target->GetAlpMode() != TextureUnit::eAlphaModeNone)
	{
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_ALPHAOP,   D3dEnum::Get( m_target->GetAlpMode())), "Dx9TextureRenderer :: Render - AlpMode", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_ALPHAARG1, D3dEnum::Get( m_target->GetAlpArgument( 0))), "Dx9TextureRenderer :: Render - AlpArgument", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_ALPHAARG2, D3dEnum::Get( m_target->GetAlpArgument( 1))), "Dx9TextureRenderer :: Render - AlpArgument", false);
	}

	if (m_target->GetAlphaFunc() != TextureUnit::eAlphaAlways)
	{
		CheckDxError( l_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true), CU_T( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHATESTENABLE"), false);
		CheckDxError( l_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3dEnum::Get( m_target->GetAlphaFunc())), CU_T( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHAFUNC"), false);
		CheckDxError( l_pDevice->SetRenderState( D3DRS_ALPHAREF, DWORD( m_target->GetAlphaValue() * 255)), CU_T( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHAFUNC"), false);
	}

	Pipeline::MatrixMode( Pipeline::eMATRIX_MODE( Pipeline::eMatrixTexture0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
}

void Dx9TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eMatrixModelView);

	if (m_target->GetAlphaFunc() != TextureUnit::eAlphaAlways)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, false), CU_T( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHATESTENABLE"), false);
	}

	CheckDxError( Dx9RenderSystem::GetDevice()->SetTexture( m_target->GetIndex(), NULL), CU_T( "Dx9TextureRenderer :: _endRender - "), false);
}
#pragma warning( pop)