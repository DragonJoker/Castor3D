/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_TreeListContainerT_H___
#define ___GUICOMMON_TreeListContainerT_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include "GuiCommon/Properties/PropertiesContainer.hpp"
#include "GuiCommon/Properties/PropertiesHolder.hpp"

#include <wx/panel.h>
#include <wx/aui/aui.h>

namespace GuiCommon
{
	template< typename ListT >
	class TreeListContainerT
		: public wxPanel
	{
	public:
		TreeListContainerT( wxWindow * parent
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize )
			: wxPanel{ parent, wxID_ANY, pos, size, wxDEFAULT_FRAME_STYLE }
			, m_auiManager{ this }
		{
			this->m_holder = new PropertiesHolder{ this };
			this->m_holder->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			this->m_holder->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			this->m_container = new PropertiesContainer{ true, this->m_holder, wxDefaultPosition, wxDefaultSize };
			this->m_holder->setGrid( this->m_container );
			this->m_holder->SetMinSize( { 100, 300 } );

			auto holder = new TreeHolder{ this, wxDefaultPosition, wxDefaultSize };
			this->m_list = new ListT{ this->m_container, holder, wxDefaultPosition, wxDefaultSize };
			this->m_list->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			this->m_list->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			this->m_list->SetMinSize( { 100, 300 } );
			holder->setTree( this->m_list );

			this->m_auiManager.SetArtProvider( new AuiDockArt );
			this->m_auiManager.AddPane( holder
				, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Show()
				.Name( wxT( "Tree" ) )
				.Caption( _( "Tree" ) )
				.Top()
				.Dock()
				.PaneBorder( false )
				.MinSize( 100, 300 ) );
			this->m_auiManager.AddPane( this->m_holder
				, wxAuiPaneInfo()
				.CaptionVisible( false )
				.Show()
				.Name( wxT( "Properties" ) )
				.Caption( _( "Properties" ) )
				.Center()
				.Dock()
				.PaneBorder( false )
				.MinSize( 100, 300 ) );
			this->m_auiManager.Update();
		}

		~TreeListContainerT()
		{
			m_auiManager.UnInit();
		}

		ListT * getList()const
		{
			return m_list;
		}

	private:
		ListT * m_list{ nullptr };
		PropertiesHolder * m_holder{ nullptr };
		PropertiesContainer * m_container{ nullptr };
		wxAuiManager m_auiManager;
	};
}

#endif
