/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CASTOR_ASSERTION_H___
#define ___CASTOR_ASSERTION_H___

#include "CastorUtils.hpp"

#include "Exception.hpp"
#include "Logger.hpp"

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

namespace Castor
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
	inline void Assert( bool p_expr, char const * const p_description )
	{
		if ( !p_expr )
		{
			Logger::LogError( std::stringstream() << "Assertion failed: " << p_description );
			Logger::LogError( std::stringstream() << Debug::Backtrace{} );
			assert( false );
		}
	}
}

#		define CASTOR_ASSERT( pred, text )
	if ( !( pred ) )\
	{\
		throw Castor::AssertException( ( text ), __FILE__, __FUNCTION__, __LINE__ );\
	}

#	else

namespace Castor
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
	inline void Assert( Expr const & p_expr, char const * const p_description )
	{
		if ( !p_expr )
		{
			Logger::LogError( std::stringstream() << "Assertion failed: " << p_description );
			Logger::LogError( std::stringstream() << Debug::Backtrace{} );
			assert( false );
		}
	}
}

#		define CASTOR_ASSERT( pred, text ) Castor::Assert( pred, text )

#	endif
#else

#	define CASTOR_ASSERT( pred, text )

#endif

//!\~english Checks a pre-condition.	\~french Vérifie une pré-condition.
#define REQUIRE( cond ) CASTOR_ASSERT( cond, "Required condition failed: "#cond )
//!\~english Checks a post-condition.	\~french Vérifie une post-condition.
#define ENSURE( cond ) CASTOR_ASSERT( cond, "Ensured condition failed: "#cond )
//!\~english Checks an invariant condition.	\~french Vérifie une condition invariante.
#define CHECK_INVARIANT( cond ) CASTOR_ASSERT( cond, "Invariant check failed: "#cond )
//!\~english Attests a failure.	\~french Atteste d'un échec.
#define FAILURE( text ) CASTOR_ASSERT( false, "Failure: "#text )
//!\~english Declares the invariant checking function.	\~french Déclare la fonction de vérification des invariants de classe.
#define DECLARE_INVARIANT_BLOCK() void DoContractCheckInvariants()const;
//!\~english Begins invariant checking function implementation.	\~french Commence l'implémentation de la fonction de vérification des invariants de classe.
#define BEGIN_INVARIANT_BLOCK( className ) void className::DoContractCheckInvariants()const {
//!\~english Ends invariant checking function implementation.	\~french Termine l'implémentation de la fonction de vérification des invariants de classe.
#define END_INVARIANT_BLOCK() }
//!\~english Calls invariant checking function.	\~french Appelle la fonction de vérification des invariants de classe.
#define CHECK_INVARIANTS DoContractCheckInvariants

#endif
