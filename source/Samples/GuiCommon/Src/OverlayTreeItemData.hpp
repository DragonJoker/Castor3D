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
#ifndef ___GUICOMMON_OVERLAY_TREE_ITEM_DATA_H___
#define ___GUICOMMON_OVERLAY_TREE_ITEM_DATA_H___

#include "TreeItemPropertyData.hpp"

namespace GuiCommon
{
	class wxOverlayTreeItemData
		: public wxTreeItemPropertyData
	{
	public:
		wxOverlayTreeItemData( Castor3D::OverlaySPtr p_overlay );
		~wxOverlayTreeItemData();

		inline Castor3D::OverlaySPtr GetOverlay()
		{
			return m_overlay.lock();
		}

		void OnMaterialChange( Castor::String const & p_name );
		void OnPositionChange( Castor::Position const & p_position );
		void OnSizeChange( Castor::Size const & p_size );
		void OnBorderMaterialChange( Castor::String const & p_name );
		void OnBorderSizeChange( Castor::Rectangle const & p_size );
		void OnBorderInnerUVChange( Castor::Point4d const & p_uv );
		void OnBorderOuterUVChange( Castor::Point4d const & p_uv );
		void OnBorderPositionChange( Castor3D::eBORDER_POSITION p_position );
		void OnCaptionChange( Castor::String const & p_caption );
		void OnFontChange( Castor::FontSPtr p_font );

	protected:
		Castor3D::OverlayWPtr m_overlay;
	};
}

#endif
