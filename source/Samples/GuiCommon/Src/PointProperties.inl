namespace GuiCommon
{
	//************************************************************************************************

	template<>
	inline Castor::Point< bool, 2 > const & PointRefFromVariant< bool, 2 >( wxVariant const & p_variant )
	{
		return Point2bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< bool, 3 > const & PointRefFromVariant< bool, 3 >( wxVariant const & p_variant )
	{
		return Point3bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< bool, 4 > const & PointRefFromVariant< bool, 4 >( wxVariant const & p_variant )
	{
		return Point4bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 2 > const & PointRefFromVariant< int, 2 >( wxVariant const & p_variant )
	{
		return Point2iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 3 > const & PointRefFromVariant< int, 3 >( wxVariant const & p_variant )
	{
		return Point3iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 4 > const & PointRefFromVariant< int, 4 >( wxVariant const & p_variant )
	{
		return Point4iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 2 > const & PointRefFromVariant< uint32_t, 2 >( wxVariant const & p_variant )
	{
		return Point2uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 3 > const & PointRefFromVariant< uint32_t, 3 >( wxVariant const & p_variant )
	{
		return Point3uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 4 > const & PointRefFromVariant< uint32_t, 4 >( wxVariant const & p_variant )
	{
		return Point4uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 2 > const & PointRefFromVariant< float, 2 >( wxVariant const & p_variant )
	{
		return Point2fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 3 > const & PointRefFromVariant< float, 3 >( wxVariant const & p_variant )
	{
		return Point3fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 4 > const & PointRefFromVariant< float, 4 >( wxVariant const & p_variant )
	{
		return Point4fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 2 > const & PointRefFromVariant< double, 2 >( wxVariant const & p_variant )
	{
		return Point2dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 3 > const & PointRefFromVariant< double, 3 >( wxVariant const & p_variant )
	{
		return Point3dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 4 > const & PointRefFromVariant< double, 4 >( wxVariant const & p_variant )
	{
		return Point4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template<>
	inline Castor::Point< bool, 2 > & PointRefFromVariant< bool, 2 >( wxVariant & p_variant )
	{
		return Point2bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< bool, 3 > & PointRefFromVariant< bool, 3 >( wxVariant & p_variant )
	{
		return Point3bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< bool, 4 > & PointRefFromVariant< bool, 4 >( wxVariant & p_variant )
	{
		return Point4bRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 2 > & PointRefFromVariant< int, 2 >( wxVariant & p_variant )
	{
		return Point2iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 3 > & PointRefFromVariant< int, 3 >( wxVariant & p_variant )
	{
		return Point3iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< int, 4 > & PointRefFromVariant< int, 4 >( wxVariant & p_variant )
	{
		return Point4iRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 2 > & PointRefFromVariant< uint32_t, 2 >( wxVariant & p_variant )
	{
		return Point2uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 3 > & PointRefFromVariant< uint32_t, 3 >( wxVariant & p_variant )
	{
		return Point3uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< uint32_t, 4 > & PointRefFromVariant< uint32_t, 4 >( wxVariant & p_variant )
	{
		return Point4uiRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 2 > & PointRefFromVariant< float, 2 >( wxVariant & p_variant )
	{
		return Point2fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 3 > & PointRefFromVariant< float, 3 >( wxVariant & p_variant )
	{
		return Point3fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< float, 4 > & PointRefFromVariant< float, 4 >( wxVariant & p_variant )
	{
		return Point4fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 2 > & PointRefFromVariant< double, 2 >( wxVariant & p_variant )
	{
		return Point2dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 3 > & PointRefFromVariant< double, 3 >( wxVariant & p_variant )
	{
		return Point3dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::Point< double, 4 > & PointRefFromVariant< double, 4 >( wxVariant & p_variant )
	{
		return Point4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template< typename Type, size_t Count > void SetVariantFromPoint( wxVariant & p_variant, Castor::Point< Type, Count > const & p_value )
	{
		PointRefFromVariant< Type, Count >( p_variant ) = p_value;
	}

	//************************************************************************************************

	template< size_t Count > wxString const * GetPointDefaultNames();

	template<>
	inline wxString const * GetPointDefaultNames< 2 >()
	{
		return GC_POINT_XY;
	}

	template<>
	inline wxString const * GetPointDefaultNames< 3 >()
	{
		return GC_POINT_XYZ;
	}

	template<>
	inline wxString const * GetPointDefaultNames< 4 >()
	{
		return GC_POINT_XYZW;
	}

	//************************************************************************************************

	template< typename T, size_t Count > struct PointPropertyHelper
	{
		static void AddChildren( PointProperty< T, Count > * p_prop, wxString const * p_names, Castor::Point< T, Count > const & p_value )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				p_prop->AddPrivateChild( CreateProperty( p_names[i], p_value[i] ) );
			}
		}
		static void RefreshChildren( PointProperty< T, Count > * p_prop )
		{
			Castor::Point< T, Count > const & l_point = PointRefFromVariant< T, Count >( p_prop->GetValue() );

			for ( size_t i = 0; i < Count; ++i )
			{
				p_prop->Item( i )->SetValue( SetValue( l_point[i] ) );
			}
		}
		static wxVariant ChildChanged( wxVariant & p_thisValue, int p_index, wxVariant & p_newValue )
		{
			Castor::Point< T, Count > & l_point = PointRefFromVariant< T, Count >( p_thisValue );
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

			wxVariant l_result;
			l_result << l_point;
			return l_result;
		}
	};

	//************************************************************************************************

	template< typename T, size_t Count >
	PointProperty< T, Count >::PointProperty( wxString const & label, wxString const & name, Castor::Point< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		PointPropertyHelper< T, Count >::AddChildren( this, GetPointDefaultNames< Count >(), value );
	}

	template< typename T, size_t Count >
	PointProperty< T, Count >::PointProperty( wxString const( & p_names )[Count], wxString const & label, wxString const & name, Castor::Point< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		PointPropertyHelper< T, Count >::AddChildren( this, p_names, value );
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

	//************************************************************************************************
}
