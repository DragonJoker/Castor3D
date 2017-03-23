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
	};
}

#endif
