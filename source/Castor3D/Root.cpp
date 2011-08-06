#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Root.hpp"
#include "Castor3D/Plugin.hpp"
#include "Castor3D/RenderWindow.hpp"
#include "Castor3D/FrameListener.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"
#include "Castor3D/Animation.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Mesh.hpp"
#include "Castor3D/Overlay.hpp"
#include "Castor3D/Light.hpp"
#include "Castor3D/SceneFileParser.hpp"
#include "Castor3D/ShaderManager.hpp"

using namespace Castor3D;
using namespace Castor::MultiThreading;

#if defined( _MSC_VER)
#	define GetTypeFunctionABIName		cuT( "?GetType@@YA?AW4ePLUGIN_TYPE@Castor3D@@XZ")
#elif defined( __GNUG__)
#	define GetTypeFunctionABIName		cuT( "_Z7GetTypev")
#endif

//*************************************************************************************************

Root::MaterialManager :: MaterialManager()
{
}

Root::MaterialManager :: ~MaterialManager()
{
}

void Root::MaterialManager :: Initialise()
{
	Collection<Material, String> l_mtlCollection;
	m_defaultMaterial = l_mtlCollection.GetElement( cuT( "DefaultMaterial"));
	if ( ! m_defaultMaterial)
	{
		m_defaultMaterial.reset( new Material( cuT( "DefaultMaterial"), 1));
		l_mtlCollection.AddElement( cuT( "DefaultMaterial"), m_defaultMaterial);
	}
	m_defaultMaterial->Initialise();
	m_defaultMaterial->GetPass( 0)->SetDoubleFace( true);
}

void Root::MaterialManager ::Cleanup()
{
	m_defaultMaterial.reset();
}

void Root::MaterialManager :: DeleteAll()
{
	Lock();
	Collection<Material, String>::const_iterator l_it = Begin();

	while (l_it != End())
	{
		m_arrayToDelete.push_back( l_it->second);
		l_it++;
	}

	Unlock();
	Clear();
}

void Root::MaterialManager :: Update()
{
	Lock();
	std::map<String, Material *>::iterator l_it = m_newMaterials.begin();

	while (l_it != m_newMaterials.end())
	{
		l_it->second->Initialise();
		++l_it;
	}

	m_newMaterials.clear();

	for (size_t i = 0 ; i < m_arrayToDelete.size() ; i++)
	{
		m_arrayToDelete[i].reset();
	}

	m_arrayToDelete.clear();
	Unlock();
}

void Root::MaterialManager :: GetNames( StringArray & l_names)
{
	Lock();
	l_names.clear();
	Collection<Material, String>::const_iterator l_it = Begin();

	while (l_it != End())
	{
		l_names.push_back( l_it->first);
		l_it++;
	}

	Unlock();
}

void Root::MaterialManager :: SetToInitialise( MaterialPtr p_material)
{
	if (p_material)
	{
		SetToInitialise( p_material.get());
	}
}

void Root::MaterialManager :: SetToInitialise( Material * p_material)
{
	Lock();

	if (p_material)
	{
		String l_strName = p_material->GetName();
		std::map<String, Material *>::iterator l_it = m_newMaterials.find( l_strName);

		if (l_it == m_newMaterials.end())
		{
			m_newMaterials.insert(std::map<String, Material *>::value_type( l_strName, p_material));
		}
	}

	Unlock();
}

bool Root::MaterialManager :: Write( File & p_file)const
{
	Lock();
	bool l_bReturn = true;
	Collection<Material, String>::const_iterator l_it = Begin();

	bool l_bFirst = true;

	while (l_bReturn && l_it != End())
	{
		if (l_bFirst)
		{
			l_bFirst = false;
		}
		else
		{
			p_file.WriteLine( "\n");
		}

		l_bReturn = Loader<Material>::Write( * l_it->second, p_file);
		++l_it;
	}

	Unlock();
	return l_bReturn;
}

bool Root::MaterialManager :: Read( File & p_file)
{
	SceneFileParser l_parser;
	return l_parser.ParseFile( p_file);
}

bool Root::MaterialManager :: Save( File & p_file)const
{
	Lock();
	bool l_bReturn = p_file.Write( GetElementCount()) == sizeof( size_t);
	Collection<Material, String>::const_iterator l_it = Begin();

	while (l_bReturn && l_it != End())
	{
		l_bReturn = Loader<Material>::Save( * l_it->second, p_file);
		++l_it;
	}

	Unlock();
	return l_bReturn;
}

