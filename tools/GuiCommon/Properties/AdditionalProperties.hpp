/*
See LICENSE file in root folder
*/
#ifndef ___GUICOMMON_ADDITIONAL_PROPERTIES_H___
#define ___GUICOMMON_ADDITIONAL_PROPERTIES_H___

#include "GuiCommon/GuiCommonPrerequisites.hpp"

#include <CastorUtils/Graphics/Image.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/editors.h>
#pragma warning( pop )

#include <functional>

#define GC_PG_NS_DECLARE_VARIANT_DATA_EXPORTED( namspace, classname, expdecl )\
	namspace::classname & operator<<( namspace::classname & object, const wxVariant & variant );\
	wxVariant & operator<<( wxVariant & variant, const namspace::classname & object );\
	const namspace::classname & classname##RefFromVariant( const wxVariant & variant );\
	namspace::classname & classname##RefFromVariant( wxVariant & variant );\
	template<> inline wxVariant WXVARIANT( const namspace::classname & value )\
	{\
		wxVariant variant;\
		variant << value;\
		return variant;\
	}\
	extern const char* classname##_VariantType;\
	namespace GuiCommon\
	{\
		template<>\
		struct ValueTraitsT< namspace::classname >\
		{\
			using ValueT = namspace::classname;\
			using ParamType = ValueT const &;\
			using RetType = ValueT;\
			static inline RetType convert( wxVariant const & var )\
			{\
				return classname##RefFromVariant( var );\
			}\
			static inline wxVariant convert( ParamType value )\
			{\
				return WXVARIANT( value );\
			}\
			static inline wxString getUnit()\
			{\
				return wxEmptyString;\
			}\
		};\
	}

#define GC_PG_NS_DECLARE_VARIANT_DATA( namspace, classname )\
	GC_PG_NS_DECLARE_VARIANT_DATA_EXPORTED( namspace, classname, wxEMPTY_PARAMETER_VALUE )

// Implements sans constructor function. Also, first arg is class name, not
// property name.
#define GC_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN( PROPNAME, T, EDITOR )\
	template<> const wxPGEditor* PROPNAME::DoGetEditorClass()const\
	{\
		return wxPGEditor_##EDITOR;\
	}

