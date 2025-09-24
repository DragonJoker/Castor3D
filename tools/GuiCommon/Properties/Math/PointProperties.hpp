/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_POINT_PROPERTIES_H___
#define ___GUICOMMON_POINT_PROPERTIES_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Math/Point.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point2b )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point3b )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point4b )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point2i )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point3i )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point4i )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point2ui )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point3ui )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point4ui )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point2f )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point3f )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point4f )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point2d )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point3d )
GC_PG_NS_DECLARE_VARIANT_DATA( c3d, Point4d )

namespace GuiCommon
{
	static const c3d::Array< wxString, 2 > GC_POINT_XY{ wxT( "X" ), wxT( "Y" ) };
	static const c3d::Array< wxString, 3 > GC_POINT_XYZ{ wxT( "X" ), wxT( "Y" ), wxT( "Z" ) };
	static const c3d::Array< wxString, 4 > GC_POINT_XYZW{ wxT( "X" ), wxT( "Y" ), wxT( "Z" ), wxT( "W" ) };

	static const c3d::Array< wxString, 3 > GC_COL_RGB{ wxT( "R" ), wxT( "G" ), wxT( "B" ) };
	static const c3d::Array< wxString, 4 > GC_COL_RGBA{ wxT( "R" ), wxT( "G" ), wxT( "B" ), wxT( "A" ) };

	static const c3d::Array< wxString, 2 > GC_POINT_12{ wxT( "1" ), wxT( "2" ) };
	static const c3d::Array< wxString, 3 > GC_POINT_123{ wxT( "1" ), wxT( "2" ), wxT( "3" ) };
	static const c3d::Array< wxString, 4 > GC_POINT_1234{ wxT( "1" ), wxT( "2" ), wxT( "3" ), wxT( "4" ) };
	static const c3d::Array< wxString, 2 > GC_POINT_SIZE{ _( "Width" ), _( "Height" ) };
	static const c3d::Array< wxString, 2 > GC_POINT_POSITION{ _( "Left" ), _( "Top" ) };
	static const c3d::Array< wxString, 4 > GC_POINT_RECTANGLE{ _( "Left" ), _( "Top" ), _( "Right" ), _( "Bottom" ) };

	static const c3d::Array< wxString, 4 > GC_HDR_COLOUR{ _( "R" ), _( "G" ), _( "B" ), _( "A" ) };

	template< typename T, uint32_t Count >
	class PointProperty
		: public wxPGProperty
	{
	public:
		static wxObject * wxCreateObject()
		{
			return new PointProperty;
		}

		wxClassInfo * GetClassInfo() const override
		{
			static wxString s_name{ wxString{} << wxT( "Point" ) << Count << getTypeNameSuffix< T >() << wxT( "Property" ) };
			static wxClassInfo s_classInfo = { s_name.c_str()
				, &wxPGProperty::ms_classInfo
				, nullptr
				, int( sizeof( PointProperty ) )
				, PointProperty::wxCreateObject };
			return &s_classInfo;
		}

		const wxPGEditor * DoGetEditorClass()const override
		{
			return wxPGEditor_TextCtrl;
		}

	public:
		explicit PointProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, c3d::Point< T, Count > const & value = c3d::Point< T, Count >() );
		PointProperty( wxString const & label
			, wxString const & name
			, c3d::PointView< T, Count > const & value );
		explicit PointProperty( c3d::Array< wxString, Count > const & names
			, wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, c3d::Point< T, Count > const & value = c3d::Point< T, Count >() );
		PointProperty( c3d::Array< wxString, Count > const & names
			, wxString const & label
			, wxString const & name
			, c3d::PointView< T, Count > const & value );
		PointProperty( wxString const & label
			, wxString const & name
			, c3d::HdrRgbColour const & value );
		PointProperty( wxString const & label
			, wxString const & name
			, c3d::HdrRgbaColour const & value );

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( c3d::Point< T, Count > const & value );
	};

	template< typename Type, uint32_t Count > c3d::Point< Type, Count > const & PointRefFromVariant( wxVariant const & variant );
	template< typename Type, uint32_t Count > c3d::Point< Type, Count > & PointRefFromVariant( wxVariant & variant );
	template< typename Type, uint32_t Count > void setVariantFromPoint( wxVariant & variant, c3d::Point< Type, Count > const & value );

	template< uint32_t Count > using BoolPointProperty = PointProperty< bool, Count >;
	template< uint32_t Count > using IntPointProperty = PointProperty< int, Count >;
	template< uint32_t Count > using UIntPointProperty = PointProperty< uint32_t, Count >;
	template< uint32_t Count > using FloatPointProperty = PointProperty< float, Count >;
	template< uint32_t Count > using DoublePointProperty = PointProperty< double, Count >;

	using Point2bProperty = BoolPointProperty< 2 >;
	using Point3bProperty = BoolPointProperty< 3 >;
	using Point4bProperty = BoolPointProperty< 4 >;

	using Point2iProperty = IntPointProperty< 2 >;
	using Point3iProperty = IntPointProperty< 3 >;
	using Point4iProperty = IntPointProperty< 4 >;

	using Point2uiProperty = UIntPointProperty< 2 >;
	using Point3uiProperty = UIntPointProperty< 3 >;
	using Point4uiProperty = UIntPointProperty< 4 >;

	using Point2fProperty = FloatPointProperty< 2 >;
	using Point3fProperty = FloatPointProperty< 3 >;
	using Point4fProperty = FloatPointProperty< 4 >;

	using Point2dProperty = DoublePointProperty< 2 >;
	using Point3dProperty = DoublePointProperty< 3 >;
	using Point4dProperty = DoublePointProperty< 4 >;
}

#include "GuiCommon/Properties/Math/PointProperties.inl"

#endif
