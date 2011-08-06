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
#ifndef ___C3D_RenderTarget___
#define ___C3D_RenderTarget___

#include "Prerequisites.hpp"
#include "Renderable.hpp"
#include "Viewport.hpp"

namespace Castor3D
{
	//! render target type
	typedef enum
	{	eTARGET_TYPE_SCREEN
	,	eTARGET_TYPE_TEXTURE
	,	eTARGET_TYPE_COUNT
	}	eTARGET_TYPE;
	//! Data rendered in the render target
	typedef enum
	{	eTARGET_DATA_COLOUR
	,	eTARGET_DATA_DEPTH
	,	eTARGET_DATA_STENCIL
	,	eTARGET_DATA_COUNT
	}	eTARGET_DATA;
	//!< Render target main class C3D_API
	/*!
	A render target may be a window to have a direct render or a texture, to have offscreen rendering
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 01/04/2011
	*/
	class C3D_API RenderTarget
	{
	protected:
		ScenePtr				m_pMainScene;		//!< The main scene
		CameraPtr				m_pCamera;			//!< The camera associated to the window
		Root *					m_pRoot;			//! Root object
		String					m_strName;			//! The name
		eVIEWPORT_TYPE			m_eType;			//!< The projection type
		Point2i					m_ptTargetSize;		//!< The target size
		ePRIMITIVE_TYPE			m_eDrawType;		//!< The type of the primitives to draw
		FrameListenerPtr		m_pListener;		//! The events listener
		ePROJECTION_DIRECTION	m_eLookAt;			//!< The projection direction
		eTARGET_TYPE			m_eTargetType;
		eTARGET_DATA			m_eTargetData;

	public:
		/**
		 * Constructor
		 *@param p_strName : [in] The name of this Rendertarget
		 *@param p_pRoot : [in] The engine root
		 *@param p_mainScene : [in] The main scene, if nullptr, nothing is rendered
		 *@param p_ptSize : [in] The RenderTarget size
		 */
		RenderTarget( String const & p_strName, Root * p_pRoot, ScenePtr p_mainScene, Point2i const & p_ptSize, eVIEWPORT_TYPE	p_eType, ePROJECTION_DIRECTION p_look=ePROJECTION_DIRECTION_FRONT, eTARGET_TYPE p_eTargetType=eTARGET_TYPE_SCREEN, eTARGET_DATA p_eTargetData=eTARGET_DATA_COLOUR);
		/**
		 * Destructor
		 */
		virtual ~RenderTarget();
		/**
		 * Makes pre render actions
		 */
		virtual bool PreRender()=0;
		/**
		 * Renders one frame
		 *@param p_tslf : [in] The time since the last frame was rendered
		 *@param p_bForce : [in] Forces the rendering
		 */
		virtual void RenderOneFrame( const real & p_tslf, const bool & p_bForce);
		/**
		 * makes post render actions
		 */
		virtual bool PostRender()=0;

		/**@name Accessors */
		//@{
		inline ScenePtr			GetMainScene	()const { return m_pMainScene; }
		inline int				GetTargetWidth	()const { return m_ptTargetSize[0]; }
		inline int				GetTargetHeight	()const { return m_ptTargetSize[1]; }
		inline Point2i const &	GetTargetSize	()const { return m_ptTargetSize; }
		inline ePRIMITIVE_TYPE	GetDrawType		()const { return m_eDrawType; }
		inline CameraPtr		GetCamera		()const { return m_pCamera; }
		inline eVIEWPORT_TYPE	GetType			()const { return m_eType; }
		inline FrameListenerPtr	GetListener		()const { return m_pListener; }

		inline void	SetDrawType	( ePRIMITIVE_TYPE p_eDrawType)	{ m_eDrawType = p_eDrawType; }
		//@}

	private:
		void _setViewPoint();
	};
}

#endif