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
#ifndef ___C3D_Viewport___
#define ___C3D_Viewport___

#include "Module_Camera.h"
#include "../render_system/Module_Render.h"
#include "../render_system/Renderable.h"
//#include "../render_system/CameraRenderer.h"

namespace Castor3D
{
	//! Viewport manager class
	/*!
	The class which manages all infos about viewport : projection type, FOV, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API Viewport : public Renderable<Viewport, ViewportRenderer>
	{
	public:
		//! The  viewport projection types enumerator
		/*!
		There are 2 projection types : 2D and 3D
		*/
		typedef enum
		{
			pt3DView,	//!< 3 Dimensions projection type
			pt2DView	//!< 2 Dimensions projection type

		} eTYPE;

	private:
		real m_left;			//!< The left viewport extent
		real m_right;			//!< The right viewport extent
		real m_top;				//!< The top viewport extent
		real m_bottom;			//!< The bottom viewport extent
		real m_farView;			//!< The viewport farthest viewable distance
		real m_nearView;		//!< The viewport nearest viewable distance
		real m_fovY;			//!< The viewport vertical FOV 

		eTYPE m_type;			//!< The projection type
		int m_windowWidth;		//!< The display window width
		int m_windowHeight;		//!< The display window height
		real m_ratio;			//!< The display window ratio (4:3, 16:9, ...)

	public:
		/**
		 * Constructor, needs the renderer, window size and projection type
		 *@param p_ww, p_wh : the window size
		 *@param p_type : tje projection type
		 */
		Viewport( int p_ww, int p_wh, eTYPE p_type);
		/**
		 * Destructor
		 */
		virtual ~Viewport();
		/**
		 * Asks the renderer to apply the viewport
		 */
		virtual void Apply( eDRAW_TYPE p_displayMode);
		/**
		 * Asks the direction from the renderer and returns it
		 */
		Point3r GetDirection( const Point<int, 2> & p_mouse);
		/**
		 * Writes the viewport in a file
		 *@param p_file : [in] The file to write in
		 *@return true if successful, false if not
		 */
		virtual bool Write( Castor::Utils::File & p_file)const;
		/**
		 * Reads the viewport from a file
		 *@param p_file : [in] The file to read from
		 *@return true if successful, false if not
		 */
		virtual bool Read( Castor::Utils::File & p_file);

	public:
		/**@name Accessors */
		//@{
		inline eTYPE	GetType			()const { return m_type; }
		inline real		GetRatio		()const { return m_ratio; }
		inline real		GetNearView		()const { return m_nearView; }
		inline real		GetFarView		()const { return m_farView; }
		inline real		GetFOVY			()const { return m_fovY; }
		inline real		GetLeft			()const { return m_left; }
		inline real		GetRight		()const { return m_right; }
		inline real		GetTop			()const { return m_top; }
		inline real		GetBottom		()const { return m_bottom; }
		inline int		GetWindowWidth	()const { return m_windowWidth; }
		inline int		GetWindowHeight	()const { return m_windowHeight; }
		
		inline void SetType			( eTYPE p_type)		{ m_type = p_type; }
		inline void SetRatio		( real p_ratio)		{ m_ratio = p_ratio; }
		inline void SetNearView		( real p_near)		{ m_nearView = p_near; }
		inline void SetFarView		( real p_far)		{ m_farView = p_far; }
		inline void SetFOVY			( real p_fovY)		{ m_fovY = p_fovY; }
		inline void SetLeft			( real p_left)		{ m_left = p_left; }
		inline void SetRight		( real p_right)		{ m_right = p_right; }
		inline void SetTop			( real p_top)		{ m_top = p_top; }
		inline void SetBottom		( real p_bottom)	{ m_bottom = p_bottom; }
		inline void SetWindowWidth	( int p_ww)			{ m_windowWidth = p_ww; }
		inline void SetWindowHeight	( int p_wh)			{ m_windowHeight = p_wh; }
		//@}
	};
	//! The Viewport renderer
	/*!
	Renders a viewport : applies it's fov, sets the display width and height, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class CS3D_API ViewportRenderer : public Renderer<Viewport, ViewportRenderer>
	{
	public:
		/**
		 * Constructor
		 */
		ViewportRenderer()
		{}
		/**
		 * Destructor
		 */
		virtual ~ViewportRenderer(){}
		/**
		 * Renders the viewport's transformations
		 */
		virtual void Apply( Viewport::eTYPE p_type) = 0;
		/**
		 * Retrieves the viewport direction and returns it
		 */
		virtual Point3r GetDirection( const Point<int, 2> & p_mouse)=0;
	};
}

#endif
