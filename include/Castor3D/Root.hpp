/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_Root___
#define ___C3D_Root___

#include "Prerequisites.hpp"
#include "Viewport.hpp"
#include "Plugin.hpp"
#include "FrameListener.hpp"
#include "Version.hpp"

#include <CastorUtils/Font.hpp>
#include <CastorUtils/Image.hpp>

#ifdef _WIN32
#	ifndef __GNUC__
#		define CASTOR_DLL_PREFIX cuT( "")
#	else
#		define CASTOR_DLL_PREFIX cuT( "lib")
#	endif
#	define CASTOR_DLL_EXT cuT( "dll")
#	define dlerror() GetLastError()
#else
#	define CASTOR_DLL_EXT cuT( "so")
#	define CASTOR_DLL_PREFIX cuT( "lib")
#endif

namespace Castor3D
{
	//! Main system
	/*!
	Holds the render windows, the plugins, the render drivers...
	It is a singleton, so only one can be launched at a time
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Root : public Castor::Theory::NonCopyable, public MemoryTraced<Root>
	{
	public:
		typedef array< RendererPluginPtr, eRENDERER_TYPE_COUNT >			RendererPtrArray;
		typedef array< PluginStrMap, ePLUGIN_TYPE_COUNT >				PluginStrMapArray;
		typedef std::map< Path, Utils::DynamicLibraryPtr >			DynamicLibraryPtrPathMap;
		typedef array< DynamicLibraryPtrPathMap, ePLUGIN_TYPE_COUNT >	DynamicLibraryPtrPathMapArray;

		class C3D_API MaterialManager : public Collection<Material, String>
		{
		private:
			MaterialPtr							m_defaultMaterial;		//!< The default material
			std::map <String,		Material *>	m_newMaterials;			//!< The newly created materials, a material is in the list until it is initialised
			MaterialPtrArray					m_arrayToDelete;

		public:
			MaterialManager();
			virtual ~MaterialManager();
			/**
			 * Materials initialisation function, intialises the default material, a renderer must have been loaded
			 */
			void Initialise();
			/**
			 * 
			 */
			void Cleanup();
			/**
			 * Deletes all the materials held
			 */
			void DeleteAll();
			/**
			 * Initialises all the newly created materials
			 */
			void Update();
			/**
			 * Puts all the materials names in the given array
			 *@param p_names : [in/out] The array of names to be filled
			 */
			void GetNames( StringArray & p_names);
			/**
			 * Puts the given material in the newly created materials, to re-initialise it
			 *@param p_material : [in] The material we want to initialise again
			 */
			void SetToInitialise( MaterialPtr p_material);
			/**
			 * Puts the given material in the newly created materials, to re-initialise it
			 *@param p_material : [in] The material we want to initialise again
			 */
			void SetToInitialise( Material * p_material);
			/**
			 * Writes all materials in a text file
			 */
			virtual bool Write( File & p_file)const;
			/**
			 * Read materials from a text file
			 */
			virtual bool Read( File & p_file);
			/**
			 * Saves all materials in a binary file
			 */
			virtual bool Save( File & p_file)const;
			/**
			 * Loads materials from a binary file
			 */
			virtual bool Load( File & p_file);