bool Root::MaterialManager :: Load( File & p_file)
{
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	Collection<Material, String> l_mtlCollection;

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		MaterialPtr l_pMaterial = l_mtlCollection.GetElement( cuEmptyString);
		if ( ! l_pMaterial)
		{
			l_pMaterial.reset( new Material( cuEmptyString, 0));
			l_mtlCollection.AddElement( cuEmptyString, l_pMaterial);
		}
		l_bReturn = Loader<Material>::Load( * l_pMaterial, p_file);
	}

	return l_bReturn;
}

//*************************************************************************************************

RecursiveMutex	Root :: sm_mutex;
Root *			Root :: sm_singleton = NULL;

Root :: Root()
	:	m_itWindowsBegin		( m_mapWindows.begin()					)
	,	m_itWindowsEnd			( m_mapWindows.end()					)
	,	m_bEnded				( false									)
	,	m_pMainLoop				( NULL									)
	,	m_uiWantedFPS			( 1000									)
	,	m_rTimeSinceLastFrame 	( real( 1.0) / m_uiWantedFPS			)
	,	m_rTimeToWait			( m_rTimeSinceLastFrame					)
	,	m_rElapsedTime			( 0										)
	,	m_uiWindowsCount		( 0										)
	,	m_bMainLoopCreated		( false									)
	,	m_bThreaded				( false									)
	,	m_pAnimationManager		( new Collection<Animation,	String>()	)
	,	m_pMeshManager			( new Collection<Mesh,		String>()	)
	,	m_pOverlayManager		( new Collection<Overlay,	String>()	)
	,	m_pFontManager			( new Collection<Font,		String>()	)
	,	m_pImageManager			( new Collection<Image,		String>()	)
	,	m_pSceneManager			( new Collection<Scene,		String>()	)
	,	m_pShaderManager		( new ShaderManager()					)
	,	m_pBufferManager		( new BufferManager()					)
	,	m_pMaterialManager		( new MaterialManager()					)
{
	if (sm_singleton != NULL)
	{
		CASTOR_EXCEPTION( "No more than one instance of Root can be created");
	}

	sm_singleton = this;

	Mesh::Register();
	Overlay::Register();
	Light::Register();
}

Root :: ~Root()
{
	delete m_pAnimationManager;
	delete m_pMeshManager;
	delete m_pOverlayManager;
	delete m_pFontManager;
	delete m_pImageManager;
	delete m_pSceneManager;
	delete m_pShaderManager;
	delete m_pBufferManager;
	delete m_pMaterialManager;
}

void Root :: Initialise( unsigned int p_wantedFPS)
{
	CASTOR_TRACK;
	Version l_version;
	Logger::Log( eLOG_TYPE_MESSAGE) << cuT( "Castor3D - Core engine version : ") << l_version << cuT( "\n");
	std::locale::global( std::locale());
	m_timer.Time();
	m_uiWantedFPS = p_wantedFPS;
	m_rTimeSinceLastFrame = real( 1.0) / m_uiWantedFPS;
	m_rTimeToWait = m_rTimeSinceLastFrame;
}

void Root :: Clear()
{
	CASTOR_TRACK;
	if ( ! m_bEnded)
	{
		EndRendering();
	}

	m_bEnded = true;
	delete RenderSystem::GetSingletonPtr();
	m_uiWindowsCount = 0;
	m_mapWindows.clear();

	for (int i = 0 ; i < ePLUGIN_TYPE_COUNT ; i++)
	{
		m_librariesMap[i].clear();
		m_arrayLoadedPlugins[i].clear();
	}

	m_pAnimationManager->Clear();
	m_pFontManager->Clear();
	m_pImageManager->Clear();
	m_pShaderManager->Clear();
}

RenderWindowPtr Root :: CreateRenderWindow( ScenePtr p_mainScene, void * p_handle, int p_windowWidth, int p_windowHeight,
										   eVIEWPORT_TYPE p_type, ePIXEL_FORMAT p_pixelFormat, ePROJECTION_DIRECTION p_look)
{
	RenderWindowPtr l_res( new RenderWindow( this, p_mainScene, p_handle, p_windowWidth, p_windowHeight, p_type, p_pixelFormat, p_look));
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	m_mapWindows[l_res->GetIndex()] = l_res;
	m_uiWindowsCount++;
	m_itWindowsBegin = m_mapWindows.begin();
	m_itWindowsEnd = m_mapWindows.end();
	return l_res;
}

