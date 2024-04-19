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
		: public Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	givenType	The user given type.
		 *\param[in]	file		The incriminated file name.
		 *\param[in]	function	The incriminated function name.
		 *\param[in]	line		The incriminated line name.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	givenType	Le type donné par l'utilisateur.
		 *\param[in]	file		Le nom du fichier incriminé.
		 *\param[in]	function	Le nom de la fonction incriminée.
		 *\param[in]	line		Le numéro de la ligne incriminée.
		 */
		explicit ParserParameterTypeException( ParameterType givenType
			, char const * file
			, char const * function
			, uint32_t line )
			: Exception{ "Wrong parameter type in parser: user gave " + toUtf8( getTypeName( givenType ) )
					+ " while parameter base type is " + toUtf8( ParserParameterStringType< ExpectedT > )
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
