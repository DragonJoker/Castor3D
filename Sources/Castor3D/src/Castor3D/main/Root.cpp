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

#include "Log.h"

using namespace Castor3D;
using namespace General::MultiThreading;

Root * Root :: sm_singleton = NULL;
RenderSystem * Root :: sm_renderSystem = NULL;
RecursiveMutex Root :: sm_mutex;

Root :: Root( unsigned int p_wantedFPS)
	:	m_ended					( false),
		m_wantedFPS				( p_wantedFPS),
		m_timeSinceLastFrame 	( 1.0f / m_wantedFPS),
		m_timeToWait			( m_timeSinceLastFrame),
		m_elapsedTime			( 0),
		m_windowsNumber			( 0),
		m_mainLoopCreated		( false)
{
	sm_singleton = this;

	MaterialManager::GetSingleton();
	MeshManager::GetSingleton();
	SceneManager::GetSingleton();
	AnimationManager::GetSingleton();
	ShaderManager::GetSingleton();

	m_timer.Time();
}

Root :: ~Root()
{
	if ( ! m_ended)
	{
		EndRendering();
	}

	m_ended = true;
	m_loadedPlugins.clear();
	delete sm_renderSystem;
	map::deleteAll( m_windows);
	m_windowsNumber = 0;
	m_windows.clear();
	sm_singleton = NULL;
}

RenderWindow * Root :: CreateRenderWindow( Scene * p_mainScene,void * p_handle,
										   int p_windowWidth, int p_windowHeight,
										   ProjectionType p_type,
										   ProjectionDirection p_look)
{
	_lock();
	RenderWindow * l_res = new RenderWindow( sm_renderSystem->CreateWindowRenderer(),
											 p_mainScene, p_handle, p_windowWidth,
											 p_windowHeight, p_type, p_look);
	m_windows[l_res->GetIndex()] = l_res;
	m_windowsNumber++;
	m_windowsBegin = m_windows.begin();
	m_windowsEnd = m_windows.end();
	_unlock();
	return l_res;
}

bool Root :: RemoveRenderWindow( size_t p_index)
{
	bool l_bReturn = false;
	_lock();

	if (map::deleteValue( m_windows, p_index))
	{
		m_windowsNumber--;
		m_windowsBegin = m_windows.begin();
		m_windowsEnd = m_windows.end();
		l_bReturn = true;
	}

	_unlock();
	return l_bReturn;
}

bool Root :: RemoveRenderWindow( RenderWindow * p_window)
{
	bool l_bReturn = false;

	if (p_window != NULL)
	{
		_lock();

		if (map::deleteValue( m_windows, p_window->GetIndex()))
		{
			m_windowsNumber--;
			m_windowsBegin = m_windows.begin();
			m_windowsEnd = m_windows.end();
			l_bReturn = true;
		}

		_unlock();
	}

	return l_bReturn;
}

void Root :: RemoveAllRenderWindows()
{
	_lock();

	map::deleteAll( m_windows);

	_unlock();
}

void Root :: LoadPlugin( const String & p_filePath)
{
	if (m_loadedPlugins.find( p_filePath) == m_loadedPlugins.end())
	{
		try
		{
			Plugin l_plugin( p_filePath);
			l_plugin.RegisterPlugin( * this);
			m_loadedPlugins.insert( PluginStrMap::value_type( p_filePath, l_plugin));
		}
		catch ( ... )
		{
			Log::LogMessage( "Root :: LoadPlugin - Fail");
		}
	}
}

void Root :: StartRendering()
{
	if (sm_renderSystem != NULL && ! m_mainLoopCreated)
	{
		m_mainLoop = GENLIB_THREAD_CREATE_MEMBER_FUNC_THREAD( Root, this, _mainLoop);

		if (m_mainLoop != NULL)
		{
			m_mainLoopCreated = true;
		}
	}
}

void Root :: EndRendering()
{
	if (m_mainLoopCreated)
	{
		MaterialManager::GetSingleton().DeleteAll();
		SceneManager::GetSingleton().ClearScenes();
		ShaderManager::GetSingleton().ClearShaders();
		BufferManager::GetSingleton().Cleanup();
		_renderOneFrame( true);
		m_ended = true;
		Sleep( 500);
		GENLIB_THREAD_WAIT_FOR_END_OF( m_mainLoop);
		GENLIB_THREAD_DELETE_THREAD( m_mainLoop);
		m_mainLoopCreated = false;
	}
}

void Root :: RenderOneFrame()
{
	if ( ! m_mainLoopCreated)
	{
		if (sm_renderSystem != NULL && _preUpdate())
		{
			_update( true);
			_postUpdate();
		}
	}
}

void Root :: _mainLoop()
{
	while ( ! m_ended)
	{
		_renderOneFrame( false);
	}
}

void Root :: _renderOneFrame( bool p_bForce)
{
	_lock();

	ShaderManager::GetSingleton().Update();

	if (_preUpdate())
	{
		_update( p_bForce);
		_postUpdate();
	}

	_unlock();

	Sleep( m_timeToWait > 0 ? static_cast <int>( m_timeToWait * 1000) : 10);
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
	AnimationManager::GetSingletonPtr()->Update();
	map::cycle( m_windows, & RenderWindow::RenderOneFrame, m_timeSinceLastFrame, p_bForce);
	m_timeToWait = m_timeSinceLastFrame - static_cast <float>( m_timer.Time());
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

void Root :: _lock()
{
	GENLIB_LOCK_MUTEX( sm_mutex);
}

void Root :: _unlock()
{
	GENLIB_UNLOCK_MUTEX( sm_mutex);
}
