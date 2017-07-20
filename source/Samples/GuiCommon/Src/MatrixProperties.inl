#include "PointProperties.hpp"

namespace GuiCommon
{
	//************************************************************************************************

	template<>
	inline Castor::SquareMatrix< float, 2 > const & MatrixRefFromVariant< float, 2 >( wxVariant const & p_variant )
	{
		return Matrix2x2fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< float, 3 > const & MatrixRefFromVariant< float, 3 >( wxVariant const & p_variant )
	{
		return Matrix3x3fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< float, 4 > const & MatrixRefFromVariant< float, 4 >( wxVariant const & p_variant )
	{
		return Matrix4x4fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< double, 2 > const & MatrixRefFromVariant< double, 2 >( wxVariant const & p_variant )
	{
		return Matrix2x2dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< double, 3 > const & MatrixRefFromVariant< double, 3 >( wxVariant const & p_variant )
	{
		return Matrix3x3dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< double, 4 > const & MatrixRefFromVariant< double, 4 >( wxVariant const & p_variant )
	{
		return Matrix4x4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template<>
	inline Castor::SquareMatrix< float, 2 > & MatrixRefFromVariant< float, 2 >( wxVariant & p_variant )
	{
		return Matrix2x2fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< float, 3 > & MatrixRefFromVariant< float, 3 >( wxVariant & p_variant )
	{
		return Matrix3x3fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< float, 4 > & MatrixRefFromVariant< float, 4 >( wxVariant & p_variant )
	{
		return Matrix4x4fRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< double, 2 > & MatrixRefFromVariant< double, 2 >( wxVariant & p_variant )
	{
		return Matrix2x2dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< double, 3 > & MatrixRefFromVariant< double, 3 >( wxVariant & p_variant )
	{
		return Matrix3x3dRefFromVariant( p_variant );
	}

	template<>
	inline Castor::SquareMatrix< double, 4 > & MatrixRefFromVariant< double, 4 >( wxVariant & p_variant )
	{
		return Matrix4x4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template< typename Type, size_t Count > void SetVariantFromMatrix( wxVariant & p_variant, Castor::SquareMatrix< Type, Count > const & p_value )
	{
		MatrixRefFromVariant< Type, Count >( p_variant ) = p_value;
	}

	//************************************************************************************************

	template< typename T, size_t Count > struct MatrixPropertyHelper
	{
		static void AddChildren( MatrixProperty< T, Count > * p_prop, wxString const * p_rowNames, wxString const * p_colNames, Castor::SquareMatrix< T, Count > const & p_value )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				wxString names[Count];

				for ( size_t j = 0; j < Count; ++j )
				{
					names[j] << p_colNames[i] << wxT( "." ) << p_rowNames[j];
				}

				wxPGProperty * property = new PointProperty< T, Count >( names, wxString() << _( "Col " ) << p_rowNames[i], wxPG_LABEL, Castor::Point< T, Count >( p_value[i].const_ptr() ) );
				property->Enable( false );
				p_prop->AddPrivateChild( property );
			}
		}
		static void RefreshChildren( MatrixProperty< T, Count > * p_prop )
		{
			Castor::SquareMatrix< T, Count > const & matrix = MatrixRefFromVariant< T, Count >( p_prop->GetValue() );

			for ( size_t i = 0; i < Count; ++i )
			{
				wxVariant value = p_prop->Item( i )->GetValue();
				SetVariantFromPoint< T, Count >( value, Castor::Point< T, Count >( matrix[i].const_ptr() ) );
				p_prop->Item( i )->SetValue( value );
			}
		}
		static wxVariant ChildChanged( wxVariant & p_thisValue, int p_index, wxVariant & p_newValue )
		{
			Castor::SquareMatrix< T, Count > & matrix = MatrixRefFromVariant< T, Count >( p_thisValue );
			Castor::Point< T, Count > const & row = PointRefFromVariant< T, Count >( p_newValue );
			matrix.set_row( p_index, row );
			wxVariant result;
			result << matrix;
			return result;
		}
	};

	//************************************************************************************************

	template< size_t Count > wxString const * GetMatrixRowDefaultNames();

	template<>
	inline wxString const * GetMatrixRowDefaultNames< 2 >()
	{
		return GC_POINT_12;
	}

	template<>
	inline wxString const * GetMatrixRowDefaultNames< 3 >()
	{
		return GC_POINT_123;
	}

	template<>
	inline wxString const * GetMatrixRowDefaultNames< 4 >()
	{
		return GC_POINT_1234;
	}

	//************************************************************************************************

	template< size_t Count > wxString const * GetMatrixColDefaultNames();

	template<>
	inline wxString const * GetMatrixColDefaultNames< 2 >()
	{
		return GC_POINT_12;
	}

	template<>
	inline wxString const * GetMatrixColDefaultNames< 3 >()
	{
		return GC_POINT_123;
	}

	template<>
	inline wxString const * GetMatrixColDefaultNames< 4 >()
	{
		return GC_POINT_1234;
	}

	//************************************************************************************************

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const & label, wxString const & name, Castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		MatrixPropertyHelper< T, Count >::AddChildren( this, GetMatrixRowDefaultNames< Count >(), GetMatrixColDefaultNames< Count >(), value );
	}

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const( & p_rowNames )[Count], wxString const( & p_colNames )[Count], wxString const & label, wxString const & name, Castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		MatrixPropertyHelper< T, Count >::AddChildren( this, p_rowNames, p_colNames, value );
	}

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::~MatrixProperty()
	{
	}

	template< typename T, size_t Count >
	void MatrixProperty< T, Count >::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			MatrixPropertyHelper< T, Count >::RefreshChildren( this );
		}
	}

	template< typename T, size_t Count >
	wxVariant MatrixProperty< T, Count >::ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		return MatrixPropertyHelper< T, Count >::ChildChanged( thisValue, childIndex, childValue );
	}

	template< typename T, size_t Count >
	inline void MatrixProperty< T, Count >::SetValueI( Castor::SquareMatrix< T, Count > const & value )
	{
		m_value = WXVARIANT( value );
	}
}
