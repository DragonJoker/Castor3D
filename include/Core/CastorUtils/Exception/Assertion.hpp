/*
See LICENSE file in root folder
*/
#ifndef ___CU_Assertion_H___
#define ___CU_Assertion_H___

#include "CastorUtils/Exception/ExceptionModule.hpp"

#include "CastorUtils/CastorUtils.hpp"

namespace castor
{
	CU_API void cuLogError( char const * const description )noexcept;
	CU_API void cuFailure( char const * const description )noexcept;
}

#if CU_UseAssert

#	if defined( CU_Assert )
#		undef CU_Assert
#	endif

#	if !defined( NDEBUG )
#		define CU_Assert( pred, text ) ( !!( pred ) )\
			|| ( castor::cuFailure( text ), 0 )
#	else
#		define CU_Assert( pred, text ) ( !!( pred ) )\
			|| ( castor::cuLogError( text ), 0 )
#	endif

//!\~english	Calls invariant checking function.
//!\~french		Appelle la fonction de vérification des invariants de classe.
#	define CU_CheckInvariants doContractCheckInvariants

#else

#	if !defined( NDEBUG )
#		define CU_Assert( pred, text ) assert( pred && text )
#	else
#		define CU_Assert( pred, text ) ( !!( pred ) )\
			|| ( castor::cuLogError( text ), 0 )
#	endif
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
