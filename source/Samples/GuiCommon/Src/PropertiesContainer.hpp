/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_PropertiesContainer_H___
#define ___GuiCommon_PropertiesContainer_H___

#include "TreeItemProperty.hpp"

#include <wx/propgrid/propgrid.h>

namespace GuiCommon
{
	class PropertiesContainer
		: public wxPropertyGrid
	{
	public:
		PropertiesContainer( bool p_bCanEdit, wxWindow * p_parent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~PropertiesContainer();

		void setPropertyData( TreeItemProperty * p_data );
		/**
		 *\~english
		 *\brief		Retrieves the editable status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modifiabilità
		 *\return		La valeur
		 */
		inline bool IsEditable()const
		{
			return m_bCanEdit;
		}
		/**
		 *\~english
		 *\brief		Retrieves the button editor
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'àditeur bouton
		 *\return		La valeur
		 */
		static inline wxPGEditor * getButtonEditor()
		{
			return m_buttonEditor;
		}

	private:
		void onPropertyChange( wxPropertyGridEvent & p_event );

	private:
		bool m_bCanEdit;
		TreeItemProperty * m_data;
		static wxPGEditor * m_buttonEditor;
	};
}

#endif
