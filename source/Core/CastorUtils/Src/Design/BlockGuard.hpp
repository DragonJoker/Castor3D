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
#ifndef ___CU_BLOCK_GUARD_H___
#define ___CU_BLOCK_GUARD_H___

#include "CastorUtilsPrerequisites.hpp"

#include <functional>

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		11/12/2016
	\~english
	\brief		Allows to declare a scoped variable with an action on construction
				<br />and an action on destruction.
	\remarks	Useful to force execution of code when an exception is thrown.
	\~english
	\brief		Permet de d�clarer une variable de scope, avec un action � la construction
				<br />et une action � la destruction.
	\remarks	Utile pour forcer l'ex�cution de code, si une exception est lanc�e.
	\b Example
	@code
		char * l_buffer = nullptr;
		{
			auto l_guard = make_block_guard( [&l_buffer, &l_size]()
			{
				l_buffer = new char[l_size + 1];
			},
			[&buffer]()
			{
				delete [] l_buffer;
			} );
			//
			// ... Code that might throw an exception ...
			//
		}
	@endcode
	*/
	template< typename CleanFunc >
	struct BlockGuard
	{
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Executes the init action.
		 *\param[in]	p_init	The action executed on construction.
		 *\param[in]	p_clean	The action executed on destruction.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		Ex�cute l'action d'initialisation.
		 *\param[in]	p_init	L'action effectu�e � la construction.
		 *\param[in]	p_clean	L'action effectue lors de la destruction.
		*/
		template< typename InitFunc >
		BlockGuard( InitFunc p_init, CleanFunc p_clean )
			: m_clean( p_clean )
		{
			p_init();
		};
		/**
		 *\~english
		 *\brief		Destructor.
		 *\remarks		Executes the clean action.
		 *\~french
		 *\brief		Destructeur.
		 *\remarks		Ex�cute l'action de nettoyage.
		 */
		~BlockGuard()
		{
			m_clean();
		}

	private:
		//!\~english	The clean action.
		//!\~french		L'action de nettoyage.
		CleanFunc m_clean;
	};
	/**
	 *\~english
	 *\brief		Helper function to declare a BlockGuard.
	 *\param[in]	p_init	The action executed on construction.
	 *\param[in]	p_clean	The action executed on destruction.
	 *\return		The block guard.
	 *\~french
	 *\brief		fonction d'aide � la construction d'un BlockGuard.
	 *\param[in]	p_init	L'action effectu�e � la construction.
	 *\param[in]	p_clean	L'action effectue lors de la destruction.
	 *\return		La guarde.
	*/
	template< typename InitFunc, typename CleanFunc >
	BlockGuard< CleanFunc > make_block_guard( InitFunc p_init, CleanFunc p_clean )
	{
		return BlockGuard< CleanFunc >( p_init, p_clean );
	}
}

#endif
