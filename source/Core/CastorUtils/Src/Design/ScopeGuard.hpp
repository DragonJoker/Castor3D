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
#ifndef ___CASTOR_SCOPE_GUARD_H___
#define ___CASTOR_SCOPE_GUARD_H___

#include "Miscellaneous/StringUtils.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		05/09/2016
	\~english
	\brief		Class used to execute code at scope exit.
	\~french
	\brief		Classe utilisée pour exécuter du code à la sortie d'un scope.
	*/
	template< typename ScopeExitFuncType >
	class ScopeGuard
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_function	The function.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_function	La fonction.
		 */
		ScopeGuard( ScopeExitFuncType const & p_function )
			: m_function{ p_function }
		{
		}
		/**
		 *\~english
		 *\brief		Copy Constructor.
		 *\param[in]	p_rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_rhs	L'objet à copier.
		 */
		ScopeGuard( ScopeGuard const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Move Constructor.
		 *\param[in]	p_rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	p_rhs	L'objet à déplacer.
		 */
		ScopeGuard( ScopeGuard && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~ScopeGuard()
		{
			m_function();
		}
		/**
		 *\~english
		 *\brief		Copy Constructor.
		 *\param[in]	p_rhs	The object to copy.
		 *\~french
		 *\brief		Constructeur par copie.
		 *\param[in]	p_rhs	L'objet à copier.
		 */
		ScopeGuard & operator=( ScopeGuard const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Move Constructor.
		 *\param[in]	p_rhs	The object to move.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 *\param[in]	p_rhs	L'objet à déplacer.
		 */
		ScopeGuard & operator=( ScopeGuard && p_rhs ) = default;

	protected:
		ScopeExitFuncType m_function;
	};
	/**
	 *\~english
	 *\brief		Helper function to create a ScopeGuard.
	 *\param[in]	p_function	The function.
	 *\return		The ScopeGuard.
	 *\~french
	 *\brief		Fonction d'aide à la création d'un ScopeGuard.
	 *\param[in]	p_function	La fonction.
	 *\return		Le ScopeGuard.
	 */
	template< typename ScopeExitFuncType >
	ScopeGuard< ScopeExitFuncType > makeScopeGuard( ScopeExitFuncType const & p_function )
	{
		return ScopeGuard< ScopeExitFuncType >( p_function );
	}
}

#endif
