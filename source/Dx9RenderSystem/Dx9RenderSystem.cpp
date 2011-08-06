#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9RenderSystem.hpp"
#include "Dx9RenderSystem/Dx9SubmeshRenderer.hpp"
#include "Dx9RenderSystem/Dx9CameraRenderer.hpp"
#include "Dx9RenderSystem/Dx9LightRenderer.hpp"
#include "Dx9RenderSystem/Dx9OverlayRenderer.hpp"
#include "Dx9RenderSystem/Dx9MaterialRenderer.hpp"
#include "Dx9RenderSystem/Dx9TextureRenderer.hpp"
#include "Dx9RenderSystem/Dx9WindowRenderer.hpp"
#include "Dx9RenderSystem/Dx9ShaderProgram.hpp"
#include "Dx9RenderSystem/Dx9ShaderObject.hpp"
#include "Dx9RenderSystem/CgDx9ShaderProgram.hpp"
#include "Dx9RenderSystem/CgDx9ShaderObject.hpp"

using namespace Castor3D;

Dx9RenderSystem :: Dx9RenderSystem()
	:	RenderSystem()
	,	m_pDirect3D( nullptr)
	,	m_pDevice( nullptr)
{
	Logger::LogMessage( cuT( "Dx9RenderSystem :: Dx9RenderSystem"));
	m_setAvailableIndexes.insert( 0);
	m_setAvailableIndexes.insert( 1);
	m_setAvailableIndexes.insert( 2);
	m_setAvailableIndexes.insert( 3);
	m_setAvailableIndexes.insert( 4);
	m_setAvailableIndexes.insert( 5);
	m_setAvailableIndexes.insert( 6);
	m_setAvailableIndexes.insert( 7);
}

Dx9RenderSystem :: ~Dx9RenderSystem()
{
	Delete();
}

void Dx9RenderSystem :: Initialise()
{
	Logger::LogMessage( cuT( "Dx9RenderSystem :: Initialise"));

	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( cuT( "************************************************************************************************************************"));
	Logger::LogMessage( cuT( "Initialising Direct3D"));

	if ((m_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION)) == NULL)
	{
		CASTOR_EXCEPTION( "Can't create D3D9 object");
	}


	sm_useMultiTexturing = true;
	sm_initialised = true;

	m_cgContext = cgCreateContext();
	CheckShaderSupport();
	_checkCgAvailableProfiles();

	Dx9Pipeline::InitFunctions();

	Logger::LogMessage( cuT( "Direct3D Initialisation Ended"));
	Logger::LogMessage( cuT( "************************************************************************************************************************"));
}

void Dx9RenderSystem :: _isCgProfileSupported( eSHADER_TYPE p_eType, char const * p_szName)
{
	CGprofile l_cgProfile = cgGetProfile( p_szName);
	bool l_bSupported = cgIsProfileSupported( l_cgProfile) != 0 && cgD3D9IsProfileSupported( l_cgProfile) != 0;
	Logger::LogMessage( String( "RenderSystem :: CheckCgProfileSupport - Profile : ") + p_szName + String( " - Support : ") + (l_bSupported ? cuT( "true") : cuT( "false")));

	if (l_bSupported)
	{
		if (m_mapSupportedProfiles[p_eType] == CG_PROFILE_UNKNOWN)
		{
			m_mapSupportedProfiles[p_eType] = l_cgProfile;
		}
	}
}

