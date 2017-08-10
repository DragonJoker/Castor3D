/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_ASSERTION_H___
#define ___CASTOR_ASSERTION_H___

#include "CastorUtils.hpp"

#include "Exception.hpp"
#include "Log/Logger.hpp"

#ifndef CASTOR_USE_ASSERT
#	ifndef NDEBUG
#		define CASTOR_USE_ASSERT 1
#		define CASTOR_EXCEPT_ASSERT 0
#	else
#		define CASTOR_USE_ASSERT 0
#		define CASTOR_EXCEPT_ASSERT 0
#	endif
#endif

#if CASTOR_USE_ASSERT

#	if defined( CASTOR_ASSERT )
#		undef CASTOR_ASSERT
#	endif

#	if CASTOR_EXCEPT_ASSERT

namespace castor
{
	/*!
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
		 *\param[in]	p_description	The exception description
		 *\param[in]	p_file			The file name
		 *\param[in]	p_function		The function name
		 *\param[in]	p_line			The line number
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	p_description	La description de l'exception
		 *\param[in]	p_file			Le nom du fichier
		 *\param[in]	p_function		Le nom de la fonction
		 *\param[in]	p_line			Le numéro de ligne
		 */
		AssertException( std::string const & p_description, char const * p_file, char const * p_function, uint32_t p_line )
			: Debug::Backtraced()
			, Exception( "Assertion failed: " + p_description + "\n" + m_callStack, p_file, p_function, p_line )
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
	 *\param[in]	p_description	The exception description
	 *\param[in]	p_file			The file name
	 *\param[in]	p_function		The function name
	 *\param[in]	p_line			The line number
	 *\~french
	 *\brief		Constructeur spécifié
	 *\param[in]	p_description	La description de l'exception
	 *\param[in]	p_file			Le nom du fichier
	 *\param[in]	p_function		Le nom de la fonction
	 *\param[in]	p_line			Le numéro de ligne
	 */
	inline void cuAssert( bool p_expr, char const * const p_description )
	{
		if ( !p_expr )
		{
			Logger::logError( std::stringstream() << "Assertion failed: " << p_description );
			Logger::logError( std::stringstream() << Debug::Backtrace{} );
			assert( false );
		}
	}
}

#		define CASTOR_ASSERT( pred, text )
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
	 *\param[in]	p_expr			The expression to test.
	 *\param[in]	p_description	The assertion description.
	 *\~french
	 *\brief		Constructeur spécifié
	 *\param[in]	p_expr			L'expression à tester
	 *\param[in]	p_description	La description de l'assertion.
	 */
	template< typename Expr >
	inline void cuAssert( Expr const & p_expr, char const * const p_description )
	{
		if ( !p_expr )
		{
			Logger::logError( std::stringstream() << "Assertion failed: " << p_description );
			Logger::logError( std::stringstream() << Debug::Backtrace{} );
			assert( false );
		}
	}
}

#		define CASTOR_ASSERT( pred, text ) castor::cuAssert( pred, text )

#	endif

//!\~english	Calls invariant checking function.
//!\~french		Appelle la fonction de vérification des invariants de classe.
#	define CHECK_INVARIANTS doContractCheckInvariants

#else

#	define CASTOR_ASSERT( pred, text )
#	define CHECK_INVARIANTS()

#endif

//!\~english	Checks a pre-condition.
//!\~french		Vérifie une pré-condition.
#define REQUIRE( cond ) CASTOR_ASSERT( cond, "Required condition failed: "#cond )
//!\~english	Checks a post-condition.
//!\~french		Vérifie une post-condition.
#define ENSURE( cond ) CASTOR_ASSERT( cond, "Ensured condition failed: "#cond )
//!\~english	Checks an invariant condition.
//!\~french		Vérifie une condition invariante.
#define CHECK_INVARIANT( cond ) CASTOR_ASSERT( cond, "Invariant check failed: "#cond )
//!\~english	Attests a failure.
//!\~french		Atteste d'un échec.
#define FAILURE( text ) CASTOR_ASSERT( false, "Failure: "#text )
//!\~english	Declares the invariant checking function.
//!\~french		Déclare la fonction de vérification des invariants de classe.
#define DECLARE_INVARIANT_BLOCK() void doContractCheckInvariants()const;
//!\~english	Begins invariant checking function implementation.
//!\~french		Commence l'implémentation de la fonction de vérification des invariants de classe.
#define BEGIN_INVARIANT_BLOCK( className ) void className::doContractCheckInvariants()const {
//!\~english	Ends invariant checking function implementation.
//!\~french		Termine l'implémentation de la fonction de vérification des invariants de classe.
#define END_INVARIANT_BLOCK() }

#endif
