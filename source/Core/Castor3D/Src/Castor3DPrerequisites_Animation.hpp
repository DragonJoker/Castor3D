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
	\brief		Interpolator modes enumeration.
	\~french
	\brief		Enumération des modes d'înterpolateur.
	*/
	typedef enum eINTERPOLATOR_MODE
	{
		eINTERPOLATOR_MODE_NONE,	//!<\~english No interpolation.	\~french Pas d'interpolation.
		eINTERPOLATOR_MODE_LINEAR,	//!<\~english Linear mode.	\~french Mode linéaire.
		eINTERPOLATOR_MODE_COUNT
	}	eINTERPOLATOR_MODE;
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
	typedef enum eMOVING_OBJECT_TYPE CASTOR_TYPE( uint8_t )
	{
		eMOVING_OBJECT_TYPE_NODE,
		eMOVING_OBJECT_TYPE_OBJECT,
		eMOVING_OBJECT_TYPE_BONE,
	}	eMOVING_OBJECT_TYPE;

	class Animable;
	class MovingObjectBase;
	class MovingObject;
	class MovingBone;
	class MovingNode;
	class Animation;
	class AnimatedObject;
	class AnimatedObjectGroup;
	class Bone;
	class Skeleton;
	struct stVERTEX_BONE_DATA;
	template< typename T > class KeyFrame;

	typedef KeyFrame< Castor::Point3r > Point3rKeyFrame;
	typedef KeyFrame< Castor::Quaternion > QuaternionKeyFrame;

	typedef std::pair< uint32_t, real > VertexWeight;//!<\~english Vertex weight, a simple pair of ID and weight	\~french Poids de vertice, simple paire d'un ID et du poids
	using Castor::UIntStrMap;
	using Castor::UInt64StrMap;

	DECLARE_SMART_PTR( AnimatedObject );
	DECLARE_SMART_PTR( MovingObjectBase );
	DECLARE_SMART_PTR( MovingObject );
	DECLARE_SMART_PTR( MovingBone );
	DECLARE_SMART_PTR( MovingNode );
	DECLARE_SMART_PTR( Animation );
	DECLARE_SMART_PTR( AnimatedObjectGroup );
	DECLARE_SMART_PTR( Bone );
	DECLARE_SMART_PTR( Skeleton );
	DECLARE_SMART_PTR( Animable );

	//! AnimatedObjectGroup pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectGroupSPtr, AnimatedObjectGroupPtrStr );
	//! Animation pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimationSPtr, AnimationPtrStr );
	//! KeyFrame pointer map, sorted by time index
	DECLARE_MAP( real, Point3rKeyFrame, Point3rKeyFrameReal );
	//! KeyFrame pointer map, sorted by time index
	DECLARE_MAP( real, QuaternionKeyFrame, QuaternionKeyFrameReal );
	//! AnimatedObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, MovingObjectBaseSPtr, MovingObjectPtrStr );
	//! VertexWeight pointer array
	DECLARE_VECTOR( VertexWeight, VertexWeight );
	//! Animation pointer array
	DECLARE_VECTOR( AnimationSPtr, AnimationPtr );
	//! Bone pointer array
	DECLARE_VECTOR( BoneSPtr, BonePtr );
	//! MovingObject pointer array
	DECLARE_VECTOR( MovingObjectBaseSPtr, MovingObjectPtr );

	//@}
}

#endif
