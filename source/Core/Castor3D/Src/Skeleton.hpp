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
#include "Mesh.hpp"

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
		 *\param[in]	p_pMesh	The parent mesh
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pMesh	Le maillage parent
		 */
		C3D_API Skeleton( MeshSPtr p_pMesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Skeleton();
		/**
		 *\~english
		 *\brief		Adds a bone to the skeleton
		 *\param[in]	p_pBone		The bone
		 *\return
		 *\~french
		 *\brief		Ajoute un os au squelette
		 *\param[in]	p_pBone		L'os
		 */
		C3D_API void AddBone( BoneSPtr p_pBone );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first bone
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier os
		 *\return		La valeur
		 */
		inline BonePtrArrayIt Begin()
		{
			return m_arrayBones.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first bone
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier os
		 *\return		La valeur
		 */
		inline BonePtrArrayConstIt Begin()const
		{
			return m_arrayBones.begin();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the bones array
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'os
		 *\return		La valeur
		 */
		inline BonePtrArrayIt End()
		{
			return m_arrayBones.end();
		}
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the bones array
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'os
		 *\return		La valeur
		 */
		inline BonePtrArrayConstIt End()const
		{
			return m_arrayBones.end();
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
			return m_mtxGlobalInverse;
		}
		/**
		 *\~english
		 *\brief		Sets the global inverse transform
		 *\param[in]	p_mtxTransform	The new value
		 *\~french
		 *\brief		Définit la transformation globale inversée
		 *\param[in]	p_mtxTransform	La nouvelle valeur
		 */
		inline void SetGlobalInverseTransform( Castor::Matrix4x4r const & p_mtxTransform )
		{
			m_mtxGlobalInverse = p_mtxTransform;
		}
		/**
		 *\~english
		 *\brief		Retrieves the mesh name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom du maillage
		 *\return		La valeur
		 */
		inline Castor::String const & GetMeshName()const
		{
			return m_wpMesh.lock()->GetName();
		}

	private:
		//!\~english The mesh	\~french Le maillage
		MeshWPtr m_wpMesh;
		//!\~english The bones	\~french Les bones
		BonePtrArray m_arrayBones;
		//!\~english The global skeleton transform	\~french La transformation globale du squelette
		Castor::Matrix4x4r m_mtxGlobalInverse;
	};
}

#endif
