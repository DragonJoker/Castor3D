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
#ifndef ___GUICOMMON_PROPERTIES_HOLDER_H___
#define ___GUICOMMON_PROPERTIES_HOLDER_H___

#include "TreeItemPropertyData.hpp"

#include <wx/propgrid/propgrid.h>

namespace GuiCommon
{
	class wxPropertiesHolder
		: public wxPropertyGrid
	{
	public:
		wxPropertiesHolder( bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~wxPropertiesHolder();

		void SetPropertyData( wxTreeItemPropertyData * p_data );

	private:
		void DoSetCameraProperties( wxCameraTreeItemData * p_data );
		void DoSetGeometryProperties( wxGeometryTreeItemData * p_data );
		void DoSetSubmeshProperties( wxSubmeshTreeItemData * p_data );
		void DoSetLightProperties( wxLightTreeItemData * p_data );
		void DoSetNodeProperties( wxNodeTreeItemData * p_data );
		void DoSetOverlayProperties( wxOverlayTreeItemData * p_data );
		void DoSetMaterialProperties( wxMaterialTreeItemData * p_data );
		void DoSetPassProperties( wxPassTreeItemData * p_data );
		void DoSetTextureProperties( wxTextureTreeItemData * p_data );
		void DoSetDirectionalLightProperties( Castor3D::DirectionalLightSPtr p_light );
		void DoSetPointLightProperties( Castor3D::PointLightSPtr p_light );
		void DoSetSpotLightProperties( Castor3D::SpotLightSPtr p_light );
		void DoSetPanelOverlayProperties( Castor3D::PanelOverlaySPtr p_overlay );
		void DoSetBorderPanelOverlayProperties( Castor3D::BorderPanelOverlaySPtr p_overlay );
		void DoSetTextOverlayProperties( Castor3D::TextOverlaySPtr p_overlay );

		wxEnumProperty * DoBuildMaterialProperty( wxString const & p_name, Castor3D::Engine * p_engine );

		void OnCameraPropertyChanged( wxPropertyGridEvent & p_event );
		void OnGeometryPropertyChanged( wxPropertyGridEvent & p_event );
		void OnSubmeshPropertyChanged( wxPropertyGridEvent & p_event );
		void OnDirectionalLightPropertyChanged( wxPropertyGridEvent & p_event );
		void OnPointLightPropertyChanged( wxPropertyGridEvent & p_event );
		void OnSpotLightPropertyChanged( wxPropertyGridEvent & p_event );
		void OnNodePropertyChanged( wxPropertyGridEvent & p_event );
		void OnPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnBorderPanelOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnTextOverlayPropertyChanged( wxPropertyGridEvent & p_event );
		void OnMaterialPropertyChanged( wxPropertyGridEvent & p_event );
		void OnPassPropertyChanged( wxPropertyGridEvent & p_event );
		void OnTexturePropertyChanged( wxPropertyGridEvent & p_event );

	private:
		bool m_bCanEdit;
		wxTreeItemPropertyData * m_data;
		wxObjectEventFunction m_currentHandler;
	};
}

#endif
