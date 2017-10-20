/*
See LICENSE file in root folder
*/
#ifndef ___CASTOR_DEBUG_H___
#define ___CASTOR_DEBUG_H___

#include "CastorUtilsPrerequisites.hpp"

namespace castor
{
	namespace Debug
	{
		struct Backtrace
		{
			int const m_toCapture;
			int const m_toSkip;

			inline Backtrace( int p_toCapture = 20, int p_toSkip = 2 )
				: m_toCapture{ p_toCapture }
				, m_toSkip{ p_toSkip }
			{
			}
		};
		/**
		 *\~english
		 *\brief			Puts the backtrace into a stream
		 *\param[in,out]	p_stream	The stream
		 *\~french
		 *\brief			Transmet la pile d'appels dans un flux
		 *\param[in,out]	p_stream	Le flux
		 */
		CU_API std::wostream & operator<<( std::wostream & p_stream, Backtrace const & );
		/**
		 *\~english
		 *\brief			Puts the backtrace into a stream
		 *\param[in,out]	p_stream	The stream
		 *\~french
		 *\brief			Transmet la pile d'appels dans un flux
		 *\param[in,out]	p_stream	Le flux
		 */
		CU_API std::ostream & operator<<( std::ostream & p_stream, Backtrace const & );

		/*!
		\author 	Sylvain DOREMUS
		\date		05/10/2015
		\version	0.8.0
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
			{
				std::stringstream callStack;
				callStack << castor::Debug::Backtrace{};
				m_callStack = callStack.str();
			}

			~Backtraced()
			{
			}

		protected:
			std::string m_callStack;
			friend std::ostream & operator<<( std::ostream & p_stream, Backtraced const & p_traced );

#endif
		};

		inline std::ostream & operator<<( std::ostream & p_stream, Backtraced const & p_traced )
		{
#if !defined( NDEBUG )

			p_stream << p_traced.m_callStack;

#endif

			return p_stream;
		}
	}
}

#endif
