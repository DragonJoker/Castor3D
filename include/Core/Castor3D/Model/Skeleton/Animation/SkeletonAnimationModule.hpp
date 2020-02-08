/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkeletonAnimationModule_H___
#define ___C3D_SkeletonAnimationModule_H___

#include <CastorUtils/Miscellaneous/StringUtils.hpp>

namespace castor3d
{
	/**@name Skeleton Animation */
	//@{

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
	enum class SkeletonAnimationObjectType
		: uint8_t
	{
		//!\~english	Node objects.
		//!\~french		Objets noeud.
		eNode,
		//!\~english	Bone objects.
		//!\~french		Objets os.
		eBone,
		CU_ScopedEnumBounds( eNode )
	};
	castor::String getName( SkeletonAnimationObjectType value );

	class SkeletonAnimation;
	class SkeletonAnimationKeyFrame;
	class SkeletonAnimationObject;
	class SkeletonAnimationBone;
	class SkeletonAnimationNode;

	//@}
}

#endif
