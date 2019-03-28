/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_MATERIALS_LIST_VIEW_H___
#define ___GUICOMMON_MATERIALS_LIST_VIEW_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	class MaterialsList
		: public wxTreeCtrl
	{
	public:
		MaterialsList( PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~MaterialsList();

		void LoadMaterials( castor3d::Engine * engine
			, castor3d::Scene & scene );
		void UnloadMaterials();

	private:
		void doAddMaterial( wxTreeItemId id
			, castor3d::MaterialSPtr material );
		void doAddPass( wxTreeItemId id
			, uint32_t index
			, castor3d::PassSPtr pass );
		void doAddTexture( wxTreeItemId id
			, uint32_t index
			, castor3d::TextureUnitSPtr texture
			, castor3d::MaterialType type );

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );
		void onMouseRButtonUp( wxTreeEvent & event );

	private:
		castor3d::Engine * m_engine{ nullptr };
		castor3d::Scene * m_scene{ nullptr };
		PropertiesContainer * m_propertiesHolder{ nullptr };
	};
}

#endif
