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

#include "TreeItemProperty.hpp"

#include <wx/propgrid/propgrid.h>

namespace GuiCommon
{
	class PropertiesHolder
		: public wxPropertyGrid
	{
	public:
		PropertiesHolder( bool p_bCanEdit, wxWindow * p_pParent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~PropertiesHolder();

		void SetPropertyData( TreeItemProperty * p_data );
		/**
		 *\~english
		 *\brief		Retrieves the editable status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut de modifiabilité
		 *\return		La valeur
		 */
		inline bool IsEditable()const
		{
			return m_bCanEdit;
		}

	private:
		void OnPropertyChange( wxPropertyGridEvent & p_event );

	private:
		bool m_bCanEdit;
		TreeItemProperty * m_data;
		static wxPGEditor * m_buttonEditor;
	};
}

#endif
