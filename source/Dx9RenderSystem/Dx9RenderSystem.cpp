#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9RenderSystem.h"
#include "Dx9RenderSystem/Dx9SubmeshRenderer.h"
#include "Dx9RenderSystem/Dx9CameraRenderer.h"
#include "Dx9RenderSystem/Dx9LightRenderer.h"
#include "Dx9RenderSystem/Dx9OverlayRenderer.h"
#include "Dx9RenderSystem/Dx9MaterialRenderer.h"
#include "Dx9RenderSystem/Dx9TextureRenderer.h"
#include "Dx9RenderSystem/Dx9TextureEnvironmentRenderer.h"
#include "Dx9RenderSystem/Dx9WindowRenderer.h"
#include "Dx9RenderSystem/Dx9ShaderProgram.h"
#include "Dx9RenderSystem/Dx9ShaderObject.h"
#include "Dx9RenderSystem/CgDx9ShaderProgram.h"
#include "Dx9RenderSystem/CgDx9ShaderObject.h"

using namespace Castor3D;

Dx9RenderSystem :: Dx9RenderSystem( SceneManager * p_pSceneManager)
	:	RenderSystem( p_pSceneManager)
	,	m_pDirect3D( NULL)
	,	m_pDevice( NULL)
{
	Logger::LogMessage( CU_T( "Dx9RenderSystem :: Dx9RenderSystem"));
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
	Logger::LogMessage( CU_T( "Dx9RenderSystem :: Initialise"));

	if (sm_initialised)
	{
		return;
	}

	Logger::LogMessage( CU_T( "************************************************************************************************************************"));
	Logger::LogMessage( CU_T( "Initialising Direct3D"));

	if ((m_pDirect3D = Direct3DCreate9( D3D_SDK_VERSION)) == NULL)
	{
		CASTOR_EXCEPTION( "Can't create D3D9 object");
	}


	sm_useMultiTexturing = true;

	Logger::LogMessage( CU_T( "Direct3D Initialisation Ended"));
	Logger::LogMessage( CU_T( "************************************************************************************************************************"));
	sm_initialised = true;

	Dx9Pipeline::InitFunctions();
}

void Dx9RenderSystem :: Delete()
{
	Cleanup();
	map::deleteAll( m_contextMap);

	if (m_pDevice != NULL)
	{
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	if (m_pDirect3D != NULL)
	{
		m_pDirect3D->Release();
		m_pDirect3D = NULL;
	}
}

void Dx9RenderSystem :: Cleanup()
{
	m_submeshesRenderers.clear();
	m_texEnvRenderers.clear();
	m_textureRenderers.clear();
	m_passRenderers.clear();
	m_lightRenderers.clear();
	m_windowRenderers.clear();
	m_cameraRenderers.clear();
}

void Dx9RenderSystem :: RenderAmbientLight( const Colour & p_clColour)
{
	if (RenderSystem::UseShaders())
	{
		if (m_pCurrentProgram != NULL)
		{
			if (m_pCurrentProgram->GetType() == ShaderProgramBase::eHlslShader)
			{
				static_cast <Dx9ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
			else if (m_pCurrentProgram->GetType() == ShaderProgramBase::eCgShader)
			{
				static_cast <CgDx9ShaderProgram *>( m_pCurrentProgram)->SetAmbientLight( p_clColour);
			}
		}
	}
	else
	{
		CheckDxError( m_pDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( p_clColour.Red(), p_clColour.Green(), p_clColour.Blue(), p_clColour.Alpha())), CU_T( "Dx9RenderSystem :: RenderAmbientLight - SetRenderState"), false);
	}
}

void Dx9RenderSystem :: InitialiseDevice( HWND p_hWnd, D3DPRESENT_PARAMETERS * p_presentParameters)
{
	if (m_pDevice != NULL)
	{
		return;
	}

	if (m_pDirect3D != NULL)
	{
		if (CheckDxError( m_pDirect3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, p_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, p_presentParameters, & m_pDevice), CU_T( "Dx9RenderSystem :: InitialiseDevice - CreateDevice"), false))
		{
			CheckDxError( m_pDevice->SetRenderState( D3DRS_DITHERENABLE,     	TRUE),			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_DITHERENABLE"),		false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_LIGHTING,         	TRUE),			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_LIGHTING"),			false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_SPECULARENABLE,     	TRUE),			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_SPECULARENABLE"),	false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_ZENABLE,          	TRUE),			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_ZENABLE"),			false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_FOGENABLE,        	FALSE), 		CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_FOGENABLE "),		false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_STENCILMASK,      	0xFF), 			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_STENCILMASK"),		false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_STENCILWRITEMASK, 	0xFF), 			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_STENCILWRITEMASK"),	false);
			CheckDxError( m_pDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,	TRUE),			CU_T( "Dx9RenderSystem :: InitialiseDevice - SetRenderState - D3DRS_ALPHABLENDENABLE"), false);
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

HlslShaderProgramPtr Dx9RenderSystem :: _createHlslShaderProgram( const String & p_vertexShaderFile, 
																  const String & p_fragmentShaderFile,
																  const String & p_geometryShaderFile)
{
	return HlslShaderProgramPtr( new Dx9ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile));
}

HlslShaderObjectPtr Dx9RenderSystem :: _createHlslVertexShader()
{
	return HlslShaderObjectPtr( new Dx9VertexShader());
}

HlslShaderObjectPtr Dx9RenderSystem :: _createHlslFragmentShader()
{
	return HlslShaderObjectPtr( new Dx9FragmentShader());
}

