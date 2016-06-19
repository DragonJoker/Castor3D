/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_SUBMESH_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_SUBMESH_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include "Animation/Mesh/MeshAnimationSubmesh.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents the moving things.
	\remark		Manages translation, scaling, rotation of the thing.
	\~french
	\brief		Classe de représentation de choses mouvantes.
	\remark		Gère les translations, mises à l'échelle, rotations de la chose.
	*/
	class MeshAnimationInstanceSubmesh
		: public Castor::OwnedBy< MeshAnimationInstance >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animationInstance	The parent skeleton animation instance.
		 *\param[in]	p_animationObject	The animation object.
		 *\param[out]	p_allObjects		Receives this object's children.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	p_animationObject	L'animation d'objet.
		 *\param[out]	p_allObjects		Reçoit les enfants de cet objet.
		 */
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstance & p_animationInstance, MeshAnimationSubmesh const & p_animationObject );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstanceSubmesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API MeshAnimationInstanceSubmesh & operator=( MeshAnimationInstanceSubmesh const & p_rhs ) = delete;
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstanceSubmesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API MeshAnimationInstanceSubmesh & operator=( MeshAnimationInstanceSubmesh && p_rhs ) = default;
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~MeshAnimationInstanceSubmesh();
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations at given time.
		 *\param[in]	p_time				Current time index.
		 *\param[in]	p_transformations	The current transformation matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné.
		 *\param[in]	p_time				Index de temps courant.
		 *\param[in]	p_transformations	La matrice de transformation courante.
		 */
		C3D_API void Update( real p_time );

	protected:
		//!\~english	The animation object.
		//!\~french		L'animation d'objet.
		MeshAnimationSubmesh const & m_animationObject;
		//!\~english	Iterator to the buffers beginning.
		//!\~french		Itérateur sur le début des tampons.
		SubmeshAnimationBufferArray::const_iterator m_begin;
		//!\~english	Iterator to the buffer end.
		//!\~french		Itérateur sur la fin des tampons.
		SubmeshAnimationBufferArray::const_iterator m_end;
		//!\~english	Iterator to the previous buffer (when playing the animation).
		//!\~french		Itérateur sur le tampon précédent (quand l'animation est jouée).
		SubmeshAnimationBufferArray::const_iterator m_prev;
		//!\~english	Iterator to the current buffer (when playing the animation).
		//!\~french		Itérateur sur le tampon courant (quand l'animation est jouée).
		SubmeshAnimationBufferArray::const_iterator m_curr;
	};
}

#endif
