/*
See LICENSE file in root folder
*/
#ifndef ___CU_ParserParameterBase_H___
#define ___CU_ParserParameterBase_H___

#include "CastorUtils/CastorUtilsPrerequisites.hpp"

namespace castor
{
	/**
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
	/**
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
		CU_API virtual ParameterType getType()const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter base type (like ParameterType::eText for ParameterType::eName).
		 *\return		The type.
		 *\~french
		 *\brief		Récupère le type de base du paramètre (comme ParameterType::eText pour ParameterType::eName).
		 *\return		Le type.
		 */
		CU_API virtual ParameterType getBaseType()const
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
		CU_API virtual xchar const * const getStrType()const = 0;
		/**
		 *\~english
		 *\return		A copy of this parameter.
		 *\~french
		 *\return		Une copie de ce paramètre.
		 */
		CU_API virtual ParserParameterBaseSPtr clone()const = 0;
		/**
		 *\~english
		 *\brief			Checks the parameter.
		 *\param[in,out]	params	The text containing the parameter value.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Vérifie le paramètre.
		 *\param[in,out]	params	Le texte contenant la valeur du paramètre.
		 *\return			\p si un problème quelconque est arrivé.
		 */
		CU_API virtual bool parse( String & params ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the parameter value.
		 *\param[out]	value	Receives the value.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère la valeur du paramètre.
		 *\param[out]	value	Reçoit la valeur.
		 *\return		La valeur.
		 */
		template< typename T >
		T const & get( T & value );
	};

	CU_API void getParameterValue( ParserParameterBase const & parameter
		, bool & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, int8_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, uint8_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, int16_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, uint16_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, int32_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, uint32_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, int64_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, uint64_t & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, float & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, double & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, long double & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, String & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Path & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, PixelFormat & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point2i & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point3i & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point4i & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point2f & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point3f & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point4f & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point2d & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point3d & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Point4d & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Size & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Position & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, Rectangle & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, RgbColour & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, RgbaColour & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, HdrRgbColour & value );
	CU_API void getParameterValue( ParserParameterBase const & parameter
		, HdrRgbaColour & value );
}

#include "ParserParameterBase.inl"

#endif
