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
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4i );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4f );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point2d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point3d );
WX_PG_NS_DECLARE_VARIANT_DATA( Castor, Point4d );
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

	class Point2iProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point2iProperty )
	public:

		Point2iProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point2i const & value = Castor::Point2i() );
		virtual ~Point2iProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point2i const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point3iProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point3iProperty )
	public:

		Point3iProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point3i const & value = Castor::Point3i() );
		virtual ~Point3iProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point3i const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point4iProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point4iProperty )
	public:

		Point4iProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point4i const & value = Castor::Point4i() );
		virtual ~Point4iProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point4i const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point2fProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point2fProperty )
	public:

		Point2fProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point2f const & value = Castor::Point2f() );
		virtual ~Point2fProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point2f const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point3fProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point3fProperty )
	public:

		Point3fProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point3f const & value = Castor::Point3f() );
		virtual ~Point3fProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point3f const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point4fProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point4fProperty )
	public:

		Point4fProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point4f const & value = Castor::Point4f() );
		virtual ~Point4fProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point4f const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point2dProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point2dProperty )
	public:

		Point2dProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point2d const & value = Castor::Point2d() );
		virtual ~Point2dProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point2d const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point3dProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point3dProperty )
	public:

		Point3dProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point3d const & value = Castor::Point3d() );
		virtual ~Point3dProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point3d const & value )
		{
			m_value = WXVARIANT( value );
		}
	};

	class Point4dProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( Point4dProperty )
	public:

		Point4dProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::Point4d const & value = Castor::Point4d() );
		virtual ~Point4dProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		void SetValueI( Castor::Point4d const & value )
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
#endif
