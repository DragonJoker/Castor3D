/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_PARSER_PARAMETER_HELPERS_H___
#define ___CASTOR_PARSER_PARAMETER_HELPERS_H___

#include "FileParser/ParserParameterBase.hpp"

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
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		Parser parameter helper structure.
	\~french
	\brief		Structure d'aide pour les paramètres de parseur.
	*/
	template< ePARAMETER_TYPE Type > struct ParserParameterHelper;
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_TEXT.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_TEXT.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_TEXT >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = String;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_TEXT;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_NAME.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_NAME.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_NAME >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = String;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_NAME;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterBaseType = ePARAMETER_TYPE_TEXT;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_PATH.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_PATH.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_PATH >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Path;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_PATH;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_CHECKED_TEXT.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_CHECKED_TEXT.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_CHECKED_TEXT >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_CHECKED_TEXT;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ePARAMETER_TYPE ParameterBaseType = ePARAMETER_TYPE_UINT32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ePARAMETER_TYPE ParameterBaseType = ePARAMETER_TYPE_UINT32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT;
		//!~english The parameter base type.	\~french Le type de base du paramètre.
		CU_API static const ePARAMETER_TYPE ParameterBaseType = ePARAMETER_TYPE_UINT64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_BOOL.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_BOOL.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_BOOL >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = bool;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_BOOL;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_INT8.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_INT8.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_INT8 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int8_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_INT8;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_INT16.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_INT16.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_INT16 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int16_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_INT16;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_INT32.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_INT32.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_INT32 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_INT32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_INT64.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_INT64.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_INT64 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = int64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_INT64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_UINT8.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_UINT8.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_UINT8 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint8_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_UINT8;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_UINT16.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_UINT16.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_UINT16 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint16_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_UINT16;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_UINT32.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_UINT32.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_UINT32 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint32_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_UINT32;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_UINT64.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_UINT64.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_UINT64 >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = uint64_t;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_UINT64;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_FLOAT.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_FLOAT.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_FLOAT >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = float;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_FLOAT;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_DOUBLE.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_DOUBLE.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_DOUBLE >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = double;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_DOUBLE;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_LONGDOUBLE.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_LONGDOUBLE.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_LONGDOUBLE >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = long double;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_LONGDOUBLE;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_PIXELFORMAT.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_PIXELFORMAT.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_PIXELFORMAT >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = ePIXEL_FORMAT;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_PIXELFORMAT;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT2I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT2I.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT2I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT2I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT3I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT3I.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT3I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT3I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT4I.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT4I.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT4I >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4i;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT4I;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT2F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT2F.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT2F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT2F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT3F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT3F.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT3F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT3F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT4F.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT4F.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT4F >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4f;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT4F;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT2D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT2D.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT2D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point2d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT2D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT3D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT3D.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT3D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point3d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT3D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POINT4D.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POINT4D.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POINT4D >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Point4d;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POINT4D;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_SIZE.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_SIZE.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_SIZE >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Size;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_SIZE;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_POSITION.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_POSITION.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_POSITION >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Position;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_POSITION;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_RECTANGLE.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_RECTANGLE.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_RECTANGLE >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Rectangle;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_RECTANGLE;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		ParserParameterHelper specialisation for ePARAMETER_TYPE_COLOUR.
	\~french
	\brief		Specialisation de ParserParameterHelper pour ePARAMETER_TYPE_COLOUR.
	*/
	template<> struct ParserParameterHelper< ePARAMETER_TYPE_COLOUR >
	{
		//!~english The parameter value type.	\~french Le type de valeur du paramètre.
		using ValueType = Colour;
		//!~english The parameter type.	\~french Le type de paramètre.
		CU_API static const ePARAMETER_TYPE ParameterType = ePARAMETER_TYPE_COLOUR;
		//!~english The parameter type name.	\~french Le nom du type de paramètre.
		CU_API static xchar const * const StringType;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		Structure used to tell if an ePARAMETER_TYPE has a base parameter type.
	\~french
	\brief		Structure utilisée pour déterminer si un ePARAMETER_TYPE a un type de paramètre de base.
	*/
	template< ePARAMETER_TYPE Type >
	struct has_base_parameter_type
		: public std::false_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ePARAMETER_TYPE_NAME.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ePARAMETER_TYPE_NAME.
	*/
	template<>
	struct has_base_parameter_type< ePARAMETER_TYPE_NAME >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ePARAMETER_TYPE_CHECKED_TEXT.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ePARAMETER_TYPE_CHECKED_TEXT.
	*/
	template<>
	struct has_base_parameter_type< ePARAMETER_TYPE_CHECKED_TEXT >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT.
	*/
	template<>
	struct has_base_parameter_type< ePARAMETER_TYPE_32BITWISE_ORED_CHECKED_TEXT >
		: public std::true_type
	{
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		12/02/2016
	\version	0.8.0
	\~english
	\brief		has_base_parameter_type specialisation for ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT.
	\~french
	\brief		Spécialisation de has_base_parameter_type pour ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT.
	*/
	template<>
	struct has_base_parameter_type< ePARAMETER_TYPE_64BITWISE_ORED_CHECKED_TEXT >
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
	CU_API String const & GetTypeName( ePARAMETER_TYPE p_type );
}

#endif
