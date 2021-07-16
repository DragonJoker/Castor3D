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

		void loadMaterials( castor3d::Engine * engine
			, castor3d::Scene & scene );
		void unloadMaterials();
		static void addMaterial( wxTreeCtrl * treeCtrl
			, castor3d::Scene & scene
			, bool editable
			, wxTreeItemId id
			, castor3d::MaterialSPtr material
			, uint32_t iconOffset = 0u );

	private:
		static void doAddPass( wxTreeCtrl * treeCtrl
			, castor3d::Scene & scene
			, bool editable
			, wxTreeItemId id
			, uint32_t index
			, castor3d::PassSPtr pass
			, uint32_t iconOffset );
		static void doAddTexture( wxTreeCtrl * treeCtrl
			, bool editable
			, wxTreeItemId id
			, uint32_t index
			, castor3d::Pass & pass
			, castor3d::TextureUnitSPtr texture
			, uint32_t iconOffset );

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
