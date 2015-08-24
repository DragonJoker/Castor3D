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
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2r );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3r );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4r );
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
		void SetValueI( Castor::Size const & value )
		{
			m_value = WXVARIANT( value );
		}
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
		void SetValueI( Castor::Position const & value )
		{
			m_value = WXVARIANT( value );
		}
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
		void SetValueI( Castor::Point4i const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point2rProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point2rProperty )
	public:

		Point2rProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point2r const & value = Castor::Point2r() );
		virtual ~Point2rProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point2r const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point3rProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point3rProperty )
	public:

		Point3rProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point3r const & value = Castor::Point3r() );
		virtual ~Point3rProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point3r const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point4rProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point3rProperty )
	public:

		Point4rProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point4r const & value = Castor::Point4r() );
		virtual ~Point4rProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point4r const & value )
		{
			m_value = WXVARIANT( value );
		}
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
		void SetValueI( Castor::Quaternion const & value )
		{
			m_value = WXVARIANT( value );
		}
	};
}
#endif
