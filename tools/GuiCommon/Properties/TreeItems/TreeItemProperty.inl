#include "GuiCommon/Properties/Math/CubeBoxProperties.hpp"
#include "GuiCommon/Properties/Math/SphereBoxProperties.hpp"

#include <Castor3D/Engine.hpp>

#pragma warning( push )
#pragma warning( disable: 4371 )

namespace GuiCommon
{
	inline wxPGProperty * appendProp( wxPropertyGrid * parent
		, wxPGProperty * prop )
	{
		return parent->Append( prop );
	}
	
	inline wxPGProperty * appendProp( wxPGProperty * parent
		, wxPGProperty * prop )
	{
		return parent->AppendChild( prop );
	}

	template< typename ParentT, typename MyValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::createProperty( ParentT * parent
		, wxString const & name
		, MyValueT && value
		, PropertyChangeHandler handler
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		using ValueT = std::remove_cv_t< std::remove_reference_t< MyValueT > >;
		m_handlers.emplace( m_prefix + name, doGetHandler( handler, std::move( controls ) ) );

		if constexpr ( std::is_same_v< ValueT, bool > )
		{
			wxPGProperty * prop = appendProp( parent, new wxBoolProperty( name, m_prefix + name, value ) );
			prop->SetAttribute( wxPG_BOOL_USE_CHECKBOX, true );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint8_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxUIntProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< uint8_t >( std::numeric_limits< uint8_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< uint8_t >( std::numeric_limits< uint8_t >::max() ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, int16_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxIntProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< int16_t >( std::numeric_limits< int16_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< int16_t >( std::numeric_limits< int16_t >::max() ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint16_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxUIntProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< uint16_t >( std::numeric_limits< uint16_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< uint16_t >( std::numeric_limits< uint16_t >::max() ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, int32_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxIntProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< int32_t >( std::numeric_limits< int32_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< int32_t >( std::numeric_limits< int32_t >::max() ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint32_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxUIntProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_MIN, 0 );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, int64_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxIntProperty( name, m_prefix + name, wxLongLong( value ) ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint64_t > )
		{
			wxPGProperty * prop = appendProp( parent, new wxUIntProperty( name, m_prefix + name, wxULongLong( value ) ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_MIN, 0 );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, float > )
		{
			wxPGProperty * prop = appendProp( parent, new wxFloatProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, double > )
		{
			wxPGProperty * prop = appendProp( parent, new wxFloatProperty( name, m_prefix + name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::RgbColour > )
		{
			return appendProp( parent, new wxColourProperty( name, m_prefix + name, wxColour{ castor::toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::RgbaColour > )
		{
			return appendProp( parent, new wxColourProperty( name, m_prefix + name, wxColour{ castor::toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::HdrRgbColour > )
		{
			return appendProp( parent, new HdrRgbColourProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::HdrRgbaColour > )
		{
			return appendProp( parent, new HdrRgbaColourProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor3d::ColourWrapper > )
		{
			return appendProp( parent, new wxColourProperty( name, m_prefix + name, wxColour{ toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2f > )
		{
			return appendProp( parent, new Point2fProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3f > )
		{
			return appendProp( parent, new Point3fProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4f > )
		{
			return appendProp( parent, new Point4fProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2d > )
		{
			return appendProp( parent, new Point2dProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3d > )
		{
			return appendProp( parent, new Point3dProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4d > )
		{
			return appendProp( parent, new Point4dProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2i > )
		{
			return appendProp( parent, new Point2iProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3i > )
		{
			return appendProp( parent, new Point3iProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4i > )
		{
			return appendProp( parent, new Point4iProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2ui > )
		{
			return appendProp( parent, new Point2uiProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3ui > )
		{
			return appendProp( parent, new Point3uiProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4ui > )
		{
			return appendProp( parent, new Point4uiProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Matrix4x4f > )
		{
			return appendProp( parent, new Matrix4fProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Quaternion > )
		{
			return appendProp( parent, new QuaternionProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Position > )
		{
			return appendProp( parent, new PositionProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Size > )
		{
			return appendProp( parent, new SizeProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Rectangle > )
		{
			return appendProp( parent, new RectangleProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::String > )
		{
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, make_wxString( value ) ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Path > )
		{
			return appendProp( parent, new gcImageFileProperty( &m_engine->getImageLoader(), name, m_prefix + name, make_wxString(value)));
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Milliseconds > )
		{
			wxPGProperty * prop = appendProp( parent, new wxFloatProperty( name, m_prefix + name, double( value.count() ) / 1000.0 ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 0.1 ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			prop->SetAttribute( wxPG_ATTR_UNITS, ValueTraitsT< ValueT >::getUnit() );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::BoundingSphere > )
		{
			wxPGProperty * prop = appendProp( parent, new BoundingSphereProperty( name, m_prefix + name, value ) );
			prop->Enable( false );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::BoundingBox > )
		{
			wxPGProperty * prop = appendProp( parent, new BoundingBoxProperty( name, m_prefix + name, value ) );
			prop->Enable( false );
			return prop;
		}
		else if constexpr ( castor::isSpeedT< ValueT > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.getValue(), handler, std::move( controls ) );
			prop->SetAttribute( wxPG_ATTR_UNITS, ValueTraitsT< ValueT >::getUnit() );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Angle > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.degrees(), handler, std::move( controls ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 1.0 ) );
			prop->SetAttribute( wxPG_ATTR_UNITS, ValueTraitsT< ValueT >::getUnit() );
			prop->SetAttribute( wxPG_ATTR_MIN, WXVARIANT( 0.0 ) );
			prop->SetAttribute( wxPG_ATTR_MAX, WXVARIANT( 359.0 ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Font > )
		{
			wxFontInfo info( value.getHeight() );
			info.FaceName( value.getFaceName() );
			return appendProp( parent, new wxFontProperty( name, m_prefix + name, wxFont{ info } ) );
		}
		else if constexpr ( castor::isGroupChangeTrackedT< ValueT > )
		{
			return createProperty( parent, name, value.value(), handler, std::move( controls ) );
		}
		else if constexpr ( castor::isChangeTrackedT< ValueT > )
		{
			return createProperty( parent, name, value.value(), handler, std::move( controls ) );
		}
		else if constexpr ( castor::isRangedValueT< ValueT > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.value(), handler, std::move( controls ) );
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< castor::UnRangedValueT< ValueT > >( value.range().getMin() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< castor::UnRangedValueT< ValueT > >( value.range().getMax() ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, wxString > )
		{
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, wxArrayString > )
		{
			return appendProp( parent, new wxEnumProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::U32String > )
		{
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, castor::string::stringCast< castor::xchar >( value ) ) );
		}
		else
		{
			//static_assert( false, "TreeItemProperty::createProperty - Unsupported ValueT" );
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, value ) );
		}
	}

	template< typename ParentT, typename EnumT, typename FuncT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyE( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, FuncT func
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, choices
			, [func]( wxVariant const & var )
			{
				func( EnumT( variantCast< uint32_t >( var ) ) );
			}
			, std::move( controls ) );
		return prop;
	}

	template< typename ParentT, typename EnumT, typename FuncT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyE( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, EnumT selected
		, FuncT func
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = addPropertyE< ParentT, EnumT, FuncT >( parent
			, name
			, choices
			, func
			, std::move( controls ) );
		prop->SetValue( choices[uint32_t( selected )] );
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, PropertyChangeHandler handler
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, value
			, handler
			, std::move( controls ) );
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, castor::Range< ValueT > const & range
		, PropertyChangeHandler handler
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, value
			, handler
			, std::move( controls ) );
		prop->SetAttribute( wxPG_ATTR_MIN, getVariant< ValueT >( range.getMin() ) );
		prop->SetAttribute( wxPG_ATTR_MAX, getVariant< ValueT >( range.getMax() ) );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, ValueT const & step
		, PropertyChangeHandler handler
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = addProperty( parent, name, value, handler );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, getVariant< ValueT >( step ) );
		return prop;
	}

	template< typename ParentT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, castor3d::ColourWrapper value
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, value
			, [value]( wxVariant const & var )
			{
				wxColour col;
				col << var;
				( *value.value )->x = float( ( ( col.GetRGB() & 0x00FF0000 ) >> 16 ) ) / 255.0f;
				( *value.value )->y = float( ( ( col.GetRGB() & 0x0000FF00 ) >> 8 ) ) / 255.0f;
				( *value.value )->z = float( ( ( col.GetRGB() & 0x000000FF ) >> 0 ) ) / 255.0f;
			}
			, std::move( controls ) );
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, castor::RangedValue< ValueT > * value
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT * value
		, castor::Range< ValueT > const & range
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, range
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, castor::ChangeTracked< castor::RangedValue< ValueT > > * value
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, value->value()
			, [value]( wxVariant const & var )
			{
				castor::RangedValue< ValueT > ranged{ value->value() };
				ranged = variantCast< ValueT >( var );
				*value = ranged;
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT * value
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT * value
		, ValueT step
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, step
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT value
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT  value
		, ValueT step
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, step
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, ObjectT * object
		, ValueRefSetterT< ObjectU, ValueT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, castor::RangedValue< ValueT > const & value
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyET( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addPropertyE( parent
			, name
			, choices
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyET( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, EnumT selected
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addPropertyE( parent
			, name
			, choices
			, selected
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyET( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, EnumT * value
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls
		, castor3d::ConfigurationVisitorBase::OnEnumValueChangeT< EnumT > onChange )
	{
		return addPropertyE( parent
			, name
			, choices
			, *value
			, [value, onChange]( EnumT type )
			{
				auto save = *value;
				*value = type;
				onChange( save, *value );
			}
			, std::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter
		, castor3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, choices
			, selected
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			}
			, std::move( controls ) );
	}
}

#pragma warning( pop )
