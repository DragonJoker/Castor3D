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
		MatrixRefFromVariant( p_variant ) = p_value;
	}

	//************************************************************************************************
	
	template< typename T, size_t Count > struct MatrixPropertyHelper
	{
		static void AddChildren( MatrixProperty< T, Count > * p_prop, Castor::SquareMatrix< T, Count > const & p_value )
		{
			static wxString ChildName[4] =
			{
				wxT( "1" ),
				wxT( "2" ),
				wxT( "3" ),
				wxT( "4" ),
			};

			for ( size_t i = 0; i < Count; ++i )
			{
				p_prop->AddPrivateChild( new PointProperty< T, Count >( wxString() << _( "Row " ) << ChildName[i], wxPG_LABEL, Castor::Point< T, Count >( p_value[i] ) ) );
			}
		}
		static void RefreshChildren( MatrixProperty< T, Count > * p_prop )
		{
			Castor::SquareMatrix< T, Count > const & l_matrix = MatrixRefFromVariant< T, Count >( p_prop->GetValue() );

			for ( size_t i = 0; i < Count; ++i )
			{
				SetVariantFromPoint( p_prop->Item( i )->GetValue(), Castor::Point< T, Count >( l_matrix[i] ) );
			}
		}
		static wxVariant ChildChanged( wxVariant & p_thisValue, int p_index, wxVariant & p_newValue )
		{
			Castor::SquareMatrix< T, Count > & l_matrix = MatrixRefFromVariant< T, Count >( p_thisValue );
			Castor::Point< T, Count > const & l_row = PointRefFromVariant< T, Count >( p_newValue );
			l_matrix.set_row( p_index, l_row );
			wxVariant l_return;
			l_return << l_matrix;
			return l_return;
		}
	};

	//************************************************************************************************
	
	template< typename T, size_t Count >
	MatrixProperty< T, Count >::MatrixProperty( wxString const & label, wxString const & name, Castor::SquareMatrix< T, Count > const & value )
		: wxPGProperty( label, name )
	{
		SetValueI( value );
		MatrixPropertyHelper< T, Count >::AddChildren( this, value );
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
