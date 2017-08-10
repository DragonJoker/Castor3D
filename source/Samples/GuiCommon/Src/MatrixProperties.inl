#include "PointProperties.hpp"

namespace GuiCommon
{
	//************************************************************************************************

	template<>
	inline castor::SquareMatrix< float, 2 > const & matrixRefFromVariant< float, 2 >( wxVariant const & p_variant )
	{
		return Matrix2x2fRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< float, 3 > const & matrixRefFromVariant< float, 3 >( wxVariant const & p_variant )
	{
		return Matrix3x3fRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< float, 4 > const & matrixRefFromVariant< float, 4 >( wxVariant const & p_variant )
	{
		return Matrix4x4fRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< double, 2 > const & matrixRefFromVariant< double, 2 >( wxVariant const & p_variant )
	{
		return Matrix2x2dRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< double, 3 > const & matrixRefFromVariant< double, 3 >( wxVariant const & p_variant )
	{
		return Matrix3x3dRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< double, 4 > const & matrixRefFromVariant< double, 4 >( wxVariant const & p_variant )
	{
		return Matrix4x4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template<>
	inline castor::SquareMatrix< float, 2 > & matrixRefFromVariant< float, 2 >( wxVariant & p_variant )
	{
		return Matrix2x2fRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< float, 3 > & matrixRefFromVariant< float, 3 >( wxVariant & p_variant )
	{
		return Matrix3x3fRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< float, 4 > & matrixRefFromVariant< float, 4 >( wxVariant & p_variant )
	{
		return Matrix4x4fRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< double, 2 > & matrixRefFromVariant< double, 2 >( wxVariant & p_variant )
	{
		return Matrix2x2dRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< double, 3 > & matrixRefFromVariant< double, 3 >( wxVariant & p_variant )
	{
		return Matrix3x3dRefFromVariant( p_variant );
	}

	template<>
	inline castor::SquareMatrix< double, 4 > & matrixRefFromVariant< double, 4 >( wxVariant & p_variant )
	{
		return Matrix4x4dRefFromVariant( p_variant );
	}

	//************************************************************************************************

	template< typename Type, size_t Count > void setVariantFromMatrix( wxVariant & p_variant, castor::SquareMatrix< Type, Count > const & p_value )
	{
		matrixRefFromVariant< Type, Count >( p_variant ) = p_value;
	}

	//************************************************************************************************

	template< typename T, size_t Count > struct MatrixPropertyHelper
	{
		static void addChildren( MatrixProperty< T, Count > * p_prop, wxString const * p_rowNames, wxString const * p_colNames, castor::SquareMatrix< T, Count > const & p_value )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				wxString names[Count];

				for ( size_t j = 0; j < Count; ++j )
				{
					names[j] << p_colNames[i] << wxT( "." ) << p_rowNames[j];
				}

				wxPGProperty * property = new PointProperty< T, Count >( names, wxString() << _( "Col " ) << p_rowNames[i], wxPG_LABEL, castor::Point< T, Count >( p_value[i].constPtr() ) );
				property->Enable( false );
				p_prop->AddPrivateChild( property );
			}
		}
		static void refreshChildren( MatrixProperty< T, Count > * p_prop )
		{
			castor::SquareMatrix< T, Count > const & matrix = matrixRefFromVariant< T, Count >( p_prop->GetValue() );

			for ( size_t i = 0; i < Count; ++i )
			{
				wxVariant value = p_prop->Item( i )->GetValue();
				setVariantFromPoint< T, Count >( value, castor::Point< T, Count >( matrix[i].constPtr() ) );
				p_prop->Item( i )->SetValue( value );
			}
		}
		static wxVariant childChanged( wxVariant & p_thisValue, int p_index, wxVariant & p_newValue )
		{
			castor::SquareMatrix< T, Count > & matrix = matrixRefFromVariant< T, Count >( p_thisValue );
			castor::Point< T, Count > const & row = PointRefFromVariant< T, Count >( p_newValue );
			matrix.setRow( p_index, row );
			wxVariant result;
			result << matrix;
			return result;
		}
	};

	//************************************************************************************************

	template< size_t Count > wxString const * getMatrixRowDefaultNames();

	template<>
	inline wxString const * getMatrixRowDefaultNames< 2 >()
	{
		return GC_POINT_12;
	}

	template<>
	inline wxString const * getMatrixRowDefaultNames< 3 >()
	{
		return GC_POINT_123;
	}

	template<>
	inline wxString const * getMatrixRowDefaultNames< 4 >()
	{
		return GC_POINT_1234;
	}

	//************************************************************************************************

	template< size_t Count > wxString const * getMatrixColDefaultNames();

	template<>
	inline wxString const * getMatrixColDefaultNames< 2 >()
	{
		return GC_POINT_12;
	}

	template<>
	inline wxString const * getMatrixColDefaultNames< 3 >()
	{
		return GC_POINT_123;
	}

	template<>
	inline wxString const * getMatrixColDefaultNames< 4 >()
	{
		return GC_POINT_1234;
	}

	//************************************************************************************************

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const & label, wxString const & name, castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		MatrixPropertyHelper< T, Count >::addChildren( this, getMatrixRowDefaultNames< Count >(), getMatrixColDefaultNames< Count >(), value );
	}

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const( & p_rowNames )[Count], wxString const( & p_colNames )[Count], wxString const & label, wxString const & name, castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		MatrixPropertyHelper< T, Count >::addChildren( this, p_rowNames, p_colNames, value );
	}

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::~MatrixProperty()
	{
	}

	template< typename T, size_t Count >
	void MatrixProperty< T, Count >::refreshChildren()
	{
		if ( GetChildCount() )
		{
			MatrixPropertyHelper< T, Count >::refreshChildren( this );
		}
	}

	template< typename T, size_t Count >
	wxVariant MatrixProperty< T, Count >::childChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const
	{
		return MatrixPropertyHelper< T, Count >::childChanged( thisValue, childIndex, childValue );
	}

	template< typename T, size_t Count >
	inline void MatrixProperty< T, Count >::setValueI( castor::SquareMatrix< T, Count > const & value )
	{
		m_value = WXVARIANT( value );
	}
}
