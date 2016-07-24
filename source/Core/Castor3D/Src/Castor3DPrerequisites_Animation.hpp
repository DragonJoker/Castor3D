/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_PREREQUISITES_ANIMTION_H___
#define ___C3D_PREREQUISITES_ANIMTION_H___

#include "Mesh/VertexGroup.hpp"

namespace Castor3D
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
		Movable,
		//!\~english	Skeleton animation.
		//!\~french		Animation de squelette.
		Skeleton,
		//!\~english	Submesh animation (morphing).
		//!\~french		Animation de sous-maillage (morphing).
		Mesh,
		CASTOR_ENUM_CLASS_BOUNDS( Movable )
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
		Nearest,
		//!\~english	Linear mode.
		//!\~french		Mode linéaire.
		Linear,
		CASTOR_ENUM_CLASS_BOUNDS( Nearest )
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
		Playing,
		//!\~english	Stopped animation state.
		//!\~french		Animation stoppée.
		Stopped,
		//!\~english	Paused animation state.
		//!\~french		Animation en pause.
		Paused,
		CASTOR_ENUM_CLASS_BOUNDS( Playing )
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
		Node,
		//!\~english	Bone objects.
		//!\~french		Objets os.
		Bone,
		CASTOR_ENUM_CLASS_BOUNDS( Node )
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

	struct GroupAnimation
	{
		AnimationState m_state;
		bool m_looped;
		float m_scale;
	};

	using GroupAnimationMap = std::map< Castor::String, GroupAnimation >;
	using Point3rInterpolator = Interpolator< Castor::Point3r >;
	using QuaternionInterpolator = Interpolator< Castor::Quaternion >;
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
	DECLARE_MAP( Castor::String, AnimationSPtr, AnimationPtrStr );
	//! SkeletonAnimationObject pointer map, sorted by name.
	DECLARE_MAP( Castor::String, SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtrStr );
	//! SkeletonAnimationObject pointer array.
	DECLARE_VECTOR( SkeletonAnimationObjectSPtr, SkeletonAnimationObjectPtr );
	//! SkeletonAnimationObject array.
	DECLARE_VECTOR( MeshAnimationSubmesh, MeshAnimationSubmesh );

	//@}
}

#endif
