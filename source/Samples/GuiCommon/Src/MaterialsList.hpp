/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_MATERIALS_LIST_VIEW_H___
#define ___GUICOMMON_MATERIALS_LIST_VIEW_H___

#include "GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	class MaterialsList
		: public wxTreeCtrl
	{
	public:
		MaterialsList( PropertiesHolder * p_propertiesHolder
			, wxWindow * p_parent
			, wxPoint const & p_ptPos = wxDefaultPosition
			, wxSize const & p_size = wxDefaultSize );
		~MaterialsList();

		void LoadMaterials( castor3d::Engine * engine
			, castor3d::Scene & p_scene );
		void UnloadMaterials();

	private:
		void doAddMaterial( wxTreeItemId p_id
			, castor3d::MaterialSPtr p_material );
		void doAddPass( wxTreeItemId p_id
			, uint32_t p_index
			, castor3d::PassSPtr p_pass );
		void doAddTexture( wxTreeItemId p_id
			, uint32_t p_index
			, castor3d::TextureUnitSPtr p_texture
			, castor3d::MaterialType p_type );

		DECLARE_EVENT_TABLE()
		void OnClose( wxCloseEvent & p_event );
		void OnSelectItem( wxTreeEvent & p_event );
		void OnMouseRButtonUp( wxTreeEvent & p_event );

	private:
		castor3d::Engine * m_engine{ nullptr };
		castor3d::Scene * m_scene{ nullptr };
		PropertiesHolder * m_propertiesHolder{ nullptr };
	};
}

#endif
