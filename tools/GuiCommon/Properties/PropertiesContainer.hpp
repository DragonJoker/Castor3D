/*
See LICENSE file in root folder
*/
#ifndef ___GuiCommon_PropertiesContainer_H___
#define ___GuiCommon_PropertiesContainer_H___

#include "GuiCommon/Properties/TreeItems/TreeItemProperty.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <wx/propgrid/propgrid.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

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

	private:
		void onPropertyChange( wxPropertyGridEvent & event );

	private:
		bool m_canEdit;
		TreeItemProperty * m_data;
	};
}

#endif
