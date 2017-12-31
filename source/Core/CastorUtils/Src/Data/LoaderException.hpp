/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LOADER_EXCEPTION___
#define ___CASTOR_LOADER_EXCEPTION___

#include "Exception/Exception.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Resource loading exception
	\remark		Thrown if the resource loader has encountered an error
	\~french
	\brief		Resource loading exception
	\remark		Lancée si un Loader rencontre un erreur
	*/
	class LoaderException
		: public castor::Exception
	{
	public:
		LoaderException( std::string const & p_description
			, char const * p_file
			, char const * p_function
			, uint32_t p_line )
			: Exception( p_description
				, p_file
				, p_function
				, p_line )
		{
		}
	};
	//!\~english Helper macro to use LoaderException	\~french Macro pour faciliter l'utilisation de LoaderException
#	define LOADER_ERROR( p_text) throw LoaderException( p_text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
