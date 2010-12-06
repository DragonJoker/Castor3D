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
#ifndef ___C3D_OverlayManager___
#define ___C3D_OverlayManager___

#include <CastorUtils/Manager.h>
#include <CastorUtils/AutoSingleton.h>

#include "Module_Overlay.h"

namespace Castor3D
{
	//! The class managing and owning Overlays
	/*!
	This manager holds the references to the created overlays. It creates them end renders them.
	\author Sylvain DOREMUS
	\date 25/08/2010
	*/
	class C3D_API OverlayManager : public Castor::Theory::AutoSingleton<OverlayManager>
	{
	private:
		int m_iCurrentZIndex;
		OverlayPtrIntMap m_mapOverlaysByZIndex;
		OverlayPtrStrMap m_mapOverlaysByName;

	public:
		/**
		 * Constructor
		 */
		OverlayManager();
		/**
		 * Destructor
		 */
		~OverlayManager();
		/**
		 * Creates an Overlay of the given type
		 *@param p_strName : [in] The overlay name
		 *@param p_pParent : [in] The parent overlay.
		 *@param p_ptPosition : [in] The relative to parent position (if parent is NULL, it is screen relative)
		 *@param p_ptSize : [in] The relative to parent size (if parent is NULL, it is screen relative)
		 *@param p_iZIndex : [in] The wanted Z Index.
		 *@return The created overlay
		 */
		template <class T>
		Templates::SharedPtr<T> CreateOverlay( const String & p_strName, OverlayPtr p_pParent, const Point2r & p_ptPosition, const Point2r & p_ptSize, int p_iZIndex=0)
		{
			Templates::SharedPtr<T>  l_pReturn;

			if (m_mapOverlaysByName.find( p_strName) == m_mapOverlaysByName.end())
			{
				if (p_pParent.null())
				{
					if (p_iZIndex == 0)
					{
						l_pReturn = new T( p_strName, p_pParent);
						m_mapOverlaysByZIndex.insert( OverlayPtrIntMap::value_type( m_iCurrentZIndex++, l_pReturn));
					}
					else if (m_mapOverlaysByZIndex.find( p_iZIndex) == m_mapOverlaysByZIndex.end())
					{
						l_pReturn = new T( p_strName, p_pParent);
						m_mapOverlaysByZIndex.insert( OverlayPtrIntMap::value_type( p_iZIndex, l_pReturn));
					}
				}
				else
				{
					l_pReturn = new T( p_strName, p_pParent);
					if ( ! p_pParent->AddChild( l_pReturn, p_iZIndex * 100))
					{
						l_pReturn.reset();
					}
				}
			}

			if ( ! l_pReturn.null())
			{
				l_pReturn->SetPosition( p_ptPosition);
				l_pReturn->SetSize( p_ptSize);

				m_mapOverlaysByName.insert( OverlayPtrStrMap::value_type( p_strName, l_pReturn));

				while (m_mapOverlaysByZIndex.find( m_iCurrentZIndex) != m_mapOverlaysByZIndex.end())
				{
					m_iCurrentZIndex++;
				}
			}

			return l_pReturn;
		}
		/**
		 * Renders all visible overlays
		 */
		void RenderOverlays();
	};
}

#endif