/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_FRAME_VARIABLES_LIST_VIEW_H___
#define ___GUICOMMON_FRAME_VARIABLES_LIST_VIEW_H___

#include "ShaderSource.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	class FrameVariablesList
		: public wxTreeCtrl
	{
	public:
		FrameVariablesList( castor3d::Engine * engine
			, PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & ptPos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~FrameVariablesList();

		void loadVariables( ashes::ShaderStageFlag stage
			, std::vector< UniformBufferValues > & ubos );
		void unloadVariables();

	private:
		void doAddBuffer( wxTreeItemId id
			, UniformBufferValues & buffer );
		void doAddVariable( wxTreeItemId id
			, UniformValueBase & uniform );

		DECLARE_EVENT_TABLE()
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );
		void onMouseRButtonUp( wxTreeEvent & event );

	private:
		castor3d::Engine * m_engine;
		PropertiesContainer * m_propertiesHolder;
	};
}

#endif
