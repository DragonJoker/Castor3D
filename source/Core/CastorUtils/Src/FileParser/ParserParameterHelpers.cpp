#include "ParserParameterHelpers.hpp"

namespace Castor
{
	xchar const * const RegexFormat< uint8_t >::Value = cuT( "(\\#[0-9a-fA-F]{2}"
			"|0[xX][0-9a-fA-F]{2}"
			"|25[0-5]"
			"|2[0-4]\\d"
			"|1\\d{2}"
			"|\\d{1,2})" );
	xchar const * const RegexFormat< int8_t >::Value = cuT( "(-12[0-8]"
			"|12[0-7]"
			"|-?1[0-1]\\d"
			"|-?\\d{2})" );
	xchar const * const RegexFormat< uint16_t >::Value = cuT( "(\\#([0-9a-fA-F]{4})"
			"|0[xX]([0-9a-fA-F]{4})"
			"|6553[0-5]"
			"|655[0-2]\\d"
			"|65[0-4]\\d{2}"
			"|6[0-4]\\d{3}"
			"|[1-5]\\d{4}"
			"|\\d{1,4})" );
	xchar const * const RegexFormat< int16_t >::Value = cuT( "(-3276[0-8]"
			"|3276[0-7]"
			"|-?327[0-5]\\d"
			"|-?32[0-6]\\d{2}"
			"|-?3[0-1]\\d{3}"
			"|-?[1-2]\\d{4}"
			"|-?\\d{1,4})" );
	xchar const * const RegexFormat< uint32_t >::Value = cuT( "(\\#[0-9a-fA-F]{8}"
			"|0[xX][0-9a-fA-F]{8}"
			"|429496729[0-5]"
			"|42949672[0-8]\\d"
			"|4294967[0-1]\\d{2}"
			"|429496[0-6]\\d{3}"
			"|42949[0-5]\\d{4}"
			"|4294[0-8]\\d{5}"
			"|429[0-3]\\d{6}"
			"|42[0-8]\\d{7}"
			"|4[0-1]\\d{8}"
			"|[1-3]\\d{9}"
			"|\\d{1,9})" );
	xchar const * const RegexFormat< int32_t >::Value = cuT( "(-214748364[0-8]"
			"|214748364[0-7]"
			"|-?21474836[0-3]\\d"
			"|-?2147483[0-5]\\d{2}"
			"|-?214748[0-2]\\d{3}"
			"|-?21474[0-7]\\d{4}"
			"|-?2147[0-3]\\d{5}"
			"|-?214[0-6]\\d{6}"
			"|-?21[0-3]\\d{7}"
			"|-?20\\d{8}"
			"|-?1\\d{9}"
			"|-?\\d{1,9})" );
	xchar const * const RegexFormat< uint64_t >::Value = cuT( "(\\#[0-9a-fA-F]{16}"
			"|0[xX][0-9a-fA-F]{16}"
			"|1844674407370955161[0-5]"
			"|1844674407370955160\\d"
			"|18446744073709551[0-5]\\d{2}"
			"|18446744073709550\\d{3}"
			"|184467440737095[0-4]\\d{4}"
			"|18446744073709[0-4]\\d{5}"
			"|1844674407370[0-8]\\d{6}"
			"|184467440736\\d{8}"
			"|18446744073[0-5]\\d{8}"
			"|1844674407[0-2]\\d{9}"
			"|184467440[0-6]\\d{10}"
			"|18446743\\d{12}"
			"|1844674[0-2]\\d{12}"
			"|184467[0-3]\\d{13}"
			"|18446[0-6]\\d{14}"
			"|1844[0-5]\\d{15}"
			"|184[0-3]\\d{16}"
			"|18[0-3]\\d{17}"
			"|1[0-7]\\d{18}"
			"|\\d{1,19})" );
	xchar const * const RegexFormat< int64_t >::Value = cuT( "(-922337203685477580[0-8]"
			"|922337203685477580[0-7]"
			"|-?92233720368547757\\d{2}"
			"|-?9223372036854775[0-6]\\d{2}"
			"|-?922337203685477[0-4]\\d{3}"
			"|-?92233720368547[0-6]\\d{4}"
			"|-?9223372036854[0-6]\\d{5}"
			"|-?922337203685[0-3]\\d{6}"
			"|-?92233720368[0-4]\\d{7}"
			"|-?9223372036[0-7]\\d{8}"
			"|-?922337203[0-5]\\d{9}"
			"|-?92233720[0-2]\\d{10}"
			"|-?9223371\\d{12}"
			"|-?9223370\\d{12}"
			"|-?922336\\d{13}"
			"|-?92233[0-5]\\d{13}"
			"|-?9223[0-2]\\d{14}"
			"|-?922[0-2]\\d{15}"
			"|-?92[0-1]\\d{16}"
			"|-?9[0-1]\\d{17}"
			"|-?[1-8]\\d{18}"
			"|-?\\d{1,18})" );
	xchar const * const RegexFormat< float >::Value = cuT( "(-?[0-9]{1,6}\\.[0-9]{1,9}e-?[0-9]{1,3}"
			"|-?[0-9]{1,6}\\.[0-9]{1,9}"
			"|-?[0-9]{1,6}"
			"|-?\\.[0-9]{1,9})" );
	xchar const * const RegexFormat< double >::Value = cuT( "(-?[0-9]{1,9}\\.[0-9]{1,13}e-?[0-9]{1,3}"
			"|-?[0-9]{1,9}\\.[0-9]{1,13}"
			"|-?[0-9]{1,9}"
			"|-?\\.[0-9]{1,13})" );
	xchar const * const RegexFormat< long double >::Value = cuT( "(-?[0-9]{1,9}\\.[0-9]{1,13}"
			"|-?[0-9]{1,9}"
			"|-?\\.[0-9]{1,13})" );
	xchar const * const RegexFormat< Colour >::Value = cuT( "#([0-9a-fA-F]{2}[0-9a-fA-F]{2}[0-9a-fA-F]{2}[0-9a-fA-F]{2})"
			"|#([0-9a-fA-F]{2}[0-9a-fA-F]{2}[0-9a-fA-F]{2})" );