			inline MaterialPtr							GetDefaultMaterial	()const							{ return m_defaultMaterial; }
		};

	private:
		static Castor::MultiThreading::RecursiveMutex		sm_mutex;				//!< The mutex, to make the Root threadsafe
		static Root										*	sm_singleton;

	private:
		DynamicLibraryPtrPathMapArray						m_librariesMap;
		PluginStrMapArray									m_arrayLoadedPlugins;
		std::map<Path, ePLUGIN_TYPE>						m_mapLoadedPluginTypes;
		RendererPtrArray									m_arrayRenderers;

		RenderWindowMap										m_mapWindows;			//!< The render windows
		RenderWindowMap::iterator							m_itWindowsBegin;
		RenderWindowMap::iterator							m_itWindowsEnd;
		Castor::Utils::PreciseTimer							m_timer;				//!< The timer, to compute times (especially time since last frame)
		bool												m_bEnded;				//!< Tells if render has ended, by any reason
		Castor::MultiThreading::Thread					*	m_pMainLoop;			//!< The threaded render loop
		unsigned int										m_uiWantedFPS;			//!< The wanted frames per second, the root tries to respect it as much as possible
		real												m_rTimeSinceLastFrame;	//!< The time since the last frame was rendered
		real 												m_rTimeToWait;			//!< The time to wait before the next frame render (if we have to wait, to respect wanted fps)
		real 												m_rElapsedTime;			//!< Elapsed time since the launch of the root
		unsigned int										m_uiWindowsCount;		//!< The number of render windows
		bool												m_bMainLoopCreated;
		Version 											m_version;
		FrameListener										m_frameListener;
		Collection<Animation,			String>			*	m_pAnimationManager;
		Collection<Mesh,				String>			*	m_pMeshManager;
		Collection<Overlay,				String>			*	m_pOverlayManager;
		Collection<Resources::Font,		String>			*	m_pFontManager;
		Collection<Resources::Image,	String>			*	m_pImageManager;
		Collection<Scene,				String>			*	m_pSceneManager;
		ShaderManager									*	m_pShaderManager;
		BufferManager									*	m_pBufferManager;
		MaterialManager									*	m_pMaterialManager;
		bool												m_bThreaded;

	public:
		/**
		 * Constructor
		 */
		Root();
		/**
		 * Destructor
		 */
		~Root();
		/**
		 * Initialisation function, sets the wanted frame rate
		 *@param p_wantedTBEF : [in] The wanted time between each frame
		 */
		void Initialise( unsigned int p_wantedTBEF = 1000);
		/**
		 * Cleanup function, to call before you destroy the last instance of the Root
		 */
		void Clear();
		/**
		 * Creates a render window, in a display window given by it's handle
		 *@param p_mainScene : [in] An externally created scene
		 *@param p_handle : [in] The display window handle
		 *@param p_windowWidth : [in] The display window width
		 *@param p_windowHeight : [in] The display window height
		 *@param p_type : [in] The projection type of the render window's camera's viewport
		 *@param p_pixelFormat : [in] The window's pixel format
		 *@param p_look : [in] Where the render window's camera must look
		 */
		RenderWindowPtr CreateRenderWindow( ScenePtr p_mainScene, void * p_handle, int p_windowWidth, int p_windowHeight,
										   eVIEWPORT_TYPE p_type, Castor::Resources::ePIXEL_FORMAT p_pixelFormat=Castor::Resources::ePIXEL_FORMAT_A8R8G8B8,
										   ePROJECTION_DIRECTION p_look = ePROJECTION_DIRECTION_FRONT);
		/**
		 * Removes a render window, by index
		 *@param p_index : [in] The window index
		 */
		bool RemoveRenderWindow( size_t p_index);
		/**
		 * Removes a render window, by pointer
		 *@param p_window : [in] The render window pointer
		 */
		bool RemoveRenderWindow( RenderWindowPtr p_window);
		/**
		 * Removes all render windows
		 */
		void RemoveAllRenderWindows();
		/**
		 * Loads a plugin, given the file's path
		 *@param p_filePath : [in] The plugin file path
		 *@param p_strOptPath : [in] Optional path where the plugin may be looked for
		 */
		PluginPtr LoadPlugin( Path const & p_filePath, Path const & p_strOptPath);
		/**
		 * Loads a plugin, given the file's path
		 *@param p_filePath : [in] The plugin full file path
		 */
		PluginPtr LoadPlugin( Path const & p_fileFullPath);
		/**
		 * Loads a plugin, given the file's path
		 *@param p_eType : [in] The renderer type
		 *@param p_strOptPath : [in] Optional path where the plugin may be looked for
		 */
		bool LoadRenderer( eRENDERER_TYPE p_eType);
		/**
		 * Starts the render loop
		 */
		void StartRendering();
		/**
		 * Ends the main loop if it has been created
		 */
		void EndRendering();
		/**
		 * Renders one frame, only if not in render loop
		 */
		void RenderOneFrame();
		/**
		 * Posts a frame event
		 *@param p_pEvent : [in] The event to add
		 */
		void PostEvent( FrameEventPtr p_pEvent);
		/**
		 * Fire all events of given type
		 *@param p_eType : [in] type of events to fire
		 */
		void FireEvents( eEVENT_TYPE p_eType);
		/**
		 * Loads all the plugins located in working folder
		 */
		void LoadAllPlugins( Path const & p_strFolder);
		virtual bool SaveMeshes( File & p_file)const;
		virtual bool LoadMeshes( File & p_file);

		void ClearScenes();

		static size_t	MainLoop( void * p_pThis);
		static Root *	GetSingleton() { return sm_singleton; }

	private:
		void _renderOneFrame( bool p_bForce);
		bool _preUpdate();
		void _update( bool p_bForce);
		bool _postUpdate();
		void _lock();
		void _unlock();
		PluginPtr _loadPlugin( Path const & p_pathFile);

	public:
		/**@name Accessors */
		//@{
		inline PluginStrMap::iterator				PluginsBegin		( ePLUGIN_TYPE p_eType)			{ return m_arrayLoadedPlugins[p_eType].begin(); }
		inline PluginStrMap::const_iterator			PluginsBegin		( ePLUGIN_TYPE p_eType)const	{ return m_arrayLoadedPlugins[p_eType].begin(); }
		inline PluginStrMap::const_iterator			PluginsEnd			( ePLUGIN_TYPE p_eType)const	{ return m_arrayLoadedPlugins[p_eType].end(); }
		inline RendererPtrArray::iterator			RenderersBegin		()								{ return m_arrayRenderers.begin(); }
		inline RendererPtrArray::const_iterator		RenderersBegin		()const							{ return m_arrayRenderers.begin(); }
		inline RendererPtrArray::const_iterator		RenderersEnd		()const							{ return m_arrayRenderers.end(); }
		inline Root::MaterialManager *				GetMaterialManager	()const							{ return m_pMaterialManager; }
		inline BufferManager *						GetBufferManager	()const							{ return m_pBufferManager; }
		//@}
	};
}

#endif

