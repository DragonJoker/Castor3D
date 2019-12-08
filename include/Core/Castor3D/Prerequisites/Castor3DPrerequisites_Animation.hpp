/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_ANIMTION_H___
#define ___C3D_PREREQUISITES_ANIMTION_H___

#include "Castor3D/Mesh/VertexGroup.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>

namespace castor3d
{
	/**@name Animation */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		31/05/2016
	\~english
	\brief		Animation types enumeration.
	\~french
	\brief		Enumération des types d'animation.
	*/
	enum class AnimationType
		: uint8_t
	{
		//!\~english	Scene node animation.
		//!\~french		Animation de noeud de scène.
		eMovable,
		//!\~english	Skeleton animation.
		//!\~french		Animation de squelette.
		eSkeleton,
		//!\~english	Submesh animation (morphing).
		//!\~french		Animation de sous-maillage (morphing).
		eMesh,
		CU_ScopedEnumBounds( eMovable )
	};
	castor::String getName( AnimationType value );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Interpolation modes enumeration.
	\~french
	\brief		Enumération des modes d'înterpolation.
	*/
	enum class InterpolatorType
		: uint8_t
	{
		//!\~english	No interpolation.
		//!\~french		Pas d'interpolation.
		eNearest,
		//!\~english	Linear mode.
		//!\~french		Mode linéaire.
		eLinear,
		CU_ScopedEnumBounds( eNearest )
	};
	castor::String getName( InterpolatorType value );
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		Animation State Enumerator.
	\~french
	\brief		Enumération des états d'une animation.
	*/
	enum class AnimationState
		: uint8_t
	{
		//!\~english	Playing animation state.
		//!\~french		Animation en cours de lecture.
		ePlaying,
		//!\~english	Stopped animation state.
		//!\~french		Animation stoppée.
		eStopped,
		//!\~english	Paused animation state.
		//!\~french		Animation en pause.
		ePaused,
		CU_ScopedEnumBounds( ePlaying )
	};
	castor::String getName( AnimationState value );
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
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		14/06/2016
	\~english
	\brief		Class which represents the mesh animation submeshes buffers.
	\~french
	\brief		Classe de représentation des tampons de sous-maillages d'animations de maillage.
	*/
	template< typename T >
	struct SubmeshAnimationBufferT
	{
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		std::vector< InterleavedVertexT< T > > m_buffer;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;
	};

	class Submesh;

	class Animable;
	class Animation;
	class AnimationKeyFrame;
	class SkeletonAnimation;
	class SkeletonAnimationKeyFrame;
	class SkeletonAnimationObject;
	class SkeletonAnimationBone;
	class SkeletonAnimationNode;
	class MeshAnimation;
	class MeshAnimationKeyFrame;
	class MeshAnimationSubmesh;
	template< typename T >
	class Interpolator;
	template< typename T, InterpolatorType >
	class InterpolatorT;

	struct GroupAnimation
	{
		AnimationState m_state;
		bool m_looped;
		float m_scale;
	};

	CU_DeclareSmartPtr( Animation );
	CU_DeclareSmartPtr( AnimationKeyFrame );
	CU_DeclareSmartPtr( SkeletonAnimation );
	CU_DeclareSmartPtr( SkeletonAnimationKeyFrame );
	CU_DeclareSmartPtr( SkeletonAnimationObject );
	CU_DeclareSmartPtr( SkeletonAnimationBone );
	CU_DeclareSmartPtr( SkeletonAnimationNode );
	CU_DeclareSmartPtr( MeshAnimation );
	CU_DeclareSmartPtr( MeshAnimationKeyFrame );
	CU_DeclareSmartPtr( MeshAnimationSubmesh );
	CU_DeclareSmartPtr( Animable );

	using SubmeshAnimationBuffer = SubmeshAnimationBufferT< float >;
	using SubmeshAnimationBufferMap = std::map< uint32_t, SubmeshAnimationBuffer >;
	using GroupAnimationMap = std::map< castor::String, GroupAnimation >;
	using Point3rInterpolator = Interpolator< castor::Point3f >;
	using QuaternionInterpolator = Interpolator< castor::Quaternion >;
	using AnimationKeyFrameArray = std::vector< AnimationKeyFrameUPtr >;

	//! Animation pointer map, sorted by name.
	CU_DeclareMap( castor::String, AnimationSPtr, AnimationPtrStr );
	//! SkeletonAnimationObject pointer map, sorted by name.
	CU_DeclareMap( castor::String, SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtrStr );
	//! SkeletonAnimationObject pointer array.
	CU_DeclareVector( SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtr );
	//! SkeletonAnimationObject array.
	CU_DeclareVector( MeshAnimationSubmesh, MeshAnimationSubmesh );

	//@}
}

#endif
