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
#ifndef ___C3D_Overlay___
#define ___C3D_Overlay___

#include "../Module_General.h"
#include "../render_system/Module_Render.h"
#include "../material/Module_Material.h"

namespace Castor3D
{
	//! The overlay abstract class
	/*!
	An overlay is a 2D element which is displayed at foreground
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API Overlay
	{
	protected:
		OverlayRenderer * m_pRenderer;					//!< The renderer which holds renderer specific operations
		Point2D<float> m_ptPosition;					//!< The relative position (to parent or screen)
		Point2D<float> m_ptSize;						//!< The relative size (to parent or screen)
		String m_strName;								//!< The overlay name
		bool m_bVisible;								//!< The visibility
		Overlay * m_pParent;							//!< The parent overlay, if any
		std::map <int, Overlay *> m_mapOverlays;		//!< The childs, if any
		int m_iCurrentZIndex;							//!< The z index of next children to add

	public:
		/**
		 * Constructor
		 *@param p_strName : [in] The overlay name
		 *@param p_pParent : [in] The parent overlay (if any)
		 *@param p_pRenderer : [in] The renderer
		 */
		Overlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer);
		/**
		 * Destructor
		 */
		virtual ~Overlay(){}
		/**
		 * Draws the overlay, virtual function to implement in children classes
		 */
		virtual void Render()=0;
		/**
		 * Adds a child to this overlay
		 *@param p_pOverlay : [in] The overlay to add
		 *@param p_iZIndex : [in] The wanted Z Index for this child
		 *@return true if successful, false if not
		 */
		bool AddChild( Overlay * p_pOverlay, int p_iZIndex);
		/**
		 * Sets the position of this overlay
		 *@param p_ptPosition : [in] The new position, relative to parent
		 */
		void SetPosition( const Point2D<float> & p_ptPosition);
		/**
		 * Sets the size of this overlay
		 *@param p_ptSize : [in] The new size, relative to parent
		 */
		void SetSize( const Point2D<float> & p_ptSize);

	public:
		inline String			GetName		()const { return m_strName; }
		inline Point2D<float>	GetPosition	()const { return m_ptPosition; }
		inline Point2D<float>	GetSize		()const { return m_ptSize; }
		inline bool				IsVisible	()const { return m_bVisible; }
	};
	//! A simple overlay area
	/*!
	Describes an actually rectangular overlay. Uses a material so it can contain an image
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API PanelOverlay : public Overlay
	{
	protected:
		Material * m_pMaterial;	//!< The material used by this overlay

	public:
		/**
		 * Constructor
		 *@param p_strName : [in] The overlay name
		 *@param p_pParent : [in] The parent overlay (if any)
		 *@param p_pRenderer : [in] The renderer
		 */
		PanelOverlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
			:	Overlay( p_strName, p_pParent, p_pRenderer),
				m_pMaterial( NULL)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~PanelOverlay(){}
		/**
		 * Draws the overlay
		 */
		virtual void Render();

	public:
		inline Material *		GetMaterial	()const { return m_pMaterial; }

		inline void SetMaterial	( Material * p_pMaterial)				{ m_pMaterial = p_pMaterial; }
	};
	//! A simple overlay bordered area
	/*!
	Describes an actually rectangular overlay with a border. Uses a material for the panel and a material for the border
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API BorderPanelOverlay : public PanelOverlay
	{
	protected:
		Material * m_pBorderMaterial;	//!< The border material
		Point4D <float> m_ptBorderSize;	//!< The border size
		float m_fRightBorderSize;
		float m_fTopBorderSize;
		float m_fBottomBorderSize;

	public:
		/**
		 * Constructor
		 *@param p_strName : [in] The overlay name
		 *@param p_pParent : [in] The parent overlay (if any)
		 *@param p_pRenderer : [in] The renderer
		 */
		BorderPanelOverlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
			:	PanelOverlay( p_strName, p_pParent, p_pRenderer),
				m_pBorderMaterial( NULL)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~BorderPanelOverlay(){}
		/**
		 * Draws the overlay
		 */
		virtual void Render();

	public:
		inline Material *				GetMaterial			()const { return m_pMaterial; }
		inline float					GetLeftBorderSize	()const { return m_ptBorderSize.left; }
		inline float					GetRightBorderSize	()const { return m_ptBorderSize.right; }
		inline float					GetTopBorderSize	()const { return m_ptBorderSize.top; }
		inline float					GetBottomBorderSize	()const { return m_ptBorderSize.bottom; }
		inline const Point4D <float>	GetBorderSize		()const { return m_ptBorderSize; }

		inline void SetMaterial			( Material * p_pMaterial)			{ m_pMaterial = p_pMaterial; }
		inline void SetLeftBorderSize	( float p_fSize)					{ m_ptBorderSize.left = p_fSize; }
		inline void SetRightBorderSize	( float p_fSize)					{ m_ptBorderSize.right = p_fSize; }
		inline void SetTopBorderSize	( float p_fSize)					{ m_ptBorderSize.top = p_fSize; }
		inline void SetBottomBorderSize	( float p_fSize)					{ m_ptBorderSize.bottom = p_fSize; }
		inline void SetBorderSize		( const Point4D <float> & p_ptSize)	{ m_ptBorderSize = p_ptSize; }
	};
	//! A text overlay
	/*!
	Describes an overlay containing a text. To be implemented
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class CS3D_API TextOverlay : public Overlay
	{
	protected:
		String m_strCaption;		//!< The overlay caption

	public:
		/**
		 * Constructor
		 *@param p_strName : [in] The overlay name
		 *@param p_pParent : [in] The parent overlay (if any)
		 *@param p_pRenderer : [in] The renderer
		 */
		TextOverlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
			:	Overlay( p_strName, p_pParent, p_pRenderer)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~TextOverlay(){}
		/**
		 * Draws the overlay
		 */
		virtual void Render(){}

	public:
		inline String	GetCaption	()const { return m_strCaption; }

		inline void SetCaption	( const String & p_strCaption)	{ m_strCaption = p_strCaption; }
	};
}

#endif