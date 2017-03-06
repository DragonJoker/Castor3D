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
#ifndef ___CASTOR_PARSER_PARAMETER_BASE_H___
#define ___CASTOR_PARSER_PARAMETER_BASE_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		04/02/2013
	\~english
	\brief		Parser function parameter types enumeration.
	\~french
	\brief		Enumération des types de paramètres pour une fonction d'analyse.
	*/
	enum class ParameterType
		: uint8_t
	{
		eText,
		eName,
		ePath,
		eCheckedText,
		eBitwiseOred32BitsCheckedText,
		eBitwiseOred64BitsCheckedText,
		eBool,
		eInt8,
		eInt16,
		eInt32,
		eInt64,
		eUInt8,
		eUInt16,
		eUInt32,
		eUInt64,
		eFloat,
		eDouble,
		eLongDouble,
		ePixelFormat,
		ePoint2I,
		ePoint3I,
		ePoint4I,
		ePoint2F,
		ePoint3F,
		ePoint4F,
		ePoint2D,
		ePoint3D,
		ePoint4D,
		eSize,
		ePosition,
		eRectangle,
		eColour,
		eHdrColour,
		CASTOR_SCOPED_ENUM_BOUNDS( eText )
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/03/2013
	\version	0.7.0
	\~english
	\brief		Template structure holding parameter specific data.
	\~french
	\brief		Structure template contenant les données spécifiques du paramètre.
	*/
	class ParserParameterBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructor.
		 */
		inline ParserParameterBase()
		{
		}
		/**
		 *\~english
		 *\brief		Retrieves the parameter type.
		 *\return		The type.
		 *\~french
		 *\brief		Récupère le type du paramètre.
		 *\return		Le type.
		 */
		CU_API virtual ParameterType GetType() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter base type (like ParameterType::eText for ParameterType::eName).
		 *\return		The type.
		 *\~french
		 *\brief		Récupère le type de base du paramètre (comme ParameterType::eText pour ParameterType::eName).
		 *\return		Le type.
		 */
		CU_API virtual ParameterType GetBaseType()
		{
			return GetType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parameter string type.
		 *\return		The type.
		 *\~french
		 *\brief		Récupère le type du chaîne paramètre.
		 *\return		Le type.
		 */
		CU_API virtual xchar const * const GetStrType() = 0;
		/**
		 *\~english
		 *\return		A copy of this parameter.
		 *\~french
		 *\return		Une copie de ce paramètre.
		 */
		CU_API virtual ParserParameterBaseSPtr Clone() = 0;
		/**
		 *\~english
		 *\brief			Checks the parameter.
		 *\param[in,out]	p_params	The text containing the parameter value.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Vérifie le paramètre.
		 *\param[in,out]	p_params	Le texte contenant la valeur du paramètre.
		 *\return			\p si un problème quelconque est arrivé.
		 */
		CU_API virtual bool Parse( String & p_params ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter value.
		 *\param[out]	p_value		Receives the value.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la valeur du paramètre.
		 *\param[out]	p_value		Reçoit la valeur.
		 *\return		La valeur.
		 */
		template< typename T > T const & Get( T & p_value );
	};
}

#include "ParserParameterBase.inl"

#endif
