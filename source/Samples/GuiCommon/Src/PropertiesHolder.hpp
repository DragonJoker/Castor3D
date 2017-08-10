/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
		PropertiesHolder( bool p_bCanEdit, wxWindow * p_parent, wxPoint const & p_ptPos = wxDefaultPosition, wxSize const & p_size = wxDefaultSize );
		~PropertiesHolder();

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