bool Root :: RemoveRenderWindow( size_t p_index)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	bool l_bReturn = false;
	RenderWindowPtr l_pWindow;

	if (map::eraseValue( m_mapWindows, p_index, l_pWindow))
	{
		l_pWindow.reset();
		m_uiWindowsCount--;
		m_itWindowsBegin = m_mapWindows.begin();
		m_itWindowsEnd = m_mapWindows.end();
		l_bReturn = true;
	}

	return l_bReturn;
}

bool Root :: RemoveRenderWindow( RenderWindowPtr p_window)
{
	bool l_bReturn = false;

	if (p_window)
	{
		CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
		RenderWindowPtr l_pWindow;

		if (map::eraseValue( m_mapWindows, p_window->GetIndex(), l_pWindow))
		{
			l_pWindow.reset();
			m_uiWindowsCount--;
			m_itWindowsBegin = m_mapWindows.begin();
			m_itWindowsEnd = m_mapWindows.end();
			l_bReturn = true;
		}
	}

	return l_bReturn;
}

void Root :: RemoveAllRenderWindows()
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);

//	map::deleteAll( m_windows);
	m_mapWindows.clear();
}

PluginPtr Root :: LoadPlugin( Path const & p_filePath, Path const & p_strOptPath)
{
	Path l_strFilePath = CASTOR_DLL_PREFIX + p_filePath + cuT( ".") + CASTOR_DLL_EXT;
	PluginPtr l_pReturn;

	try
	{
		l_pReturn = _loadPlugin( l_strFilePath);
	}
	catch( VersionException & p_exc)
	{
		l_pReturn.reset();
		Logger::LogError( cuT( "LoadPlugin - Fail - ") + p_exc.GetFullDescription());
	}
	catch( PluginException & p_exc)
	{
		try
		{
			l_pReturn = _loadPlugin( p_strOptPath / l_strFilePath);
		}
		catch( VersionException & p_exc)
		{
			l_pReturn.reset();
			Logger::LogError( cuT( "LoadPlugin - Fail - ") + p_exc.GetFullDescription());
		}
		catch(PluginException)
		{
			l_pReturn.reset();
			Logger::LogError( cuT( "LoadPlugin - Fail") + p_exc.GetFullDescription());
		}
	}

	return l_pReturn;
}

PluginPtr Root :: LoadPlugin( Path const & p_fileFullPath)
{
	PluginPtr l_pReturn;

	try
	{
		l_pReturn = _loadPlugin( p_fileFullPath);
	}
	catch ( ... )
	{
	}

	return l_pReturn;
}

bool Root :: LoadRenderer( eRENDERER_TYPE p_eType)
{
	bool l_bReturn = false;
	shared_ptr<RendererPlugin> l_pPlugin = static_pointer_cast<RendererPlugin, PluginBase>( m_arrayRenderers[p_eType]);

	if (l_pPlugin)
	{
		l_pPlugin->CreateRenderSystem();
//		StartRendering();
		l_bReturn = true;
	}

	return l_bReturn;
}

void Root :: StartRendering()
{
	CASTOR_TRACK;
	m_bThreaded = true;

	if ( ! m_bMainLoopCreated)
	{
		m_pMainLoop = MultiThreading::CreateThread( & Root::MainLoop, this);

		if (m_pMainLoop)
		{
			m_bMainLoopCreated = true;
		}
	}
}

void Root :: EndRendering()
{
	CASTOR_TRACK;
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);

	if (m_bMainLoopCreated || ! m_bThreaded)
	{
		m_pMaterialManager->DeleteAll();
		m_pOverlayManager->Clear();
		ClearScenes();
		m_pShaderManager->ClearShaders();
		m_pBufferManager->Clear();

		if (RenderSystem::GetSingletonPtr())
		{
			RenderSystem::GetSingletonPtr()->CleanupRenderers();
		}

		_renderOneFrame( true);
		m_bEnded = true;
		Sleep( 500);
		delete m_pMainLoop;
		m_bMainLoopCreated = false;
		m_pMaterialManager->Cleanup();
		m_pSceneManager->Clear();
		m_pMeshManager->Clear();
	}
}

void Root :: RenderOneFrame()
{
	CASTOR_TRACK;
	if ( ! m_bMainLoopCreated)
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
	m_frameListener.PostEvent( p_pEvent);
}

