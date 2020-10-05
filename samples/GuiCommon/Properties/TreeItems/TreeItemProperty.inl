namespace GuiCommon
{
	template< typename MyValueT >
	wxPGProperty * TreeItemProperty::createProperty( wxPropertyGrid * grid
		, wxString const & name
		, MyValueT && value
		, PropertyChangeHandler handler )
	{
		using ValueT = std::remove_cv_t< std::remove_reference_t< MyValueT > >;
		m_handlers.emplace( name, handler );

		if constexpr ( std::is_same_v< ValueT, bool > )
		{
			wxPGProperty * prop = grid->Append( new wxBoolProperty( name, name, value ) );
			prop->SetAttribute( wxPG_BOOL_USE_CHECKBOX, true );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint8_t > )
		{
			wxPGProperty * prop = grid->Append( new wxUIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< uint8_t >( std::numeric_limits< uint8_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< uint8_t >( std::numeric_limits< uint8_t >::max() ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, int16_t > )
		{
			wxPGProperty * prop = grid->Append( new wxIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< int16_t >( std::numeric_limits< int16_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< int16_t >( std::numeric_limits< int16_t >::max() ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint16_t > )
		{
			wxPGProperty * prop = grid->Append( new wxUIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< uint16_t >( std::numeric_limits< uint16_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< uint16_t >( std::numeric_limits< uint16_t >::max() ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, int32_t > )
		{
			wxPGProperty * prop = grid->Append( new wxIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< int32_t >( std::numeric_limits< int32_t >::lowest() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< int32_t >( std::numeric_limits< int32_t >::max() ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint32_t > )
		{
			wxPGProperty * prop = grid->Append( new wxUIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_MIN, 0 );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, int64_t > )
		{
			wxPGProperty * prop = grid->Append( new wxIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, uint64_t > )
		{
			wxPGProperty * prop = grid->Append( new wxUIntProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_MIN, 0 );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, float > )
		{
			wxPGProperty * prop = grid->Append( new wxFloatProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, double > )
		{
			wxPGProperty * prop = grid->Append( new wxFloatProperty( name, name, value ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::RgbColour > )
		{
			return grid->Append( new wxColourProperty( name, name, wxColour{ toBGRPacked( value ) } ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2f > )
		{
			return grid->Append( new Point2fProperty( GC_POINT_XY, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3f > )
		{
			return grid->Append( new Point3fProperty( GC_POINT_XYZ, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4f > )
		{
			return grid->Append( new Point4fProperty( GC_POINT_XYZW, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2d > )
		{
			return grid->Append( new Point2dProperty( GC_POINT_XY, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3d > )
		{
			return grid->Append( new Point3dProperty( GC_POINT_XYZ, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4d > )
		{
			return grid->Append( new Point4dProperty( GC_POINT_XYZW, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2i > )
		{
			return grid->Append( new Point2iProperty( GC_POINT_XY, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3i > )
		{
			return grid->Append( new Point3iProperty( GC_POINT_XYZ, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4i > )
		{
			return grid->Append( new Point4iProperty( GC_POINT_XYZW, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point2ui > )
		{
			return grid->Append( new Point2uiProperty( GC_POINT_XY, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point3ui > )
		{
			return grid->Append( new Point3uiProperty( GC_POINT_XYZ, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Point4ui > )
		{
			return grid->Append( new Point4uiProperty( GC_POINT_XYZW, name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Matrix4x4f > )
		{
			return grid->Append( new Matrix4fProperty( name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Quaternion > )
		{
			return grid->Append( new QuaternionProperty( name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Position > )
		{
			return grid->Append( new PositionProperty( name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Size > )
		{
			return grid->Append( new SizeProperty( name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Rectangle > )
		{
			return grid->Append( new RectangleProperty( name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::String > )
		{
			return grid->Append( new wxStringProperty( name, name, make_wxString( value ) ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Path > )
		{
			return grid->Append( new wxImageFileProperty( name, name, make_wxString( value ) ) );
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Milliseconds > )
		{
			wxPGProperty * prop = grid->Append( new wxFloatProperty( name, name, value.count() / 1000.0 ) );
			prop->SetEditor( wxPGEditor_SpinCtrl );
			prop->SetAttribute( wxPG_ATTR_UNITS, wxT( "s" ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 0.1 ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::BoundingSphere > )
		{
			wxPGProperty * prop = grid->Append( new BoundingSphereProperty( name, name, value ) );
			prop->Enable( false );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::BoundingBox > )
		{
			wxPGProperty * prop = grid->Append( new BoundingBoxProperty( name, name, value ) );
			prop->Enable( false );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Angle > )
		{
			wxPGProperty * prop = createProperty( grid, name, value.degrees(), handler );
			prop->SetAttribute( wxPG_ATTR_UNITS, wxT( "°" ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, WXVARIANT( 1.0 ) );
			prop->SetAttribute( wxPG_ATTR_MIN, WXVARIANT( 0.0 ) );
			prop->SetAttribute( wxPG_ATTR_MAX, WXVARIANT( 359.0 ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, castor::Font > )
		{
			wxFontInfo info( value.getHeight() );
			info.FaceName( value.getFaceName() );
			return grid->Append( new wxFontProperty( name, name, wxFont{ info } ) );
		}
		else if constexpr ( castor::isChangeTrackedT< ValueT > )
		{
			return createProperty( grid, name, value.value(), handler );
		}
		else if constexpr ( castor::isRangedValueT< ValueT > )
		{
			wxPGProperty * prop = createProperty( grid, name, value.value(), handler );
			prop->SetAttribute( wxPG_ATTR_MIN, getVariant< castor::UnRangedValueT< ValueT > >( value.range().getMin() ) );
			prop->SetAttribute( wxPG_ATTR_MAX, getVariant< castor::UnRangedValueT< ValueT > >( value.range().getMax() ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_WRAP, WXVARIANT( true ) );
			prop->SetAttribute( wxPG_ATTR_SPINCTRL_MOTION, WXVARIANT( true ) );
			return prop;
		}
		else if constexpr ( std::is_same_v< ValueT, wxString > )
		{
			return grid->Append( new wxStringProperty( name, name, value ) );
		}
		else if constexpr ( std::is_same_v< ValueT, wxArrayString > )
		{
			return grid->Append( new wxEnumProperty( name, name, value ) );
		}
		else
		{
			static_assert( false, "Unsupported ValueT" );
		}
	}

	template< typename EnumT, typename FuncT >
	wxPGProperty * TreeItemProperty::addPropertyE( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, FuncT func )
	{
		wxPGProperty * prop = createProperty( grid
			, name
			, choices
			, [choices, func, this]( wxVariant const & var )
			{
				func( EnumT( variantCast< uint32_t >( var ) ) );
			} );
		return prop;
	}

	template< typename EnumT, typename FuncT >
	wxPGProperty * TreeItemProperty::addPropertyE( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, EnumT selected
		, FuncT func )
	{
		wxPGProperty * prop = addPropertyE< EnumT, FuncT >( grid
			, name
			, choices
			, func );
		prop->SetValue( choices[uint32_t( selected )] );
		return prop;
	}

	template< typename ValueT >
	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, ValueT const & value
		, PropertyChangeHandler handler )
	{
		wxPGProperty * prop = createProperty( grid, name, value, handler );
		return prop;
	}

	template< typename ValueT >
	wxPGProperty * TreeItemProperty::addProperty( wxPropertyGrid * grid
		, wxString const & name
		, ValueT const & value
		, ValueT const & step
		, PropertyChangeHandler handler )
	{
		wxPGProperty * prop = addProperty( grid, name, value, handler );
		prop->SetAttribute( wxPG_ATTR_SPINCTRL_STEP, getVariant< ValueT >( step ) );
		return prop;
	}

	template< typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, castor::RangedValue< ValueT > * value )
	{
		return addProperty( grid
			, name
			, *value
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			} );
	}

	template< typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, castor::ChangeTracked< castor::RangedValue< ValueT > > * value )
	{
		return addProperty( grid
			, name
			, value->value()
			, [value]( wxVariant const & var )
			{
				castor::RangedValue< ValueT > ranged{ value->value() };
				ranged = variantCast< ValueT >( var );
				*value = ranged;
			} );
	}

	template< typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, ValueT * value )
	{
		return addProperty( grid
			, name
			, *value
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			} );
	}

	template< typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, ValueT * value
		, ValueT step )
	{
		return addProperty( grid
			, name
			, *value
			, step
			, [value]( wxVariant const & var )
			{
				*value = variantCast< ValueT >( var );
			} );
	}

	template< typename ObjectT, typename ObjectU, typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, ValueT value
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( grid
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			} );
	}

	template< typename ObjectT, typename ObjectU, typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, ValueT  value
		, ValueT step
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( grid
			, name
			, value
			, step
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			} );
	}

	template< typename ObjectT, typename ObjectU, typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, ValueT const & value
		, ObjectT * object
		, ValueRefSetterT< ObjectU, ValueT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( grid
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			} );
	}

	template< typename ObjectT, typename ObjectU, typename ValueT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, castor::RangedValue< ValueT > const & value
		, ObjectT * object
		, ValueSetterT< ObjectU, ValueT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( grid
			, name
			, value
			, [object, setter]( wxVariant const & var )
			{
				( object->*setter )( variantCast< ValueT >( var ) );
			} );
	}

	template< typename ObjectT, typename ObjectU, typename EnumT >
	wxPGProperty * TreeItemProperty::addPropertyET( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addPropertyE( grid
			, name
			, choices
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			} );
	}

	template< typename ObjectT, typename ObjectU, typename EnumT >
	wxPGProperty * TreeItemProperty::addPropertyET( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, EnumT selected
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addPropertyE( grid
			, name
			, choices
			, selected
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			} );
	}

	template< typename EnumT >
	wxPGProperty * TreeItemProperty::addPropertyET( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, EnumT * value )
	{
		return addPropertyE( grid
			, name
			, choices
			, *value
			, [value]( EnumT type )
			{
				*value  = type;
			} );
	}

	template< typename ObjectT, typename ObjectU, typename EnumT >
	wxPGProperty * TreeItemProperty::addPropertyT( wxPropertyGrid * grid
		, wxString const & name
		, wxArrayString const & choices
		, wxString const & selected
		, ObjectT * object
		, ValueSetterT< ObjectU, EnumT > setter )
	{
		static_assert( std::is_base_of_v< ObjectU, ObjectT > || std::is_same_v< ObjectU, ObjectT >, "Can't call a function on unrelated types" );
		return addProperty( grid
			, name
			, choices
			, selected
			, [object, setter]( EnumT type )
			{
				( object->*setter )( type );
			} );
	}
}
