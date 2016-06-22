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
