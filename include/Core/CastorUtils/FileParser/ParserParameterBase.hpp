/*
See LICENSE file in root folder
*/
#ifndef ___CU_ParserParameterBase_H___
#define ___CU_ParserParameterBase_H___

#include "CastorUtils/FileParser/FileParserModule.hpp"

#include "CastorUtils/Data/DataModule.hpp"
#include "CastorUtils/Graphics/GraphicsModule.hpp"
#include "CastorUtils/Log/LogModule.hpp"
#include "CastorUtils/Math/MathModule.hpp"

namespace castor
{
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
		 *\return		The parameter type.
		 *\~french
		 *\return		Le type du paramètre.
		 */
		CU_API virtual ParameterType getType()const = 0;
		/**
		 *\~english
		 *\return		The parameter base type (like ParameterType::eText for ParameterType::eName).
		 *\~french
		 *\return		Le type de base du paramètre (comme ParameterType::eText pour ParameterType::eName).
		 */
		CU_API virtual ParameterType getBaseType()const
		{
			return getType();
		}
		/**
		 *\~english
		 *\return		The parameter string type.
		 *\~french
		 *\return		Le type du chaîne paramètre.
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
		 *\param[in,out]	logger	The logger instance.
		 *\param[in,out]	params	The text containing the parameter value.
		 *\return			\p false if any error occured.
		 *\~french
		 *\brief			Vérifie le paramètre.
		 *\param[in,out]	logger	L'instance de logger.
		 *\param[in,out]	params	Le texte contenant la valeur du paramètre.
		 *\return			\p false si un problème quelconque est arrivé.
		 */
		CU_API virtual bool parse( LoggerInstance & logger
			, String & params ) = 0;
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
		T get()
		{
			T result;
			this->get( result );
			return result;
		}
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