// common part of the macros below
#define GC_IMPLEMENT_CLASS_COMMON( name, basename, baseclsinfo2, func )\
	template<>\
	wxClassInfo name::ms_classInfo( wxT( #name ),\
						&basename::ms_classInfo,\
						baseclsinfo2,\
						int( sizeof( name ) ),\
						func );\
	template<>\
	wxClassInfo * name::GetClassInfo() const\
	{\
		return &name::ms_classInfo;\
	}

#define GC_IMPLEMENT_CLASS_COMMON1( name, basename, func )\
    GC_IMPLEMENT_CLASS_COMMON( name, basename, nullptr, func )

// Single inheritance with one base class
#define GC_IMPLEMENT_DYNAMIC_CLASS( name, basename )\
	template<>\
	wxObject* name::wxCreateObject()\
	{\
		return new name;\
	}\
	GC_IMPLEMENT_CLASS_COMMON1( name, basename, name::wxCreateObject )

//
// Property class implementation helper macros.
//
#define GC_PG_IMPLEMENT_PROPERTY_CLASS( NAME, UPCLASS, T, T_AS_ARG, EDITOR )\
	GC_IMPLEMENT_DYNAMIC_CLASS( NAME, UPCLASS )\
	GC_PG_IMPLEMENT_PROPERTY_CLASS_PLAIN( NAME, T, EDITOR )

// Add getter (ie. classname << variant) separately to allow
// custom implementations.
#define GC_PG_IMPLEMENT_VARIANT_DATA_EXPORTED_NO_EQ_NO_GETTER( namspace, classname, expdecl ) \
	const char* classname##_VariantType = #classname; \
	class classname##VariantData\
		: public wxVariantData\
	{ \
	public:\
		classname##VariantData() {} \
		classname##VariantData( const namspace::classname & value ) { m_value = value; } \
		namspace::classname &GetValue() { return m_value; } \
		const namspace::classname &GetValue() const { return m_value; } \
		bool Eq( wxVariantData & data ) const override; \
		wxString GetType() const override; \
		wxVariantData* Clone() const override { return new classname##VariantData( m_value ); } \
	protected:\
		namspace::classname m_value; \
	};\
	\
	wxString classname##VariantData::GetType() const\
	{\
		return wxS( #classname );\
	}\
	\
	expdecl wxVariant & operator<<( wxVariant & variant, const namspace::classname & value )\
	{\
		auto data = new classname##VariantData( value );\
		variant.SetData( data );\
		return variant;\
	} \
	expdecl namspace::classname & classname##RefFromVariant( wxVariant & variant ) \
	{ \
		CU_Assert( variant.GetType() == wxS( #classname ), \
			castor::String{ wxString::Format( "Variant type should have been '%s'" \
									   "instead of '%s'", \
									   wxS( #classname ), \
									   variant.GetType().c_str() ) }.c_str() ); \
		auto data = static_cast< classname##VariantData * >( variant.GetData() ); \
		return data->GetValue();\
	} \
	expdecl const namspace::classname& classname##RefFromVariant( const wxVariant& variant ) \
	{ \
		CU_Assert( variant.GetType() == wxS( #classname ), \
			castor::String{ wxString::Format( "Variant type should have been '%s'" \
									   "instead of '%s'", \
									   wxS( #classname ), \
									   variant.GetType().c_str() ) }.c_str() ); \
		auto data = static_cast< classname##VariantData * >( variant.GetData() ); \
		return data->GetValue();\
	}

#define GC_PG_IMPLEMENT_VARIANT_DATA_GETTER( namspace, classname, expdecl ) \
	expdecl namspace::classname & operator<<( namspace::classname &value, const wxVariant & variant )\
	{\
		CU_Require( variant.GetType() == #classname );\
		\
		auto data = static_cast< classname##VariantData * >( variant.GetData() ); \
		value = data->GetValue();\
		return value;\
	}

// with Eq() implementation that always returns false
#define GC_PG_IMPLEMENT_VARIANT_DATA_EXPORTED_DUMMY_EQ( namspace, classname, expdecl )\
	GC_PG_IMPLEMENT_VARIANT_DATA_EXPORTED_NO_EQ_NO_GETTER( namspace, classname, wxEMPTY_PARAMETER_VALUE expdecl )\
	GC_PG_IMPLEMENT_VARIANT_DATA_GETTER( namspace, classname, wxEMPTY_PARAMETER_VALUE expdecl )\
	\
	bool classname##VariantData::Eq( wxVariantData& WXUNUSED( data ) ) const\
	{\
		return false;\
	}

#define GC_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ( namspace, classname )\
	GC_PG_IMPLEMENT_VARIANT_DATA_EXPORTED_DUMMY_EQ( namspace, classname, wxEMPTY_PARAMETER_VALUE )

namespace GuiCommon
{
	template< typename ValueT >
	struct ValueTraitsT;

	template< typename ValueT >
	using RetTypeT = typename ValueTraitsT< ValueT >::RetType;
	
	template< typename ValueT >
	using ParamTypeT = typename ValueTraitsT< ValueT >::ParamType;

	template< typename ValueT >
	RetTypeT< ValueT > variantCast( wxVariant const & var )
	{
		return ValueTraitsT< ValueT >::convert( var );
	}

	template< typename ValueT >
	wxVariant getVariant( ParamTypeT< ValueT > value )
	{
		return ValueTraitsT< ValueT >::convert( value );
	}

	using ButtonEventMethod = std::function< void ( wxVariant const & ) >;

	class ButtonData
		: public wxClientData
	{
	public:
		explicit ButtonData( ButtonEventMethod method );
		void Call( wxVariant const & var );

	private:
		ButtonEventMethod m_method;
	};

	class ButtonEventEditor
		: public wxPGEditor
	{
	protected:
		wxPGWindowList CreateControls( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxPoint const & p_pos, wxSize const & p_size )const override;
		void UpdateControl( wxPGProperty * property, wxWindow * ctrl )const override;
		bool OnEvent( wxPropertyGrid * p_propgrid, wxPGProperty * p_property, wxWindow * p_wnd_primary, wxEvent & p_event )const override;
	};

	wxFloatProperty * CreateProperty( wxString const & p_name, double const & p_value );
	wxFloatProperty * CreateProperty( wxString const & p_name, float const & p_value );
	wxIntProperty * CreateProperty( wxString const & p_name, int const & p_value );
	wxUIntProperty * CreateProperty( wxString const & p_name, uint32_t const & p_value );
	wxBoolProperty * CreateProperty( wxString const & p_name, bool const & p_value, bool p_checkbox );
	wxStringProperty * CreateProperty( wxString const & p_name, wxString const & p_value );

	wxPGProperty * addAttributes( wxPGProperty * prop );

	template< typename PropertyType >
	PropertyType * CreateProperty( wxString const & p_name, wxVariant && p_value, wxString const & p_help )
	{
		auto result = new PropertyType( p_name );
		result->SetValue( p_value );
		result->SetHelpString( p_help );
		return result;
	}

	template< typename T >
	struct TypeNameSuffixGetterT;

	template<>
	struct TypeNameSuffixGetterT< int8_t >
	{
		static wxString getValue()
		{
			return wxT( "i8" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< uint8_t >
	{
		static wxString getValue()
		{
			return wxT( "u8" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< int16_t >
	{
		static wxString getValue()
		{
			return wxT( "i16" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< uint16_t >
	{
		static wxString getValue()
		{
			return wxT( "u16" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< int32_t >
	{
		static wxString getValue()
		{
			return wxT( "i32" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< uint32_t >
	{
		static wxString getValue()
		{
			return wxT( "u32" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< int64_t >
	{
		static wxString getValue()
		{
			return wxT( "i64" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< uint64_t >
	{
		static wxString getValue()
		{
			return wxT( "u64" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< float >
	{
		static wxString getValue()
		{
			return wxT( "f32" );
		}
	};

	template<>
	struct TypeNameSuffixGetterT< double >
	{
		static wxString getValue()
		{
			return wxT( "f64" );
		}
	};

	template< typename TypeT >
	wxString getTypeNameSuffix()
	{
		return TypeNameSuffixGetterT< TypeT >::getValue();
	}

	class gcImageFileProperty
		: public wxFileProperty
	{
		wxDECLARE_DYNAMIC_CLASS( gcImageFileProperty );

	public:

		gcImageFileProperty( castor::ImageLoader * loader = nullptr
			, wxString const & label = wxPG_LABEL
			, wxString const & name = wxPG_LABEL
			, wxString const & value = wxEmptyString );

		void OnSetValue()override;

		wxSize OnMeasureImage( int item )const override;
		void OnCustomPaint( wxDC & dc
			, wxRect const & rect
			, wxPGPaintData & paintdata ) override;

	protected:
		castor::ImageLoader * m_loader;
		std::unique_ptr< castor::Image > m_image; // intermediate thumbnail area
		std::unique_ptr< wxBitmap > m_bitmap; // final thumbnail area

	private:
		// Initialize m_image using the current file name.
		void doLoadImageFromFile();
	};
}

#include "GuiCommon/Properties/AdditionalProperties.inl"

#endif
