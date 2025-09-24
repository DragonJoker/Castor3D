/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_FRAME_VARIABLES_LIST_VIEW_H___
#define ___GUICOMMON_FRAME_VARIABLES_LIST_VIEW_H___

#include "GuiCommon/Shader/ShaderSource.hpp"

#include <wx/treectrl.h>

namespace GuiCommon
{
	class FrameVariablesList
		: public wxTreeCtrl
	{
	public:
		FrameVariablesList( c3d::Engine * engine
			, ImagesLoader & imagesLoader
			, PropertiesContainer * propertiesHolder
			, wxWindow * parent
			, wxPoint const & ptPos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );
		~FrameVariablesList()override;

		void loadVariables( VkShaderStageFlagBits stage
			, c3d::Vector< UniformBufferValues > & ubos );
		void unloadVariables();

	private:
		void doAddBuffer( wxTreeItemId id
			, UniformBufferValues & buffer );
		void doAddVariable( wxTreeItemId id
			, UniformValueBase & uniform );

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-override"
		DECLARE_EVENT_TABLE()
#pragma clang diagnostic pop
		void onClose( wxCloseEvent & event );
		void onSelectItem( wxTreeEvent & event );
		void onMouseRButtonUp( wxTreeEvent & event );

	private:
		c3d::Engine * m_engine;
		ImagesLoader & m_imagesLoader;
		PropertiesContainer * m_propertiesHolder;
	};
}

#endif
