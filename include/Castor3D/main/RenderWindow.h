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
#ifndef ___C3D_RenderWindow___
#define ___C3D_RenderWindow___

#include "../Prerequisites.h"
#include "../render_system/Renderable.h"
#include "../camera/Viewport.h"

namespace Castor3D
{
	//! Render window representation
	/*!
	Manages a window wher you can render a scene.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API RenderWindow : public Renderable<RenderWindow, WindowRenderer>
	{
	private:
		static size_t s_nbRenderWindows;		//!< Number of render windows

	private:
		size_t m_index;							//!< This window's index

		Viewport::eTYPE m_type;					//!< The projection type
		ePROJECTION_DIRECTION m_lookAt;			//!< The projection direction
		void * m_hWnd;							//!< The handle of the display window
		eDRAW_TYPE m_drawType;					//!< The type of drawing
		bool m_showNormals;						//!< Tells if we show normals
		eNORMALS_MODE m_normalsMode;			//!< Normals mode

		unsigned int m_timeSinceLastFrame;		//!< The time since the last frame was rendered

		bool m_initialised;						//!< Tells if the window is initalised
		bool m_focused;							//!< Tells if the window is focused by the mouse
		bool m_changed;							//!< Tells if the window has changed and then buffers must be regenerated
		bool m_toUpdate;						//!< Tells if the window must be updated

		unsigned long long m_nbFrame;			//!< The frame count

		int m_windowWidth;						//!< The display window width
		int m_windowHeight;						//!< The display window height

		ScenePtr m_mainScene;					//!< The main scene
		CameraPtr m_camera;						//!< The camera associated to the window

		FrameListenerPtr m_listener;
		Castor::Resources::PixelFormat m_pixelFormat;
		Root * m_pRoot;

	public:
		/**
		 * Constructor
		 *@param p_mainScene : [in] The main scene, if NULL, nothing is rendered
		 *@param p_hWnd : [in] The handle of the display window which holds the render window
		 *@param p_windowWidth : [in] The display window width
		 *@param p_windowHeight : [in] The display window height
		 *@param p_type : [in] The projection type of the viewport for the camera creation
		 *@param p_pixelFormat : [in] The pixel format of the window
		 *@param p_look : [in] Where the camera must look
		 */
		RenderWindow( Root * p_pRoot, ScenePtr p_mainScene, void * p_hWnd,  int p_windowWidth, int p_windowHeight,
					  Viewport::eTYPE p_type, Castor::Resources::PixelFormat p_pixelFormat=Castor::Resources::pxfR8G8B8A8,
					  ePROJECTION_DIRECTION p_look=pdLookToFront);
		/**
		 * Destructor
		 */
		~RenderWindow();

	public:
		/**
		 * Resizes the window
		 *@param x, y : [in] The new size
		 */
		void Resize( int x, int y);
		/**
		 * Makes pre render actions
		 */
		bool PreRender();
		/**
		 * Renders one frame
		 *@param p_tslf : [in] The time since the last frame was rendered
		 *@param p_bForce : [in] Forces the rendering
		 */
		void RenderOneFrame( const real & p_tslf, const bool & p_bForce);
		/**
		 * makes post render actions
		 */
		bool PostRender();
		/**
		 * Renders the window, dummy
		 *@param p_displayMode : [in] The display mode
		 */
		virtual void Render( eDRAW_TYPE p_displayMode){}

		virtual bool Write( Castor::Utils::File & p_file)const { return true; }
		virtual bool Read( Castor::Utils::File & p_file) { return true; }

	protected:
		void _setViewPoint();

	public:
		/**@name Accessors */
		//@{
		inline size_t			GetIndex		()const { return m_index; }
		inline ScenePtr			GetMainScene	()const { return m_mainScene; }
		inline eDRAW_TYPE		GetDrawType		()const { return m_drawType; }
		inline bool				IsFocused		()const	{ return m_focused; }
		inline void *			GetHWnd			()const { return m_hWnd; }
		inline int				GetWindowWidth	()const { return m_windowWidth; }
		inline int				GetWindowHeight	()const { return m_windowHeight; }
		inline CameraPtr		GetCamera		()const { return m_camera; }
		inline Viewport::eTYPE	GetType			()const { return m_type; }
		inline bool				IsInitialised	()const { return m_initialised; }
		inline eNORMALS_MODE	GetNormalsMode	()const { return m_normalsMode; }
		inline bool				IsNormalsVisible()const { return m_showNormals; }
		inline FrameListenerPtr	GetListener		()const { return m_listener; }
		inline Castor::Resources::PixelFormat	GetPixelFormat	()const { return m_pixelFormat; }

		inline void	SetToUpdate				()						{ m_toUpdate = true; }
		inline void	SetInitialised			()						{ m_initialised = true; }
		inline void SetChanged				( bool p_changed)		{ m_changed = p_changed; }
		inline void Focus					()						{ m_focused = true; }
		inline void UnFocus					()						{ m_focused = false; }
		inline void SetNormalsMode			( eNORMALS_MODE p_nm) 	{ m_changed = (m_normalsMode != p_nm); m_normalsMode = p_nm; }
		inline void SetNormalsVisibility	( bool p_vis) 			{ m_changed = (p_vis != m_showNormals); m_showNormals = p_vis; }
		inline void	SetDrawType				( eDRAW_TYPE p_dt)		{ m_drawType = p_dt; }
		//@}
	};
	//! A window renderer
	/*!
	Creates the window context, renders the window, resizes the window
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API WindowRenderer : public Renderer<RenderWindow, WindowRenderer>
	{
	protected:
		/**
		 * Constructor, only RenderSystem can use it
		 */
		WindowRenderer( SceneManager * p_pManager)
			:	Renderer<RenderWindow, WindowRenderer>( p_pManager)
		{}

	public:
		/**
		 * Destructor
		 */
		virtual ~WindowRenderer(){ Cleanup(); }
		/**
		 * Cleans up the instance
		 */
		virtual void Cleanup(){}
		/**
		 * Initialises the context
		 */
		virtual void Initialise() = 0;
		/**
		 * Starts the render : flushes the window, ...
		 */
		virtual void StartRender() = 0;
		/**
		 * Ends the render : swaps buffers ,...
		 */
		virtual void EndRender() = 0;
		/**
		 * Resizes internally the window, id est tells the camera the new dimensions
		 */
		virtual void Resize( unsigned int p_width, unsigned int p_height) = 0;
	};
}

#endif
