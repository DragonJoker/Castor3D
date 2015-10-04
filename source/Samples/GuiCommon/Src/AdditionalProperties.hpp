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

#define GC_PG_NS_DECLARE_VARIANT_DATA_EXPORTED( namspace, classname, expdecl )\
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

#define GC_PG_NS_DECLARE_VARIANT_DATA( namspace, classname )\
	GC_PG_NS_DECLARE_VARIANT_DATA_EXPORTED( namspace, classname, wxEMPTY_PARAMETER_VALUE )

// Implements sans constructor function. Also, first arg is class name, not
// property name.
#define GC_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN( PROPNAME, T, EDITOR )\
	template<> const wxPGEditor* PROPNAME::DoGetEditorClass()const\
	{\
		return wxPGEditor_##EDITOR;\
	}
	
// common part of the macros below
#define GC_IMPLEMENT_CLASS_COMMON( name, basename, baseclsinfo2, func )\
	template<>\
	wxClassInfo name::ms_classInfo( wxT( #name ),\
						&basename::ms_classInfo,\
						baseclsinfo2,\
						int( sizeof( name ) ),\
						func );\
	template<>\
	wxClassInfo * name::GetClassInfo() const\
	{\
		return &name::ms_classInfo;\
	}

#define GC_IMPLEMENT_CLASS_COMMON1( name, basename, func )\
    GC_IMPLEMENT_CLASS_COMMON( name, basename, NULL, func )

// Single inheritance with one base class
#define GC_IMPLEMENT_DYNAMIC_CLASS( name, basename )\
	template<>\
	wxObject* name::wxCreateObject()\
	{\
		return new name;\
	}\
	GC_IMPLEMENT_CLASS_COMMON1( name, basename, name::wxCreateObject )

//
// Property class implementation helper macros.
//
#define GC_PG_IMPLEMENT_PROPERTY_CLASS( NAME, UPCLASS, T, T_AS_ARG, EDITOR )\
	GC_IMPLEMENT_DYNAMIC_CLASS( NAME, UPCLASS )\
	GC_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN( NAME, T, EDITOR )

// Add getter (ie. classname << variant) separately to allow
// custom implementations.
#define GC_PG_IMPLEMENT_ALIGNED_VARIANT_DATA_EXPORTED_NO_EQ_NO_GETTER( classname, alignment, expdecl ) \
	const char* classname##_VariantType = #classname; \
	class classname##VariantData\
		: public wxVariantData\
		, public Castor::Aligned< alignment >\
	{ \
	public:\
		classname##VariantData() {} \
		classname##VariantData( const classname & value ) { m_value = value; } \
		classname &GetValue() { return m_value; } \
		const classname &GetValue() const { return m_value; } \
		virtual bool Eq( wxVariantData & data ) const; \
		virtual wxString GetType() const; \
		virtual wxVariantData* Clone() const { return new classname##VariantData( m_value ); } \
	protected:\
		classname m_value; \
	};\
	\
	wxString classname##VariantData::GetType() const\
	{\
		return wxS( #classname );\
	}\
	\
	expdecl wxVariant & operator<<( wxVariant & variant, const classname & value )\
	{\
		classname##VariantData * data = new classname##VariantData( value );\
		variant.SetData( data );\
		return variant;\
	} \
	expdecl classname & classname##RefFromVariant( wxVariant & variant ) \
	{ \
		wxASSERT_MSG( variant.GetType() == wxS( #classname ), \
					  wxString::Format( "Variant type should have been '%s'" \
									   "instead of '%s'", \
									   wxS( #classname ), \
									   variant.GetType().c_str() ) ); \
		classname##VariantData *data = \
			( classname##VariantData * ) variant.GetData(); \
		return data->GetValue();\
	} \
	expdecl const classname& classname##RefFromVariant( const wxVariant& variant ) \
	{ \
		wxASSERT_MSG( variant.GetType() == wxS( #classname ), \
					  wxString::Format( "Variant type should have been '%s'" \
									   "instead of '%s'", \
									   wxS( #classname ), \
									   variant.GetType().c_str() ) ); \
		classname##VariantData * data = \
			( classname##VariantData * ) variant.GetData(); \
		return data->GetValue();\
	}

#define GC_PG_IMPLEMENT_ALIGNED_VARIANT_DATA_GETTER( classname, expdecl ) \
	expdecl classname & operator<<( classname &value, const wxVariant & variant )\
	{\
		wxASSERT( variant.GetType() == #classname );\
		\
		classname##VariantData * data = ( classname##VariantData * ) variant.GetData();\
		value = data->GetValue();\
		return value;\
	}

// with Eq() implementation that always returns false
#define GC_PG_IMPLEMENT_ALIGNED_VARIANT_DATA_EXPORTED_DUMMY_EQ( classname, alignment, expdecl ) \
	GC_PG_IMPLEMENT_ALIGNED_VARIANT_DATA_EXPORTED_NO_EQ_NO_GETTER( classname, alignment, wxEMPTY_PARAMETER_VALUE expdecl ) \
	GC_PG_IMPLEMENT_ALIGNED_VARIANT_DATA_GETTER( classname, wxEMPTY_PARAMETER_VALUE expdecl ) \
	\
	bool classname##VariantData::Eq( wxVariantData & WXUNUSED( data ) ) const \
	{\
		return false; \
	}

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

	wxFloatProperty * CreateProperty( wxString const & p_name, double const & p_value );
	wxFloatProperty * CreateProperty( wxString const & p_name, float const & p_value );
	wxIntProperty * CreateProperty( wxString const & p_name, int const & p_value );
	wxUIntProperty * CreateProperty( wxString const & p_name, uint32_t const & p_value );
	wxBoolProperty * CreateProperty( wxString const & p_name, bool const & p_value, bool p_checkbox );
	wxStringProperty * CreateProperty( wxString const & p_name, wxString const & p_value );
	wxStringProperty * CreateProperty( wxString const & p_name, wxString const & p_value, ButtonEventMethod p_method, wxEvtHandler * p_handler, wxPGEditor * p_editor );
}

#include "AdditionalProperties.inl"

#endif
