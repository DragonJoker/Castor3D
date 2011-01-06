#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/main/Root.h"
#include "Castor3D/main/RenderWindow.h"
#include "Castor3D/main/FrameListener.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/animation/Animation.h"
#include "Castor3D/animation/AnimationManager.h"
#include "Castor3D/scene/Scene.h"
#include "Castor3D/scene/SceneManager.h"
#include "Castor3D/material/Material.h"
#include "Castor3D/material/MaterialManager.h"
#include "Castor3D/geometry/mesh/Mesh.h"
#include "Castor3D/geometry/mesh/MeshManager.h"
#include "Castor3D/shader/ShaderManager.h"

using namespace Castor3D;
using namespace Castor::MultiThreading;

Root * Root :: sm_singleton = NULL;
RecursiveMutex Root :: sm_mutex;

Root :: Root( unsigned int p_wantedFPS)
	:	m_ended					( false)
	,	m_wantedFPS				( p_wantedFPS)
	,	m_timeSinceLastFrame 	( 1.0f / m_wantedFPS)
	,	m_timeToWait			( m_timeSinceLastFrame)
	,	m_elapsedTime			( 0)
	,	m_windowsNumber			( 0)
	,	m_mainLoopCreated		( false)
	,	m_rendererServer		( new RendererServer)
	,	m_pFrameListener		( new FrameListener)
	,	m_pSceneManager			( new SceneManager)
{
	sm_singleton = this;

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
										   PixelFormat p_pixelFormat,
										   ePROJECTION_DIRECTION p_look)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	RenderWindowPtr l_res( new RenderWindow( this, p_mainScene, p_handle, p_windowWidth,
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

	if ( ! p_window == NULL)
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

PluginPtr Root :: LoadPlugin( PluginBase::eTYPE p_eType, const Path & p_filePath, const Path & p_strOptPath)
{
	PluginPtr l_pReturn;

	if (m_loadedPlugins.find( p_filePath) == m_loadedPlugins.end())
	{
		try
		{
			switch (p_eType)
			{
			case PluginBase::ePluginRenderer:
				l_pReturn.reset( new RendererPlugin( p_filePath));
				break;

			case PluginBase::ePluginImporter:
				l_pReturn.reset( new ImporterPlugin( p_filePath));
				break;

			case PluginBase::ePluginDivider:
				l_pReturn.reset( new DividerPlugin( p_filePath));
				break;
			}
		}
		catch ( ... )
		{
			l_pReturn.reset();

			if ( ! p_strOptPath.empty())
			{ 
				try
				{
					switch (p_eType)
					{
					case PluginBase::ePluginRenderer:
						l_pReturn.reset( new RendererPlugin( p_strOptPath));
						break;

					case PluginBase::ePluginImporter:
						l_pReturn.reset( new ImporterPlugin( p_strOptPath));
						break;

					case PluginBase::ePluginDivider:
						l_pReturn.reset( new DividerPlugin( p_strOptPath));
						break;
					}
				}
				catch ( ... )
				{
					l_pReturn.reset();
					Logger::LogError( CU_T( "Root :: LoadPlugin - Fail"));
				}
			}
		}

		if (l_pReturn != NULL)
		{
			Version l_toCheck( 0, 0);
			l_pReturn->GetRequiredVersion( l_toCheck);

			if (l_toCheck <= m_version)
			{
				m_loadedPlugins.insert( PluginStrMap::value_type( p_filePath, l_pReturn));

				if (p_eType == PluginBase::ePluginRenderer)
				{
					static_pointer_cast<RendererPlugin>( l_pReturn)->RegisterPlugin( * this);
				}
			}
			else
			{
				CASTOR_VERSION_EXCEPTION( l_toCheck, m_version);
			}
		}
	}
	else
	{
		l_pReturn = m_loadedPlugins.find( p_filePath)->second;
	}

	return l_pReturn;
}

bool Root :: LoadRenderer( RendererDriver::eDRIVER_TYPE p_eType, const String & p_strOptPath)
{
	bool l_bReturn = false;

	l_bReturn = LoadPlugin( PluginBase::ePluginRenderer, GetRendererServer().GetRendererName( p_eType) + ".dll", p_strOptPath) != NULL;

	if (l_bReturn)
	{
		l_bReturn = false;
		RendererDriverPtr l_pRenderer = GetRendererServer().GetRenderer( p_eType);

		if ( ! l_pRenderer == NULL)
		{
			l_pRenderer->CreateRenderSystem( m_pSceneManager);
			StartRendering();
			l_bReturn = true;
		}
	}

	return l_bReturn;
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
		m_pSceneManager->GetMaterialManager()->DeleteAll();
		m_pSceneManager->ClearScenes();
		m_pSceneManager->GetMaterialManager()->GetShaderManager()->ClearShaders();
		BufferManager::Cleanup();
		RenderSystem::GetSingletonPtr()->CleanupRenderers();
		_renderOneFrame( true);
		m_ended = true;
		Sleep( 500);
		delete m_mainLoop;
		m_mainLoopCreated = false;
		m_pSceneManager->GetMaterialManager()->Clear();
		m_pSceneManager->Clear();
		m_pSceneManager->GetMeshManager()->Clear();
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

void Root :: PostEvent( FrameEventPtr p_pEvent)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	m_pFrameListener->PostEvent( p_pEvent);
}

void Root :: FireEvents( FrameEvent::eTYPE p_eType)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	m_pFrameListener->FireEvents( p_eType);
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

	m_pFrameListener->FireEvents( FrameEvent::ePreRender);

	return l_bReturn;
}

void Root :: _update( bool p_bForce)
{
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

	m_pFrameListener->FireEvents( FrameEvent::ePostRender);

	return l_bReturn;
}