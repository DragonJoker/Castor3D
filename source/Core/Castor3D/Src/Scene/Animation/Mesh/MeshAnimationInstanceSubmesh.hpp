/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MESH_ANIMATION_INSTANCE_SUBMESH_H___
#define ___C3D_MESH_ANIMATION_INSTANCE_SUBMESH_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"

#include "Animation/Mesh/MeshAnimationSubmesh.hpp"

namespace castor3d
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
		: public castor::OwnedBy< MeshAnimationInstance >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_animationInstance	The parent skeleton animation instance.
		 *\param[in]	p_animationObject	The animation object.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_animationInstance	L'instance d'animation de squelette parent.
		 *\param[in]	p_animationObject	L'animation d'objet.
		 */
		C3D_API MeshAnimationInstanceSubmesh( MeshAnimationInstance & p_animationInstance, MeshAnimationSubmesh & p_animationObject );
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
		 *\param[in]	p_time	Current time index.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet, l'index de temps donné.
		 *\param[in]	p_time	Index de temps courant.
		 */
		C3D_API void update( castor::Milliseconds const & p_time );
		/**
		 *\~english
		 *\brief		The current factor.
		 *\~french
		 *\brief		Le facteur courant.
		 */
		inline float getCurrentFactor()const
		{
			return m_currentFactor;
		}

	protected:
		//!\~english	The animation object.
		//!\~french		L'animation d'objet.
		MeshAnimationSubmesh & m_animationObject;
		//!\~english	Iterator to the first buffer.
		//!\~french		Itérateur sur le premier tampon.
		SubmeshAnimationBufferArray::const_iterator m_first;
		//!\~english	Iterator to the last buffer.
		//!\~french		Itérateur sur le dernier tampon.
		SubmeshAnimationBufferArray::const_iterator m_last;
		//!\~english	Iterator to the previous buffer (when playing the animation).
		//!\~french		Itérateur sur le tampon précédent (quand l'animation est jouée).
		SubmeshAnimationBufferArray::const_iterator m_prev;
		//!\~english	Iterator to the current buffer (when playing the animation).
		//!\~french		Itérateur sur le tampon courant (quand l'animation est jouée).
		SubmeshAnimationBufferArray::const_iterator m_curr;
		//!\~english	The time elapsed since the last keyframe.
		//!\~french		Le temps écoulé depuis la dernière keyframe.
		float m_currentFactor;
	};
}

#endif
