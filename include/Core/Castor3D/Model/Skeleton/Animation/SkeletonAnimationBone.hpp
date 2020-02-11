/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationBone_H___
#define ___C3D_SkeletonAnimationBone_H___

#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"

namespace castor3d
{
	class SkeletonAnimationBone
		: public SkeletonAnimationObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	animation	The parent animation.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	animation	L'animation parente.
		 */
		C3D_API explicit SkeletonAnimationBone( SkeletonAnimation & animation );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SkeletonAnimationBone();
		/**
		 *\~english
		 *\brief		Move constructor.
		 *\~french
		 *\brief		Constructeur par déplacement.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimationBone && rhs ) = default;
		/**
		 *\~english
		 *\brief		Move assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement.
		 */
		C3D_API SkeletonAnimationBone & operator=( SkeletonAnimationBone && rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy constructor.
		 *\~french
		 *\brief		Constructeur par copie.
		 */
		C3D_API SkeletonAnimationBone( SkeletonAnimationBone const & rhs ) = delete;
		/**
		 *\~english
		 *\brief		Copy assignment operator.
		 *\~french
		 *\brief		Opérateur d'affectation par copie.
		 */
		C3D_API SkeletonAnimationBone & operator=( SkeletonAnimationBone const & rhs ) = delete;
		/**
		 *\~english
		 *\return		The object name.
		 *\~french
		 *\return		Le nom de l'objet.
		 */
		virtual castor::String const & getName()const;
		/**
		 *\~english
		 *\brief		Sets the moving object.
		 *\param[in]	bone		The bone.
		 *\~french
		 *\brief		Définit l'objet mouvant.
		 *\param[in]	bone		L'os.
		 */
		inline void setBone( BoneSPtr bone )
		{
			m_bone = bone;
		}
		/**
		 *\~english
		 *\return		The moving object.
		 *\~french
		 *\return		L'objet mouvant.
		 */
		inline BoneSPtr getBone()const
		{
			return m_bone.lock();
		}

	private:
		//!\~english	The bone affected by the animations.
		//!\~french		L'os affecté par les animations.
		BoneWPtr m_bone;

		friend class BinaryWriter< SkeletonAnimationBone >;
		friend class BinaryParser< SkeletonAnimationBone >;
	};
}

#endif
