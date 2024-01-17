/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___
#define ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___

#include "CastorUtils/Exception/Exception.hpp"

#include "CastorUtils/FileParser/ParserParameterHelpers.hpp"

namespace castor
{
	template< ParameterType ExpectedT >
	class ParserParameterTypeException
		: public castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	expectedType	The real parameter type.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	expectedType	Le type réel du paramètre.
		 */
		explicit ParserParameterTypeException( ParameterType givenType
			, char const * file
			, char const * function
			, uint32_t line )
			: castor::Exception{ "Wrong parameter type in parser: user gave " + String{ getTypeName( givenType ) }
					+ " while parameter base type is " + String{ ParserParameterStringType< ExpectedT > }
				, file
				, function
				, line }
		{
		}
	};
}

#define CU_ParserParameterException( expected, given )\
	throw castor::ParserParameterTypeException< expected >{ given, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }

#endif
