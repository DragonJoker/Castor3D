/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UNSUPPORTED_FORMAT_EXCEPTION_H___
#define ___CASTOR_UNSUPPORTED_FORMAT_EXCEPTION_H___

#include "CastorUtils/Exception/Exception.hpp"

#include <vector>
#include <algorithm>
#include <numeric>

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Unsupported format exception
	\~french
	\brief		Unsupported format exception
	*/
	class UnsupportedFormatException
		: public castor::Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	description	Error description
		 *\param[in]	file		Function file
		 *\param[in]	function	Function name
		 *\param[in]	line		Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	description	Description de l'erreur
		 *\param[in]	file		Fichier contenant la fonction
		 *\param[in]	function	Nom de la fonction
		 *\param[in]	line		Ligne dans la fonction
		 */
		UnsupportedFormatException(	std::string const & description
			, char const * file
			, char const * function
			, uint32_t line )
			: Exception( description, file, function, line )
		{
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		english Helper macro to use UnsupportedFormatException
	\~french
	\brief		Macro écrite pour faciliter l'utilisation de UnsupportedFormatException
	*/
#	define CU_UnsupportedError( text) throw UnsupportedFormatException( text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
