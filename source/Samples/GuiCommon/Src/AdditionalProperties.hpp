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
#ifndef ___GUICOMMON_ADDITIONAL_PROPERTIES_H___
#define ___GUICOMMON_ADDITIONAL_PROPERTIES_H___

#include <wx/propgrid/propgrid.h>

#define WX_PG_NS_DECLARE_VARIANT_DATA( namspace, classname )\
	WX_PG_NS_DECLARE_VARIANT_DATA_EXPORTED( namspace, classname, wxEMPTY_PARAMETER_VALUE )

#define WX_PG_NS_DECLARE_VARIANT_DATA_EXPORTED( namspace, classname, expdecl )\
	namspace::classname & operator<<( namspace::classname & object, const wxVariant & variant );\
	wxVariant & operator<<( wxVariant & variant, const namspace::classname & object );\
	const namspace::classname & classname##RefFromVariant( const wxVariant & variant );\
	namspace::classname & classname##RefFromVariant( wxVariant & variant );\
	template<> inline wxVariant WXVARIANT( const namspace::classname & value )\
	{\
		wxVariant variant;\
		variant << value;\
		return variant;\
	}\
	extern const char* classname##_VariantType;

// Implements sans constructor function. Also, first arg is class name, not
// property name.
#define GC_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN( PROPNAME, T, EDITOR )\
	const wxPGEditor* PROPNAME::DoGetEditorClass()const\
	{\
		return wxPGEditor_##EDITOR;\
	}

// Single inheritance with one base class
#define GC_IMPLEMENT_DYNAMIC_CLASS( name, basename )\
	wxIMPLEMENT_CLASS_COMMON1( name, basename, name::wxCreateObject )\
	template<>\
	wxObject* name::wxCreateObject()\
		{ return new name; }

//
// Property class implementation helper macros.
//
#define GC_PG_IMPLEMENT_PROPERTY_CLASS( NAME, UPCLASS, T, T_AS_ARG, EDITOR )\
	GC_IMPLEMENT_DYNAMIC_CLASS( NAME, UPCLASS )\
	GC_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN( NAME, T, EDITOR )

namespace GuiCommon
{
	template< typename T > T GetValue( wxVariant const & p_variant );
	template< typename T > wxVariant SetValue( T const & p_value );

	typedef bool ( wxEvtHandler::*ButtonEventMethod )( wxPGProperty * );

	class ButtonData
		: public wxClientData
	{
	public:
		ButtonData( ButtonEventMethod p_method, wxEvtHandler * p_handler );
		bool Call( wxPGProperty * property );

	private:
		ButtonEventMethod m_method;
		wxEvtHandler * m_handler;
	};

	class ButtonEventEditor
		: public wxPGEditor
	{
	protected:
		virtual wxPGWindowList CreateControls( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxPoint const & p_pos, wxSize const & p_size )const;
		virtual void UpdateControl(wxPGProperty* property, wxWindow* ctrl) const;
		virtual bool OnEvent( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxWindow * p_wnd_primary, wxEvent & p_event )const;
	};
}

#include "AdditionalProperties.inl"

#endif
