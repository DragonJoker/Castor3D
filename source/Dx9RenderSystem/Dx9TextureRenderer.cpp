#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9TextureRenderer.hpp"
#include "Dx9RenderSystem/Dx9RenderSystem.hpp"
#include <CastorUtils/Pixel.hpp>

using namespace Castor3D;

#pragma warning( push)
#pragma warning( disable:4996)

Dx9TextureRenderer :: Dx9TextureRenderer()
	:	TextureRenderer()
	,	m_pTextureObject( nullptr)
{
	m_pfnCleanup	= & Dx9TextureRenderer::_cleanup;
	m_pfnInitialise	= & Dx9TextureRenderer::_initialise;
	m_pfnRender		= & Dx9TextureRenderer::_render;
	m_pfnEndRender	= & Dx9TextureRenderer::_endRender;
}

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
	if (m_pTextureObject)
	{
		m_pTextureObject->Release();
		m_pTextureObject = nullptr;
	}
}

bool Dx9TextureRenderer :: _initialise()
{
	IDirect3DDevice9 * l_pDevice = Dx9RenderSystem::GetDevice();
	bool l_bHasMipmaps  = true;
	bool l_bAutoMipmaps = Dx9RenderSystem::GetD3dObject()->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_AUTOGENMIPMAP, D3DRTYPE_TEXTURE, D3dEnum::Get( m_target->GetPixelFormat())) != D3D_OK;
	unsigned long l_ulUsage = l_bAutoMipmaps ? D3DUSAGE_AUTOGENMIPMAP : 0;

	CheckDxError( l_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU,  D3dEnum::Get( m_target->GetWrapMode( TextureUnit::eUVW_U))), cuT( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_ADDRESSU"), false);
	CheckDxError( l_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV,  D3dEnum::Get( m_target->GetWrapMode( TextureUnit::eUVW_V))), cuT( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_ADDRESSV"), false);
	CheckDxError( l_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3dEnum::Get( m_target->GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER_MIN))), cuT( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_MINFILTER"), false);
	CheckDxError( l_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3dEnum::Get( m_target->GetInterpolationMode( TextureUnit::eINTERPOLATION_FILTER_MAG))), cuT( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_MAGFILTER"), false);
//	CheckDxError( l_pDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE), cuT( "Dx9TextureRenderer :: Initialise - SetSamplerState - D3DSAMP_MIPFILTER"), false);
	CheckDxError( l_pDevice->CreateTexture( m_target->GetWidth(), m_target->GetHeight(), 0, D3DUSAGE_AUTOGENMIPMAP, D3dEnum::Get( m_target->GetPixelFormat()), D3DPOOL_MANAGED, & m_pTextureObject, nullptr), cuT( "Dx9TextureRenderer :: Initialise - Can't create texture"), true);

	D3DLOCKED_RECT l_lockedRect;
	RECT l_rcLock = { 0, 0, m_target->GetWidth(), m_target->GetHeight() };
	CheckDxError( m_pTextureObject->LockRect( 0, & l_lockedRect, & l_rcLock, 0), cuT( "Dx9TextureRenderer :: Initialise - LockRect"), false);
	unsigned char * l_pDestPix = reinterpret_cast<unsigned char *>( l_lockedRect.pBits);
	unsigned char const * l_pSrcPix = m_target->GetImagePixels();
	unsigned int l_uiBpp = Castor::Resources::GetBytesPerPixel( m_target->GetPixelFormat());
	std::copy( l_pSrcPix, l_pSrcPix + m_target->GetWidth() * l_uiBpp * m_target->GetHeight(), l_pDestPix);
	CheckDxError( m_pTextureObject->UnlockRect( 0), cuT( "Dx9TextureRenderer :: Initialise - UnlockRect"), false);

	return true;
}

void Dx9TextureRenderer :: _render()
{
	IDirect3DDevice9 * l_pDevice = Dx9RenderSystem::GetDevice();
	CheckDxError( l_pDevice->SetTexture( m_target->GetIndex(), m_pTextureObject), cuT( "Dx9TextureRenderer :: Render - SetTexture"), false);
	CheckDxError( l_pDevice->SetRenderState( D3DRS_TEXTUREFACTOR, m_target->GetPrimaryColour().ARGB()), "Dx9TextureRenderer :: Render - PrimaryColour", false);


	if (m_target->GetRgbFunction() != TextureUnit::eRGB_BLEND_FUNC_NONE)
	{
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_COLOROP,   D3dEnum::Get( m_target->GetRgbFunction())), "Dx9TextureRenderer :: Render - RgbMode", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_COLORARG1, D3dEnum::Get( m_target->GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_0))), "Dx9TextureRenderer :: Render - RgbArgument", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_COLORARG2, D3dEnum::Get( m_target->GetRgbArgument( TextureUnit::eBLEND_SRC_INDEX_1))), "Dx9TextureRenderer :: Render - RgbArgument", false);
	}

	if (m_target->GetAlpFunction() != TextureUnit::eALPHA_BLEND_FUNC_NONE)
	{
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_ALPHAOP,   D3dEnum::Get( m_target->GetAlpFunction())), "Dx9TextureRenderer :: Render - AlpMode", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_ALPHAARG1, D3dEnum::Get( m_target->GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_0))), "Dx9TextureRenderer :: Render - AlpArgument", false);
		CheckDxError( l_pDevice->SetTextureStageState( m_target->GetIndex(), D3DTSS_ALPHAARG2, D3dEnum::Get( m_target->GetAlpArgument( TextureUnit::eBLEND_SRC_INDEX_1))), "Dx9TextureRenderer :: Render - AlpArgument", false);
	}

	if (m_target->GetAlphaFunc() != TextureUnit::eALPHA_FUNC_ALWAYS)
	{
		CheckDxError( l_pDevice->SetRenderState( D3DRS_ALPHATESTENABLE, true), cuT( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHATESTENABLE"), false);
		CheckDxError( l_pDevice->SetRenderState( D3DRS_ALPHAFUNC, D3dEnum::Get( m_target->GetAlphaFunc())), cuT( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHAFUNC"), false);
		CheckDxError( l_pDevice->SetRenderState( D3DRS_ALPHAREF, DWORD( m_target->GetAlphaValue() * 255)), cuT( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHAFUNC"), false);
	}

	Pipeline::MatrixMode( Pipeline::eMODE( Pipeline::eMODE_TEXTURE0 + m_target->GetIndex()));
	Pipeline::LoadIdentity();
}

void Dx9TextureRenderer :: _endRender()
{
	Pipeline::MatrixMode( Pipeline::eMODE_MODELVIEW);

	if (m_target->GetAlphaFunc() != TextureUnit::eALPHA_FUNC_ALWAYS)
	{
		CheckDxError( Dx9RenderSystem::GetDevice()->SetRenderState( D3DRS_ALPHATESTENABLE, false), cuT( "Dx9TextureRenderer :: _render - SetRenderState - D3DRS_ALPHATESTENABLE"), false);
	}

	CheckDxError( Dx9RenderSystem::GetDevice()->SetTexture( m_target->GetIndex(), nullptr), cuT( "Dx9TextureRenderer :: _endRender - "), false);
}
#pragma warning( pop)