void Root :: FireEvents( eEVENT_TYPE p_eType)
{
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);
	m_frameListener.FireEvents( p_eType);
}

void Root :: LoadAllPlugins( Path const & p_strFolder)
{
	StringArray l_arrayFiles;
	File::ListDirectoryFiles( p_strFolder, l_arrayFiles);

	if (l_arrayFiles.size() > 0)
	{
		for (size_t i = 0 ; i < l_arrayFiles.size() ; i++)
		{
			Path l_file = l_arrayFiles[i];

			if (l_file.GetExtension() == CASTOR_DLL_EXT)
			{
				try
				{
					_loadPlugin( l_file);
				}
				catch ( ... )
				{
				}
			}
		}
	}
}

bool Root :: SaveMeshes( File & p_file)const
{
	m_pMeshManager->Lock();
	Path l_path = p_file.GetFileFullPath();
	bool l_bReturn = true;
	Collection<Mesh, String>::const_iterator l_it = m_pMeshManager->Begin();

	if (l_path.GetExtension() == "cmsh")
	{
		l_bReturn = p_file.Write( m_pMeshManager->GetElementCount()) == sizeof( size_t);

		while (l_it != m_pMeshManager->End() && l_bReturn)
		{
			l_bReturn = Loader<Mesh>::Save( * l_it->second, p_file);
			++l_it;
		}
	}
	else
	{
		l_path = l_path.GetPath() / l_path.GetFileName() + cuT( ".cmsh");
		File l_file( l_path, File::eOPEN_MODE_WRITE | File::eOPEN_MODE_BINARY);

		l_bReturn = l_file.Write( m_pMeshManager->GetElementCount()) == sizeof( size_t);

		while (l_it != m_pMeshManager->End() && l_bReturn)
		{
			l_bReturn = Loader<Mesh>::Save( * l_it->second, p_file);
			++l_it;
		}
	}

	m_pMeshManager->Unlock();
	return l_bReturn;
}

bool Root :: LoadMeshes( File & p_file)
{
	size_t l_uiSize;
	bool l_bReturn = p_file.Read( l_uiSize) == sizeof( size_t);
	Collection<Mesh, String> l_mshCollection;

	for (size_t i = 0 ; i < l_uiSize && l_bReturn ; i++)
	{
		MeshPtr l_pMesh = l_mshCollection.GetElement( cuEmptyString);
		if ( ! l_pMesh)
		{
			l_pMesh.reset( new Mesh( cuEmptyString, eMESH_TYPE_CUSTOM));
			l_mshCollection.AddElement( cuEmptyString, l_pMesh);
		}
		l_bReturn = Loader<Mesh>::Load( * l_pMesh, p_file);
	}

	return l_bReturn;
}

void Root :: ClearScenes()
{
	m_pSceneManager->Lock();

	for (Collection<Scene, String>::iterator l_it = m_pSceneManager->Begin() ; l_it != m_pSceneManager->End() ; ++l_it)
	{
		l_it->second->ClearScene();
	}

	m_pSceneManager->Unlock();
}

size_t Root :: MainLoop( void * p_pThis)
{
	CASTOR_TRACK;
	Root * l_pThis = (Root *)p_pThis;

	while ( ! l_pThis->m_bEnded)
	{
		l_pThis->_renderOneFrame( false);
		Sleep( l_pThis->m_rTimeToWait > 0 ? static_cast <int>( l_pThis->m_rTimeToWait * 1000) : 1);
	}

	return 0;
}

