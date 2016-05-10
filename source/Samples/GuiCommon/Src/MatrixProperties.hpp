/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GUICOMMON_MATRIX_PROPERTIES_H___
#define ___GUICOMMON_MATRIX_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <SquareMatrix.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix2x2f );
GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix3x3f );
GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix4x4f );
GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix2x2d );
GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix3x3d );
GC_PG_NS_DECLARE_VARIANT_DATA( Castor, Matrix4x4d );

namespace GuiCommon
{
	template< typename T, size_t Count >
	class MatrixProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( MatrixProperty )

	public:
		MatrixProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::SquareMatrix< T, Count > const & value = Castor::SquareMatrix< T, Count >() );
		MatrixProperty( wxString const( & p_rowNames )[Count], wxString const( & p_colNames )[Count], wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, Castor::SquareMatrix< T, Count > const & value = Castor::SquareMatrix< T, Count >() );
		virtual ~MatrixProperty();

		virtual wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue ) const;
		virtual void RefreshChildren();

	protected:
		// I stands for internal
		inline void SetValueI( Castor::SquareMatrix< T, Count > const & value );
	};

	template< typename Type, size_t Count > Castor::SquareMatrix< Type, Count > const & MatrixRefFromVariant( wxVariant const & p_variant );
	template< typename Type, size_t Count > Castor::SquareMatrix< Type, Count > & MatrixRefFromVariant( wxVariant & p_variant );
	template< typename Type, size_t Count > void SetVariantFromMatrix( wxVariant & p_variant, Castor::SquareMatrix< Type, Count > const & p_value );

	template< size_t Count > using FloatMatrixProperty = MatrixProperty< float, Count >;
	template< size_t Count > using DoubleMatrixProperty = MatrixProperty< double, Count >;

	typedef FloatMatrixProperty< 2 > Matrix2fProperty;
	typedef FloatMatrixProperty< 3 > Matrix3fProperty;
	typedef FloatMatrixProperty< 4 > Matrix4fProperty;

	typedef DoubleMatrixProperty< 2 > Matrix2dProperty;
	typedef DoubleMatrixProperty< 3 > Matrix3dProperty;
	typedef DoubleMatrixProperty< 4 > Matrix4dProperty;

#if CASTOR_USE_DOUBLE
	typedef Matrix2dProperty Matrix2rProperty;
	typedef Matrix3dProperty Matrix3rProperty;
	typedef Matrix4dProperty Matrix4rProperty;
#else
	typedef Matrix2fProperty Matrix2rProperty;
	typedef Matrix3fProperty Matrix3rProperty;
	typedef Matrix4fProperty Matrix4rProperty;
#endif
}

#include "MatrixProperties.inl"

#endif
