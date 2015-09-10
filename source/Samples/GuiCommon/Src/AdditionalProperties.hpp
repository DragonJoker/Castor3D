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

#include <Point.hpp>
#include <Quaternion.hpp>

#define WX_PG_NS_DECLARE_VARIANT_DATA(namspace,classname) \
    WX_PG_NS_DECLARE_VARIANT_DATA_EXPORTED(namspace, classname, wxEMPTY_PARAMETER_VALUE)

#define WX_PG_NS_DECLARE_VARIANT_DATA_EXPORTED(namspace,classname,expdecl) \
namspace::classname & operator << ( namspace::classname &object, const wxVariant &variant ); \
wxVariant& operator << ( wxVariant &variant, const namspace::classname &object ); \
const namspace::classname& classname##RefFromVariant( const wxVariant& variant ); \
namspace::classname& classname##RefFromVariant( wxVariant& variant ); \
template<> inline wxVariant WXVARIANT( const namspace::classname& value ) \
{ \
    wxVariant variant; \
    variant << value; \
    return variant; \
} \
extern const char* classname##_VariantType;

WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Size );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Position );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2b );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3b );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4b );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2ui );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3ui );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4ui );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix2x2f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix3x3f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix4x4f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix2x2d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix3x3d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix4x4d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Quaternion );

namespace GuiCommon
{
	class SizeProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( SizeProperty )
	public:

		SizeProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Size const & value = Castor::Size() );
		virtual ~SizeProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Size const & value );
	};

	class PositionProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( PositionProperty )
	public:

		PositionProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Position const & value = Castor::Position() );
		virtual ~PositionProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Position const & value );
	};

	class RectangleProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( RectangleProperty )
	public:

		RectangleProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point4i const & value = Castor::Point4i() );
		virtual ~RectangleProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Point4i const & value );
	};

	class QuaternionProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( QuaternionProperty )
	public:

		QuaternionProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Quaternion const & value = Castor::Quaternion() );
		virtual ~QuaternionProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Quaternion const & value );
	};

	template< typename T, size_t Count >
	class PointProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( PointProperty )

	public:
		PointProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point< T, Count > const & value = Castor::Point< T, Count >() );
		virtual ~PointProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Point< T, Count > const & value );
	};

	template< typename T, size_t Rows, size_t Columns >
	class MatrixProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( MatrixProperty )

	public:
		MatrixProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Matrix< T, Rows, Columns > const & value = Castor::Matrix< T, Rows, Columns >() );
		virtual ~MatrixProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::Matrix< T, Rows, Columns > const & value );
	};

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

	template< typename Type, size_t Count > Castor::Point< Type, Count > const & ValueRefFromVariant( wxVariant const & p_variant );
	template< typename Type, size_t Count > Castor::Point< Type, Count > & ValueRefFromVariant( wxVariant & p_variant );
	
	template< size_t Count > using BoolPointProperty = PointProperty< bool, Count >;
	template< size_t Count > using IntPointProperty = PointProperty< int, Count >;
	template< size_t Count > using UIntPointProperty = PointProperty< uint32_t, Count >;
	template< size_t Count > using FloatPointProperty = PointProperty< float, Count >;
	template< size_t Count > using DoublePointProperty = PointProperty< double, Count >;

	template< size_t Rows, size_t Columns > using FloatMatrixProperty = MatrixProperty< float, Rows, Columns >;
	template< size_t Rows, size_t Columns > using DoubleMatrixProperty = MatrixProperty< double, Rows, Columns >;

	typedef BoolPointProperty< 2 > Point2bProperty;
	typedef BoolPointProperty< 3 > Point3bProperty;
	typedef BoolPointProperty< 4 > Point4bProperty;

	typedef IntPointProperty< 2 > Point2iProperty;
	typedef IntPointProperty< 3 > Point3iProperty;
	typedef IntPointProperty< 4 > Point4iProperty;

	typedef UIntPointProperty< 2 > Point2uiProperty;
	typedef UIntPointProperty< 3 > Point3uiProperty;
	typedef UIntPointProperty< 4 > Point4uiProperty;

	typedef FloatPointProperty< 2 > Point2fProperty;
	typedef FloatPointProperty< 3 > Point3fProperty;
	typedef FloatPointProperty< 4 > Point4fProperty;

	typedef DoublePointProperty< 2 > Point2dProperty;
	typedef DoublePointProperty< 3 > Point3dProperty;
	typedef DoublePointProperty< 4 > Point4dProperty;

#if CASTOR_USE_DOUBLE
	typedef Point2dProperty Point2rProperty;
	typedef Point3dProperty Point3rProperty;
	typedef Point4dProperty Point4rProperty;

	inline Castor::Point2r const & Point2rRefFromVariant( wxVariant const & p_variant )
	{
		return Point2dRefFromVariant( p_variant );
	}

	inline Castor::Point3r const & Point3rRefFromVariant( wxVariant const & p_variant )
	{
		return Point3dRefFromVariant( p_variant );
	}

	inline Castor::Point4r const & Point4rRefFromVariant( wxVariant const & p_variant )
	{
		return Point4dRefFromVariant( p_variant );
	}

	inline Castor::Point2r & Point2rRefFromVariant( wxVariant & p_variant )
	{
		return Point2dRefFromVariant( p_variant );
	}

	inline Castor::Point3r & Point3rRefFromVariant( wxVariant & p_variant )
	{
		return Point3dRefFromVariant( p_variant );
	}

	inline Castor::Point4r & Point4rRefFromVariant( wxVariant & p_variant )
	{
		return Point4dRefFromVariant( p_variant );
	}
#else
	typedef Point2fProperty Point2rProperty;
	typedef Point3fProperty Point3rProperty;
	typedef Point4fProperty Point4rProperty;

	inline Castor::Point2r const & Point2rRefFromVariant( wxVariant const & p_variant )
	{
		return Point2fRefFromVariant( p_variant );
	}

	inline Castor::Point3r const & Point3rRefFromVariant( wxVariant const & p_variant )
	{
		return Point3fRefFromVariant( p_variant );
	}

	inline Castor::Point4r const & Point4rRefFromVariant( wxVariant const & p_variant )
	{
		return Point4fRefFromVariant( p_variant );
	}

	inline Castor::Point2r & Point2rRefFromVariant( wxVariant & p_variant )
	{
		return Point2fRefFromVariant( p_variant );
	}

	inline Castor::Point3r & Point3rRefFromVariant( wxVariant & p_variant )
	{
		return Point3fRefFromVariant( p_variant );
	}

	inline Castor::Point4r & Point4rRefFromVariant( wxVariant & p_variant )
	{
		return Point4fRefFromVariant( p_variant );
	}
#endif
}

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

#include "AdditionalProperties.inl"

#endif