	xchar const * const ParserParameterHelper< ParameterType::eText >::StringType = cuT( "text" );
	xchar const * const ParserParameterHelper< ParameterType::eName >::StringType = cuT( "name" );
	xchar const * const ParserParameterHelper< ParameterType::ePath >::StringType = cuT( "path" );
	xchar const * const ParserParameterHelper< ParameterType::eCheckedText >::StringType = cuT( "checked text" );
	xchar const * const ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >::StringType = cuT( "32 bits bitwise ORed checked texts" );
	xchar const * const ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >::StringType = cuT( "64 bits bitwise ORed checked texts" );
	xchar const * const ParserParameterHelper< ParameterType::eBool >::StringType = cuT( "boolean" );
	xchar const * const ParserParameterHelper< ParameterType::eInt8 >::StringType = cuT( "8 bits signed integer" );
	xchar const * const ParserParameterHelper< ParameterType::eInt16 >::StringType = cuT( "16 bits signed integer" );
	xchar const * const ParserParameterHelper< ParameterType::eInt32 >::StringType = cuT( "32 bits signed integer" );
	xchar const * const ParserParameterHelper< ParameterType::eInt64 >::StringType = cuT( "64 bits signed integer" );
	xchar const * const ParserParameterHelper< ParameterType::eUInt8 >::StringType = cuT( "8 bits unsigned integer" );
	xchar const * const ParserParameterHelper< ParameterType::eUInt16 >::StringType = cuT( "16 bits unsigned integer" );
	xchar const * const ParserParameterHelper< ParameterType::eUInt32 >::StringType = cuT( "32 bits unsigned integer" );
	xchar const * const ParserParameterHelper< ParameterType::eUInt64 >::StringType = cuT( "64 bits unsigned integer" );
	xchar const * const ParserParameterHelper< ParameterType::eFloat >::StringType = cuT( "simple precision floating point" );
	xchar const * const ParserParameterHelper< ParameterType::eDouble >::StringType = cuT( "double precision floating point" );
	xchar const * const ParserParameterHelper< ParameterType::eLongDouble >::StringType = cuT( "long double precision floating point" );
	xchar const * const ParserParameterHelper< ParameterType::ePixelFormat >::StringType = cuT( "pixel format" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint2I >::StringType = cuT( "2 integers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint3I >::StringType = cuT( "3 integers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint4I >::StringType = cuT( "4 integers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint2F >::StringType = cuT( "2 simple precision floating point numbers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint3F >::StringType = cuT( "3 simple precision floating point numbers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint4F >::StringType = cuT( "4 simple precision floating point numbers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint2D >::StringType = cuT( "2 double precision floating point numbers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint3D >::StringType = cuT( "3 double precision floating point numbers point" );
	xchar const * const ParserParameterHelper< ParameterType::ePoint4D >::StringType = cuT( "4 double precision floating point numbers point" );
	xchar const * const ParserParameterHelper< ParameterType::eSize >::StringType = cuT( "size" );
	xchar const * const ParserParameterHelper< ParameterType::ePosition >::StringType = cuT( "position" );
	xchar const * const ParserParameterHelper< ParameterType::eRectangle >::StringType = cuT( "rectangle" );
	xchar const * const ParserParameterHelper< ParameterType::eColour >::StringType = cuT( "colour" );

	static const std::array< String, size_t( ParameterType::eCount ) > TypeName
	{
		cuT( "text" ),
		cuT( "name" ),
		cuT( "path" ),
		cuT( "checked text" ),
		cuT( "32 bits bitwise ORed checked texts" ),
		cuT( "64 bits bitwise ORed checked texts" ),
		cuT( "boolean" ),
		cuT( "8 bits signed integer" ),
		cuT( "16 bits signed integer" ),
		cuT( "32 bits signed integer" ),
		cuT( "64 bits signed integer" ),
		cuT( "8 bits unsigned integer" ),
		cuT( "16 bits unsigned integer" ),
		cuT( "32 bits unsigned integer" ),
		cuT( "64 bits unsigned integer" ),
		cuT( "simple precision floating point" ),
		cuT( "double precision floating point" ),
		cuT( "long double precision floating point" ),
		cuT( "pixel format" ),
		cuT( "2 integers point" ),
		cuT( "3 integers point" ),
		cuT( "4 integers point" ),
		cuT( "2 simple precision floating point numbers point" ),
		cuT( "3 simple precision floating point numbers point" ),
		cuT( "4 simple precision floating point numbers point" ),
		cuT( "2 double precision floating point numbers point" ),
		cuT( "3 double precision floating point numbers point" ),
		cuT( "4 double precision floating point numbers point" ),
		cuT( "size" ),
		cuT( "position" ),
		cuT( "rectangle" ),
		cuT( "colour" ),
	};

	String const & GetTypeName( ParameterType p_type )
	{
		return TypeName[uint32_t( p_type )];
	}
}

