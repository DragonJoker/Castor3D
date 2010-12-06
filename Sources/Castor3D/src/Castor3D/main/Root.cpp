#include "PrecompiledHeader.h"

#include "main/Module_Main.h"

#include "main/Root.h"
#include "main/RenderWindow.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"
#include "animation/Module_Animation.h"
#include "animation/Animation.h"
#include "animation/AnimationManager.h"
#include "scene/Scene.h"
#include "scene/SceneManager.h"
#include "material/Material.h"
#include "material/MaterialManager.h"
#include "geometry/mesh/Mesh.h"
#include "geometry/mesh/MeshManager.h"
#include "shader/ShaderManager.h"

using namespace Castor3D;
using namespace Castor::MultiThreading;

Root * Root :: sm_singleton = NULL;
RecursiveMutex Root :: sm_mutex;

Root :: Root( unsigned int p_wantedFPS)
	:	m_ended					( false),
		m_wantedFPS				( p_wantedFPS),
		m_timeSinceLastFrame 	( 1.0f / m_wantedFPS),
		m_timeToWait			( m_timeSinceLastFrame),
		m_elapsedTime			( 0),
		m_windowsNumber			( 0),
		m_mainLoopCreated		( false),
		m_rendererServer		( new RendererServer)
{
	sm_singleton = this;

	MaterialManager::GetSingleton();
	MeshManager::GetSingleton();
	SceneManager::GetSingleton();
	AnimationManager::GetSingleton();
	ShaderManager::GetSingleton();
	m_windowsBegin = m_windows.begin();
	m_windowsEnd = m_windows.end();

	m_timer.Time();
}

Root :: ~Root()
{
	if ( ! m_ended)
	{
		EndRendering();
	}

	m_ended = true;
	delete RenderSystem::GetSingletonPtr();
	m_windowsNumber = 0;
	m_windows.clear();
	delete m_rendererServer;
	m_loadedPlugins.clear();
	sm_singleton = NULL;
}

RenderWindowPtr Root :: CreateRenderWindow( ScenePtr p_mainScene,void * p_handle,
										   int p_windowWidth, int p_windowHeight,
										   Viewport::eTYPE p_type,
										   Castor::Resource::PixelFormat p_pixelFormat,
										   ProjectionDirection p_look)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	RenderWindowPtr l_res( new RenderWindow( p_mainScene, p_handle, p_windowWidth,
											 p_windowHeight, p_type, p_pixelFormat, p_look));
	m_windows[l_res->GetIndex()] = l_res;
	m_windowsNumber++;
	m_windowsBegin = m_windows.begin();
	m_windowsEnd = m_windows.end();
	return l_res;
}

bool Root :: RemoveRenderWindow( size_t p_index)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	bool l_bReturn = false;
	RenderWindowPtr l_pWindow;

	if (map::eraseValue( m_windows, p_index, l_pWindow))
	{
		l_pWindow.reset();
		m_windowsNumber--;
		m_windowsBegin = m_windows.begin();
		m_windowsEnd = m_windows.end();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Root :: RemoveRenderWindow( RenderWindowPtr p_window)
{
	bool l_bReturn = false;

	if ( ! p_window.null())
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
		RenderWindowPtr l_pWindow;

		if (map::eraseValue( m_windows, p_window->GetIndex(), l_pWindow))
		{
			l_pWindow.reset();
			m_windowsNumber--;
			m_windowsBegin = m_windows.begin();
			m_windowsEnd = m_windows.end();
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

void Root :: RemoveAllRenderWindows()
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);

//	map::deleteAll( m_windows);
	m_windows.clear();
}

void Root :: LoadPlugin( const String & p_filePath)
{
	if (m_loadedPlugins.find( p_filePath) == m_loadedPlugins.end())
	{
		try
		{
			PluginPtr l_plugin( new Plugin( p_filePath));
			l_plugin->RegisterPlugin( * this);
			m_loadedPlugins.insert( PluginStrMap::value_type( p_filePath, l_plugin));
		}
		catch ( ... )
		{
			Logger::LogError( CU_T( "Root :: LoadPlugin - Fail"));
		}
	}
}

void Root :: LoadRenderer( RendererDriver::eDRIVER_TYPE p_eType)
{
#ifdef _DEBUG
#	if CASTOR_UNICODE
			LoadPlugin( GetRendererServer().GetRendererName( p_eType) + "du.dll");
#	else
			LoadPlugin( GetRendererServer().GetRendererName( p_eType) + "d.dll");
#	endif
#else
#	if C3D_UNICODE
			LoadPlugin( GetRendererServer().GetRendererName( p_eType) + "u.dll");
#	else
			LoadPlugin( GetRendererServer().GetRendererName( p_eType) + ".dll");
#	endif
#endif

	RendererDriverPtr l_pRenderer = GetRendererServer().GetRenderer( p_eType);

	if ( ! l_pRenderer.null())
	{
		l_pRenderer->CreateRenderSystem();
		StartRendering();
	}
}

void Root :: StartRendering()
{
	if ( ! m_mainLoopCreated)
	{
		m_mainLoop = MultiThreading::CreateThread( & Root::MainLoop, this);

		if (m_mainLoop != NULL)
		{
			m_mainLoop->ResumeThread();
			m_mainLoopCreated = true;
		}
	}
}

void Root :: EndRendering()
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);

	if (m_mainLoopCreated)
	{
		MaterialManager::DeleteAll();
		SceneManager::ClearScenes();
		ShaderManager::ClearShaders();
		BufferManager::Cleanup();
		RenderSystem::GetSingletonPtr()->CleanupRenderers();
		_renderOneFrame( true);
		m_ended = true;
		Sleep( 500);
		delete m_mainLoop;
		m_mainLoopCreated = false;
		MaterialManager::Clear();
		SceneManager::Clear();
		MeshManager::Clear();
		AnimationManager::Clear();
	}
}

void Root :: RenderOneFrame()
{
	if ( ! m_mainLoopCreated)
	{
		if (_preUpdate())
		{
			_update( true);
			_postUpdate();
		}
	}
}

size_t Root :: MainLoop( void * p_pThis)
{
	Root * l_pThis = (Root *)p_pThis;

	while ( ! l_pThis->m_ended)
	{
		l_pThis->_renderOneFrame( false);
		Sleep( l_pThis->m_timeToWait > 0 ? static_cast <int>( l_pThis->m_timeToWait * 1000) : 10);
	}

	return 0;
}

void Root :: _renderOneFrame( bool p_bForce)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);

	if (_preUpdate())
	{
		_update( p_bForce);
		_postUpdate();
	}
}

bool Root :: _preUpdate()
{
	bool l_bReturn = true;
	RenderWindowMap::iterator l_it = m_windowsBegin;

	for ( ; l_it != m_windowsEnd && l_bReturn ; ++ l_it)
	{
		l_bReturn = l_it->second->PreRender();
	}

	return l_bReturn;
}

void Root :: _update( bool p_bForce)
{
	AnimationManager::Update();

	for (RenderWindowMap::iterator l_it = m_windowsBegin ; l_it != m_windowsEnd ; ++l_it)
	{
		l_it->second->RenderOneFrame( m_timeSinceLastFrame, p_bForce);
	}

	m_timeToWait = m_timeSinceLastFrame - static_cast <real>( m_timer.Time());
}

bool Root :: _postUpdate()
{
	bool l_bReturn = true;
	RenderWindowMap::iterator i = m_windowsBegin;

	for ( ; i != m_windowsEnd && l_bReturn ; ++ i)
	{
		l_bReturn = i->second->PostRender();
	}

	return l_bReturn;
}