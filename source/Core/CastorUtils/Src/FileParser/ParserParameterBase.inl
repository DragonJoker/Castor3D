#include "Graphics/Colour.hpp"
#include "Data/Path.hpp"
#include "Math/Point.hpp"
#include "Graphics/Position.hpp"
#include "Graphics/Rectangle.hpp"
#include "Graphics/Size.hpp"

namespace castor
{
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template< typename T > struct ParserValueTyper;
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< String >
	{
		static const ParameterType Type = ParameterType::eText;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Path >
	{
		static const ParameterType Type = ParameterType::ePath;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< bool >
	{
		static const ParameterType Type = ParameterType::eBool;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int8_t >
	{
		static const ParameterType Type = ParameterType::eInt8;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int16_t >
	{
		static const ParameterType Type = ParameterType::eInt16;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int32_t >
	{
		static const ParameterType Type = ParameterType::eInt32;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int64_t >
	{
		static const ParameterType Type = ParameterType::eInt64;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint8_t >
	{
		static const ParameterType Type = ParameterType::eUInt8;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint16_t >
	{
		static const ParameterType Type = ParameterType::eUInt16;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint32_t >
	{
		static const ParameterType Type = ParameterType::eUInt32;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint64_t >
	{
		static const ParameterType Type = ParameterType::eUInt64;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< float >
	{
		static const ParameterType Type = ParameterType::eFloat;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< double >
	{
		static const ParameterType Type = ParameterType::eDouble;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< long double >
	{
		static const ParameterType Type = ParameterType::eLongDouble;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< PixelFormat >
	{
		static const ParameterType Type = ParameterType::ePixelFormat;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2i >
	{
		static const ParameterType Type = ParameterType::ePoint2I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3i >
	{
		static const ParameterType Type = ParameterType::ePoint3I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4i >
	{
		static const ParameterType Type = ParameterType::ePoint4I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2f >
	{
		static const ParameterType Type = ParameterType::ePoint2F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3f >
	{
		static const ParameterType Type = ParameterType::ePoint3F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4f >
	{
		static const ParameterType Type = ParameterType::ePoint4F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2d >
	{
		static const ParameterType Type = ParameterType::ePoint2D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3d >
	{
		static const ParameterType Type = ParameterType::ePoint3D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4d >
	{
		static const ParameterType Type = ParameterType::ePoint4D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Size >
	{
		static const ParameterType Type = ParameterType::eSize;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Position >
	{
		static const ParameterType Type = ParameterType::ePosition;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Rectangle >
	{
		static const ParameterType Type = ParameterType::eRectangle;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Colour >
	{
		static const ParameterType Type = ParameterType::eColour;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< HdrColour >
	{
		static const ParameterType Type = ParameterType::eHdrColour;
	};
}

