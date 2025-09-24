#include "GuiCommon/Properties/Math/MatrixProperties.hpp"
#include "GuiCommon/Properties/Math/PointProperties.hpp"

namespace GuiCommon
{
	//************************************************************************************************

	template<>
	inline c3d::SquareMatrix< float, 2 > const & matrixRefFromVariant< float, 2 >( wxVariant const & variant )
	{
		return Matrix2x2fRefFromVariant( variant );
	}

	template<>
	inline c3d::SquareMatrix< float, 3 > const & matrixRefFromVariant< float, 3 >( wxVariant const & variant )
	{
		return Matrix3x3fRefFromVariant( variant );
	}

	template<>
	inline c3d::SquareMatrix< float, 4 > const & matrixRefFromVariant< float, 4 >( wxVariant const & variant )
	{
		return Matrix4x4fRefFromVariant( variant );
	}

	//************************************************************************************************

	template<>
	inline c3d::SquareMatrix< float, 2 > & matrixRefFromVariant< float, 2 >( wxVariant & variant )
	{
		return Matrix2x2fRefFromVariant( variant );
	}

	template<>
	inline c3d::SquareMatrix< float, 3 > & matrixRefFromVariant< float, 3 >( wxVariant & variant )
	{
		return Matrix3x3fRefFromVariant( variant );
	}

	template<>
	inline c3d::SquareMatrix< float, 4 > & matrixRefFromVariant< float, 4 >( wxVariant & variant )
	{
		return Matrix4x4fRefFromVariant( variant );
	}

	//************************************************************************************************

	template< typename Type, uint32_t Count > void setVariantFromMatrix( wxVariant & variant
		, c3d::SquareMatrix< Type, Count > const & value )
	{
		matrixRefFromVariant< Type, Count >( variant ) = value;
	}

	//************************************************************************************************

	template< typename T, uint32_t Count > struct MatrixPropertyHelper
	{
		static void addChildren( MatrixProperty< T, Count > * prop
			, c3d::Array< wxString, Count > const & rowNames
			, c3d::Array< wxString, Count > const & colNames
			, c3d::SquareMatrix< T, Count > const & value )
		{
			for ( uint32_t i = 0; i < Count; ++i )
			{
				c3d::Array< wxString, Count > names;

				for ( uint32_t j = 0; j < Count; ++j )
				{
					names[j] << colNames[i] << wxT( "." ) << rowNames[j];
				}

				wxPGProperty * property = new PointProperty< T, Count >{ names
					, wxString() << _( "Col " ) << rowNames[i]
					, wxPG_LABEL
					, c3d::Point< T, Count >( value[i].constPtr() ) };
				property->Enable( false );
				prop->AddPrivateChild( property );
			}
		}

		static void refreshChildren( MatrixProperty< T, Count > * prop )
		{
			c3d::SquareMatrix< T, Count > const & matrix = matrixRefFromVariant< T, Count >( prop->GetValue() );

			for ( uint32_t i = 0; i < Count; ++i )
			{
				wxVariant value = prop->Item( i )->GetValue();
				setVariantFromPoint< T, Count >( value, c3d::Point< T, Count >( matrix[i].constPtr() ) );
				prop->Item( i )->SetValue( value );
			}
		}
		static wxVariant childChanged( wxVariant & thisValue
			, int index
			, wxVariant & newValue )
		{
			c3d::SquareMatrix< T, Count > & matrix = matrixRefFromVariant< T, Count >( thisValue );
			c3d::Point< T, Count > const & row = PointRefFromVariant< T, Count >( newValue );
			matrix.setRow( uint32_t( index ), row );
			wxVariant result;
			result << matrix;
			return result;
		}
	};

	//************************************************************************************************

	template< uint32_t Count > c3d::Array< wxString, Count > const & getMatrixRowDefaultNames();

	template<>
	inline c3d::Array< wxString, 2 > const & getMatrixRowDefaultNames< 2 >()
	{
		return GC_POINT_12;
	}

	template<>
	inline c3d::Array< wxString, 3 > const & getMatrixRowDefaultNames< 3 >()
	{
		return GC_POINT_123;
	}

	template<>
	inline c3d::Array< wxString, 4 > const & getMatrixRowDefaultNames< 4 >()
	{
		return GC_POINT_1234;
	}

	//************************************************************************************************

	template< uint32_t Count > c3d::Array< wxString, Count > const & getMatrixColDefaultNames();

	template<>
	inline c3d::Array< wxString, 2 > const & getMatrixColDefaultNames< 2 >()
	{
		return GC_POINT_12;
	}

	template<>
	inline c3d::Array< wxString, 3 > const & getMatrixColDefaultNames< 3 >()
	{
		return GC_POINT_123;
	}

	template<>
	inline c3d::Array< wxString, 4 > const & getMatrixColDefaultNames< 4 >()
	{
		return GC_POINT_1234;
	}

	//************************************************************************************************

	template< typename T, uint32_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const & label
		, wxString const & name
		, c3d::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		MatrixPropertyHelper< T, Count >::addChildren( this
			, getMatrixRowDefaultNames< Count >()
			, getMatrixColDefaultNames< Count >()
			, value );
	}

	template< typename T, uint32_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const( & rowNames )[Count]
		, wxString const( & colNames )[Count]
		, wxString const & label
		, wxString const & name
		, c3d::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		setValueI( value );
		MatrixPropertyHelper< T, Count >::addChildren( this
			, rowNames
			, colNames
			, value );
	}

	template< typename T, uint32_t Count >
	void MatrixProperty< T, Count >::RefreshChildren()
	{
		if ( GetChildCount() )
		{
			MatrixPropertyHelper< T, Count >::refreshChildren( this );
		}
	}

	template< typename T, uint32_t Count >
	wxVariant MatrixProperty< T, Count >::ChildChanged( wxVariant & thisValue
		, int childIndex
		, wxVariant & childValue ) const
	{
		return MatrixPropertyHelper< T, Count >::childChanged( thisValue
			, childIndex
			, childValue );
	}

	template< typename T, uint32_t Count >
	inline void MatrixProperty< T, Count >::setValueI( c3d::SquareMatrix< T, Count > const & value )
	{
		m_value = WXVARIANT( value );
	}

	//************************************************************************************************
}
