/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_ASSERTION_H___
#define ___CASTOR_ASSERTION_H___

#include "CastorUtils/CastorUtils.hpp"

#include "CastorUtils/Exception/Exception.hpp"
#include "CastorUtils/Log/Logger.hpp"

#ifndef CU_UseAssert
#	ifndef NDEBUG
#		define CU_UseAssert 1
#		define CU_ExceptAssert 0
#	else
#		define CU_UseAssert 0
#		define CU_ExceptAssert 0
#	endif
#endif

#if CU_UseAssert

#	if defined( CU_Assert )
#		undef CU_Assert
#	endif

#	if CU_ExceptAssert

namespace castor
{
	/**
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		16/10/2015
	\~english
	\brief		Exception class thrown on Debug builds, when an assertion fails.
	\~french
	\brief		Classe d'exception levée en Debug, lorsqu'une assertion échoue.
	*/
	class AssertException
		: public Debug::Backtraced
		, public Exception
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	description	The exception description
		 *\param[in]	file		The file name
		 *\param[in]	function	The function name
		 *\param[in]	line		The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	description	La description de l'exception
		 *\param[in]	file		Le nom du fichier
		 *\param[in]	function	Le nom de la fonction
		 *\param[in]	line		Le numéro de ligne
		 */
		AssertException( std::string const & description, char const * file, char const * function, uint32_t line )
			: Debug::Backtraced()
			, Exception( "Assertion failed: " + description + "\n" + m_callStack, file, function, line )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~AssertException() throw()
		{
		}
	};
	/**
	 *\~english
	 *\brief		Specified constructor
	 *\param[in]	description	The exception description
	 *\param[in]	file		The file name
	 *\param[in]	function	The function name
	 *\param[in]	line		The line number
	 *\~french
	 *\brief		Constructeur spécifié
	 *\param[in]	description	La description de l'exception
	 *\param[in]	file		Le nom du fichier
	 *\param[in]	function	Le nom de la fonction
	 *\param[in]	line		Le numéro de ligne
	 */
	inline void cuAssert( bool expr, char const * const description )
	{
		if ( !expr )
		{
			Logger::logError( std::stringstream() << "Assertion failed: " << description );
			Logger::logError( std::stringstream() << Debug::Backtrace{} );
			assert( false );
		}
	}
}

#		define CU_Assert( pred, text )
	if ( !( pred ) )\
	{\
		throw castor::AssertException( ( text ), __FILE__, __FUNCTION__, __LINE__ );\
	}

#	else

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

#		define CU_Assert( pred, text ) castor::cuAssert( pred, text )

#	endif

//!\~english	Calls invariant checking function.
//!\~french		Appelle la fonction de vérification des invariants de classe.
#	define CU_CheckInvariants doContractCheckInvariants

#else

#	define CU_Assert( pred, text )
#	define CU_CheckInvariants()

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
