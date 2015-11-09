/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___C3D_MOVING_OBJECT_H___
#define ___C3D_MOVING_OBJECT_H___

#include "MovingObjectBase.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for MovableObject
	\~french
	\brief		Implémentation de MovingObjectBase pour les MovableObject
	*/
	class MovingObject
		: public MovingObjectBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API MovingObject();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MovingObject();
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		C3D_API virtual Castor::String const & GetName()const;
		/**
		 *\~english
		 *\brief		Defines the movable object
		 *\param[in]	p_object	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_object	L'objet
		 */
		inline void SetObject( MovableObjectSPtr p_object )
		{
			m_pObject = p_object;
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving object
		 *\return		The moving object
		 *\~french
		 *\brief		Récupère l'objet mouvant
		 *\return		L'objet mouvant
		 */
		inline MovableObjectSPtr GetObject()const
		{
			return m_pObject.lock();
		}

	private:
		C3D_API virtual void DoApply();
		C3D_API virtual MovingObjectBaseSPtr DoClone();

	private:
		//!\~english The object affected by the animations	\~french L'objet affecté par les animations
		MovableObjectWPtr m_pObject;
	};
}

#endif
