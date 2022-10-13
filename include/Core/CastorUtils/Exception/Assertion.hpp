/*
See LICENSE file in root folder
*/
#ifndef ___CU_Assertion_H___
#define ___CU_Assertion_H___

#include "CastorUtils/Exception/ExceptionModule.hpp"

#include "CastorUtils/CastorUtils.hpp"

#if CU_UseAssert

#include "CastorUtils/Log/Logger.hpp"

#	if defined( CU_Assert )
#		undef CU_Assert
#	endif

namespace castor
{
	/**
	 *\~english
	 *\brief		Checks an assertion.
	 *\param[in]	expr		The expression to test.
	 *\param[in]	description	The assertion description.
	 *\~french
	 *\brief		Constructeur spécifié
	 *\param[in]	expr		L'expression à tester
	 *\param[in]	description	La description de l'assertion.
	 */
	template< typename Expr >
	inline void cuAssert( Expr const & expr
		, char const * const description )
	{
		if ( !expr )
		{
			Logger::logError( std::stringstream() << "Assertion failed: " << description );
			Logger::logError( std::stringstream() << Debug::Backtrace{} );
			assert( false );
		}
	}
}

#	define CU_Assert( pred, text ) castor::cuAssert( pred, text )

//!\~english	Calls invariant checking function.
//!\~french		Appelle la fonction de vérification des invariants de classe.
#	define CU_CheckInvariants doContractCheckInvariants

#else

#	define CU_Assert( pred, text ) assert( pred && text )
#	define CU_CheckInvariants() ((void)0)

#endif

//!\~english	Checks a pre-condition.
//!\~french		Vérifie une pré-condition.
#define CU_Require( cond ) CU_Assert( cond, "Required condition failed: "#cond )
//!\~english	Checks a post-condition.
//!\~french		Vérifie une post-condition.
#define CU_Ensure( cond ) CU_Assert( cond, "Ensured condition failed: "#cond )
//!\~english	Checks an invariant condition.
//!\~french		Vérifie une condition invariante.
#define CU_CheckInvariant( cond ) CU_Assert( cond, "Invariant check failed: "#cond )
//!\~english	Attests a failure.
//!\~french		Atteste d'un échec.
#define CU_Failure( text ) CU_Assert( false, "Failure: "#text )
//!\~english	Declares the invariant checking function.
//!\~french		Déclare la fonction de vérification des invariants de classe.
#define CU_DeclareInvariantBlock() void doContractCheckInvariants()const
//!\~english	Begins invariant checking function implementation.
//!\~french		Commence l'implémentation de la fonction de vérification des invariants de classe.
#define CU_BeginInvariantBlock( className ) void className::doContractCheckInvariants()const {
//!\~english	Ends invariant checking function implementation.
//!\~french		Termine l'implémentation de la fonction de vérification des invariants de classe.
#define CU_EndInvariantBlock() }

#endif