void Dx9RenderSystem :: _checkCgAvailableProfiles()
{
	for (int i = 0 ; i < eSHADER_TYPE_COUNT ; i++)
	{
		m_mapSupportedProfiles[i] = CG_PROFILE_UNKNOWN;
	}

	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vp40"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"fp40"		);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vp30"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"fp30"		);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vp20"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"fp20"		);
	_isCgProfileSupported( eSHADER_TYPE_DOMAIN,		"ds_5_0"	);
	_isCgProfileSupported( eSHADER_TYPE_HULL,		"hs_5_0"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_5_0"	);
	_isCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_5_0"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_5_0"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_4_0"	);
	_isCgProfileSupported( eSHADER_TYPE_GEOMETRY,	"gs_4_0"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_4_0"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"hlslv"		);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"hlslf"		);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_3_0"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_3_0"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_2_x"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_x"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_2_sw"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_sw"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_2_0"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_2_0"	);
	_isCgProfileSupported( eSHADER_TYPE_VERTEX,		"vs_1_1"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_3"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_2"	);
	_isCgProfileSupported( eSHADER_TYPE_PIXEL,		"ps_1_1"	);
}

void Dx9RenderSystem :: Delete()
{
	if (m_cgContext)
	{
		cgDestroyContext( m_cgContext);
		m_cgContext = nullptr;
	}

	Cleanup();
	map::deleteAll( m_contextMap);

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = nullptr;
	}

	if (m_pDirect3D)
	{
		m_pDirect3D->Release();
		m_pDirect3D = nullptr;
	}
}

void Dx9RenderSystem :: Cleanup()
{
	m_submeshesRenderers.clear();
	m_textureRenderers.clear();
	m_passRenderers.clear();
	m_lightRenderers.clear();
	m_windowRenderers.clear();
	m_cameraRenderers.clear();
}

void Dx9RenderSystem :: CheckShaderSupport()
{
	sm_useShader[eSHADER_TYPE_VERTEX] = true;
	sm_useShader[eSHADER_TYPE_PIXEL] = true;
	sm_useShader[eSHADER_TYPE_GEOMETRY] = false;
	sm_useShader[eSHADER_TYPE_HULL] = false;
	sm_useShader[eSHADER_TYPE_DOMAIN] = false;
}

void Dx9RenderSystem :: RenderAmbientLight( Colour const & p_clColour)
{
	if (RenderSystem::UseShaders())
	{
		if (m_pCurrentProgram)
		{
			if (m_pCurrentProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_HLSL)
			{
				static_cast <Dx9ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
			else if (m_pCurrentProgram->GetType() == ShaderProgramBase::eSHADER_LANGUAGE_CG)
			{
				static_cast <CgDx9ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
		}
	}
	else
	{
		CheckDxError( m_pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( p_clColour.Red(), p_clColour.Green(), p_clColour.Blue(), p_clColour.Alpha())), cuT( "Dx9RenderSystem :: RenderAmbientLight - SetRenderState"), false);
	}
}

void Dx9RenderSystem :: InitialiseDevice( HWND p_hWnd, D3DPRESENT_PARAMETERS * p_presentParameters)
{
	if (m_pDevice)
	{
		return;
	}

	if (m_pDirect3D)
	{
		if (CheckDxError( m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, p_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, p_presentParameters, & m_pDevice), cuT( "Dx9RenderSystem :: InitialiseDevice - CreateDevice"), false))
		{
			CheckDxError( m_pDevice->SetRenderState( D3DRS_DITHERENABLE,     	TRUE),			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_DITHERENABLE"),		false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_LIGHTING,         	TRUE),			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_LIGHTING"),			false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_SPECULARENABLE,     	TRUE),			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_SPECULARENABLE"),	false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_ZENABLE,          	TRUE),			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_ZENABLE"),			false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_FOGENABLE,        	FALSE), 		cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_FOGENABLE "),		false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_STENCILMASK,      	0xFF), 			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_STENCILMASK"),		false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 	0xFF), 			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_STENCILWRITEMASK"),	false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE),			cuT( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_ALPHABLENDENABLE"), false);
		}
		else
		{
			Logger::LogError( "Dx9Context ::_createDxContext  - CreateDevice");
		}

		sm_useShaders = true;
	}
}