PluginPtr Root :: _loadPlugin( Path const & p_pathFile)
{
	PluginPtr l_pReturn;

	if (m_mapLoadedPluginTypes.find( p_pathFile) == m_mapLoadedPluginTypes.end())
	{
		if (File::FileExists( p_pathFile))
		{
			DynamicLibraryPtr l_pLibrary( new Utils::DynamicLibrary);

			if ( ! l_pLibrary->Open( p_pathFile))
			{
				String l_strError = String( cuT( "Error encountered while loading dll [")) + p_pathFile.c_str() + cuT( "] : ");
				l_strError << dlerror();
				Logger::LogMessage( l_strError);
				CASTOR_PLUGIN_EXCEPTION( cuT( "Loading plugin library"));
			}

			PluginBase::PGetTypeFunction l_pfnGetType = reinterpret_cast <PluginBase::PGetTypeFunction> ( l_pLibrary->GetFunction( GetTypeFunctionABIName));

			if (l_pfnGetType == NULL)
			{
				String l_strError = String( cuT( "Error encountered while loading dll [")) + p_pathFile.GetFileName() + cuT( "] GetType plugin function => Not a Castor3D plugin");
				Logger::LogMessage( l_strError);
				CASTOR_PLUGIN_EXCEPTION( cuT( "Retrieving GetType function"));
			}

			ePLUGIN_TYPE l_eType = l_pfnGetType();

			switch( l_eType)
			{
			case ePLUGIN_TYPE_DIVIDER:
				l_pReturn.reset( new DividerPlugin( l_pLibrary));
				break;

			case ePLUGIN_TYPE_IMPORTER:
				l_pReturn.reset( new ImporterPlugin( l_pLibrary));
				break;

			case ePLUGIN_TYPE_RENDERER:
				{
					RendererPluginPtr l_pRenderer( new RendererPlugin( l_pLibrary));
					l_pReturn = static_pointer_cast<PluginBase, RendererPlugin>( l_pRenderer);
					eRENDERER_TYPE l_eRendererType = l_pRenderer->GetRendererType();

					if (l_eRendererType == eRENDERER_TYPE_UNDEFINED)
					{
						l_pReturn.reset();
					}
					else
					{
						m_arrayRenderers[l_eRendererType] = l_pRenderer;
					}
				}
				break;

			default:
				break;
			}

			if (l_pReturn)
			{
				Version l_toCheck( 0, 0);
				l_pReturn->GetRequiredVersion( l_toCheck);
				Logger::Log( eLOG_TYPE_MESSAGE) << cuT( "LoadPlugin - Plugin [") << l_pReturn->GetName() << cuT( "] - Required engine version : ") << l_toCheck.m_iMajor << cuT( ".") << l_toCheck.m_iAnnex << cuT( ".") << l_toCheck.m_iCorrect << cuT( ".") << l_toCheck.m_iPoint << cuT( "\n");

				if (l_toCheck <= m_version)
				{
					m_mapLoadedPluginTypes.insert( std::pair<String, ePLUGIN_TYPE>( p_pathFile, l_eType));
					m_arrayLoadedPlugins[l_eType].insert( PluginStrMap::value_type( p_pathFile, l_pReturn));
					m_librariesMap[l_eType].insert( std::pair<Path, DynamicLibraryPtr>( p_pathFile, l_pLibrary));
					Logger::LogMessage( cuT( "LoadPlugin - Plugin [") + l_pReturn->GetName() + cuT( "] - Loaded"));
				}
				else
				{
					CASTOR_VERSION_EXCEPTION( l_toCheck, m_version);
				}
			}
			else
			{
				// Plugin not loaded, due to an error
			}
		}
		else
		{
			// File doesn't exist
		}
	}
	else
	{
		ePLUGIN_TYPE l_eType = m_mapLoadedPluginTypes.find( p_pathFile)->second;
		l_pReturn = m_arrayLoadedPlugins[l_eType].find( p_pathFile)->second;
	}

	return l_pReturn;
}

void Root :: _renderOneFrame( bool p_bForce)
{
	CASTOR_TRACK;
	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( sm_mutex);

	if (_preUpdate())
	{
		_update( p_bForce);
		_postUpdate();
	}
}

bool Root :: _preUpdate()
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	RenderWindowMap::iterator l_it = m_itWindowsBegin;

	for ( ; l_it != m_itWindowsEnd && l_bReturn ; ++ l_it)
	{
		l_bReturn = l_it->second->PreRender();
	}

	m_frameListener.FireEvents( eEVENT_TYPE_PRE_RENDER);

	return l_bReturn;
}

void Root :: _update( bool p_bForce)
{
	CASTOR_TRACK;
	for (RenderWindowMap::iterator l_it = m_itWindowsBegin ; l_it != m_itWindowsEnd ; ++l_it)
	{
		l_it->second->RenderOneFrame( m_rTimeSinceLastFrame, p_bForce);
	}

	m_rTimeToWait = m_rTimeSinceLastFrame - static_cast <real>( m_timer.Time());
}

bool Root :: _postUpdate()
{
	CASTOR_TRACK;
	bool l_bReturn = true;
	RenderWindowMap::iterator i = m_itWindowsBegin;

	for ( ; i != m_itWindowsEnd && l_bReturn ; ++ i)
	{
		l_bReturn = i->second->PostRender();
	}

	m_frameListener.FireEvents( eEVENT_TYPE_POST_RENDER);

	return l_bReturn;
}
