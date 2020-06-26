/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___
#define ___CASTOR_PARSER_PARAMETER_TYPE_EXCEPTION_H___

#include "CastorUtils/Exception/Exception.hpp"

#include "CastorUtils/FileParser/ParserParameterHelpers.hpp"

namespace castor
{
	template< ParameterType GivenType >
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
		inline explicit ParserParameterTypeException( ParameterType expectedType )
			: castor::Exception( "", "", "", 0 )
		{
			m_description = "Wrong parameter type in parser: user gave " + string::stringCast< xchar >( ParserParameterHelper< GivenType >::StringType ) + " while parameter base type is " + string::stringCast< xchar >( getTypeName( expectedType ) );
		}
	};
}

#endif
