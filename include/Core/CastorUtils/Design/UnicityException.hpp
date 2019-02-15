/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UNICITY_EXCEPTION___
#define ___CASTOR_UNICITY_EXCEPTION___

#include "CastorUtils/Exception/Exception.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		12/01/2016
	\~english
	\brief		Unicity errors enumeration.
	\~french
	\brief		Enumération des erreurs d'unicité.
	*/
	enum class UnicityError
	{
		//\~english No instance found when expecting one.	\~french Pas d'intance alors qu'on en attend une.
		eNoInstance,
		//\~english An instance found when there shouldn't be one.	\~french Un intance alors qu'on ne devrait pas en avoir.
		eAnInstance,
		CU_ScopedEnumBounds( eNoInstance )
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		12/01/2016
	\~english
	\brief		Unicity error texts.
	\~french
	\brief		Textes des erreurs d'unicité
	*/
	static char const * const STR_UNICITY_ERROR[] =
	{
		"No instance for Unique class ",
		"Duplicate instance for Unique class ",
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Unicity exception
	\~french
	\brief		Exception d'unicité
	*/
	class UnicityException
		: public castor::Exception
	{
	public:
		UnicityException( UnicityError error
			, std::string const & description
			, char const * file
			, char const * function
			, uint32_t line )
			: Exception( STR_UNICITY_ERROR[uint32_t( error )] + description
				, file
				, function
				, line )
		{
		}
	};
	//!\~english	Helper macro to use UnicityException.
	//\~french		Macro pour faciliter l'utilisation de UnicityException.
#	define CU_UnicityError( error, text ) throw UnicityException( error, text, __FILE__, __FUNCTION__, __LINE__)
}

#endif
