/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_DEBUG_H___
#define ___CASTOR_DEBUG_H___

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <ostream>
#include <sstream>
#include <string>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class DynamicLibrary;

	namespace debug
	{
		namespace backtrace
		{
			CU_API void showBacktrace( OutputStream & stream, int toCapture, int toSkip );
		}

		struct Backtrace
		{
			int m_toCapture;
			int m_toSkip;

			explicit Backtrace( int toCapture = 20, int toSkip = 2 )
				: m_toCapture{ toCapture }
				, m_toSkip{ toSkip }
			{
			}

		private:
			friend OutputStream & operator<<( OutputStream & stream, Backtrace const & trace )
			{
				backtrace::showBacktrace( stream, trace.m_toCapture, trace.m_toSkip );
				return stream;
			}
		};
		/**
		 *\~english
		 *\brief		Initialises debug data.
		 *\~french
		 *\brief		Initialise les données de débogage.
		 */
		CU_API void initialise();
		/**
		 *\~english
		 *\brief		Loads debug data for given library.
		 *\~french
		 *\brief		Charge les données de débogage pour la bibliothèque donnée.
		 */
		CU_API void loadModule( DynamicLibrary const & library );
		/**
		 *\~english
		 *\brief		Unloads debug data for given library.
		 *\~french
		 *\brief		Décharge les données de débogage pour la bibliothèque donnée.
		 */
		CU_API void unloadModule( DynamicLibrary const & library );
		/**
		 *\~english
		 *\brief		Cleans up debug data.
		 *\~french
		 *\brief		Nettoie les données de débogage.
		 */
		CU_API void cleanup();
		/**
		\~english
		\brief		Helper class used to enable allocation backtrace retrieval.
		\~french
		\brief		Classe d'aide permettant la récupération de la pile d'appels d'allocation.
		*/
		class Backtraced
		{
#if !defined( NDEBUG )

		protected:
			Backtraced()
				: m_callStack{ doGetCallStack() }
			{
			}

		private:
			static String doGetCallStack()
			{
				StringStream callStack;
				callStack << debug::Backtrace{};
				return callStack.str();
			}

		protected:
			String m_callStack;

#endif

			friend OutputStream & operator<<( OutputStream & stream, [[maybe_unused]] Backtraced const & traced )
			{
#if !defined( NDEBUG )
				stream << traced.m_callStack;
#endif
				return stream;
			}
		};
	}
}

#if !defined( NDEBUG )
#	define CU_DeclareDebugVar( type, var )\
	type var
#	define CU_DefineDebugVar( type, var, value )\
	type var{ value }
#	define CU_AssignDebugVar( var, value )\
	var = value
#else
#	define CU_DeclareDebugVar( type, var )
#	define CU_DefineDebugVar( type, var, value )
#	define CU_AssignDebugVar( var, value )
#endif

#endif
