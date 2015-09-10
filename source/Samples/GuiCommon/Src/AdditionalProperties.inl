namespace GuiCommon
{
	inline void SizeProperty::SetValueI( Castor::Size const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************

	inline void PositionProperty::SetValueI( Castor::Position const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************

	inline void RectangleProperty::SetValueI( Castor::Point4i const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************

	void QuaternionProperty::SetValueI( Castor::Quaternion const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************

	template<>
	inline Castor::Point< bool, 2 > const & ValueRefFromVariant< bool, 2 >( wxVariant const & p_variant )
	{
		return Point2bRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< bool, 3 > const & ValueRefFromVariant< bool, 3 >( wxVariant const & p_variant )
	{
		return Point3bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< bool, 4 > const & ValueRefFromVariant< bool, 4 >( wxVariant const & p_variant )
	{
		return Point4bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 2 > const & ValueRefFromVariant< int, 2 >( wxVariant const & p_variant )
	{
		return Point2iRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< int, 3 > const & ValueRefFromVariant< int, 3 >( wxVariant const & p_variant )
	{
		return Point3iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 4 > const & ValueRefFromVariant< int, 4 >( wxVariant const & p_variant )
	{
		return Point4iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 2 > const & ValueRefFromVariant< uint32_t, 2 >( wxVariant const & p_variant )
	{
		return Point2uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 3 > const & ValueRefFromVariant< uint32_t, 3 >( wxVariant const & p_variant )
	{
		return Point3uiRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< uint32_t, 4 > const & ValueRefFromVariant< uint32_t, 4 >( wxVariant const & p_variant )
	{
		return Point4uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 2 > const & ValueRefFromVariant< float, 2 >( wxVariant const & p_variant )
	{
		return Point2fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 3 > const & ValueRefFromVariant< float, 3 >( wxVariant const & p_variant )
	{
		return Point3fRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< float, 4 > const & ValueRefFromVariant< float, 4 >( wxVariant const & p_variant )
	{
		return Point4fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 2 > const & ValueRefFromVariant< double, 2 >( wxVariant const & p_variant )
	{
		return Point2dRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< double, 3 > const & ValueRefFromVariant< double, 3 >( wxVariant const & p_variant )
	{
		return Point3dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 4 > const & ValueRefFromVariant< double, 4 >( wxVariant const & p_variant )
	{
		return Point4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template<>
	inline Castor::Point< bool, 2 > & ValueRefFromVariant< bool, 2 >( wxVariant & p_variant )
	{
		return Point2bRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< bool, 3 > & ValueRefFromVariant< bool, 3 >( wxVariant & p_variant )
	{
		return Point3bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< bool, 4 > & ValueRefFromVariant< bool, 4 >( wxVariant & p_variant )
	{
		return Point4bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 2 > & ValueRefFromVariant< int, 2 >( wxVariant & p_variant )
	{
		return Point2iRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< int, 3 > & ValueRefFromVariant< int, 3 >( wxVariant & p_variant )
	{
		return Point3iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 4 > & ValueRefFromVariant< int, 4 >( wxVariant & p_variant )
	{
		return Point4iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 2 > & ValueRefFromVariant< uint32_t, 2 >( wxVariant & p_variant )
	{
		return Point2uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 3 > & ValueRefFromVariant< uint32_t, 3 >( wxVariant & p_variant )
	{
		return Point3uiRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< uint32_t, 4 > & ValueRefFromVariant< uint32_t, 4 >( wxVariant & p_variant )
	{
		return Point4uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 2 > & ValueRefFromVariant< float, 2 >( wxVariant & p_variant )
	{
		return Point2fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 3 > & ValueRefFromVariant< float, 3 >( wxVariant & p_variant )
	{
		return Point3fRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< float, 4 > & ValueRefFromVariant< float, 4 >( wxVariant & p_variant )
	{
		return Point4fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 2 > & ValueRefFromVariant< double, 2 >( wxVariant & p_variant )
	{
		return Point2dRefFromVariant( p_variant );
	}
	
	template<>
	inline Castor::Point< double, 3 > & ValueRefFromVariant< double, 3 >( wxVariant & p_variant )
	{
		return Point3dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 4 > & ValueRefFromVariant< double, 4 >( wxVariant & p_variant )
	{
		return Point4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	inline wxPGProperty * CreateProperty( wxString const & p_name, double const & p_value )
	{
		return new wxFloatProperty( p_name, wxPG_LABEL, p_value );
	}

	inline wxPGProperty * CreateProperty( wxString const & p_name, float const & p_value )
	{
		return new wxFloatProperty( p_name, wxPG_LABEL, p_value );
	}

	inline wxPGProperty * CreateProperty( wxString const & p_name, int const & p_value )
	{
		return new wxIntProperty( p_name, wxPG_LABEL, p_value );
	}

	inline wxPGProperty * CreateProperty( wxString const & p_name, uint32_t const & p_value )
	{
		return new wxUIntProperty( p_name, wxPG_LABEL, p_value );
	}

	inline wxPGProperty * CreateProperty( wxString const & p_name, bool const & p_value )
	{
		return new wxBoolProperty( p_name, wxPG_LABEL, p_value );
	}

	template< typename T > T GetValue( wxVariant const & p_variant );

	template<>
	inline bool GetValue< bool >( wxVariant const & p_variant )
	{
		return p_variant.GetBool();
	}

	template<>
	inline int GetValue< int >( wxVariant const & p_variant )
	{
		return p_variant.GetInteger();
	}

	template<>
	inline uint32_t GetValue< uint32_t >( wxVariant const & p_variant )
	{
		return uint32_t( p_variant.GetInteger() );
	}

	template<>
	inline float GetValue< float >( wxVariant const & p_variant )
	{
		return p_variant.GetDouble();
	}

	template<>
	inline double GetValue< double >( wxVariant const & p_variant )
	{
		return p_variant.GetDouble();
	}

	template< typename T > wxVariant SetValue( T const & p_value );

	template<>
	inline wxVariant SetValue< bool >( bool const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant SetValue< int >( int const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant SetValue< uint32_t >( uint32_t const & p_value )
	{
		return wxVariant( int( p_value ) );
	}

	template<>
	inline wxVariant SetValue< float >( float const & p_value )
	{
		return wxVariant( p_value );
	}

	template<>
	inline wxVariant SetValue< double >( double const & p_value )
	{
		return wxVariant( p_value );
	}

	//************************************************************************************************
	
	template< typename T, size_t Count > struct PointPropertyHelper
	{
		static void AddChildren( PointProperty< T, Count > * p_prop, Castor::Point< T, Count > const & p_value )
		{
			static wxString ChildName[4] =
			{
				wxT( "X" ),
				wxT( "Y" ),
				wxT( "Z" ),
				wxT( "W" ),
			};

			for ( size_t i = 0; i < Count; ++i )
			{
				p_prop->AddPrivateChild( CreateProperty( ChildName[i], p_value[i] ) );
			}
		}
		static void RefreshChildren( PointProperty< T, Count > * p_prop )
		{
			Castor::Point< T, Count > const & l_point = ValueRefFromVariant< T, Count >( p_prop->GetValue() );

			for ( size_t i = 0; i < Count; ++i )
			{
				p_prop->Item( i )->SetValue( SetValue( l_point[i] ) );
			}
		}
		static wxVariant ChildChanged( wxVariant & p_thisValue, int p_index, wxVariant & p_newValue )
		{
			Castor::Point< T, Count > & l_point = ValueRefFromVariant< T, Count >( p_thisValue );
			T val = GetValue< T >( p_newValue );

			switch ( p_index )
			{
			case 0:
				l_point[0] = val;
				break;

			case 1:
				l_point[1] = val;
				break;

			case 2:
				l_point[2] = val;
				break;

			case 3:
				l_point[3] = val;
				break;
			}

			wxVariant l_return;
			l_return << l_point;
			return l_return;
		}
	};

	//************************************************************************************************
	
	template< typename T, size_t Count >
	PointProperty< T, Count >::PointProperty( wxString const & label, wxString const & name, Castor::Point< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		PointPropertyHelper< T, Count >::AddChildren( this, value );
	}

	template< typename T, size_t Count >
	PointProperty< T, Count >::~PointProperty()
	{
	}

	template< typename T, size_t Count >
	void PointProperty< T, Count >::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			PointPropertyHelper< T, Count >::RefreshChildren( this );
		}
	}

	template< typename T, size_t Count >
	wxVariant PointProperty< T, Count >::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		return PointPropertyHelper< T, Count >::ChildChanged( thisValue, childIndex, childValue );
	}

	template< typename T, size_t Count >
	inline void PointProperty< T, Count >::SetValueI( Castor::Point< T, Count > const & value )
	{
		m_value = WXVARIANT( value );
	}
}