void Dx9RenderSystem :: DrawIndexedPrimitives( ePRIMITIVE_TYPE p_eType, size_t p_uiMinVertex, size_t p_uiVertexCount, size_t p_uiFirstIndex, size_t p_uiCount)
{
	m_pDevice->DrawIndexedPrimitive( D3dEnum::Get( p_eType), 0, p_uiMinVertex, p_uiVertexCount, p_uiFirstIndex, p_uiCount);
}

HlslShaderProgramPtr Dx9RenderSystem :: _createHlslShaderProgram()
{
	HlslShaderProgramPtr l_pReturn;
	l_pReturn = HlslShaderProgramPtr( new Dx9ShaderProgram);
	return l_pReturn;
}

CgShaderProgramPtr Dx9RenderSystem :: _createCgShaderProgram()
{
	CgShaderProgramPtr l_pReturn;
	l_pReturn = CgShaderProgramPtr( new CgDx9ShaderProgram);
	return l_pReturn;
}

int Dx9RenderSystem :: LockLight()
{
	int l_iReturn = -1;

	if (m_setAvailableIndexes.size() > 0)
	{
		l_iReturn = *m_setAvailableIndexes.begin();
		m_setAvailableIndexes.erase( m_setAvailableIndexes.begin());
	}

	return l_iReturn;
}

void Dx9RenderSystem :: UnlockLight( int p_iIndex)
{
	if (p_iIndex >= 0 && m_setAvailableIndexes.find( p_iIndex) == m_setAvailableIndexes.end())
	{
		m_setAvailableIndexes.insert( std::set <int>::value_type( p_iIndex));
	}
}

void Dx9RenderSystem :: SetCurrentShaderProgram( ShaderProgramBase * p_pVal)
{
	if (m_pCurrentProgram != p_pVal)
	{
		RenderSystem::SetCurrentShaderProgram( p_pVal);

		for (size_t i = 0 ; i < m_lightRenderers.size() ; i++)
		{
			m_lightRenderers[i]->Initialise();
		}

		Dx9Pipeline::InitFunctions();
	}
}

void Dx9RenderSystem :: BeginOverlaysRendering()
{
	RenderSystem::BeginOverlaysRendering();
}

SubmeshRendererPtr Dx9RenderSystem :: _createSubmeshRenderer()
{
	return SubmeshRendererPtr( new Dx9SubmeshRenderer());
}

TextureRendererPtr Dx9RenderSystem :: _createTextureRenderer()
{
	return TextureRendererPtr( new Dx9TextureRenderer());
}

PassRendererPtr Dx9RenderSystem :: _createPassRenderer()
{
	return PassRendererPtr( new Dx9PassRenderer());
}

CameraRendererPtr Dx9RenderSystem :: _createCameraRenderer()
{
	return CameraRendererPtr( new Dx9CameraRenderer());
}

LightRendererPtr Dx9RenderSystem :: _createLightRenderer()
{
	return LightRendererPtr( new Dx9LightRenderer());
}

WindowRendererPtr Dx9RenderSystem :: _createWindowRenderer()
{
	return WindowRendererPtr( new Dx9WindowRenderer());
}

OverlayRendererPtr Dx9RenderSystem :: _createOverlayRenderer()
{
	return OverlayRendererPtr( new Dx9OverlayRenderer());
}

IndexBufferPtr Dx9RenderSystem :: _createIndexBuffer()
{
	IndexBufferPtr l_pReturn;

	l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateBuffer<unsigned int, Dx9IndexBuffer>();
	return l_pReturn;
}

VertexBufferPtr Dx9RenderSystem :: _createVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
{
	VertexBufferPtr l_pReturn;
	l_pReturn = Root::GetSingleton()->GetBufferManager()->CreateVertexBuffer<Dx9VertexBuffer>( p_pElements, p_uiNbElements);
	return l_pReturn;
}

TextureBufferObjectPtr Dx9RenderSystem :: _createTextureBuffer()
{
	TextureBufferObjectPtr l_pReturn;
	return l_pReturn;
}
