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
#ifndef ___CASTOR_NON_COPYABLE_H___
#define ___CASTOR_NON_COPYABLE_H___

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\date		08/12/2011
	\~english
	\brief		The non-copyable concept implementation
	\remark		Forbids a class which derivates from this one to be copied, either way
	\~french
	\brief		Implémentation du concept de non-copiable
	\remark		Interdit la copie des classes dérivant de celle-ci, de quelque manière que ce soit
	*/
	class NonCopyable
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		NonCopyable() {}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~NonCopyable() {}

	private:
		/**
		 *\~english
		 *\brief		Private copy constructor, to forbid copy by construction
		 *\~french
		 *\brief		Constructeur par copie privé, afin d'en interdire la possiblité
		 */
		NonCopyable( NonCopyable const & ) = delete;
		/**
		 *\~english
		 *\brief		Private copy assignment operator, to forbid assignment copy
		 *\~french
		 *\brief		Operateur d'affectation par copie privé, afin d'en interdire la possibilité
		 */
		NonCopyable & operator =( NonCopyable const & ) = delete;
		/**
		 *\~english
		 *\brief		Private move constructor, to forbid move by construction
		 *\~french
		 *\brief		Constructeur par déplacement privé, afin d'en interdire la possiblité
		 */
		NonCopyable( NonCopyable && /*src*/ ) = delete;
		/**
		 *\~english
		 *\brief		Private move assignment operator, to forbid assignment move
		 *\~french
		 *\brief		Operateur d'affectation par déplacement privé, afin d'en interdire la possibilité
		 */
		NonCopyable & operator =( NonCopyable && ) = delete;
	};
}

#endif
