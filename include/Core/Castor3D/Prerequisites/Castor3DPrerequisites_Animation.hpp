/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_ANIMTION_H___
#define ___C3D_PREREQUISITES_ANIMTION_H___

#include "Castor3D/Mesh/VertexGroup.hpp"

#include <CastorUtils/Graphics/BoundingBox.hpp>

namespace castor3d
{


	struct GroupAnimation
	{
		AnimationState m_state;
		bool m_looped;
		float m_scale;
	};

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
