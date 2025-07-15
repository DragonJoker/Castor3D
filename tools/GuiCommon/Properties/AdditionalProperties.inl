#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Illumination.hpp>
#include <CastorUtils/Math/LuminousIntensity.hpp>
#include <CastorUtils/Math/RangedValue.hpp>
#include <CastorUtils/Math/Speed.hpp>

namespace GuiCommon
{
	//************************************************************************************************

	template<>
	struct ValueTraitsT< bool >
	{
		using ValueT = bool;
		using ParamType = ValueT;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return var.GetBool();
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#define IntValueTraitsT( IntTypeT )\
	template<>\
	struct ValueTraitsT< IntTypeT >\
	{\
		using ValueT = IntTypeT;\
		using ParamType = ValueT;\
		using RetType = ValueT;\
		static inline RetType convert( wxVariant const & var )\
		{\
			return ValueT( var.GetInteger() );\
		}\
		static inline wxVariant convert( ParamType value )\
		{\
			return WXVARIANT( long( value ) );\
		}\
		static inline wxString getUnit()\
		{\
			return wxEmptyString;\
		}\
	}

	IntValueTraitsT( int8_t );
	IntValueTraitsT( uint8_t );
	IntValueTraitsT( int16_t );
	IntValueTraitsT( uint16_t );
	IntValueTraitsT( int32_t );
	IntValueTraitsT( uint32_t );
	IntValueTraitsT( int64_t );
	IntValueTraitsT( uint64_t );

#undef IntValueTraitsT
#pragma GCC diagnostic pop

	//************************************************************************************************

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wuseless-cast"
#define FloatValueTraitsT( FloatTypeT )\
	template<>\
	struct ValueTraitsT< FloatTypeT >\
	{\
		using ValueT = FloatTypeT;\
		using ParamType = ValueT;\
		using RetType = ValueT;\
		static inline RetType convert( wxVariant const & var )\
		{\
			return ValueT( var.GetDouble() );\
		}\
		static inline wxVariant convert( ParamType value )\
		{\
			return WXVARIANT( double( value ) );\
		}\
		static inline wxString getUnit()\
		{\
			return wxEmptyString;\
		}\
	}

