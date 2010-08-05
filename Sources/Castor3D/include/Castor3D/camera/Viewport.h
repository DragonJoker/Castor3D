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
#ifndef ___C3D_Viewport___
#define ___C3D_Viewport___

#include "../render_system/Module_Render.h"

namespace Castor3D
{
	//! Viewport manager class
	/*!
	The class which manages all infos about viewport : projection type, FOV, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Viewport
	{
	private:
		float m_left;			//!< The left viewport extent
		float m_right;			//!< The right viewport extent
		float m_top;			//!< The top viewport extent
		float m_bottom;			//!< The bottom viewport extent
		float m_farView;		//!< The viewport farthest viewable distance
		float m_nearView;		//!< The viewport nearest viewable distance
		float m_fovY;			//!< The viewport vertical FOV 

		ProjectionType m_type;	//!< The projection type
		int m_windowWidth;		//!< The display window width
		int m_windowHeight;		//!< The display window height
		float m_ratio;			//!< The display window ratio (4:3, 16:9, ...)

		ViewportRenderer * m_renderer;	//!< The Viewport renderer, used to render this

	public:
		/**
		 * Constructor, needs the renderer, window size and projection type
		 *@param p_renderer : the renderer, maybe OpenGL or Direct3D
		 *@param p_ww, p_wh : the window size
		 *@param p_type : tje projection type
		 */
		Viewport( ViewportRenderer * p_renderer, int p_ww, int p_wh, ProjectionType p_type);
		/**
		 * Destructor
		 */
		~Viewport();
		/**
		 * Asks the renderer to apply the viewport
		 */
		void Apply();
		/**
		 * Asks the direction from the renderer and returns it
		 */
		Vector3f GetDirection( const Point2D<int> & p_mouse);
		/**
		 * Writes the viewport in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		bool Write( General::Utils::File & p_file)const;
		/**
		 * Reads the viewport from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		bool Read( General::Utils::File & p_file);

	public:
		inline ProjectionType	GetType			()const { return m_type; }
		inline float			GetRatio		()const { return m_ratio; }
		inline float			GetNearView		()const { return m_nearView; }
		inline float			GetFarView		()const { return m_farView; }
		inline float			GetFOVY			()const { return m_fovY; }
		inline float			GetLeft			()const { return m_left; }
		inline float			GetRight		()const { return m_right; }
		inline float			GetTop			()const { return m_top; }
		inline float			GetBottom		()const { return m_bottom; }
		inline int				GetWindowWidth	()const { return m_windowWidth; }
		inline int				GetWindowHeight	()const { return m_windowHeight; }
		
		inline void SetType			( ProjectionType p_type)	{ m_type = p_type; }
		inline void SetRatio		( float p_ratio)			{ m_ratio = p_ratio; }
		inline void SetNearView		( float p_near)				{ m_nearView = p_near; }
		inline void SetFarView		( float p_far)				{ m_farView = p_far; }
		inline void SetFOVY			( float p_fovY)				{ m_fovY = p_fovY; }
		inline void SetLeft			( float p_left)				{ m_left = p_left; }
		inline void SetRight		( float p_right)			{ m_right = p_right; }
		inline void SetTop			( float p_top)				{ m_top = p_top; }
		inline void SetBottom		( float p_bottom)			{ m_bottom = p_bottom; }
		inline void SetWindowWidth	( int p_ww)					{ m_windowWidth = p_ww; }
		inline void SetWindowHeight	( int p_wh)					{ m_windowHeight = p_wh; }
	};
}

#endif
