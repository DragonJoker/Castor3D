/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

namespace Castor3D
{
	/**@name Animation */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Interpolation modes enumeration.
	\~french
	\brief		Enumération des modes d'înterpolation.
	*/
	typedef enum eINTERPOLATOR_MODE
	{
		eINTERPOLATOR_MODE_NONE,	//!<\~english No interpolation.	\~french Pas d'interpolation.
		eINTERPOLATOR_MODE_LINEAR,	//!<\~english Linear mode.	\~french Mode linéaire.
		eINTERPOLATOR_MODE_COUNT
	}	eINTERPOLATOR_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		09/02/2010
	\~english
	\brief		Animation State Enumerator.
	\~french
	\brief		Enumération des états d'une animation.
	*/
	typedef enum eANIMATION_STATE CASTOR_TYPE( uint8_t )
	{
		eANIMATION_STATE_PLAYING,	//!< Playing animation state.
		eANIMATION_STATE_STOPPED,	//!< Stopped animation state.
		eANIMATION_STATE_PAUSED,	//!< Paused animation state.
		eANIMATION_STATE_COUNT
	}	eANIMATION_STATE;
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
	typedef enum eANIMATION_OBJECT_TYPE CASTOR_TYPE( uint8_t )
	{
		eANIMATION_OBJECT_TYPE_NODE,
		eANIMATION_OBJECT_TYPE_OBJECT,
		eANIMATION_OBJECT_TYPE_BONE,
	}	eANIMATION_OBJECT_TYPE;

	class Animable;
	class AnimationObject;
	class SkeletonAnimationObject;
	class SkeletonAnimationBone;
	class SkeletonAnimationNode;
	class Animation;
	class AnimatedObject;
	class AnimatedObjectGroup;
	class Bone;
	class Skeleton;
	class BonedVertex;
	struct stVERTEX_BONE_DATA;
	class KeyFrame;
	template< typename T > class Interpolator;
	template< typename T, eINTERPOLATOR_MODE > class InterpolatorT;

	using AnimationStateMap = std::map< Castor::String, eANIMATION_STATE >;
	using Point3rInterpolator = Interpolator< Castor::Point3r >;
	using QuaternionInterpolator = Interpolator< Castor::Quaternion >;
	using KeyFrameRealMap = std::map< real, KeyFrame >;
	using KeyFrameArray = std::vector< KeyFrame >;

	DECLARE_SMART_PTR( AnimatedObject );
	DECLARE_SMART_PTR( AnimationObject );
	DECLARE_SMART_PTR( SkeletonAnimationObject );
	DECLARE_SMART_PTR( SkeletonAnimationBone );
	DECLARE_SMART_PTR( SkeletonAnimationNode );
	DECLARE_SMART_PTR( Animation );
	DECLARE_SMART_PTR( AnimatedObjectGroup );
	DECLARE_SMART_PTR( Bone );
	DECLARE_SMART_PTR( Skeleton );
	DECLARE_SMART_PTR( Animable );
	DECLARE_SMART_PTR( BonedVertex );

	//! AnimatedObjectGroup pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! Animation pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimationSPtr, AnimationPtrStr );
	//! AnimatedObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimationObjectSPtr, AnimationObjectPtrStr );
	//! Animation pointer array
	DECLARE_VECTOR( AnimationSPtr, AnimationPtr );
	//! Bone pointer array
	DECLARE_VECTOR( BoneSPtr, BonePtr );
	//! AnimationObject pointer array
	DECLARE_VECTOR( AnimationObjectSPtr, AnimationObjectPtr );
	//! BonedVertex pointer array
	DECLARE_VECTOR( BonedVertexSPtr, BonedVertexPtr );

	//@}
}

#endif
