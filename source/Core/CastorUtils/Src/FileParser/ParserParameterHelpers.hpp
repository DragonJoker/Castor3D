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
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/02/2016
	\~english
	\brief		RegexFormat specialisation for Colour.
	\~french
	\brief		Spécialisation de RegexFormat pour Colour.
	*/
	template<>
	struct RegexFormat< HdrColour >
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
	\brief		ParserParameterHelper specialisation for ParameterType::eText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eText.
	*/
	template<> struct ParserParameterHelper< ParameterType::eText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = String;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eText;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eName.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eName.
	*/
	template<> struct ParserParameterHelper< ParameterType::eName >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = String;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eName;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::eText;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePath.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePath.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePath >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Path;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePath;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eCheckedText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eCheckedText.
	*/
	template<> struct ParserParameterHelper< ParameterType::eCheckedText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eCheckedText;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::eUInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eBitwiseOred32BitsCheckedText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eBitwiseOred32BitsCheckedText.
	*/
	template<> struct ParserParameterHelper< ParameterType::eBitwiseOred32BitsCheckedText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eBitwiseOred32BitsCheckedText;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::eUInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eBitwiseOred64BitsCheckedText.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eBitwiseOred64BitsCheckedText.
	*/
	template<> struct ParserParameterHelper< ParameterType::eBitwiseOred64BitsCheckedText >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eBitwiseOred64BitsCheckedText;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ParameterType ParameterBaseType = ParameterType::eUInt64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eBool.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eBool.
	*/
	template<> struct ParserParameterHelper< ParameterType::eBool >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = bool;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eBool;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eInt8.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eInt8.
	*/
	template<> struct ParserParameterHelper< ParameterType::eInt8 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int8_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eInt8;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eInt16.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eInt16.
	*/
	template<> struct ParserParameterHelper< ParameterType::eInt16 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int16_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eInt16;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eInt32.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eInt32.
	*/
	template<> struct ParserParameterHelper< ParameterType::eInt32 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eInt64.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eInt64.
	*/
	template<> struct ParserParameterHelper< ParameterType::eInt64 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eInt64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eUInt8.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eUInt8.
	*/
	template<> struct ParserParameterHelper< ParameterType::eUInt8 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint8_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eUInt8;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eUInt16.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eUInt16.
	*/
	template<> struct ParserParameterHelper< ParameterType::eUInt16 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint16_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eUInt16;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eUInt32.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eUInt32.
	*/
	template<> struct ParserParameterHelper< ParameterType::eUInt32 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eUInt32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eUInt64.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eUInt64.
	*/
	template<> struct ParserParameterHelper< ParameterType::eUInt64 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eUInt64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eFloat.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eFloat.
	*/
	template<> struct ParserParameterHelper< ParameterType::eFloat >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = float;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eFloat;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eDouble.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eDouble.
	*/
	template<> struct ParserParameterHelper< ParameterType::eDouble >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = double;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eDouble;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eLongDouble.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eLongDouble.
	*/
	template<> struct ParserParameterHelper< ParameterType::eLongDouble >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = long double;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eLongDouble;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePixelFormat.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePixelFormat.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePixelFormat >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = PixelFormat;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePixelFormat;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint2I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint2I.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint2I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint2I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint3I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint3I.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint3I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint3I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint4I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint4I.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint4I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint4I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint2F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint2F.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint2F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint2F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint3F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint3F.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint3F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint3F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint4F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint4F.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint4F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint4F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint2D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint2D.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint2D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint2D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint3D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint3D.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint3D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint3D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePoint4D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePoint4D.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePoint4D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePoint4D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eSize.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eSize.
	*/
	template<> struct ParserParameterHelper< ParameterType::eSize >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Size;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eSize;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::ePosition.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::ePosition.
	*/
	template<> struct ParserParameterHelper< ParameterType::ePosition >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Position;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::ePosition;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eRectangle.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eRectangle.
	*/
	template<> struct ParserParameterHelper< ParameterType::eRectangle >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Rectangle;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eRectangle;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eColour.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eColour.
	*/
	template<> struct ParserParameterHelper< ParameterType::eColour >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Colour;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eColour;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ParameterType::eHdrColour.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ParameterType::eHdrColour.
	*/
	template<> struct ParserParameterHelper< ParameterType::eHdrColour >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = HdrColour;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ParameterType ParamType = ParameterType::eHdrColour;
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
	\brief		has_base_parameter_type specialisation for ParameterType::eName.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::eName.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::eName >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::eCheckedText.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::eCheckedText.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::eCheckedText >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::eBitwiseOred32BitsCheckedText.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::eBitwiseOred32BitsCheckedText.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::eBitwiseOred32BitsCheckedText >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ParameterType::eBitwiseOred64BitsCheckedText.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ParameterType::eBitwiseOred64BitsCheckedText.
	*/
	template<>
	struct has_base_parameter_type< ParameterType::eBitwiseOred64BitsCheckedText >
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
