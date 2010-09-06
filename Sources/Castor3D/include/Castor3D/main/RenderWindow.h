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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_RenderWindow___
#define ___C3D_RenderWindow___

#include "../camera/Module_Camera.h"
#include "../material/Module_Material.h"
#include "../geometry/Module_Geometry.h"
#include "../scene/Module_Scene.h"
#include "../render_system/Module_Render.h"

namespace Castor3D
{
	//! Render window representation
	/*!
	Manages a window wher you can render a scene.
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API RenderWindow
	{
	private:
		static size_t s_nbRenderWindows;		//!< Number of render windows

	private:
		size_t m_index;							//!< This window's index

		ProjectionType m_type;					//!< The projection type
		ProjectionDirection m_lookAt;			//!< The projection direction
		void * m_hWnd;							//!< The handle of the display window
		DrawType m_drawType;					//!< The type of drawing
		bool m_showNormals;						//!< Tells if we show normals
		NormalsMode m_normalsMode;				//!< Normals mode

		unsigned int m_timeSinceLastFrame;		//!< The time since the last frame was rendered

		bool m_initialised;						//!< Tells if the window is initalised
		bool m_focused;							//!< Tells if the window is focused by the mouse
		bool m_changed;							//!< Tells if the window has changed and then buffers must be regenerated
		bool m_toUpdate;						//!< Tells if this must be updated

		unsigned long long m_nbFrame;			//!< The frame count

		int m_windowWidth;						//!< The display window width
		int m_windowHeight;						//!< The display window height

		Scene * m_mainScene;					//!< The main scene
		Camera * m_camera;						//!< The camera associated to this window

		WindowRenderer * m_renderer;			//!< The window renderer
		FrameListener * m_listener;

	public:
		/**
		 * Constructor
		 *@param p_renderer : [in] The render window renderer, may be OpenGL or Direct3D
		 *@param p_mainScene : [in] The main scene, if NULL, nothing is rendered
		 *@param p_hWnd : [in] The handle of the display window which holds this render window
		 *@param p_windowWidth : [in] The display window width
		 *@param p_windowHeight : [in] The display window height
		 *@param p_type : [in] The projection type of the viewport for the camera creation
		 *@param p_look : [in] Where the camera must look
		 */
		RenderWindow( WindowRenderer * p_renderer, Scene * p_mainScene, void * p_hWnd, 
					  int p_windowWidth, int p_windowHeight, ProjectionType p_type,
					  ProjectionDirection p_look=pdLookToFront);
		/**
		 * Destructor
		 */
		~RenderWindow();

	public:
		/**
		 * Resizes this window
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
		void RenderOneFrame( const float & p_tslf, const bool & p_bForce);
		/**
		 * makes post render actions
		 */
		bool PostRender();

	protected:
		void _setViewPoint();

	public:
		inline size_t			GetIndex		()const { return m_index; }
		inline Scene *			GetMainScene	()const { return m_mainScene; }
		inline DrawType			GetDrawType		()const { return m_drawType; }
		inline bool				IsFocused		()const	{ return m_focused; }
		inline void *			GetHWnd			()const { return m_hWnd; }
		inline int				GetWindowWidth	()const { return m_windowWidth; }
		inline int				GetWindowHeight	()const { return m_windowHeight; }
		inline Camera *			GetCamera		()const { return m_camera; }
		inline ProjectionType	GetType			()const { return m_type; }
		inline bool				IsInitialised	()const { return m_initialised; }
		inline NormalsMode		GetNormalsMode	()const { return m_normalsMode; }
		inline bool				IsNormalsVisible()const { return m_showNormals; }
		inline FrameListener *	GetListener		()const { return m_listener; }

		inline void	SetToUpdate				()						{ m_toUpdate = true; }
		inline void	SetInitialised			()						{ m_initialised = true; }
		inline void SetChanged				( bool p_changed)		{ m_changed = p_changed; }
		inline void Focus					()						{ m_focused = true; }
		inline void UnFocus					()						{ m_focused = false; }
		inline void SetNormalsMode			( NormalsMode p_nm) 	{ m_changed = (m_normalsMode != p_nm); m_normalsMode = p_nm; }
		inline void SetNormalsVisibility	( bool p_vis) 			{ m_changed = (p_vis != m_showNormals); m_showNormals = p_vis; }
	};
}

#endif

