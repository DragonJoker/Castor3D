/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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

#include "../Prerequisites.h"
#include "../camera/Viewport.h"
#include "Plugin.h"
#include "RendererServer.h"
#include "FrameListener.h"
#include "Version.h"

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
	private:
		static Root * sm_singleton;
		static Castor::MultiThreading::RecursiveMutex sm_mutex;	//!< The mutex, to make the Root threadsafe

	private:
	    PluginStrMap						m_loadedPlugins;		//!< Loaded plugins
		RendererServer					*	m_rendererServer;		//!< The renderers (OpenGL or Direct3D)

		RenderWindowMap						m_windows;				//!< The render windows
		RenderWindowMap::iterator			m_windowsBegin;
		RenderWindowMap::iterator			m_windowsEnd;
		Castor::Utils::PreciseTimer			m_timer;				//!< The timer, to compute times (especially time since last frame)
		bool								m_ended;				//!< Tells if render has ended, by any reason
		Castor::MultiThreading::Thread	*	m_mainLoop;				//!< The threaded render loop
		unsigned int						m_wantedFPS;			//!< The wanted frames per second, the root tries to respect it as much as possible
		real								m_timeSinceLastFrame;	//!< The time since the last frame was rendered
		real 								m_timeToWait;			//!< The time to wait before the next frame render (if we have to wait, to respect wanted fps)
		real 								m_elapsedTime;			//!< Elapsed time since the launch of the root
		unsigned int						m_windowsNumber;		//!< The number of render windows
		bool								m_mainLoopCreated;
		Version 							m_version;
		FrameListenerPtr					m_pFrameListener;
		SceneManager					*	m_pSceneManager;

	public:
		/**
		 * Constructor
		 *@param p_wantedTBEF : [in] The wanted time between each frame
		 */
		Root( unsigned int p_wantedTBEF = 1000);
		/**
		 * Destructor
		 */
		~Root();
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
										   Viewport::eTYPE p_type, Castor::Resources::ePIXEL_FORMAT p_pixelFormat=Castor::Resources::eA8R8G8B8,
										   ePROJECTION_DIRECTION p_look = pdLookToFront);
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
		PluginPtr LoadPlugin( PluginBase::eTYPE p_eType, const Path & p_filePath, const Path & p_strOptPath);
		/**
		 * Loads a plugin, given the file's path
		 *@param p_eType : [in] The renderer type
		 *@param p_strOptPath : [in] Optional path where the plugin may be looked for
		 */
		bool LoadRenderer( RendererDriver::eDRIVER_TYPE p_eType, const String & p_strOptPath=C3DEmptyString);
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
		void FireEvents( FrameEvent::eTYPE p_eType);

		static size_t MainLoop( void * p_pThis);

	private:
		void _renderOneFrame( bool p_bForce);
		bool _preUpdate();
		void _update( bool p_bForce);
		bool _postUpdate();
		void _lock();
		void _unlock();

	public:
		/**@name Accessors */
		//@{
		/**
		 *@return The root singleton
		 */
		static inline Root			*	GetSingletonPtr		()		{ return sm_singleton; }
		/**
		 *@return The renderer server
		 */
		inline RendererServer		&	GetRendererServer	()		{ return * m_rendererServer; }
		inline SceneManager			* 	GetSceneManager		()const { return m_pSceneManager; }
		//@}
	};
}

#endif

