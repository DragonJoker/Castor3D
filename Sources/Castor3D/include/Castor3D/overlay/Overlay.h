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
	class CS3D_API Overlay
	{
	protected:
		OverlayRenderer * m_pRenderer;
		Point2D<float> m_ptPosition;
		Point2D<float> m_ptSize;
		String m_strName;
		bool m_bVisible;
		Overlay * m_pParent;
		std::map <int, Overlay *> m_mapOverlays;
		int m_iCurrentZIndex;

	public:
		Overlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer);
		virtual ~Overlay(){}

		virtual void Render()=0;

		bool AddChild( Overlay * p_pOverlay, int p_iZIndex);

		void SetPosition( const Point2D<float> & p_ptPosition);
		void SetSize( const Point2D<float> & p_ptSize);

	public:
		inline String			GetName		()const { return m_strName; }
		inline Point2D<float>	GetPosition	()const { return m_ptPosition; }
		inline Point2D<float>	GetSize		()const { return m_ptSize; }
		inline bool				IsVisible	()const { return m_bVisible; }
	};

	class CS3D_API PanelOverlay : public Overlay
	{
	protected:
		Material * m_pMaterial;

	public:
		PanelOverlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
			:	Overlay( p_strName, p_pParent, p_pRenderer),
				m_pMaterial( NULL)
		{
		}
		virtual ~PanelOverlay(){}

		virtual void Render();

	public:
		inline Material *		GetMaterial	()const { return m_pMaterial; }

		inline void SetMaterial	( Material * p_pMaterial)				{ m_pMaterial = p_pMaterial; }
	};

	class CS3D_API BorderPanelOverlay : public PanelOverlay
	{
	protected:
		Material * m_pBorderMaterial;
		float m_fLeftBorderSize;
		float m_fRightBorderSize;
		float m_fTopBorderSize;
		float m_fBottomBorderSize;

	public:
		BorderPanelOverlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
			:	PanelOverlay( p_strName, p_pParent, p_pRenderer),
				m_pBorderMaterial( NULL),
				m_fLeftBorderSize( 0),
				m_fRightBorderSize( 0),
				m_fTopBorderSize( 0),
				m_fBottomBorderSize( 0)
		{
		}
		virtual ~BorderPanelOverlay(){}

		virtual void Render();

	public:
		inline Material *	GetMaterial			()const { return m_pMaterial; }
		inline float		GetLeftBorderSize	()const { return m_fLeftBorderSize; }
		inline float		GetRightBorderSize	()const { return m_fRightBorderSize; }
		inline float		GetTopBorderSize	()const { return m_fTopBorderSize; }
		inline float		GetBottomBorderSize	()const { return m_fBottomBorderSize; }

		inline void SetMaterial			( Material * p_pMaterial)	{ m_pMaterial = p_pMaterial; }
		inline void SetLeftBorderSize	( float p_fSize)			{ m_fLeftBorderSize = p_fSize; }
		inline void SetRightBorderSize	( float p_fSize)			{ m_fRightBorderSize = p_fSize; }
		inline void SetTopBorderSize	( float p_fSize)			{ m_fTopBorderSize = p_fSize; }
		inline void SetBottomBorderSize	( float p_fSize)			{ m_fBottomBorderSize = p_fSize; }
	};

	class CS3D_API TextOverlay : public Overlay
	{
	protected:
		String m_strCaption;

	public:
		TextOverlay( const String & p_strName, Overlay * p_pParent, OverlayRenderer * p_pRenderer)
			:	Overlay( p_strName, p_pParent, p_pRenderer)
		{
		}
		virtual ~TextOverlay(){}

		virtual void Render(){}

	public:
		inline String	GetCaption	()const { return m_strCaption; }

		inline void SetCaption	( const String & p_strCaption)	{ m_strCaption = p_strCaption; }
	};
}

#endif