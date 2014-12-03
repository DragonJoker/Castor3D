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
#ifndef ___C3D_BONE_H___
#define ___C3D_BONE_H___

#include "Castor3DPrerequisites.hpp"

#include <SquareMatrix.hpp>

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
		//!\~english The bone name	\~french Le nom du bone
		Castor::String m_strName;
		//!\~english The matrix from mesh to bone space	\~french La matrice de transformation de l'espace mesh vers l'espace bone
		Castor::Matrix4x4r m_mtxOffset;
		//!\~english The parent skeleton	\~french Le squelette parent
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
		inline Castor::String const & GetName()const
		{
			return m_strName;
		}
		/**
		 *\~english
		 *\brief		Sets the bone name
		 *\param[in]	p_strName	The new value
		 *\~french
		 *\brief		Définit le nom du bone
		 *\param[in]	p_strName	La nouvelle valeur
		 */
		inline void SetName( Castor::String const & p_strName )
		{
			m_strName = p_strName;
		}
		/**
		 *\~english
		 *\brief		Retrieves the transfromation matrix from mesh space to bone space
		 *\return		The value
		 *\~french
		 *\brief		Récupère la matrice de transformation de l'espace objet vers l'espace du bone
		 *\return		La valeur
		 */
		inline const Castor::Matrix4x4r & GetOffsetMatrix()const
		{
			return m_mtxOffset;
		}
		/**
		 *\~english
		 *\brief		Sets the transfromation matrix from mesh space to bone space
		 *\param[in]	p_mtxOffset	The new value
		 *\~french
		 *\brief		Définit la matrice de transformation de l'espace objet vers l'espace du bone
		 *\param[in]	p_mtxOffset	La nouvelle valeur
		 */
		inline void SetOffsetMatrix( const Castor::Matrix4x4r & p_mtxOffset )
		{
			m_mtxOffset = p_mtxOffset;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent skeleton
		 *\return		The value
		 *\~french
		 *\brief		Récupère le squelette parent
		 *\return		La valeur
		 */
		inline const Skeleton & GetSkeleton()const
		{
			return m_skeleton;
		}
	};
}

#pragma warning( pop )

#endif
