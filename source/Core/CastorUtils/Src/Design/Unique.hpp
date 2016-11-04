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
#ifndef ___CASTOR_UNIQUE_H___
#define ___CASTOR_UNIQUE_H___

#include "UnicityException.hpp"
#include "NonCopyable.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		08/12/2011
	\~english
	\brief		Representation of a Unique instance class
	\remarks	If another instance is to be created, an exception is thrown
	\~french
	\brief		Représentation d'un classe à instance unique
	\remarks	Si une seconde instance est créée, une exception est lancée
	*/
	template< class T >
	class Unique
		: private NonCopyable
	{
	public:
		/**
		 *\~english
		 *\remarks		Throws an exception if instance is not initialised.
		 *\return		The unique instance.
		 *\~french
		 *\remarks		Lance une exception si l'instance n'est pas initialisée.
		 *\return		L'instance unique.
		 */
		static inline T & GetInstance()
		{
			if ( !DoGetInstance() )
			{
				UNICITY_ERROR( UnicityError::eNoInstance, typeid( T ).name() );
			}

			return *DoGetInstance();
		}

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\remarks		Throws an exception if the instance is already initialised.
		 *\~french
		 *\brief		Constructeur.
		 *\remarks		Lance une exception si l'instance est déjà initialisée.
		 */
		inline Unique( T * p_this )
		{
			if ( !DoGetInstance() )
			{
				DoGetInstance() = p_this;
			}
			else
			{
				UNICITY_ERROR( UnicityError::eAnInstance, typeid( T ).name() );
			}
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~Unique()
		{
			DoGetInstance() = nullptr;
		}

	private:
		/**
		 *\~english
		 *\return		The unique instance, nullptr if none.
		 *\~french
		 *\return		L'instance unique, nullptr s'il n'y en a pas.
		 */
		static inline T *& DoGetInstance()
		{
			static T * l_pInstance = nullptr;
			return l_pInstance;
		}
	};
}

#endif
