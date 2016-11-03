/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_PARSER_PARAMETER_HELPERS_H___
#define ___CASTOR_PARSER_PARAMETER_HELPERS_H___

#include "ParserParameterBase.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		Gives the regex format for given type.
	\~french
	\brief		Donne le format de regex pour le type donné.
	*/
	template< typename T > struct RegexFormat;
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for uint8_t.
	\~french
	\brief		Spécialisation de RegexFormat pour uint8_t.
	*/
	template<>
	struct RegexFormat< uint8_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for int8_t.
	\~french
	\brief		Spécialisation de RegexFormat pour int8_t.
	*/
	template<>
	struct RegexFormat< int8_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for uint16_t.
	\~french
	\brief		Spécialisation de RegexFormat pour uint16_t.
	*/
	template<>
	struct RegexFormat< uint16_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for int16_t.
	\~french
	\brief		Spécialisation de RegexFormat pour int16_t.
	*/
	template<>
	struct RegexFormat< int16_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for uint32_t.
	\~french
	\brief		Spécialisation de RegexFormat pour uint32_t.
	*/
	template<>
	struct RegexFormat< uint32_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for int32_t.
	\~french
	\brief		Spécialisation de RegexFormat pour int32_t.
	*/
	template<>
	struct RegexFormat< int32_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for uint64_t.
	\~french
	\brief		Spécialisation de RegexFormat pour uint64_t.
	*/
	template<>
	struct RegexFormat< uint64_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for int64_t.
	\~french
	\brief		Spécialisation de RegexFormat pour int64_t.
	*/
	template<>
	struct RegexFormat< int64_t >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for float.
	\~french
	\brief		Spécialisation de RegexFormat pour float.
	*/
	template<>
	struct RegexFormat< float >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for double.
	\~french
	\brief		Spécialisation de RegexFormat pour double.
	*/
	template<>
	struct RegexFormat< double >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for long double.
	\~french
	\brief		Spécialisation de RegexFormat pour long double.
	*/
	template<>
	struct RegexFormat< long double >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for Colour.
	\~french
	\brief		Spécialisation de RegexFormat pour Colour.
	*/
	template<>
	struct RegexFormat< Colour >
	{
		CU_API static xchar const * const Value;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		Parser parameter helper structure.
	\~french
	\brief		Structure d'aide pour les paramètres de parseur.
	*/
	template< ParameterType Type > struct ParserParameterHelper;
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Text.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Text.
	*/
	template<> struct ParserParameterHelper< ParameterType::Text >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = String;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Text;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Name.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Name.
	*/
	template<> struct ParserParameterHelper< ParameterType::Name >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = String;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Name;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::Text;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Path.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Path.
	*/
	template<> struct ParserParameterHelper< ParameterType::Path >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Path;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Path;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::CheckedText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::CheckedText.
	*/
	template<> struct ParserParameterHelper< ParameterType::CheckedText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::CheckedText;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::UInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::BitwiseOred32BitsCheckedText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::BitwiseOred32BitsCheckedText.
	*/
	template<> struct ParserParameterHelper< ParameterType::BitwiseOred32BitsCheckedText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::BitwiseOred32BitsCheckedText;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::UInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::BitwiseOred64BitsCheckedText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::BitwiseOred64BitsCheckedText.
	*/
	template<> struct ParserParameterHelper< ParameterType::BitwiseOred64BitsCheckedText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::BitwiseOred64BitsCheckedText;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::UInt64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Bool.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Bool.
	*/
	template<> struct ParserParameterHelper< ParameterType::Bool >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = bool;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Bool;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Int8.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Int8.
	*/
	template<> struct ParserParameterHelper< ParameterType::Int8 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int8_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Int8;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Int16.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Int16.
	*/
	template<> struct ParserParameterHelper< ParameterType::Int16 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int16_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Int16;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Int32.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Int32.
	*/
	template<> struct ParserParameterHelper< ParameterType::Int32 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Int32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Int64.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Int64.
	*/
	template<> struct ParserParameterHelper< ParameterType::Int64 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Int64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::UInt8.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::UInt8.
	*/
	template<> struct ParserParameterHelper< ParameterType::UInt8 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint8_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::UInt8;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::UInt16.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::UInt16.
	*/
	template<> struct ParserParameterHelper< ParameterType::UInt16 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint16_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::UInt16;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::UInt32.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::UInt32.
	*/
	template<> struct ParserParameterHelper< ParameterType::UInt32 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::UInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::UInt64.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::UInt64.
	*/
	template<> struct ParserParameterHelper< ParameterType::UInt64 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::UInt64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Float.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Float.
	*/
	template<> struct ParserParameterHelper< ParameterType::Float >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = float;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Float;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Double.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Double.
	*/
	template<> struct ParserParameterHelper< ParameterType::Double >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = double;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Double;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::LongDouble.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::LongDouble.
	*/
	template<> struct ParserParameterHelper< ParameterType::LongDouble >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = long double;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::LongDouble;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::PixelFormat.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::PixelFormat.
	*/
	template<> struct ParserParameterHelper< ParameterType::PixelFormat >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = PixelFormat;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::PixelFormat;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point2I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point2I.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point2I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point2I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point3I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point3I.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point3I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point3I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point4I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point4I.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point4I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point4I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point2F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point2F.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point2F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point2F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point3F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point3F.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point3F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point3F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point4F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point4F.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point4F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point4F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point2D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point2D.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point2D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point2D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point3D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point3D.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point3D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point3D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Point4D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Point4D.
	*/
	template<> struct ParserParameterHelper< ParameterType::Point4D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Point4D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Size.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Size.
	*/
	template<> struct ParserParameterHelper< ParameterType::Size >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Size;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Size;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Position.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Position.
	*/
	template<> struct ParserParameterHelper< ParameterType::Position >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Position;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Position;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Rectangle.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Rectangle.
	*/
	template<> struct ParserParameterHelper< ParameterType::Rectangle >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Rectangle;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Rectangle;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::Colour.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::Colour.
	*/
	template<> struct ParserParameterHelper< ParameterType::Colour >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Colour;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::Colour;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		Structure used to tell if an ParameterType has a base parameter type.
	\~french
	\brief		Structure utilisée pour déterminer si un ParameterType a un type de paramètre de base.
	*/
	template< ParameterType Type >
	struct has_base_parameter_type
		: public std::false_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::Name.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::Name.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::Name >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::CheckedText.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::CheckedText.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::CheckedText >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::BitwiseOred32BitsCheckedText.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::BitwiseOred32BitsCheckedText.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::BitwiseOred32BitsCheckedText >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::BitwiseOred64BitsCheckedText.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::BitwiseOred64BitsCheckedText.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::BitwiseOred64BitsCheckedText >
		: public std::true_type
	{
	};
	/**
	 *\~english
	 *\brief		Retrieves the given parameter type's name.
	 *\param[in]	p_type	The parameter type.
	 *\~french
	 *\brief		Récupère le nom du type de paramètre donné.
	 *\param[in]	p_type	Le type de paramètre.
	 */
	CU_API String const & GetTypeName( ParameterType p_type );
}

#endif
