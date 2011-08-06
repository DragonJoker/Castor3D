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
#ifndef ___C3D_Viewport___
#define ___C3D_Viewport___

#include "Renderable.hpp"

namespace Castor
{	namespace Resources
{
	//! Viewport loader
	/*!
	Loads and saves viewports from/into a file
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	template <> class C3D_API Loader<Castor3D::Viewport>
	{
	public:
		/**
		 * Writes a viewports into a text file
		 *@param p_file : [in] the file to save the viewports in
		 *@param p_viewport : [in] the viewports to save
		 */
		static bool Write( const Castor3D::Viewport & p_viewport, Utils::File & p_file);
	};
}
}

namespace Castor3D
{
	//! The  viewport projection types enumerator
	/*!
	There are 2 projection types : 2D and 3D
	*/
	typedef enum
	{	eVIEWPORT_TYPE_3D	//!< 3 Dimensions projection type
	,	eVIEWPORT_TYPE_2D	//!< 2 Dimensions projection type
	,	eVIEWPORT_TYPE_COUNT
	}	eVIEWPORT_TYPE;
	//! Viewport manager class C3D_API
	/*!
	The class C3D_API which manages all infos about viewport : projection type, FOV, ...
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	*/
	class C3D_API Viewport : public Serialisable
	{
	public:
		static const String xstring_type[2];

	private:
		real m_rLeft;					//!< The left viewport extent
		real m_rRight;					//!< The right viewport extent
		real m_rTop;					//!< The top viewport extent
		real m_rBottom;					//!< The bottom viewport extent
		real m_rFarView;				//!< The viewport farthest viewable distance
		real m_rNearView;				//!< The viewport nearest viewable distance
		Angle m_aFovY;					//!< The viewport vertical FOV 

		eVIEWPORT_TYPE m_eType;					//!< The projection type
		unsigned int m_uiWindowWidth;	//!< The display window width
		unsigned int m_uiWindowHeight;	//!< The display window height
		real m_rRatio;					//!< The display window ratio (4:3, 16:9, ...)

	public:
		/**
		 * Constructor, needs the renderer, window size and projection type
		 *@param p_uiWidth, p_uiHeight : the window size
		 *@param p_eType : the projection type
		 */
		Viewport( unsigned int p_uiWidth, unsigned int p_uiHeight, eVIEWPORT_TYPE p_eType);
		/**
		 * Destructor
		 */
		virtual ~Viewport();
		/**
		 * Asks the renderer to apply the viewport
		 *@param p_eDisplayMode : [in] The display mode
		 */
		virtual void Render( ePRIMITIVE_TYPE p_eDisplayMode);
		/**
		 * Asks the direction from the renderer and returns it
		 *@param p_ptMouse : [in] The in-screen position
		 *@return The computed direction
		 */
		void GetDirection( Point2i const & p_ptMouse, Point3r & p_ptResult);

		/**@name Accessors */
		//@{
		inline eVIEWPORT_TYPE	GetType			()const { return m_eType; }
		inline real				GetRatio		()const { return m_rRatio; }
		inline real				GetNearView		()const { return m_rNearView; }
		inline real				GetFarView		()const { return m_rFarView; }
		inline Angle			GetFovY			()const { return m_aFovY; }
		inline real				GetLeft			()const { return m_rLeft; }
		inline real				GetRight		()const { return m_rRight; }
		inline real				GetTop			()const { return m_rTop; }
		inline real				GetBottom		()const { return m_rBottom; }
		inline unsigned int		GetWindowWidth	()const { return m_uiWindowWidth; }
		inline unsigned int		GetWindowHeight	()const { return m_uiWindowHeight; }

		inline void SetType			( eVIEWPORT_TYPE p_eType)	{ m_eType = p_eType; }
		inline void SetRatio		( real p_rRatio)			{ m_rRatio = p_rRatio; }
		inline void SetNearView		( real p_rNear)				{ m_rNearView = p_rNear; }
		inline void SetFarView		( real p_rFar)				{ m_rFarView = p_rFar; }
		inline void SetFovY			( Angle const & p_aFovY)	{ m_aFovY = p_aFovY; }
		inline void SetLeft			( real p_rLeft)				{ m_rLeft = p_rLeft; }
		inline void SetRight		( real p_rRight)			{ m_rRight = p_rRight; }
		inline void SetTop			( real p_rTop)				{ m_rTop = p_rTop; }
		inline void SetBottom		( real p_rBottom)			{ m_rBottom = p_rBottom; }
		inline void SetWindowWidth	( unsigned int p_uiWidth)	{ m_uiWindowWidth = p_uiWidth; }
		inline void SetWindowHeight	( unsigned int p_uiHeight)	{ m_uiWindowHeight = p_uiHeight; }
		//@}

		DECLARE_SERIALISE_MAP()
	};
}

#endif
