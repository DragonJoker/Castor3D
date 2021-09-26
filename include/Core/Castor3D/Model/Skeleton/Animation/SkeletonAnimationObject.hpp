/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationObject_H___
#define ___C3D_SkeletonAnimationObject_H___

#include "SkeletonAnimationModule.hpp"
#include "Castor3D/Animation/AnimationModule.hpp"
#include "Castor3D/Binary/BinaryModule.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Math/Quaternion.hpp>

namespace castor3d
{
	class SkeletonAnimationObject
		: public castor::OwnedBy< SkeletonAnimation >
		, public std::enable_shared_from_this< SkeletonAnimationObject >
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\param[in]	type		The skeleton animation object type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 *\param[in]	type		Le type d'objet d'animation de squelette.
		 */
		C3D_API SkeletonAnimationObject( SkeletonAnimation & animation
			, SkeletonAnimationObjectType type );
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationObject( SkeletonAnimationObject const & rhs ) = default;

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~SkeletonAnimationObject() = default;
		/**
		 *\~english
		 *\brief		adds a child to this object.
		 *\remarks		The child's transformations are affected by this object's ones.
		 *\param[in]	object	The child.
		 *\~french
		 *\brief		Ajoute un objet enfant à celui-ci.
		 *\remarks		Les transformations de l'enfant sont affectées par celles de cet objet.
		 *\param[in]	object	L'enfant.
		 */
		C3D_API void addChild( SkeletonAnimationObjectSPtr object );
		/**
		 *\~english
		 *\brief		Retrieves the object name.
		 *\return		The name.
		 *\~french
		 *\brief		Récupère le nom de l'objet.
		 *\return		Le nom.
		 */
		C3D_API virtual castor::String const & getName()const = 0;
		/**
		 *\~english
		 *\return		The scaling key frames interpolation mode.
		 *\~french
		 *\return		Le mode d'interpolation des key frames de mise à l'échelle.
		 */
		InterpolatorType getInterpolationMode()const
		{
			return m_mode;
		}
		/**
		 *\~english
		 *\return		The moving object type.
		 *\~french
		 *\return		Le type d'objet mouvant.
		 */
		SkeletonAnimationObjectType getType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Retrieves the animation node transformation.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère les animations du noeud de transformation.
		 *\return		La valeur.
		 */
		castor::Matrix4x4f const & getNodeTransform()const
		{
			return m_nodeTransform;
		}
		/**
		 *\~english
		 *\brief		Sets the animation node transformation.
		 *\param[in]	transform	The new value.
		 *\~french
		 *\brief		Définit les animations du noeud de transformation.
		 *\param[in]	transform	La nouvelle valeur.
		 */
		void setNodeTransform( castor::Matrix4x4f const & transform )
		{
			m_nodeTransform = transform;
		}
		/**
		 *\~english
		 *\return		The children array.
		 *\~french
		 *\return		Le tableau d'enfants.
		 */
		SkeletonAnimationObjectPtrArray const & getChildren()const
		{
			return m_children;
		}
		/**
		 *\~english
		 *\return		The parent object.
		 *\~french
		 *\return		L'objet parent.
		 */
		SkeletonAnimationObjectSPtr getParent()const
		{
			return m_parent.lock();
		}

	protected:
		//!\~english	The interpolation mode.
		//!\~french		Le mode d'interpolation.
		InterpolatorType m_mode{ InterpolatorType::eCount };
		//!\~english	The moving thing type.
		//!\~french		Le type du machin mouvant.
		SkeletonAnimationObjectType m_type;
		//!\~english	Animation node transformations.
		//!\~french		Transformations du noeud d'animation.
		castor::Matrix4x4f m_nodeTransform;
		//!\~english	The objects depending on this one.
		//!\~french		Les objets dépendant de celui-ci.
		SkeletonAnimationObjectPtrArray m_children;
		//!\~english	The parent object.
		//!\~french		L'objet parent.
		SkeletonAnimationObjectWPtr m_parent;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;

		friend class BinaryWriter< SkeletonAnimationObject >;
		friend class BinaryParser< SkeletonAnimationObject >;
		friend class SkeletonAnimationInstanceObject;
	};
}

#endif