HlslShaderObjectPtr Dx9RenderSystem :: _createHlslGeometryShader()
{
	CASTOR_EXCEPTION( "Direct3D 9 doesn't support Geometry shader");
}

CgShaderObjectPtr Dx9RenderSystem :: _createCgVertexShader()
{
	return CgShaderObjectPtr( new CgDx9VertexShader());
}

CgShaderObjectPtr Dx9RenderSystem :: _createCgFragmentShader()
{
	return CgShaderObjectPtr( new CgDx9FragmentShader());
}

CgShaderObjectPtr Dx9RenderSystem :: _createCgGeometryShader()
{
	return CgShaderObjectPtr( new CgDx9GeometryShader());
}

CgShaderProgramPtr Dx9RenderSystem :: _createCgShaderProgram( const String & p_vertexShaderFile, 
														    const String & p_fragmentShaderFile,
														    const String & p_geometryShaderFile)
{
	return CgShaderProgramPtr( new CgDx9ShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile));
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

VertexAttribsBufferBoolPtr Dx9RenderSystem :: _create1BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 1>( p_strArg);
}

VertexAttribsBufferIntPtr Dx9RenderSystem :: _create1IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 1>( p_strArg);
}

VertexAttribsBufferUIntPtr Dx9RenderSystem :: _create1UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 1>( p_strArg);
}

VertexAttribsBufferFloatPtr Dx9RenderSystem :: _create1FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 1>( p_strArg);
}

VertexAttribsBufferDoublePtr Dx9RenderSystem :: _create1DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 1>( p_strArg);
}

VertexAttribsBufferBoolPtr Dx9RenderSystem :: _create2BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 2>( p_strArg);
}

VertexAttribsBufferIntPtr Dx9RenderSystem :: _create2IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 2>( p_strArg);
}

VertexAttribsBufferUIntPtr Dx9RenderSystem :: _create2UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 2>( p_strArg);
}

VertexAttribsBufferFloatPtr Dx9RenderSystem :: _create2FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 2>( p_strArg);
}

VertexAttribsBufferDoublePtr Dx9RenderSystem :: _create2DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 2>( p_strArg);
}

VertexAttribsBufferBoolPtr Dx9RenderSystem :: _create3BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 3>( p_strArg);
}

VertexAttribsBufferIntPtr Dx9RenderSystem :: _create3IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 3>( p_strArg);
}

VertexAttribsBufferUIntPtr Dx9RenderSystem :: _create3UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 3>( p_strArg);
}

VertexAttribsBufferFloatPtr Dx9RenderSystem :: _create3FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 3>( p_strArg);
}

VertexAttribsBufferDoublePtr Dx9RenderSystem :: _create3DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 3>( p_strArg);
}

VertexAttribsBufferBoolPtr Dx9RenderSystem :: _create4BoolVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<bool, 4>( p_strArg);
}

VertexAttribsBufferIntPtr Dx9RenderSystem :: _create4IntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<int, 4>( p_strArg);
}

VertexAttribsBufferUIntPtr Dx9RenderSystem :: _create4UIntVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<unsigned int, 4>( p_strArg);
}

VertexAttribsBufferFloatPtr Dx9RenderSystem :: _create4FloatVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<float, 4>( p_strArg);
}

VertexAttribsBufferDoublePtr Dx9RenderSystem :: _create4DoubleVertexAttribsBuffer( const String & p_strArg)
{
	return _createAttribsBuffer<double, 4>( p_strArg);
}

SubmeshRendererPtr Dx9RenderSystem :: _createSubmeshRenderer()
{
	return SubmeshRendererPtr( new Dx9SubmeshRenderer( m_pSceneManager));
}

TextureEnvironmentRendererPtr Dx9RenderSystem :: _createTexEnvRenderer()
{
	return TextureEnvironmentRendererPtr( new Dx9TextureEnvironmentRenderer( m_pSceneManager));
}

TextureRendererPtr Dx9RenderSystem :: _createTextureRenderer()
{
	return TextureRendererPtr( new Dx9TextureRenderer( m_pSceneManager));
}

PassRendererPtr Dx9RenderSystem :: _createPassRenderer()
{
	return PassRendererPtr( new Dx9PassRenderer( m_pSceneManager));
}

CameraRendererPtr Dx9RenderSystem :: _createCameraRenderer()
{
	return CameraRendererPtr( new Dx9CameraRenderer( m_pSceneManager));
}

LightRendererPtr Dx9RenderSystem :: _createLightRenderer()
{
	return LightRendererPtr( new Dx9LightRenderer( m_pSceneManager));
}

WindowRendererPtr Dx9RenderSystem :: _createWindowRenderer()
{
	return WindowRendererPtr( new Dx9WindowRenderer( m_pSceneManager));
}

OverlayRendererPtr Dx9RenderSystem :: _createOverlayRenderer()
{
	return OverlayRendererPtr( new Dx9OverlayRenderer( m_pSceneManager));
}

IndexBufferPtr Dx9RenderSystem :: _createIndexBuffer()
{
	IndexBufferPtr l_pReturn;

	l_pReturn = BufferManager::CreateBuffer<unsigned int, Dx9IndexBuffer>();

	return l_pReturn;
}

VertexBufferPtr Dx9RenderSystem :: _createVertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements)
{
	VertexBufferPtr l_pReturn;

	l_pReturn = BufferManager::CreateVertexBuffer<Dx9VertexBuffer>( p_pElements, p_uiNbElements);

	return l_pReturn;
}

TextureBufferObjectPtr Dx9RenderSystem :: _createTextureBuffer()
{
	TextureBufferObjectPtr l_pReturn;
	return l_pReturn;
}