	FloatValueTraitsT( float );
	FloatValueTraitsT( double );

#undef FloatValueTraitsT
#pragma GCC diagnostic pop

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::String >
	{
		using ValueT = c3d::String;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return make_String( var.GetString() );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( make_wxString( value ) );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::U32String >
	{
		using ValueT = c3d::U32String;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return make_U32String( var.GetString() );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( make_wxString( value ) );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Seconds >
	{
		using ValueT = c3d::Seconds;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return ValueT( int64_t( var.GetDouble() ) );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.count() );
		}

		static inline wxString getUnit()
		{
			return wxT( "s" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Milliseconds >
	{
		using ValueT = c3d::Milliseconds;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return ValueT( int64_t( var.GetDouble() ) );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.count() );
		}

		static inline wxString getUnit()
		{
			return wxT( "ms" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Microseconds >
	{
		using ValueT = c3d::Microseconds;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return ValueT( int64_t( var.GetDouble() ) );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.count() );
		}

		static inline wxString getUnit()
		{
			return wxT( "µs" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Nanoseconds >
	{
		using ValueT = c3d::Nanoseconds;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return ValueT( int64_t( var.GetDouble() ) );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.count() );
		}

		static inline wxString getUnit()
		{
			return wxT( "ns" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Path >
	{
		using ValueT = c3d::Path;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline ValueT convert( wxVariant const & var )
		{
			return ValueT{ variantCast< c3d::String >( var ) };
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant< c3d::String >( value );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::RgbColour >
	{
		using ValueT = c3d::RgbColour;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			wxColour col;
			col << var;
			return c3d::RgbColour::fromBGR( col.GetRGB() );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( wxColour{ toBGRPacked( value ) } );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::RgbaColour >
	{
		using ValueT = c3d::RgbaColour;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			wxColour col;
			col << var;
			return c3d::RgbaColour::fromBGRA( col.GetRGBA() );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( wxColour{ toBGRAPacked( value ) } );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Angle >
	{
		using ValueT = c3d::Angle;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return c3d::Angle::fromDegrees( var.GetDouble() );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.degrees() );
		}

		static inline wxString getUnit()
		{
			return wxT( "°" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::Illumination >
	{
		using ValueT = c3d::Illumination;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return c3d::Illumination{ var.GetDouble() };
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.lux() );
		}

		static inline wxString getUnit()
		{
			return wxT( "lx" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::LuminousIntensity >
	{
		using ValueT = c3d::LuminousIntensity;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return c3d::LuminousIntensity{ var.GetDouble() };
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value.candela() );
		}

		static inline wxString getUnit()
		{
			return wxT( "cd" );
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::FontRPtr >
	{
		using ValueT = c3d::FontRPtr;
		using ParamType = c3d::FontRPtr const &;
		using RetType = c3d::FontUPtr;

		static inline RetType convert( wxVariant const & var )
		{
			wxFont wxfont;
			wxfont << var;
			return make_Font( wxfont );
		}

		static inline wxVariant convert( ParamType value )
		{
			wxFontInfo info( value->getHeight() );
			info.FaceName( value->getFaceName() );
			return WXVARIANT( wxFont{ info } );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< c3d::TextureSourceInfo * >
	{
		using ValueT = c3d::TextureSourceInfo *;
		using ParamType = c3d::TextureSourceInfo *;
		using RetType = c3d::TextureSourceInfo *;

		static inline RetType convert( wxVariant const & var )
		{
			void * ptr = var.GetVoidPtr();
			return static_cast< RetType >( ptr );
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( static_cast< void * >( value ) );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::RangedValue< MyValueT > >
	{
		using ValueT = c3d::RangedValue< MyValueT >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var, c3d::Range< MyValueT > const & range )
		{
			return RetType{ variantCast< TypeT >( var ), range };
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit();
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::SpeedT< MyValueT, c3d::Nanoseconds > >
	{
		using ValueT = c3d::SpeedT< MyValueT, c3d::Nanoseconds >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return variantCast< TypeT >( var );
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit() + wxT( "/ns" );
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::SpeedT< MyValueT, c3d::Microseconds > >
	{
		using ValueT = c3d::SpeedT< MyValueT, c3d::Microseconds >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return variantCast< TypeT >( var );
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit() + wxT( "/µs" );
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::SpeedT< MyValueT, c3d::Milliseconds > >
	{
		using ValueT = c3d::SpeedT< MyValueT, c3d::Milliseconds >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return variantCast< TypeT >( var );
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit() + wxT( "/ms" );
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::SpeedT< MyValueT, c3d::Seconds > >
	{
		using ValueT = c3d::SpeedT< MyValueT, c3d::Seconds >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return ValueT{ variantCast< TypeT >( var ) };
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit() + wxT( "/s" );
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::ChangeTracked< MyValueT > >
	{
		using ValueT = c3d::ChangeTracked< MyValueT >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = TypeT;

		static inline RetType convert( wxVariant const & var )
		{
			return variantCast< TypeT >( var );
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit();
		}
	};

	//************************************************************************************************

	template< typename MyValueT >
	struct ValueTraitsT< c3d::ChangeTracked< c3d::RangedValue< MyValueT > > >
	{
		using ValueT = c3d::ChangeTracked< c3d::RangedValue< MyValueT > >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = c3d::RangedValue< MyValueT >;

		static inline RetType convert( wxVariant const & var, c3d::Range< MyValueT > const & range )
		{
			c3d::RangedValue< MyValueT > ranged{ variantCast< TypeT >( var ), range };
			return ranged;
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant( value.value() );
		}

		static inline wxString getUnit()
		{
			return ValueTraitsT< TypeT >::getUnit();
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< wxString >
	{
		using ValueT = wxString;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return var.GetString();
		}

		static inline wxVariant convert( ParamType value )
		{
			return WXVARIANT( value );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************
}
