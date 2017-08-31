/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_MATRIX_PROPERTIES_H___
#define ___GUICOMMON_MATRIX_PROPERTIES_H___

#include "AdditionalProperties.hpp"

#include <Math/SquareMatrix.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix2x2f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix3x3f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix4x4f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix2x2d );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix3x3d );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix4x4d );

namespace GuiCommon
{
	template< typename T, size_t Count >
	class MatrixProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( MatrixProperty )

	public:
		MatrixProperty( wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::SquareMatrix< T, Count > const & value = castor::SquareMatrix< T, Count >() );
		MatrixProperty( wxString const( & p_rowNames )[Count], wxString const( & p_colNames )[Count], wxString const & label = wxPG_LABEL, wxString const & name = wxPG_LABEL, castor::SquareMatrix< T, Count > const & value = castor::SquareMatrix< T, Count >() );
		virtual ~MatrixProperty();

		wxVariant ChildChanged( wxVariant & thisValue, int childIndex, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::SquareMatrix< T, Count > const & value );
	};

	template< typename Type, size_t Count > castor::SquareMatrix< Type, Count > const & matrixRefFromVariant( wxVariant const & p_variant );
	template< typename Type, size_t Count > castor::SquareMatrix< Type, Count > & matrixRefFromVariant( wxVariant & p_variant );
	template< typename Type, size_t Count > void setVariantFromMatrix( wxVariant & p_variant, castor::SquareMatrix< Type, Count > const & p_value );

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
