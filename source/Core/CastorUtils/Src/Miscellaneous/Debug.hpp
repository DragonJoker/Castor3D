/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
				std::stringstream l_callStack;
				l_callStack << Castor::Debug::Backtrace{};
				m_callStack = l_callStack.str();
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
		}
	}
}

#endif
