#include "Graphics/Colour.hpp"
#include "Data/Path.hpp"
#include "Math/Point.hpp"
#include "Graphics/Position.hpp"
#include "Graphics/Rectangle.hpp"
#include "Graphics/Size.hpp"

namespace Castor
{
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template< typename T > struct ParserValueTyper;
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< String >
	{
		static const ParameterType Type = ParameterType::Text;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Path >
	{
		static const ParameterType Type = ParameterType::Path;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< bool >
	{
		static const ParameterType Type = ParameterType::Bool;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int8_t >
	{
		static const ParameterType Type = ParameterType::Int8;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int16_t >
	{
		static const ParameterType Type = ParameterType::Int16;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int32_t >
	{
		static const ParameterType Type = ParameterType::Int32;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< int64_t >
	{
		static const ParameterType Type = ParameterType::Int64;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint8_t >
	{
		static const ParameterType Type = ParameterType::UInt8;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint16_t >
	{
		static const ParameterType Type = ParameterType::UInt16;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint32_t >
	{
		static const ParameterType Type = ParameterType::UInt32;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< uint64_t >
	{
		static const ParameterType Type = ParameterType::UInt64;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< float >
	{
		static const ParameterType Type = ParameterType::Float;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< double >
	{
		static const ParameterType Type = ParameterType::Double;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< long double >
	{
		static const ParameterType Type = ParameterType::LongDouble;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< PixelFormat >
	{
		static const ParameterType Type = ParameterType::PixelFormat;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2i >
	{
		static const ParameterType Type = ParameterType::Point2I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3i >
	{
		static const ParameterType Type = ParameterType::Point3I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4i >
	{
		static const ParameterType Type = ParameterType::Point4I;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2f >
	{
		static const ParameterType Type = ParameterType::Point2F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3f >
	{
		static const ParameterType Type = ParameterType::Point3F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4f >
	{
		static const ParameterType Type = ParameterType::Point4F;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point2d >
	{
		static const ParameterType Type = ParameterType::Point2D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point3d >
	{
		static const ParameterType Type = ParameterType::Point3D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Point4d >
	{
		static const ParameterType Type = ParameterType::Point4D;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Size >
	{
		static const ParameterType Type = ParameterType::Size;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Position >
	{
		static const ParameterType Type = ParameterType::Position;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Rectangle >
	{
		static const ParameterType Type = ParameterType::Rectangle;
	};
	//!\~english Retrieves parameter type from c++ type	\~french Récupère le type de paramètre à partir du type C++
	template<> struct ParserValueTyper< Colour >
	{
		static const ParameterType Type = ParameterType::Colour;
	};
}

