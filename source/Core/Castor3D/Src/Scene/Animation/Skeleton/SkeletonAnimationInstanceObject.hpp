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
#ifndef ___C3D_SKELETON_ANIMATION_INSTANCE_OBJECT_H___
#define ___C3D_SKELETON_ANIMATION_INSTANCE_OBJECT_H___

#include "Binary/BinaryParser.hpp"
#include "Binary/BinaryWriter.hpp"
#include "Animation/Interpolator.hpp"

#include <SquareMatrix.hpp>
#include <Quaternion.hpp>

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
	class SkeletonAnimationInstanceObject
		: public Castor::OwnedBy< SkeletonAnimationInstance >
	{
	protected:
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
		C3D_API SkeletonAnimationInstanceObject( SkeletonAnimationInstance & p_animationInstance, SkeletonAnimationObject & p_animationObject, SkeletonAnimationInstanceObjectPtrStrMap & p_allObjects );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationInstanceObject( SkeletonAnimationInstanceObject const & p_rhs ) = delete;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SkeletonAnimationInstanceObject();
		/**
		 *\~english
		 *\brief		Adds a child to this object.
		 *\remarks		The child's transformations are affected by this object's ones.
		 *\param[in]	p_object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remarks		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	p_object	L'enfant.
		 */
		C3D_API void AddChild( SkeletonAnimationInstanceObjectSPtr p_object );
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
		C3D_API void Update( real p_time, Castor::Matrix4x4r const & p_transformations );
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		C3D_API void SetInterpolationMode( InterpolatorType p_mode );
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		inline InterpolatorType GetInterpolationMode()const
		{
			return m_mode;
		}
		/**
		 *\~english
		 *\brief		The final object's animations transformation.
		 *\~french
		 *\brief		La transfomation finale des animations du de cet objet.
		 */
		inline Castor::Matrix4x4r const & GetFinalTransform()const
		{
			return m_finalTransform;
		}
		/**
		 *\~english
		 *\return		The children array.
		 *\~french
		 *\return		Le tableau d'enfants.
		 */
		inline SkeletonAnimationInstanceObjectPtrArray const & GetChildren()const
		{
			return m_children;
		}

	protected:
		/**
		 *\~english
		 *\brief		Updates the object, applies the transformations matrix.
		 *\~french
		 *\brief		Met à jour les transformations appliquées à l'objet.
		 */
		C3D_API virtual void DoApply() = 0;

	protected:
		//!\~english	The animation object.
		//!\~french		L'animation d'objet.
		SkeletonAnimationObject & m_animationObject;
		//!\~english	The interpolation mode.
		//!\~french		Le mode d'interpolation.
		InterpolatorType m_mode{ InterpolatorType::Count };
		//!\~english	The point interpolator.
		//!\~french		L'interpolateur de points.
		std::unique_ptr< Point3rInterpolator > m_pointInterpolator;
		//!\~english	The quaternion interpolator.
		//!\~french		L'interpolateur de quaternions.
		std::unique_ptr< QuaternionInterpolator > m_quaternionInterpolator;
		//!\~english	Iterator to the previous keyframe (when playing the animation).
		//!\~french		Itérateur sur la key frame précédente (quand l'animation est jouée).
		KeyFrameArray::const_iterator m_prev;
		//!\~english	Iterator to the current keyframe (when playing the animation).
		//!\~french		Itérateur sur la key frame courante (quand l'animation est jouée).
		KeyFrameArray::const_iterator m_curr;
		//!\~english	The objects depending on this one.
		//!\~french		Les objets dépendant de celui-ci.
		SkeletonAnimationInstanceObjectPtrArray m_children;
		//!\~english	The cumulative animation transformations.
		//!\~french		Les transformations cumulées de l'animation.
		Castor::Matrix4x4r m_cumulativeTransform;
		//!\~english	The matrix holding transformation at current time.
		//!\~french		La matrice de transformation complète au temps courant de l'animation.
		Castor::Matrix4x4r m_finalTransform;
	};
}

#endif
