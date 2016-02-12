#include "ParserParameterTypeException.hpp"

#include "StringUtils.hpp"

namespace Castor
{
	ParserParameterTypeException::ParserParameterTypeException( ePARAMETER_TYPE p_givenType, ePARAMETER_TYPE p_expectedType )
		:	Exception( "", "", "", 0 )
	{
		static std::string StrParameterType[] =
		{
			"text",
			"name",
			"path",
			"checked_text",
			"32 bits bitwise ORed checked_text",
			"64 bits bitwise ORed checked_text",
			"bool",
			"int8",
			"int16",
			"int32",
			"int64",
			"uint8",
			"uint16",
			"uint32",
			"uint64",
			"float",
			"double",
			"longdouble",
			"pixelformat",
			"point2i",
			"point3i",
			"point4i",
			"point2f",
			"point3f",
			"point4f",
			"point2d",
			"point3d",
			"point4d",
			"size",
			"position",
			"rectangle",
			"colour",
		};
		m_description = "Wrong parameter type: user gave " + StrParameterType[p_givenType] + " while parameter base type is " + StrParameterType[p_expectedType];
	}
}
