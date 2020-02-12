#include "CastorUtils/Data/Path.hpp"
#include "CastorUtils/Graphics/Colour.hpp"
#include "CastorUtils/Graphics/Position.hpp"
#include "CastorUtils/Graphics/Rectangle.hpp"
#include "CastorUtils/Graphics/Size.hpp"
#include "CastorUtils/Math/Point.hpp"

namespace castor
{
	//*************************************************************************

	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	*/
	template< typename T >
	struct ParserValueTyper;
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for bool.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour bool.
	*/
	template<>
	struct ParserValueTyper< bool >
	{
		static const ParameterType Type = ParameterType::eBool;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for int8_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour int8_t.
	*/
	template<>
	struct ParserValueTyper< int8_t >
	{
		static const ParameterType Type = ParameterType::eInt8;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for int16_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour int16_t.
	*/
	template<>
	struct ParserValueTyper< int16_t >
	{
		static const ParameterType Type = ParameterType::eInt16;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for int32_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour int32_t.
	*/
	template<>
	struct ParserValueTyper< int32_t >
	{
		static const ParameterType Type = ParameterType::eInt32;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for int64_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour int64_t.
	*/
	template<>
	struct ParserValueTyper< int64_t >
	{
		static const ParameterType Type = ParameterType::eInt64;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for uint8_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour uint8_t.
	*/
	template<>
	struct ParserValueTyper< uint8_t >
	{
		static const ParameterType Type = ParameterType::eUInt8;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for uint16_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour uint16_t.
	*/
	template<>
	struct ParserValueTyper< uint16_t >
	{
		static const ParameterType Type = ParameterType::eUInt16;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for uint32_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour uint32_t.
	*/
	template<>
	struct ParserValueTyper< uint32_t >
	{
		static const ParameterType Type = ParameterType::eUInt32;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for uint64_t.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour uint64_t.
	*/
	template<>
	struct ParserValueTyper< uint64_t >
	{
		static const ParameterType Type = ParameterType::eUInt64;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for float.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour float.
	*/
	template<>
	struct ParserValueTyper< float >
	{
		static const ParameterType Type = ParameterType::eFloat;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for double.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour double.
	*/
	template<>
	struct ParserValueTyper< double >
	{
		static const ParameterType Type = ParameterType::eDouble;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for long double.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour long double.
	*/
	template<>
	struct ParserValueTyper< long double >
	{
		static const ParameterType Type = ParameterType::eLongDouble;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::String.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::String.
	*/
	template<>
	struct ParserValueTyper< String >
	{
		static const ParameterType Type = ParameterType::eText;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Path.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Path.
	*/
	template<>
	struct ParserValueTyper< Path >
	{
		static const ParameterType Type = ParameterType::ePath;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::PixelFormat.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::PixelFormat.
	*/
	template<>
	struct ParserValueTyper< PixelFormat >
	{
		static const ParameterType Type = ParameterType::ePixelFormat;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point2i.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point2i.
	*/
	template<>
	struct ParserValueTyper< Point2i >
	{
		static const ParameterType Type = ParameterType::ePoint2I;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point3i.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point3i.
	*/
	template<>
	struct ParserValueTyper< Point3i >
	{
		static const ParameterType Type = ParameterType::ePoint3I;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point4i.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point4i.
	*/
	template<>
	struct ParserValueTyper< Point4i >
	{
		static const ParameterType Type = ParameterType::ePoint4I;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point2f.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point2f.
	*/
	template<>
	struct ParserValueTyper< Point2f >
	{
		static const ParameterType Type = ParameterType::ePoint2F;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point3f.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point3f.
	*/
	template<>
	struct ParserValueTyper< Point3f >
	{
		static const ParameterType Type = ParameterType::ePoint3F;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point4f.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point4f.
	*/
	template<>
	struct ParserValueTyper< Point4f >
	{
		static const ParameterType Type = ParameterType::ePoint4F;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point2d.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point2d.
	*/
	template<>
	struct ParserValueTyper< Point2d >
	{
		static const ParameterType Type = ParameterType::ePoint2D;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point3d.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point3d.
	*/
	template<>
	struct ParserValueTyper< Point3d >
	{
		static const ParameterType Type = ParameterType::ePoint3D;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Point4d.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Point4d.
	*/
	template<>
	struct ParserValueTyper< Point4d >
	{
		static const ParameterType Type = ParameterType::ePoint4D;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Size.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Size.
	*/
	template<>
	struct ParserValueTyper< Size >
	{
		static const ParameterType Type = ParameterType::eSize;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Position.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Position.
	*/
	template<>
	struct ParserValueTyper< Position >
	{
		static const ParameterType Type = ParameterType::ePosition;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::Rectangle.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::Rectangle.
	*/
	template<>
	struct ParserValueTyper< Rectangle >
	{
		static const ParameterType Type = ParameterType::eRectangle;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::RgbColour.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::RgbColour.
	*/
	template<>
	struct ParserValueTyper< RgbColour >
	{
		static const ParameterType Type = ParameterType::eRgbColour;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::RgbaColour.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::RgbaColour.
	*/
	template<>
	struct ParserValueTyper< RgbaColour >
	{
		static const ParameterType Type = ParameterType::eRgbaColour;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::HdrRgbColour.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::HdrRgbColour.
	*/
	template<>
	struct ParserValueTyper< HdrRgbColour >
	{
		static const ParameterType Type = ParameterType::eHdrRgbColour;
	};
	/**
	\~english
	\brief		Retrieves parameter type from c++ type.
	\remarks	Specialisation for castor::HdrRgbaColour.
	\~french
	\brief		Récupère le type de paramètre à partir du type C++.
	\remarks	Spécialisation pour castor::HdrRgbaColour.
	*/
	template<>
	struct ParserValueTyper< HdrRgbaColour >
	{
		static const ParameterType Type = ParameterType::eHdrRgbaColour;
	};

	//*************************************************************************

	template< typename T >
	T const & ParserParameterBase::get( T & value )
	{
		getParameterValue( *this, value );
		return value;
	}

	//*************************************************************************
}

