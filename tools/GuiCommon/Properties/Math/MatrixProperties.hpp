/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_MATRIX_PROPERTIES_H___
#define ___GUICOMMON_MATRIX_PROPERTIES_H___

#include "GuiCommon/Properties/AdditionalProperties.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix2x2f )
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix3x3f )
GC_PG_NS_DECLARE_VARIANT_DATA( castor, Matrix4x4f )

namespace GuiCommon
{
	template< typename T, uint32_t Count >
	class MatrixProperty
		: public wxPGProperty
	{
	public:
		static wxObject * wxCreateObject()
		{
			return new MatrixProperty;
		}

		wxClassInfo * GetClassInfo() const override
		{
			static wxString s_name{ wxString{} << wxT( "Matrix" ) << Count << wxT( "x" ) << Count << getTypeNameSuffix< T >() << wxT( "Property" ) };
			static wxClassInfo s_classInfo = { s_name.c_str()
				, &wxPGProperty::ms_classInfo
				, nullptr
				, int( sizeof( MatrixProperty ) )
				, MatrixProperty::wxCreateObject };
			return &s_classInfo;
		}

		const wxPGEditor * DoGetEditorClass()const override
		{
			return wxPGEditor_TextCtrl;
		}

	public:
		MatrixProperty( wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::SquareMatrix< T, Count > const & value = castor::SquareMatrix< T, Count >() );
		explicit MatrixProperty( wxString const( & rowNames )[Count]
			, wxString const( & colNames )[Count]
			, wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, castor::SquareMatrix< T, Count > const & value = castor::SquareMatrix< T, Count >() );

		wxVariant ChildChanged( wxVariant & thisValue
			, int childIndex
			, wxVariant & childValue )const override;
		void RefreshChildren()override;

	protected:
		// I stands for internal
		inline void setValueI( castor::SquareMatrix< T, Count > const & value );
	};

	template< typename Type, uint32_t Count > castor::SquareMatrix< Type, Count > const & matrixRefFromVariant( wxVariant const & variant );
	template< typename Type, uint32_t Count > castor::SquareMatrix< Type, Count > & matrixRefFromVariant( wxVariant & variant );
	template< typename Type, uint32_t Count > void setVariantFromMatrix( wxVariant & variant
		, castor::SquareMatrix< Type, Count > const & value );

	template< uint32_t Count > using FloatMatrixProperty = MatrixProperty< float, Count >;

	typedef FloatMatrixProperty< 2 > Matrix2fProperty;
	typedef FloatMatrixProperty< 3 > Matrix3fProperty;
	typedef FloatMatrixProperty< 4 > Matrix4fProperty;
}

#include "GuiCommon/Properties/Math/MatrixProperties.inl"

#endif
