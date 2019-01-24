/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PARSER_PARAMETER_BASE_H___
#define ___CASTOR_PARSER_PARAMETER_BASE_H___

#include "CastorUtilsPrerequisites.hpp"

namespace castor
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
		eRgbColour,
		eRgbaColour,
		eHdrRgbColour,
		eHdrRgbaColour,
		CU_ScopedEnumBounds( eText )
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
		CU_API virtual ParameterType getType() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter base type (like ParameterType::eText for ParameterType::eName).
		 *\return		The type.
		 *\~french
		 *\brief		Récupère le type de base du paramètre (comme ParameterType::eText pour ParameterType::eName).
		 *\return		Le type.
		 */
		CU_API virtual ParameterType getBaseType()
		{
			return getType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the parameter string type.
		 *\return		The type.
		 *\~french
		 *\brief		Récupère le type du chaîne paramètre.
		 *\return		Le type.
		 */
		CU_API virtual xchar const * const getStrType() = 0;
		/**
		 *\~english
		 *\return		A copy of this parameter.
		 *\~french
		 *\return		Une copie de ce paramètre.
		 */
		CU_API virtual ParserParameterBaseSPtr clone() = 0;
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
		CU_API virtual bool parse( String & p_params ) = 0;
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
		template< typename T > T const & get( T & p_value );
	};
}

#include "ParserParameterBase.inl"

#endif
