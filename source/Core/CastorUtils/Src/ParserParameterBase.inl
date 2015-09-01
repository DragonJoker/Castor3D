#include "ParserParameterTypeException.hpp"
#include "ParserParameter.hpp"
#include "Colour.hpp"
#include "Path.hpp"
#include "Point.hpp"
#include "Position.hpp"
#include "Rectangle.hpp"
#include "Size.hpp"

namespace Castor
{
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template< typename T > struct ParserValueTyper;
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< String >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_TEXT;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Path >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_PATH;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< bool >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_BOOL;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int8_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_INT8;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int16_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_INT16;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int32_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_INT32;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int64_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_INT64;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint8_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_UINT8;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint16_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_UINT16;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint32_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_UINT32;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint64_t >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_UINT64;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< float >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_FLOAT;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< double >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_DOUBLE;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< long double >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_LONGDOUBLE;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< ePIXEL_FORMAT >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_PIXELFORMAT;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2i >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT2I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3i >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT3I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4i >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT4I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2f >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT2F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3f >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT3F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4f >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT4F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2d >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT2D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3d >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT3D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4d >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POINT4D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Size >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_SIZE;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Position >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_POSITION;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Rectangle >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_RECTANGLE;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Colour >
	{
		static const ePARAMETER_TYPE Type = ePARAMETER_TYPE_COLOUR;
	};
}

