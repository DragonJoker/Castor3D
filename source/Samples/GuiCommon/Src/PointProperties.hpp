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
#ifndef ___GUICOMMON_POINT_PROPERTIES_H___
#define ___GUICOMMON_POINT_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <Math/Point.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point2b );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point3b );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point4b );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point2i );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point3i );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point4i );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point2ui );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point3ui );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point4ui );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point2f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point3f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point4f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point2d );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point3d );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Point4d );

namespace GuiCommon
{
	static const wxString GC_POINT_XY[2] = { wxT( "X" ), wxT( "Y" ) };
	static const wxString GC_POINT_XYZ[3] = { wxT( "X" ), wxT( "Y" ), wxT( "Z" ) };
	static const wxString GC_POINT_XYZW[4] = { wxT( "X" ), wxT( "Y" ), wxT( "Z" ), wxT( "W" ) };

	static const wxString GC_POINT_12[2] = { wxT( "1" ), wxT( "2" ) };
	static const wxString GC_POINT_123[3] = { wxT( "1" ), wxT( "2" ), wxT( "3" ) };
	static const wxString GC_POINT_1234[4] = { wxT( "1" ), wxT( "2" ), wxT( "3" ), wxT( "4" ) };

	static const wxString GC_POINT_SIZE[2] = { _( "Width" ), _( "Height" ) };
	static const wxString GC_POINT_POSITION[2] = { _( "Left" ), _( "Top" ) };
	static const wxString GC_POINT_RECTANGLE[4] = { _( "Left" ), _( "Top" ), _( "Right" ), _( "Bottom" ) };
	static const wxString GC_HDR_COLOUR[4] = { _( "R" ), _( "G" ), _( "B" ), _( "A" ) };

	template< typename T, size_t Count >
	class PointProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( PointProperty )

	public:
		PointProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Point< T, Count > const & value = castor::Point< T, Count >() );
		PointProperty( wxString const( & p_names )[Count], wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::Point< T, Count > const & value = castor::Point< T, Count >() );
		virtual ~PointProperty();

		virtual wxVariant childChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void refreshChildren();

	protected:
		// I stands for internal
		inline void setValueI( castor::Point< T, Count > const & value );
	};

	template< typename Type, size_t Count > castor::Point< Type, Count > const & PointRefFromVariant( wxVariant const & p_variant );
	template< typename Type, size_t Count > castor::Point< Type, Count > & PointRefFromVariant( wxVariant & p_variant );
	template< typename Type, size_t Count > void setVariantFromPoint( wxVariant & p_variant, castor::Point< Type, Count > const & p_value );

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
