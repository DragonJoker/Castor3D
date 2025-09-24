#include "GuiCommon/Properties/Math/CubeBoxProperties.hpp"
#include "GuiCommon/Properties/Math/SphereBoxProperties.hpp"
#include "GuiCommon/Properties/TreeItems/TreeItemConfigurationBuilder.hpp"

#include <Castor3D/Engine.hpp>
#include <CastorUtils/Math/Illumination.hpp>
#include <CastorUtils/Math/LuminousIntensity.hpp>

#pragma warning( push )
#pragma warning( disable: 4371 )

namespace GuiCommon
{
	//*********************************************************************************************

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

	//*********************************************************************************************

	template< typename ParentT, typename MyValueT >
	wxPGProperty * TreeItemProperty::createProperty( ParentT * parent
		, wxString const & name
		, MyValueT const & value )
	{
		using ValueT = std::remove_cv_t< std::remove_reference_t< MyValueT > >;

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
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 0.1 ) );
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
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 0.1 ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
#if wxCHECK_VERSION( 3, 1, 0 )
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
#endif
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::RgbColour > )
		{
			return appendProp( parent, new wxColourProperty( name, m_prefix + name, wxColour{ c3d::toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::RgbaColour > )
		{
			return appendProp( parent, new wxColourProperty( name, m_prefix + name, wxColour{ c3d::toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::HdrRgbColour > )
		{
			return appendProp( parent, new HdrRgbColourProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::HdrRgbaColour > )
		{
			return appendProp( parent, new HdrRgbaColourProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::ColourWrapper > )
		{
			return appendProp( parent, new wxColourProperty( name, m_prefix + name, wxColour{ toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point2f > )
		{
			return appendProp( parent, new Point2fProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point3f > )
		{
			return appendProp( parent, new Point3fProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point4f > )
		{
			return appendProp( parent, new Point4fProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point2d > )
		{
			return appendProp( parent, new Point2dProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point3d > )
		{
			return appendProp( parent, new Point3dProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point4d > )
		{
			return appendProp( parent, new Point4dProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point2i > )
		{
			return appendProp( parent, new Point2iProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point3i > )
		{
			return appendProp( parent, new Point3iProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point4i > )
		{
			return appendProp( parent, new Point4iProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point2ui > )
		{
			return appendProp( parent, new Point2uiProperty( GC_POINT_XY, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point3ui > )
		{
			return appendProp( parent, new Point3uiProperty( GC_POINT_XYZ, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Point4ui > )
		{
			return appendProp( parent, new Point4uiProperty( GC_POINT_XYZW, name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Matrix4x4f > )
		{
			return appendProp( parent, new Matrix4fProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Quaternion > )
		{
			return appendProp( parent, new QuaternionProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Position > )
		{
			return appendProp( parent, new PositionProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Size > )
		{
			return appendProp( parent, new SizeProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Rectangle > )
		{
			return appendProp( parent, new RectangleProperty( name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::String > )
		{
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, make_wxString( value ) ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Path > )
		{
			return appendProp( parent, new gcImageFileProperty( &m_engine->getImageLoader(), name, m_prefix + name, make_wxString( value ) ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::TextureSourceInfo * > )
		{
			return appendProp( parent, new gcTextureProperty( &m_engine->getImageLoader(), name, m_prefix + name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Milliseconds > )
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
		else if constexpr ( std::is_same_v< ValueT, c3d::BoundingSphere > )
		{
			wxPGProperty * prop = appendProp( parent, new BoundingSphereProperty( name, m_prefix + name, value ) );
			prop->Enable( false );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::BoundingBox > )
		{
			wxPGProperty * prop = appendProp( parent, new BoundingBoxProperty( name, m_prefix + name, value ) );
			prop->Enable( false );
			return prop;
		}
		else if constexpr ( c3d::isSpeedT< ValueT > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.getValue() );
			prop->SetAttribute( wxPG_ATTR_UNITS, ValueTraitsT< ValueT >::getUnit() );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Angle > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.degrees() );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 1.0 ) );
			prop->SetAttribute( wxPG_ATTR_UNITS, ValueTraitsT< ValueT >::getUnit() );
			prop->SetAttribute( wxPG_ATTR_MIN, WXVARIANT( 0.0 ) );
			prop->SetAttribute( wxPG_ATTR_MAX, WXVARIANT( 359.0 ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::LuminousIntensity > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.candela() );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 0.1 ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Illumination > )
		{
			wxPGProperty * prop = createProperty( parent, name, value.lux() );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 0.1 ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, c3d::Font > )
		{
			if ( value.isSDF() )
			{
				wxFontInfo info;
				info.FaceName( value.getFaceName() );
				return appendProp( parent, new wxFontProperty( name, m_prefix + name, wxFont{ info } ) );
			}
			else
			{
				wxFontInfo info( value.getHeight() );
				info.FaceName( value.getFaceName() );
				return appendProp( parent, new wxFontProperty( name, m_prefix + name, wxFont{ info } ) );
			}
		}
		else if constexpr ( c3d::isGroupChangeTrackedT< ValueT > )
		{
			return createProperty( parent, name, value.value() );
		}
		else if constexpr ( c3d::isChangeTrackedT< ValueT > )
		{
			return createProperty( parent, name, value.value() );
		}
		else if constexpr ( c3d::isRangedValueT< ValueT > )
		{
			wxPGProperty * prop = appendProp( parent, new wxFloatProperty( name, m_prefix + name, value.value() ) );
			prop->SetAttribute( SliderEditor::AttrMinValue, WXVARIANT( double( value.range().getMin() ) ) );
			prop->SetAttribute( SliderEditor::AttrMaxValue, WXVARIANT( double( value.range().getMax() ) ) );
			prop->SetEditor( wxPGConstructSliderCtrlEditorClass() );
			prop->SetValidator( SliderEditor::Validator{} );

			if constexpr ( std::is_same_v< c3d::UnRangedValueT< ValueT >, double > || std::is_same_v< c3d::UnRangedValueT< ValueT >, float > )
			{
				prop->SetAttribute( SliderEditor::AttrPrecision, WXVARIANT( 100.0 ) );
			}
			else
			{
				prop->SetAttribute( SliderEditor::AttrPrecision, WXVARIANT( 1.0 ) );
			}
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
		else if constexpr ( std::is_same_v< ValueT, c3d::U32String > )
		{
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, c3d::makeString( value ) ) );
		}
		else
		{
			return appendProp( parent, new wxStringProperty( name, m_prefix + name, value ) );
		}
	}

	template< typename ParentT, typename MyValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::createProperty( ParentT * parent
		, wxString const & name
		, MyValueT const & value
		, PropertyChangeHandler handler
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		m_handlers.emplace( m_prefix + name, doGetHandler( handler, c3d::move( controls ) ) );
		return createProperty( parent, name, value );
	}

	template< typename ParentT, typename EnumT, typename FuncT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyE( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, FuncT func
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, choices
			, [func]( wxVariant const & var )
			{
				func( EnumT( variantCast< uint32_t >( var ) ) );
			}
			, c3d::move( controls ) );
		return prop;
	}

	template< typename ParentT, typename EnumT, typename FuncT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyE( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, EnumT selected
		, FuncT func
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = addPropertyE< ParentT, EnumT, FuncT >( parent
			, name
			, choices
			, func
			, c3d::move( controls ) );
		prop->SetValue( choices[uint32_t( selected )] );
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, PropertyChangeHandler handler
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, value
			, handler
			, c3d::move( controls ) );
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, c3d::Range< ValueT > const & range
		, PropertyChangeHandler handler
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, value
			, handler
			, c3d::move( controls ) );
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
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = addProperty( parent, name, value, handler );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, getVariant< ValueT >( step ) );
		return prop;
	}

	template< typename ParentT, typename ControlT >
	wxPGProperty * TreeItemProperty::addProperty( ParentT * parent
		, wxString const & name
		, c3d::ColourWrapper value
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		wxPGProperty * prop = createProperty( parent
			, name
			, value
			, [value]( wxVariant const & var )
			{
				wxColour col;
				col << var;
				( *value.value )->x = float( col.Red() ) / 255.0f;
				( *value.value )->y = float( col.Green() ) / 255.0f;
				( *value.value )->z = float( col.Blue() ) / 255.0f;
			}
			, c3d::move( controls ) );
		return prop;
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, c3d::RangedValue< ValueT > * value
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT * value
		, c3d::Range< ValueT > const & range
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, range
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, c3d::ChangeTracked< c3d::RangedValue< ValueT > > * value
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, value->value()
			, [value]( wxVariant const & var )
			{
				c3d::RangedValue< ValueT > ranged{ value->value() };
				ranged = variantCast< ValueT >( var );
				*value = ranged;
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT * value
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT * value
		, ValueT step
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		return addProperty( parent
			, name
			, *value
			, step
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT value
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT  value
		, ValueT step
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
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
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, ValueT const & value
		, ObjectT * object
		, ValueRefSetterT< ObjectU, ValueT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename ValueT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, c3d::RangedValue< ValueT > const & value
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( parent
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyET( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addPropertyE( parent
			, name
			, choices
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			}
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyET( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, EnumT selected
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
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
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyET( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, EnumT * value
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls
		, c3d::ConfigurationVisitorBase::OnEnumValueChangeT< EnumT > onChange )
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
			, c3d::move( controls ) );
	}

	template< typename ParentT, typename ObjectT, typename ObjectU, typename EnumT, typename ControlT >
	wxPGProperty * TreeItemProperty::addPropertyT( ParentT * parent
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter
		, c3d::ConfigurationVisitorBase::ControlsListT< ControlT > controls )
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
			, c3d::move( controls ) );
	}

	//*********************************************************************************************

	template< typename ConfigT >
	TreeItemPropertyT< ConfigT >::TreeItemPropertyT( ImagesLoader & imagesLoader
		, bool editable
		, c3d::Engine * engine
		, ConfigT & config )
		: TreeItemProperty{ engine, imagesLoader, editable }
		, m_config{ config }
	{
		CreateTreeItemMenu();
	}

	template< typename ConfigT >
	void TreeItemPropertyT< ConfigT >::doCreateProperties( wxPropertyGrid * grid )
	{
		TreeItemConfigurationBuilder::submit( grid, *this, m_config );
	}

	//*********************************************************************************************
}

#pragma warning( pop )
