#include <CastorUtils/Data/Path.hpp>
#include <CastorUtils/Design/ChangeTracked.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Math/Angle.hpp>
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

	//************************************************************************************************

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

	//************************************************************************************************

	template<>
	struct ValueTraitsT< castor::String >
	{
		using ValueT = castor::String;
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
	struct ValueTraitsT< castor::Seconds >
	{
		using ValueT = castor::Seconds;
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
	struct ValueTraitsT< castor::Milliseconds >
	{
		using ValueT = castor::Milliseconds;
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
	struct ValueTraitsT< castor::Microseconds >
	{
		using ValueT = castor::Microseconds;
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
	struct ValueTraitsT< castor::Nanoseconds >
	{
		using ValueT = castor::Nanoseconds;
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
	struct ValueTraitsT< castor::Path >
	{
		using ValueT = castor::Path;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline ValueT convert( wxVariant const & var )
		{
			return ValueT{ variantCast< castor::String >( var ) };
		}

		static inline wxVariant convert( ParamType value )
		{
			return getVariant< castor::String >( value );
		}

		static inline wxString getUnit()
		{
			return wxEmptyString;
		}
	};

	//************************************************************************************************

	template<>
	struct ValueTraitsT< castor::RgbColour >
	{
		using ValueT = castor::RgbColour;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			wxColour col;
			col << var;
			return castor::RgbColour::fromBGR( col.GetRGB() );
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
	struct ValueTraitsT< castor::RgbaColour >
	{
		using ValueT = castor::RgbaColour;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			wxColour col;
			col << var;
			return castor::RgbaColour::fromBGRA( col.GetRGBA() );
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
	struct ValueTraitsT< castor::Angle >
	{
		using ValueT = castor::Angle;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var )
		{
			return castor::Angle::fromDegrees( var.GetDouble() );
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
	struct ValueTraitsT< castor::FontSPtr >
	{
		using ValueT = castor::FontSPtr;
		using ParamType = ValueT const &;
		using RetType = ValueT;

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

	template< typename MyValueT >
	struct ValueTraitsT< castor::RangedValue< MyValueT > >
	{
		using ValueT = castor::RangedValue< MyValueT >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = ValueT;

		static inline RetType convert( wxVariant const & var, castor::Range< MyValueT > const & range )
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
	struct ValueTraitsT< castor::SpeedT< MyValueT, castor::Nanoseconds > >
	{
		using ValueT = castor::SpeedT< MyValueT, castor::Nanoseconds >;
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
	struct ValueTraitsT< castor::SpeedT< MyValueT, castor::Microseconds > >
	{
		using ValueT = castor::SpeedT< MyValueT, castor::Microseconds >;
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
	struct ValueTraitsT< castor::SpeedT< MyValueT, castor::Milliseconds > >
	{
		using ValueT = castor::SpeedT< MyValueT, castor::Milliseconds >;
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
	struct ValueTraitsT< castor::SpeedT< MyValueT, castor::Seconds > >
	{
		using ValueT = castor::SpeedT< MyValueT, castor::Seconds >;
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
	struct ValueTraitsT< castor::ChangeTracked< MyValueT > >
	{
		using ValueT = castor::ChangeTracked< MyValueT >;
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
	struct ValueTraitsT< castor::ChangeTracked< castor::RangedValue< MyValueT > > >
	{
		using ValueT = castor::ChangeTracked< castor::RangedValue< MyValueT > >;
		using TypeT = MyValueT;
		using ParamType = ValueT const &;
		using RetType = castor::RangedValue< MyValueT >;

		static inline RetType convert( wxVariant const & var, castor::Range< MyValueT > const & range )
		{
			castor::RangedValue< MyValueT > ranged{ variantCast< TypeT >( var ), range };
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
