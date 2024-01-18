/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_UNICITY_EXCEPTION___
#define ___CASTOR_UNICITY_EXCEPTION___

#include "CastorUtils/Design/DesignModule.hpp"

#include "CastorUtils/Exception/Exception.hpp"

namespace castor
{
	/**
	\~english
	\brief		Unicity errors enumeration.
	\~french
	\brief		Enumération des erreurs d'unicité.
	*/
	enum class UnicityError
	{
		//\~english No instance found when expecting one.	\~french Pas d'intance alors qu'on en attend une.
		eNoInstance = 0,
		//\~english An instance found when there shouldn't be one.	\~french Un intance alors qu'on ne devrait pas en avoir.
		eAnInstance = 1,
		CU_ScopedEnumBounds( eNoInstance, eAnInstance )
	};
	/**
	\~english
	\brief		Unicity error texts.
	\~french
	\brief		Textes des erreurs d'unicité
	*/
	static std::array< StringView, size_t( UnicityError::eCount ) > const STR_UNICITY_ERROR
	{
		"No instance for Unique class ",
		"Duplicate instance for Unique class ",
	};

	class UnicityException
		: public Exception
	{
	public:
		UnicityException( UnicityError error
			, std::string const & description
			, char const * file
			, char const * function
			, uint32_t line )
			: Exception( String{ STR_UNICITY_ERROR[uint32_t( error )] } + description
				, file
				, function
				, line )
		{
		}
	};
}
//!\~english	Helper macro to use UnicityException.
//\~french		Macro pour faciliter l'utilisation de UnicityException.
#define CU_UnicityError( error, text ) throw castor::UnicityException{ error, text, __FILE__, __FUNCTION__, uint32_t( __LINE__ ) }

#endif
