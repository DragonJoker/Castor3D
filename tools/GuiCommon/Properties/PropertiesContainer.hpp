/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_PropertiesContainer_H___
#define ___GuiCommon_PropertiesContainer_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#pragma warning( push )
#pragma warning( disable: 4365 )
#include <wx/propgrid/propgrid.h>
#pragma warning( pop )

namespace GuiCommon
{
	class PropertiesContainer
		: public wxPropertyGrid
	{
	public:
		PropertiesContainer( bool canEdit
			, wxWindow * parent
			, wxPoint const & pos = wxDefaultPosition
			, wxSize const & size = wxDefaultSize );

		void setPropertyData( TreeItemProperty * data );
		/**
		 *\~english
		 *\brief		Retrieves the editable status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modifiabilità
		 *\return		La valeur
		 */
		bool isEditable()const
		{
			return m_canEdit;
		}
		/**
		 *\~english
		 *\brief		Retrieves the button editor
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'àditeur bouton
		 *\return		La valeur
		 */
		static wxPGEditor * getButtonEditor()
		{
			return m_buttonEditor;
		}

	private:
		void onPropertyChange( wxPropertyGridEvent & event );

	private:
		bool m_canEdit;
		TreeItemProperty * m_data;
		static wxPGEditor * m_buttonEditor;
	};
}

#endif
