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
#ifndef ___C3D_MOVING_BONE_H___
#define ___C3D_MOVING_BONE_H___

#include "MovingObjectBase.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		09/12/2013
	\~english
	\brief		Implementation of MovingObjectBase for Bones.
	\~french
	\brief		Implémentation de MovingObjectBase pour les Bones.
	*/
	class C3D_API MovingBone
		: public MovingObjectBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		MovingBone();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~MovingBone();
		/**
		 *\~english
		 *\brief		Defines the movable object
		 *\param[in]	p_pObject	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_pObject	L'objet
		 */
		inline void SetBone( BoneSPtr p_pBone )
		{
			m_pBone = p_pBone;
		}
		/**
		 *\~english
		 *\brief		Retrieves the moving object
		 *\return		The moving object
		 *\~french
		 *\brief		Récupère l'objet mouvant
		 *\return		L'objet mouvant
		 */
		inline BoneSPtr GetBone()const
		{
			return m_pBone.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;

	private:
		virtual void DoApply();
		virtual MovingObjectBaseSPtr DoClone();

	private:
		//!\~english The bone affected by the animations	\~french L'os affecté par les animations
		BoneWPtr m_pBone;
	};
}

#endif
