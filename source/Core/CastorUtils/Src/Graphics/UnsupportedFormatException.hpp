/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UNSUPPORTED_FORMAT_EXCEPTION_H___
#define ___CASTOR_UNSUPPORTED_FORMAT_EXCEPTION_H___

#include "Exception/Exception.hpp"
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
		 *\param[in]	p_description	Error description
		 *\param[in]	p_file			Function file
		 *\param[in]	p_function		Function name
		 *\param[in]	p_line			Function line
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_description	Description de l'erreur
		 *\param[in]	p_file			Fichier contenant la fonction
		 *\param[in]	p_function		Nom de la fonction
		 *\param[in]	p_line			Ligne dans la fonction
		 */
		UnsupportedFormatException(	std::string const & p_description
			, char const * p_file
			, char const * p_function
			, uint32_t p_line )
			: Exception( p_description, p_file, p_function, p_line )
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
#	define UNSUPPORTED_ERROR( p_text) throw UnsupportedFormatException( p_text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
