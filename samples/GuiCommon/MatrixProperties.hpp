/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_MATRIX_PROPERTIES_H___
#define ___GUICOMMON_MATRIX_PROPERTIES_H___

#include "GuiCommon/AdditionalProperties.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix2x2f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix3x3f );
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix4x4f );

namespace GuiCommon
{
	template< typename T, size_t Count >
	class MatrixProperty
		: public wxPGProperty
	{
		WX_PG_DECLARE_PROPERTY_CLASS( MatrixProperty )

	public:
		MatrixProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::SquareMatrix< T, Count > const & value = castor::SquareMatrix< T, Count >() );
		explicit MatrixProperty( wxString const( & rowNames )[Count]
			, wxString const( & colNames )[Count]
			, wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::SquareMatrix< T, Count > const & value = castor::SquareMatrix< T, Count >() );
		virtual ~MatrixProperty();

		wxVariant ChildChanged( wxVariant & thisValue
			, int childIndex
			, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::SquareMatrix< T, Count > const & value );
	};

	template< typename Type, size_t Count > castor::SquareMatrix< Type, Count > const & matrixRefFromVariant( wxVariant const & variant );
	template< typename Type, size_t Count > castor::SquareMatrix< Type, Count > & matrixRefFromVariant( wxVariant & variant );
	template< typename Type, size_t Count > void setVariantFromMatrix( wxVariant & variant
		, castor::SquareMatrix< Type, Count > const & value );

	template< size_t Count > using FloatMatrixProperty = MatrixProperty< float, Count >;

	typedef FloatMatrixProperty< 2 > Matrix2fProperty;
	typedef FloatMatrixProperty< 3 > Matrix3fProperty;
	typedef FloatMatrixProperty< 4 > Matrix4fProperty;
}

#include "GuiCommon/MatrixProperties.inl"

#endif
