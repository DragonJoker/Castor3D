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
#ifndef ___CASTOR_NAMED_H___
#define ___CASTOR_NAMED_H___

#include "Miscellaneous/StringUtils.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Class for named elements
	\remark		The name type is a template argument so anything can be a name for this class (default is castor::String)
	\~french
	\brief		Classe de base pour les éléments nommés
	\remark		Le nom est un argument template, ainsi n'importe quoi peut être un nom pour cette classe (même si c'est castor::String par défaut)
	*/
	template< typename T = String >
	class NamedBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom
		 */
		NamedBase( T const & p_name )
			: m_name( p_name )
		{
		}
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_named	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_named	L'objet à copier
		 */
		NamedBase( NamedBase const & p_named ) = default;
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_named	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_named	L'objet à déplacer
		 */
		NamedBase( NamedBase && p_named ) = default;
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~NamedBase() = default;
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_named	The object to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_named	L'objet à copier
		 */
		NamedBase & operator=( NamedBase const & p_named ) = default;
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_named	The object to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_named	L'objet à déplacer
		 */
		NamedBase & operator=( NamedBase && p_named ) = default;
		/**
		 *\~english
		 *\brief		Retrieves the name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom
		 *\return		Le nom
		 */
		inline T const & getName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\brief		sets the name
		 *\param[in]	p_name	The name
		 *\~french
		 *\brief		Définit le nom
		 *\param[in]	p_name	Le nom
		 */
		inline void setName( T const & p_name )
		{
			m_name = p_name;
		}

	protected:
		T m_name;
	};
	//!\~english	A typedef for NamedBase with String as a name
	//!\~french		Un typedef pour NamedBase avec String comme type de nom (le plus classique, quoi)
	typedef NamedBase< String > Named;
}

#endif
