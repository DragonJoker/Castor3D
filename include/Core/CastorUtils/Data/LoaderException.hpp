/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_LOADER_EXCEPTION___
#define ___CASTOR_LOADER_EXCEPTION___

#include "CastorUtils/Data/DataModule.hpp"

#include "CastorUtils/Exception/Exception.hpp"

namespace castor
{
	/**
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
		using castor::Exception::Exception;
	};
}
//!\~english	Helper macro to use LoaderException.
//\~french		Macro pour faciliter l'utilisation de LoaderException.
#define CU_LoaderError( text ) throw castor::LoaderException{ text, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }

#endif
