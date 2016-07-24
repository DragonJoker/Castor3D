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
#ifndef ___CASTOR_RESOURCE_H___
#define ___CASTOR_RESOURCE_H___

#include "Collection.hpp"
#include "Named.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		External resource representation
	\remark		A resource is a collectionnable object. You can change it's name, the Collection is noticed of the change
	\~french
	\brief		Représentation d'une ressource externe
	\remark		Une ressource est un objet collectionnable. Si le nom de la ressource est changé, la Collection est notifiée du changement
	*/
	template< class ResType >
	class Resource
		: public Named
	{
	protected:
		typedef Collection< ResType, String > collection;
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name	The name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name	Le nom
		 */
		Resource( String const & p_name );
		/**
		*\~english
		*\brief		Destructor
		*\~french
		*\brief		Destructeur
		*/
		~Resource();

	public:
		/**
		 *\~english
		 *\brief		Copy Constructor
		 *\param[in]	p_other	The Resource to copy
		 *\~french
		 *\brief		Constructeur par copie
		 *\param[in]	p_other	La Resource à copier
		 */
		Resource( Resource const & p_other );
		/**
		 *\~english
		 *\brief		Move Constructor
		 *\param[in]	p_other	The Resource to move
		 *\~french
		 *\brief		Constructeur par déplacement
		 *\param[in]	p_other	La Resource à déplacer
		 */
		Resource( Resource && p_other );
		/**
		 *\~english
		 *\brief		Copy assignment operator
		 *\param[in]	p_other	The Resource to copy
		 *\return		A reference to this Resource
		 *\~french
		 *\brief		Opérateur d'affectation par copie
		 *\param[in]	p_other	La Resource à copier
		 *\return		Une référence sur ce Resource
		 */
		Resource & operator=( Resource const & p_other );
		/**
		 *\~english
		 *\brief		Move assignment operator
		 *\param[in]	p_other	The Resource to move
		 *\return		A reference to this Resource
		 *\~french
		 *\brief		Opérateur d'affectation  par déplacement
		 *\param[in]	p_other	La Resource à déplacer
		 *\return		Une référence sur ce Resource
		 */
		Resource & operator=( Resource && p_other );
		/**
		 *\~english
		 *\brief		Defines the resource name
		 *\remarks		Also tells the collection the new resource name
		 *\param[in]	p_name	The new name
		 *\~french
		 *\brief		Définit le nom de la ressource
		 *\remarks		Dit aussi à la collection le nouveau de la ressource
		 *\param[in]	p_name	Le nouveau nom
		 */
		void ChangeName( String const & p_name );
	};

#	include "Resource.inl"
}

#endif
