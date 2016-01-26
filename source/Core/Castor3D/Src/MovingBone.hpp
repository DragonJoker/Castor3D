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
#ifndef ___C3D_MOVING_BONE___
#define ___C3D_MOVING_BONE___

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
	class MovingBone
		: public MovingObjectBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API MovingBone();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~MovingBone();
		/**
		 *\~english
		 *\brief		Retrieves the object name
		 *\return		The name
		 *\~french
		 *\brief		Récupère le nom de l'objet
		 *\return		Le nom
		 */
		virtual Castor::String const & GetName()const;
		/**
		 *\~english
		 *\brief		Defines the movable object
		 *\param[in]	p_bone	The object
		 *\~french
		 *\brief		Définit l'objet mouvant
		 *\param[in]	p_bone	L'objet
		 */
		inline void SetBone( BoneSPtr p_bone )
		{
			m_bone = p_bone;
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
			return m_bone.lock();
		}

	private:
		virtual void DoApply();
		virtual MovingObjectBaseSPtr DoClone( Animation & p_animation );

	private:
		//!\~english	The bone affected by the animations	\~french	L'os affecté par les animations
		BoneWPtr m_bone;
	};
}

#endif
