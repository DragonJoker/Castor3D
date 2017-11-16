/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_ANIMTION_H___
#define ___C3D_PREREQUISITES_ANIMTION_H___

#include "Mesh/VertexGroup.hpp"

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
		CASTOR_SCOPED_ENUM_BOUNDS( eMovable )
	};
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
		CASTOR_SCOPED_ENUM_BOUNDS( eNearest )
	};
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
		CASTOR_SCOPED_ENUM_BOUNDS( ePlaying )
	};
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
		CASTOR_SCOPED_ENUM_BOUNDS( eNode )
	};
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
		//!\~english	The start time index.
		//!\~french		L'index de temps de début.
		float m_timeIndex;
		//!\~english	The vertex buffer.
		//!\~french		Le tampon de sommets.
		std::vector< InterleavedVertexT< T > > m_buffer;
		//!\~english	The bounding box.
		//!\~french		La bounding box.
		castor::BoundingBox m_boundingBox;
	};

	class Animable;
	class Animation;
	class SkeletonAnimation;
	class SkeletonAnimationObject;
	class SkeletonAnimationBone;
	class SkeletonAnimationNode;
	class MeshAnimation;
	class MeshAnimationSubmesh;
	class KeyFrame;
	template< typename T > class Interpolator;
	template< typename T, InterpolatorType > class InterpolatorT;
	using SubmeshAnimationBuffer = SubmeshAnimationBufferT< float >;
	using SubmeshAnimationBufferArray = std::vector< SubmeshAnimationBuffer >;

	struct GroupAnimation
	{
		AnimationState m_state;
		bool m_looped;
		float m_scale;
	};

	using GroupAnimationMap = std::map< castor::String, GroupAnimation >;
	using Point3rInterpolator = Interpolator< castor::Point3r >;
	using QuaternionInterpolator = Interpolator< castor::Quaternion >;
	using KeyFrameRealMap = std::map< real, KeyFrame >;
	using KeyFrameArray = std::vector< KeyFrame >;

	DECLARE_SMART_PTR( Animation );
	DECLARE_SMART_PTR( SkeletonAnimation );
	DECLARE_SMART_PTR( SkeletonAnimationObject );
	DECLARE_SMART_PTR( SkeletonAnimationBone );
	DECLARE_SMART_PTR( SkeletonAnimationNode );
	DECLARE_SMART_PTR( MeshAnimation );
	DECLARE_SMART_PTR( MeshAnimationSubmesh );
	DECLARE_SMART_PTR( Animable );

	//! Animation pointer map, sorted by name.
	DECLARE_MAP( castor::String, AnimationSPtr, AnimationPtrStr );
	//! SkeletonAnimationObject pointer map, sorted by name.
	DECLARE_MAP( castor::String, SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtrStr );
	//! SkeletonAnimationObject pointer array.
	DECLARE_VECTOR( SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtr );
	//! SkeletonAnimationObject array.
	DECLARE_VECTOR( MeshAnimationSubmesh, MeshAnimationSubmesh );

	//@}
}

#endif
