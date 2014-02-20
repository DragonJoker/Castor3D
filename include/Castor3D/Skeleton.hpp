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
#ifndef ___C3D_Skeleton___
#define ___C3D_Skeleton___

#include "Prerequisites.hpp"
#include "Mesh.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		26/06/2013
	\version	0.7.0
	\~english
	\brief		Class holding bone data
	\remark		Holds weight for each vertice and matrix from mesh space to bone space
	\~french
	\brief		Classe contenant les données d'un bone
	\remark		Contient les poids pour chaque vertice et la matrice de transformation de l'espace objet vers l'espace bone
	*/
	class C3D_API Bone
	{
	private:
		//!\~english	The bone name	\~french	Le nom du bone
		Castor::String m_strName;
		//!\~english	The matrix from mesh to bone space	\~french	La matrice de transformation de l'espace mesh vers l'espace bone
		Castor::Matrix4x4r m_mtxOffset;
		//!\~english	The parent skeleton	\~french	Le squelette parent
		Skeleton & m_skeleton;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_skeleton	The parent skeleton
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_skeleton	Le squelette parent
		 */
		Bone( Skeleton & p_skeleton );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Bone();
		/**
		 *\~english
		 *\brief		Retrieves the bone name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom du bone
		 *\return		La valeur
		 */
		inline Castor::String const & GetName()const { return m_strName; }
		/**
		 *\~english
		 *\brief		Sets the bone name
		 *\param[in]	p_strName	The new value
		 *\~french
		 *\brief		Définit le nom du bone
		 *\param[in]	p_strName	La nouvelle valeur
		 */
		inline void SetName( Castor::String const & p_strName ) { m_strName = p_strName; }
		/**
		 *\~english
		 *\brief		Retrieves the transfromation matrix from mesh space to bone space
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation de l'espace objet vers l'espace du bone
		 *\return		La valeur
		 */
		inline const Castor::Matrix4x4r & GetOffsetMatrix()const { return m_mtxOffset; }
		/**
		 *\~english
		 *\brief		Sets the transfromation matrix from mesh space to bone space
		 *\param[in]	p_mtxOffset	The new value
		 *\~french
		 *\brief		Définit la matrice de transformation de l'espace objet vers l'espace du bone
		 *\param[in]	p_mtxOffset	La nouvelle valeur
		 */
		inline void SetOffsetMatrix( const Castor::Matrix4x4r & p_mtxOffset ) { m_mtxOffset = p_mtxOffset; }
		/**
		 *\~english
		 *\brief		Retrieves the parent skeleton
		 *\return		The value
		 *\~french
		 *\brief		Récupère le squelette parent
		 *\return		La valeur
		 */
		inline const Skeleton & GetSkeleton()const { return m_skeleton; }
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\~english
	\brief		The skeleton, holds each bone
	\~french
	\brief		Le squelette, contient chaque bone
	*/
	class C3D_API Skeleton : public Animable
	{
	private:
		//!\~english	The mesh	\~french	Le maillage
		MeshWPtr m_wpMesh;
		//!\~english	The bones	\~french	Les bones
		BonePtrArray m_arrayBones;
		//!\~english	The global skeleton transform	\~french	La transformation globale du squelette
		Castor::Matrix4x4r m_mtxGlobalInverse;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pMesh	The parent mesh
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pMesh	Le maillage parent
		 */
		Skeleton( MeshSPtr p_pMesh );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Skeleton();
		/**
		 *\~english
		 *\brief		Adds a bone to the skeleton
		 *\param[in]	p_pBone		The bone
		 *\return		
		 *\~french
		 *\brief		Ajoute un os au squelette
		 *\param[in]	p_pBone		L'os
		 */
		void AddBone( BoneSPtr p_pBone );
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first bone
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier os
		 *\return		La valeur
		 */
		inline BonePtrArrayIt Begin() { return m_arrayBones.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the first bone
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur le premier os
		 *\return		La valeur
		 */
		inline BonePtrArrayConstIt Begin()const { return m_arrayBones.begin(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the bones array
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'os
		 *\return		La valeur
		 */
		inline BonePtrArrayIt End() { return m_arrayBones.end(); }
		/**
		 *\~english
		 *\brief		Retrieves an iterator to the end of the bones array
		 *\return		The value
		 *\~french
		 *\brief		Récupère un itérateur sur la fin du tableau d'os
		 *\return		La valeur
		 */
		inline BonePtrArrayConstIt End()const { return m_arrayBones.end(); }
		/**
		 *\~english
		 *\brief		Retrieves the global inverse transform
		 *\return		The value
		 *\~french
		 *\brief		Récupère la transformation globale inversée
		 *\return		La valeur
		 */
		inline Castor::Matrix4x4r const & GetGlobalInverseTransform()const { return m_mtxGlobalInverse; }
		/**
		 *\~english
		 *\brief		Sets the global inverse transform
		 *\param[in]	p_mtxTransform	The new value
		 *\~french
		 *\brief		Définit la transformation globale inversée
		 *\param[in]	p_mtxTransform	La nouvelle valeur
		 */
		inline void SetGlobalInverseTransform( Castor::Matrix4x4r const & p_mtxTransform ) { m_mtxGlobalInverse = p_mtxTransform; }
		/**
		 *\~english
		 *\brief		Retrieves the mesh name
		 *\return		The value
		 *\~french
		 *\brief		Récupère le nom du maillage
		 *\return		La valeur
		 */
		inline Castor::String const & GetMeshName()const { return m_wpMesh.lock()->GetName(); }
	};
}

#pragma warning( pop )

#endif
