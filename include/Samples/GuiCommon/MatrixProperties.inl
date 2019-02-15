#include "PointProperties.hpp"

namespace GuiCommon
{
	//************************************************************************************************

	template<>
	inline castor::SquareMatrix< float, 2 > const & matrixRefFromVariant< float, 2 >( wxVariant const & variant )
	{
		return Matrix2x2fRefFromVariant( variant );
	}

	template<>
	inline castor::SquareMatrix< float, 3 > const & matrixRefFromVariant< float, 3 >( wxVariant const & variant )
	{
		return Matrix3x3fRefFromVariant( variant );
	}

	template<>
	inline castor::SquareMatrix< float, 4 > const & matrixRefFromVariant< float, 4 >( wxVariant const & variant )
	{
		return Matrix4x4fRefFromVariant( variant );
	}

	//************************************************************************************************

	template<>
	inline castor::SquareMatrix< float, 2 > & matrixRefFromVariant< float, 2 >( wxVariant & variant )
	{
		return Matrix2x2fRefFromVariant( variant );
	}

	template<>
	inline castor::SquareMatrix< float, 3 > & matrixRefFromVariant< float, 3 >( wxVariant & variant )
	{
		return Matrix3x3fRefFromVariant( variant );
	}

	template<>
	inline castor::SquareMatrix< float, 4 > & matrixRefFromVariant< float, 4 >( wxVariant & variant )
	{
		return Matrix4x4fRefFromVariant( variant );
	}

	//************************************************************************************************

	template< typename Type, size_t Count > void setVariantFromMatrix( wxVariant & variant
		, castor::SquareMatrix< Type, Count > const & value )
	{
		matrixRefFromVariant< Type, Count >( variant ) = value;
	}

	//************************************************************************************************

	template< typename T, size_t Count > struct MatrixPropertyHelper
	{
		static void addChildren( MatrixProperty< T, Count > * prop
			, wxString const * rowNames
			, wxString const * colNames
			, castor::SquareMatrix< T, Count > const & value )
		{
			for ( size_t i = 0; i < Count; ++i )
			{
				wxString names[Count];

				for ( size_t j = 0; j < Count; ++j )
				{
					names[j] << colNames[i] << wxT( "." ) << rowNames[j];
				}

				wxPGProperty * property = new PointProperty< T, Count >( names
					, wxString() << _( "Col " ) << rowNames[i]
					, wxPG_LABEL
					, castor::Point< T, Count >( value[i].constPtr() ) );
				property->Enable( false );
				prop->AddPrivateChild( property );
			}
		}

		static void refreshChildren( MatrixProperty< T, Count > * prop )
		{
			castor::SquareMatrix< T, Count > const & matrix = matrixRefFromVariant< T, Count >( prop->GetValue() );

			for ( size_t i = 0; i < Count; ++i )
			{
				wxVariant value = prop->Item( i )->GetValue();
				setVariantFromPoint< T, Count >( value, castor::Point< T, Count >( matrix[i].constPtr() ) );
				prop->Item( i )->SetValue( value );
			}
		}
		static wxVariant childChanged( wxVariant & thisValue
			, int index
			, wxVariant & newValue )
		{
			castor::SquareMatrix< T, Count > & matrix = matrixRefFromVariant< T, Count >( thisValue );
			castor::Point< T, Count > const & row = PointRefFromVariant< T, Count >( newValue );
			matrix.setRow( index, row );
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
	MatrixProperty< T, Count >::MatrixProperty( wxString const & label
		, wxString const & name
		, castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		MatrixPropertyHelper< T, Count >::addChildren( this
			, getMatrixRowDefaultNames< Count >()
			, getMatrixColDefaultNames< Count >()
			, value );
	}

	template< typename T, size_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const( & rowNames )[Count]
		, wxString const( & colNames )[Count]
		, wxString const & label
		, wxString const & name
		, castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		MatrixPropertyHelper< T, Count >::addChildren( this
			, rowNames
			, colNames
			, value );
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
			MatrixPropertyHelper< T, Count >::refreshChildren( this );
		}
	}

	template< typename T, size_t Count >
	wxVariant MatrixProperty< T, Count >::ChildChanged( wxVariant & thisValue
		, int childIndex
		, wxVariant & childValue ) const
	{
		return MatrixPropertyHelper< T, Count >::childChanged( thisValue
			, childIndex
			, childValue );
	}

	template< typename T, size_t Count >
	inline void MatrixProperty< T, Count >::setValueI( castor::SquareMatrix< T, Count > const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************
}
