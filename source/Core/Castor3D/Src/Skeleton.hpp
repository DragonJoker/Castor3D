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
#ifndef ___C3D_SKELETON_H___
#define ___C3D_SKELETON_H___

#include "Castor3DPrerequisites.hpp"

#include "Animable.hpp"
#include "Bone.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		The skeleton, holds each bone
	\~french
	\brief		Le squelette, contient chaque bone
	*/
	class Skeleton
		: public Animable
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API Skeleton();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Skeleton();
		/**
		 *\~english
		 *\brief		Adds a bone to the skeleton
		 *\param[in]	p_bone	The bone.
		 *\~french
		 *\brief		Ajoute un os au squelette
		 *\param[in]	p_bone	L'os.
		 */
		C3D_API void AddBone( BoneSPtr p_bone );
		/**
		 *\~english
		 *\brief		Adds a bone to another bone's children
		 *\param[in]	p_bone		The bone.
		 *\param[in]	p_parent	The parent bone.
		 *\~french
		 *\brief		Ajoute un os aux enfants d'un autre os.
		 *\param[in]	p_bone		L'os.
		 *\param[in]	p_parent	L'os parent.
		 */
		C3D_API void SetBoneParent( BoneSPtr p_bone, BoneSPtr p_parent );
		/**
		 *\~english
		 *\brief		Traverses bone hierarchy and applies given function to each bone.
		 *\param[in]	p_function	The function to apply.
		 *\~french
		 *\brief		Traverse la hiérachie du skelette et applique la fonction à chaque os.
		 *\param[in]	p_function	La fonction à appliquer.
		 */
		template< typename FuncT >
		inline void TraverseHierarchy( FuncT p_function )
		{
			for ( auto l_bone : m_bones )
			{
				if ( !l_bone->GetParent() )
				{
					p_function( l_bone );
					l_bone->TraverseHierarchy( p_function );
				}
			}
		}
		/**
		 *\~english
		 *\brief		Retrieves the global inverse transform
		 *\return		The value
		 *\~french
		 *\brief		Récupère la transformation globale inversée
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetGlobalInverseTransform()const
		{
			return m_globalInverse;
		}
		/**
		 *\~english
		 *\brief		Sets the global inverse transform
		 *\param[in]	p_transform	The new value
		 *\~french
		 *\brief		Définit la transformation globale inversée
		 *\param[in]	p_transform	La nouvelle valeur
		 */
		inline void SetGlobalInverseTransform( Castor::Matrix4x4r const & p_transform )
		{
			m_globalInverse = p_transform;
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first bone
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier os
		 *\return		La valeur
		 */
		inline decltype( auto ) begin()
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first bone
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier os
		 *\return		La valeur
		 */
		inline decltype( auto ) begin()const
		{
			return m_bones.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the bones array
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'os
		 *\return		La valeur
		 */
		inline decltype( auto ) end()
		{
			return m_bones.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the bones array
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'os
		 *\return		La valeur
		 */
		inline decltype( auto ) end()const
		{
			return m_bones.end();
		}

	private:
		//!\~english The bones	\~french Les bones
		BonePtrArray m_bones;
		//!\~english	The global skeleton transform	\~french	La transformation globale du squelette
		Castor::Matrix4x4r m_globalInverse;
	};
}

#endif
