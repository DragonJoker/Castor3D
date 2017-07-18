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
#ifndef ___CASTOR_DEBUG_H___
#define ___CASTOR_DEBUG_H___

#include "CastorUtilsPrerequisites.hpp"

namespace Castor
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
				callStack << Castor::Debug::Backtrace{};
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
