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
#ifndef ___GUICOMMON_POINT_PROPERTIES_H___
#define ___GUICOMMON_POINT_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <Point.hpp>

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

namespace GuiCommon
{

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

	template< typename Type, size_t Count > Castor::Point< Type, Count > const & PointRefFromVariant( wxVariant const & p_variant );
	template< typename Type, size_t Count > Castor::Point< Type, Count > & PointRefFromVariant( wxVariant & p_variant );
	template< typename Type, size_t Count > void SetVariantFromPoint( wxVariant & p_variant, Castor::Point< Type, Count > const & p_value );
	
	template< size_t Count > using BoolPointProperty = PointProperty< bool, Count >;
	template< size_t Count > using IntPointProperty = PointProperty< int, Count >;
	template< size_t Count > using UIntPointProperty = PointProperty< uint32_t, Count >;
	template< size_t Count > using FloatPointProperty = PointProperty< float, Count >;
	template< size_t Count > using DoublePointProperty = PointProperty< double, Count >;

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
#else
	typedef Point2fProperty Point2rProperty;
	typedef Point3fProperty Point3rProperty;
	typedef Point4fProperty Point4rProperty;
#endif
}

#include "PointProperties.inl"

#endif
