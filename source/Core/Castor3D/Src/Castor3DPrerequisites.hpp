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
#ifndef ___C3D_PREREQUISITES_H___
#define ___C3D_PREREQUISITES_H___

#if defined( _MSC_VER )
// disable: "<type > needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#	pragma warning( disable:4251 )
// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#	pragma warning( disable:4275 )
#	pragma warning( disable:4996 )
#endif

#undef RGB

#if defined( __linux__ )
#	define C3D_API
#	define CASTOR_X11
#elif defined( _WIN32 )
#	define CASTOR_MSW
#	if defined( _MSC_VER )
#		pragma warning( push )
// disable: "<symbol > no suitable definition provided for explicit instantiation request"
// Happens when deriving from Manager
#   	pragma warning( disable : 4661 )
// disable: "C++ exception specification ignored except to indicate a function is not __declspec(nothrow)"
// Happens only on Visual Studio
#		pragma warning (disable : 4290 )
#		pragma warning( disable : 4311 )
#		pragma warning( disable : 4312 )
#	endif
#	if defined( Castor3D_EXPORTS )
#		define C3D_API __declspec(dllexport)
#	else
#		define C3D_API __declspec(dllimport)
#	endif
#endif

#include <CastorUtilsPrerequisites.hpp >
#include <Collection.hpp >

namespace Castor3D
{
	using Castor::real;

	/**@name Animation */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Interpolator modes enumeration
	\~french
	\brief		Enumération des modes d'înterpolateur
	*/
	typedef enum eINTERPOLATOR_MODE
	{
		eINTERPOLATOR_MODE_LINEAR, //!<\~english Linear mode	\~french Mode linéaire
		eINTERPOLATOR_MODE_COUNT
	}	eINTERPOLATOR_MODE;

	class Animable;				//!< Base class for all objects with animations
	class MovingObjectBase;		//!< Class which represents the moving objects
	class ObjMovingObject;		//!< Specialisation of MovingObject for MovableObjects
	class BoneMovingObject;		//!< Specialisation of MovingObject for Bone
	class Animation;			//!< The class which manages key frames
	class AnimatedObject;		//!< Class which represents the animated objects
	class AnimatedObjectGroup;	//!< Class which represents the animated object groups
	class Bone;
	class Skeleton;
	struct stVERTEX_BONE_DATA;
	template< class Type, eINTERPOLATOR_MODE Mode > class Interpolator;
	template< typename T > class KeyFrame;

	typedef KeyFrame< Castor::Point3r > ScaleKeyFrame;
	typedef KeyFrame< Castor::Point3r > TranslateKeyFrame;
	typedef KeyFrame< Castor::Quaternion > RotateKeyFrame;

	typedef Interpolator< Castor::Point3r, eINTERPOLATOR_MODE_LINEAR > LinearPointInterpolator;
	typedef Interpolator< Castor::Quaternion, eINTERPOLATOR_MODE_LINEAR > LinearQuaternionInterpolator;

	typedef std::pair< uint32_t, real > VertexWeight;//!<\~english Vertex weight, a simple pair of ID and weight	\~french Poids de vertice, simple paire d'un ID et du poids
	typedef std::map< Castor::String, uint32_t > UIntStrMap;

	DECLARE_SMART_PTR( ScaleKeyFrame );
	DECLARE_SMART_PTR( TranslateKeyFrame );
	DECLARE_SMART_PTR( RotateKeyFrame );
	DECLARE_SMART_PTR( AnimatedObject );
	DECLARE_SMART_PTR( MovingObjectBase );
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
	DECLARE_MAP( real, ScaleKeyFrameSPtr, ScaleKeyFramePtrReal );
	//! KeyFrame pointer map, sorted by time index
	DECLARE_MAP( real, TranslateKeyFrameSPtr, TranslateKeyFramePtrReal );
	//! KeyFrame pointer map, sorted by time index
	DECLARE_MAP( real, RotateKeyFrameSPtr, RotateKeyFramePtrReal );
	//! AnimatedObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, AnimatedObjectSPtr, AnimatedObjectPtrStr );
	//! MovingObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, MovingObjectBaseSPtr, MovingObjectPtrStr );
	//! KeyFrame pointer array
	DECLARE_VECTOR( ScaleKeyFrameSPtr, ScaleKeyFramePtr );
	//! KeyFrame pointer array
	DECLARE_VECTOR( TranslateKeyFrameSPtr, TranslateKeyFramePtr );
	//! KeyFrame pointer array
	DECLARE_VECTOR( RotateKeyFrameSPtr, RotateKeyFramePtr );
	//! VertexWeight pointer array
	DECLARE_VECTOR( VertexWeight, VertexWeight );
	//! Bone pointer array
	DECLARE_VECTOR( BoneSPtr, BonePtr );
	//! MovingObject pointer array
	DECLARE_VECTOR( MovingObjectBaseSPtr, MovingObjectPtr );

	//@}
	/**@name Camera */
	//@{

	class Camera;
	class RenderTarget;
	class Viewport;
	class Ray;

	DECLARE_SMART_PTR( Camera );
	DECLARE_SMART_PTR( RenderTarget );
	DECLARE_SMART_PTR( Viewport );
	DECLARE_SMART_PTR( Ray );

	//! Camera pointer map, sorted by name
	DECLARE_MAP( Castor::String, CameraSPtr, CameraPtrStr );
	//! Camera pointer array
	DECLARE_VECTOR( CameraSPtr, CameraPtr );

	//@}
	/**@name Geometry */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Mesh type enumeration
	\~french
	\brief		Enumération des types de maillages
	*/
	typedef enum eMESH_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eMESH_TYPE_CUSTOM,		//!< Custom mesh type = > User defined vertex...
		eMESH_TYPE_CONE,		//!< Cone mesh type
		eMESH_TYPE_CYLINDER,	//!< Cylinder mesh type
		eMESH_TYPE_SPHERE,		//!< Rectangular faces sphere mesh type
		eMESH_TYPE_CUBE,		//!< Cube mesh type
		eMESH_TYPE_TORUS,		//!< Torus mesh type
		eMESH_TYPE_PLANE,		//!< Plane mesh type
		eMESH_TYPE_ICOSAHEDRON,	//!< Triangular faces sphere mesh type
		eMESH_TYPE_PROJECTION,	//!< Projection mesh type
		eMESH_TYPE_COUNT,
	}	eMESH_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Enumerates supported normal generation
	\~french
	\brief		Enumération des types de génération de normales supportés
	*/
	typedef enum eNORMAL
	CASTOR_TYPE( uint8_t )
	{
		eNORMAL_FLAT,	//!<\~english Flat (face) normals	\~french Normales plates (par face)
		eNORMAL_SMOOTH,	//!<\~english Smooth normals	\~french Normales smooth
		eNORMAL_COUNT,	//!<\~english Count	\~french Compte
	}	eNORMAL;

	class Vertex;
	class BufferElementGroup;

	DECLARE_SMART_PTR( Vertex );
	DECLARE_SMART_PTR( BufferElementGroup );

	template <typename T > class Pattern;
	typedef Pattern< Vertex > VertexPattern;
	typedef Pattern< Castor::Point3r > Point3rPattern;
	typedef Pattern< VertexSPtr > VertexPtrPattern;
	typedef Pattern< Castor::Point3rSPtr > Point3rPtrPattern;

	struct FaceGroup;
	class Face;
	class Graph;
	class Mesh;
	class MeshCategory;
	class Submesh;
	class Cone;
	class Cylinder;
	class Icosahedron;
	class Cube;
	class Plane;
	class Projection;
	class Sphere;
	class Torus;
	class Geometry;
	class MovableObject;
	class Subdivider;

	DECLARE_SMART_PTR( VertexPattern );
	DECLARE_SMART_PTR( Point3rPattern );
	DECLARE_SMART_PTR( VertexPtrPattern );
	DECLARE_SMART_PTR( Point3rPtrPattern );
	DECLARE_SMART_PTR( Face );
	DECLARE_SMART_PTR( Graph );
	DECLARE_SMART_PTR( Mesh );
	DECLARE_SMART_PTR( MeshCategory );
	DECLARE_SMART_PTR( Submesh );
	DECLARE_SMART_PTR( Cone );
	DECLARE_SMART_PTR( Cylinder );
	DECLARE_SMART_PTR( Icosahedron );
	DECLARE_SMART_PTR( Cube );
	DECLARE_SMART_PTR( Plane );
	DECLARE_SMART_PTR( Projection );
	DECLARE_SMART_PTR( Sphere );
	DECLARE_SMART_PTR( Torus );
	DECLARE_SMART_PTR( Geometry );
	DECLARE_SMART_PTR( MovableObject );
	DECLARE_SMART_PTR( Subdivider );
	DECLARE_SMART_PTR( FaceGroup );

	class MeshFactory;
	DECLARE_SMART_PTR( MeshFactory );

	//! Vertex array
	DECLARE_VECTOR( Vertex, Vertex );
	//! Vertex pointer array
	DECLARE_VECTOR( BufferElementGroupSPtr, VertexPtr );
	//! Face array
	DECLARE_VECTOR( Face, Face );
	//! Face pointer array
	DECLARE_VECTOR( FaceSPtr, FacePtr );
	//! FaceGroup pointer array
	DECLARE_VECTOR( FaceGroupSPtr, FaceGroupPtr );
	//! Submesh pointer array
	DECLARE_VECTOR( SubmeshSPtr, SubmeshPtr );
	//! Geometry pointer array
	DECLARE_VECTOR( GeometrySPtr, GeometryPtr );
	//! Vertex pointer list
	DECLARE_LIST( VertexSPtr, VertexPtr );
	//! Mesh pointer array
	DECLARE_MAP( Castor::String, MeshSPtr, MeshPtrStr );
	//! MovableObject pointer map, sorted by name
	DECLARE_MAP( Castor::String, MovableObjectSPtr, MovableObjectPtrStr );
	//! Geometry pointer map, sorted by name
	DECLARE_MAP( Castor::String, GeometrySPtr, GeometryPtrStr );
	DECLARE_MAP( uint32_t, int, IntUInt );
	DECLARE_MAP( Castor::String, SubmeshSPtr, SubmeshPtrStr );
	DECLARE_MAP( Castor::String, int, IntStr );

	//@}
	/**@name Importer */
	//@{

	class Importer;
	DECLARE_SMART_PTR( Importer );

	//@}
	/**@name Light */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Light types enumeration
	\~french
	\brief		Enumération des types de lumières
	*/
	typedef enum eLIGHT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eLIGHT_TYPE_DIRECTIONAL,	//!< Directional light type
		eLIGHT_TYPE_POINT,			//!< Point light type
		eLIGHT_TYPE_SPOT,			//!< Spot light type
		eLIGHT_TYPE_COUNT,
	}	eLIGHT_TYPE;

	class Light;
	class LightCategory;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	DECLARE_SMART_PTR( Light );
	DECLARE_SMART_PTR( LightCategory );
	DECLARE_SMART_PTR( DirectionalLight );
	DECLARE_SMART_PTR( PointLight );
	DECLARE_SMART_PTR( SpotLight );

	class LightFactory;
	DECLARE_SMART_PTR( LightFactory );

	//! Array of lights
	DECLARE_VECTOR( LightSPtr, LightPtr );
	//! Map of lights, sorted by name
	DECLARE_MAP( Castor::String, LightSPtr, LightPtrStr );
	//! Map of lights, sorted by index
	DECLARE_MAP( int, LightSPtr, LightPtrInt );

	//@}
	/**@name Material */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture mapping modes enumeration
	\~french
	\brief		Enumération des modes de mapping des textures
	*/
	typedef enum eTEXTURE_MAP_MODE
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_MAP_MODE_NONE,			//!<\~english Usual map mode	\~french Mode habituel (aucune transformation des UVW)
		eTEXTURE_MAP_MODE_REFLECTION,	//!<\~english Reflexion map mode	\~french Mode reflexion map
		eTEXTURE_MAP_MODE_SPHERE,		//!<\~english Sphere map mode	\~french Mode sphere map
		eTEXTURE_MAP_MODE_COUNT,
	}	eTEXTURE_MAP_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha functions enumeration
	\~french
	\brief		Enumération des fonctions alpha
	*/
	typedef enum eALPHA_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eALPHA_FUNC_ALWAYS,				//!<\~english Always take texture colour	\~french Prend toujours la couleur de la texture
		eALPHA_FUNC_LESS,				//!<\~english Takes texture colour if alpha is less than given value	\~french Prend la couleur de la texture si l'alpha est inferieur a la valeur donnee
		eALPHA_FUNC_LESS_OR_EQUAL,		//!<\~english Takes texture colour if alpha is less than or equal to given value	\~french Prend la couleur de la texture si l'alpha est inferieur ou egal a la valeur donnee
		eALPHA_FUNC_EQUAL,				//!<\~english Takes texture colour if alpha is equal to given value	\~french Prend la couleur de la texture si l'alpha est egal a la valeur donnee
		eALPHA_FUNC_NOT_EQUAL,			//!<\~english Takes texture colour if alpha is different of given value	\~french Prend la couleur de la texture si l'alpha est different de la valeur donnee
		eALPHA_FUNC_GREATER_OR_EQUAL,	//!<\~english Takes texture colour if alpha is grater than or equal to given value	\~french Prend la couleur de la texture si l'alpha est superieur ou egal a la valeur donnee
		eALPHA_FUNC_GREATER,			//!<\~english Takes texture colour if alpha is greater than given value	\~french Prend la couleur de la texture si l'alpha est superieur a la valeur donnee
		eALPHA_FUNC_NEVER,				//!<\~english Never take texture colour	\~french Ne prend jamais la couleur de la texture
		eALPHA_FUNC_COUNT,
	}	eALPHA_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture UVW enumeration
	\~french
	\brief		Enumération des UVW
	*/
	typedef enum eTEXTURE_UVW
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_UVW_U,
		eTEXTURE_UVW_V,
		eTEXTURE_UVW_W,
		eTEXTURE_UVW_COUNT,
	}	eTEXTURE_UVW;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture wrap modes enumeration
	\~french
	\brief		Enumération des modes de wrapping de texture
	*/
	typedef enum eWRAP_MODE
	CASTOR_TYPE( uint8_t )
	{
		eWRAP_MODE_REPEAT,
		eWRAP_MODE_MIRRORED_REPEAT,
		eWRAP_MODE_CLAMP_TO_BORDER,
		eWRAP_MODE_CLAMP_TO_EDGE,
		eWRAP_MODE_COUNT,
	}	eWRAP_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture interpolation filters enumeration
	\~french
	\brief		Enumération des filtres d'interpolation
	*/
	typedef enum eINTERPOLATION_FILTER
	CASTOR_TYPE( uint8_t )
	{
		eINTERPOLATION_FILTER_MIN,
		eINTERPOLATION_FILTER_MAG,
		eINTERPOLATION_FILTER_MIP,
		eINTERPOLATION_FILTER_COUNT,
	}	eINTERPOLATION_FILTER;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture filter interpolation modes enumeration
	\~french
	\brief		Enumération des modes d'interpolation
	*/
	typedef enum eINTERPOLATION_MODE
	CASTOR_TYPE( uint8_t )
	{
		eINTERPOLATION_MODE_UNDEFINED,
		eINTERPOLATION_MODE_NEAREST,
		eINTERPOLATION_MODE_LINEAR,
		eINTERPOLATION_MODE_ANISOTROPIC,
		eINTERPOLATION_MODE_COUNT,
	}	eINTERPOLATION_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending source indices enumeration
	\~french
	\brief		Enumération des indices de source de blending
	*/
	typedef enum eBLEND_SRC_INDEX
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_SRC_INDEX_0,
		eBLEND_SRC_INDEX_1,
		eBLEND_SRC_INDEX_2,
		eBLEND_SRC_INDEX_COUNT,
	}	eBLEND_SRC_INDEX;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture blending sources enumeration
	\~french
	\brief		Enumération des sources de blending
	*/
	typedef enum eBLEND_SOURCE
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_SOURCE_TEXTURE,
		eBLEND_SOURCE_TEXTURE0,
		eBLEND_SOURCE_TEXTURE1,
		eBLEND_SOURCE_TEXTURE2,
		eBLEND_SOURCE_TEXTURE3,
		eBLEND_SOURCE_CONSTANT,
		eBLEND_SOURCE_DIFFUSE,
		eBLEND_SOURCE_PREVIOUS,
		eBLEND_SOURCE_COUNT,
	}	eBLEND_SOURCE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		RGB blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange RGB
	*/
	typedef enum eRGB_BLEND_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eRGB_BLEND_FUNC_NONE,
		eRGB_BLEND_FUNC_FIRST_ARG,		//!< Arg0
		eRGB_BLEND_FUNC_ADD,			//!< Arg0 + Arg1
		eRGB_BLEND_FUNC_ADD_SIGNED,		//!< Arg0 + Arg1 - 0.5
		eRGB_BLEND_FUNC_MODULATE,		//!< Arg0 x Arg1
		eRGB_BLEND_FUNC_INTERPOLATE,	//!< Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eRGB_BLEND_FUNC_SUBTRACT,		//!< Arg0 - Arg1
		eRGB_BLEND_FUNC_DOT3_RGB,		//!< 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)))
		eRGB_BLEND_FUNC_DOT3_RGBA,		//!< 4 × (((Arg0.r - 0.5) × (Arg1.r - 0.5)) + ((Arg0.g - 0.5) × (Arg1.g - 0.5)) + ((Arg0.b - 0.5) × (Arg1.b - 0.5)) + ((Arg0.a - 0.5) × (Arg1.a - 0.5)))
		eRGB_BLEND_FUNC_COUNT,
	}	eRGB_BLEND_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Alpha blending functions enumeration
	\~french
	\brief		Enumération de fonctions de mélange alpha
	*/
	typedef enum eALPHA_BLEND_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eALPHA_BLEND_FUNC_NONE,
		eALPHA_BLEND_FUNC_FIRST_ARG,	//!< Arg0
		eALPHA_BLEND_FUNC_ADD,			//!< Arg0 + Arg1
		eALPHA_BLEND_FUNC_ADD_SIGNED,	//!< Arg0 + Arg1 - 0.5
		eALPHA_BLEND_FUNC_MODULATE,		//!< Arg0 x Arg1
		eALPHA_BLEND_FUNC_INTERPOLATE,	//!< Arg0 × Arg2 + Arg1 × (1 - Arg2)
		eALPHA_BLEND_FUNC_SUBSTRACT,	//!< Arg0 - Arg1
		eALPHA_BLEND_FUNC_COUNT,
	}	eALPHA_BLEND_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending operations enumeration
	\~french
	\brief		Enumération de opérations de mélange
	*/
	typedef enum eBLEND_OP
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_OP_ADD,				//!< Add source 1 and source 2.
		eBLEND_OP_SUBSTRACT,		//!< Subtract source 1 from source 2.
		eBLEND_OP_REV_SUBSTRACT,	//!< Subtract source 2 from source 1.
		eBLEND_OP_MIN,				//!< Find the minimum of source 1 and source 2.
		eBLEND_OP_MAX,				//!< Find the maximum of source 1 and source 2.
		eBLEND_OP_COUNT,
	}	eBLEND_OP;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blending modes enumeration
	\~french
	\brief		Enumération des modes de mélange
	*/
	typedef enum eBLEND_MODE
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_MODE_NONE,			//!< Order dependent blending.
		eBLEND_MODE_ADDITIVE,		//!< Order independent, add the components.
		eBLEND_MODE_MULTIPLICATIVE,	//!< Order independent, multiply the components.
		eBLEND_MODE_A_BUFFER,		//!< Order independent, using A-buffer, not implemented yet.
		eBLEND_MODE_DEPTH_PEELING,	//!< Order independent, using depth peeling, not implemented yet.
		eBLEND_MODE_COUNT,
	}	eBLEND_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture dimensions enumeration
	\~french
	\brief		Enumération des dimensions de texture
	*/
	typedef enum eTEXTURE_DIMENSION
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_DIMENSION_1D,		//!< 1 dimension textures = > one coordinate = > U mapping
		eTEXTURE_DIMENSION_2D,		//!< 2 dimensions textures = > two coordinates = > UV mapping
		eTEXTURE_DIMENSION_3D,		//!< 3 dimensions textures = > three coordinates = > UVW mapping
		eTEXTURE_DIMENSION_2DMS,	//!< 2 dimensions textures with multisample support = > two coordinates = > UV mapping
		eTEXTURE_DIMENSION_2DARRAY,	//!< 2 dimensions textures array = > two coordinates = > UV mapping
		eTEXTURE_DIMENSION_COUNT,
	}	eTEXTURE_DIMENSION;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture targets enumeration
	\~french
	\brief		Enumération des cibles de texture
	*/
	typedef enum eTEXTURE_TARGET
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_TARGET_1D,		//!< 1 dimension texture
		eTEXTURE_TARGET_2D,		//!< 2 dimensions texture
		eTEXTURE_TARGET_3D,		//!< 3 dimensions texture
		eTEXTURE_TARGET_LAYER,	//!< Layer texture
		eTEXTURE_TARGET_COUNT,
	}	eTEXTURE_TARGET;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture types enumeration
	\~french
	\brief		Enumération des types de texture
	*/
	typedef enum eTEXTURE_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eTEXTURE_TYPE_STATIC,
		eTEXTURE_TYPE_DYNAMIC,
	}	eTEXTURE_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Blend operators enumeration
	\~french
	\brief		Enumération des opérateurs de mélange
	*/
	typedef enum eBLEND
	CASTOR_TYPE( uint8_t )
	{
		eBLEND_ZERO,
		eBLEND_ONE,
		eBLEND_SRC_COLOUR,
		eBLEND_INV_SRC_COLOUR,
		eBLEND_DST_COLOUR,
		eBLEND_INV_DST_COLOUR,
		eBLEND_SRC_ALPHA,
		eBLEND_INV_SRC_ALPHA,
		eBLEND_DST_ALPHA,
		eBLEND_INV_DST_ALPHA,
		eBLEND_CONSTANT,
		eBLEND_INV_CONSTANT,
		eBLEND_SRC_ALPHA_SATURATE,
		eBLEND_SRC1_COLOUR,
		eBLEND_INV_SRC1_COLOUR,
		eBLEND_SRC1_ALPHA,
		eBLEND_INV_SRC1_ALPHA,
		eBLEND_COUNT,
	}	eBLEND;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Texture channels enumeration
	\~french
	\brief		Enumération des canaux de texture
	*/
	typedef enum eTEXTURE_CHANNEL
	CASTOR_TYPE( uint32_t )
	{
		eTEXTURE_CHANNEL_COLOUR = 0x00000001,	//!< Colour map
		eTEXTURE_CHANNEL_DIFFUSE = 0x00000002,	//!< Diffuse map
		eTEXTURE_CHANNEL_NORMAL = 0x00000004,	//!< Normal map
		eTEXTURE_CHANNEL_OPACITY = 0x00000008,	//!< Opacity map
		eTEXTURE_CHANNEL_SPECULAR = 0x00000010,	//!< Specular map
		eTEXTURE_CHANNEL_HEIGHT = 0x00000020,	//!< Height map
		eTEXTURE_CHANNEL_AMBIENT = 0x00000040,	//!< Ambient map
		eTEXTURE_CHANNEL_GLOSS = 0x00000080,	//!< Gloss map
		eTEXTURE_CHANNEL_TEXT = 0x00000100,		//!< Not really a texture channel (it is out of eTEXTURE_CHANNEL_ALL), used to tell we want text overlay shader source
		eTEXTURE_CHANNEL_ALL = 0x000000FF,
	}	eTEXTURE_CHANNEL;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Light indexes enumeration
	\~french
	\brief		Enumération des indeices des lumières
	*/
	typedef enum eLIGHT_INDEXES
	CASTOR_TYPE( uint8_t )
	{
		eLIGHT_INDEXES_0,
		eLIGHT_INDEXES_1,
		eLIGHT_INDEXES_2,
		eLIGHT_INDEXES_3,
		eLIGHT_INDEXES_4,
		eLIGHT_INDEXES_5,
		eLIGHT_INDEXES_6,
		eLIGHT_INDEXES_7,
		eLIGHT_INDEXES_COUNT,
	}	eLIGHT_INDEXES;

	class TextureBase;
	class StaticTexture;
	class DynamicTexture;
	//! The texture unit class
	class TextureUnit;
	//! The material class
	class Material;
	//! The material pass class (for multipass materials)
	class Pass;
	class MaterialManager;
	class Sampler;

	DECLARE_SMART_PTR( TextureBase );
	DECLARE_SMART_PTR( StaticTexture );
	DECLARE_SMART_PTR( DynamicTexture );
	DECLARE_SMART_PTR( TextureUnit );
	DECLARE_SMART_PTR( Material );
	DECLARE_SMART_PTR( Pass );
	DECLARE_SMART_PTR( MaterialManager );
	DECLARE_SMART_PTR( Sampler );

	//! Material pointer array
	DECLARE_VECTOR( MaterialSPtr, MaterialPtr );
	//! TextureUnit array
	DECLARE_VECTOR( TextureUnit, TextureUnit );
	//! TextureUnit pointer array
	DECLARE_VECTOR( TextureUnitSPtr, TextureUnitPtr );
	//! Pass array
	DECLARE_VECTOR( Pass, Pass );
	//! Pass pointer array
	DECLARE_VECTOR( PassSPtr, PassPtr );
	//! uint32_t array
	DECLARE_VECTOR( uint32_t, UInt );
	//! Material pointer map, sorted by name
	DECLARE_MAP( Castor::String, MaterialSPtr, MaterialPtrStr );
	//! Material pointer map
	DECLARE_MAP( uint32_t, MaterialSPtr, MaterialPtrUInt );
	//! Sampler collection
	DECLARE_COLLECTION( Sampler, Castor::String, Sampler );

	DECLARE_SMART_PTR( SamplerCollection );

	//@}
	/**@name Overlay */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Overlay types enumeration
	\~french
	\brief		Enumération des types d'overlays
	*/
	typedef enum eOVERLAY_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eOVERLAY_TYPE_PANEL,
		eOVERLAY_TYPE_BORDER_PANEL,
		eOVERLAY_TYPE_TEXT,
		eOVERLAY_TYPE_COUNT,
	}	eOVERLAY_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		28/11/2014
	\~english
	\brief		Text wrapping modes, handles the way text is cut when it overflows the overlay width
	\~french
	\brief		Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation
	*/
	typedef enum eTEXT_WRAPPING_MODE
	{
		//!\~english The text is cut	\~french Le texte qui dépasse est découpé
		eTEXT_WRAPPING_MODE_NONE,
		//!\~english The text jumps to next line	\~french Le texte passe à la ligne suivante
		eTEXT_WRAPPING_MODE_BREAK,
		//!\~english The text jumps to next line without cutting words	\~french Le texte passe à la ligne suivante, sans découper les mots
		eTEXT_WRAPPING_MODE_BREAK_WORDS,
	}	eTEXT_WRAPPING_MODE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		28/11/2014
	\~english
	\brief		Text wrapping modes, handles the way text is cut when it overflows the overlay width
	\~french
	\brief		Modes de découpe du texte, gère la manière dont le texte est dessiné quand il dépasse la largeur de l'incrustation
	*/
	typedef enum eBORDER_POSITION
	{
		//!\~english The border is outside the overlay	\~french La bordure est à l'intérieur de l'incrustation
		eBORDER_POSITION_INTERNAL,
		//!\~english The border is middle outside and middle inside the overlay	\~french La bordure est à moitié à l'intérieur, à moitié à l'extérieur de l'incrustation
		eBORDER_POSITION_MIDDLE,
		//!\~english The border is outside the overlay	\~french La bordure est à l'extérieur de l'incrustation
		eBORDER_POSITION_EXTERNAL,
	}	eBORDER_POSITION;

	class Overlay;
	class OverlayCategory;
	class PanelOverlay;
	class BorderPanelOverlay;
	class TextOverlay;
	class OverlayManager;
	class OverlayFactory;
	class OverlayRenderer;

	DECLARE_SMART_PTR( OverlayFactory );
	DECLARE_SMART_PTR( OverlayManager );
	DECLARE_SMART_PTR( OverlayRenderer );

	DECLARE_SMART_PTR( Overlay );
	DECLARE_SMART_PTR( OverlayCategory );
	DECLARE_SMART_PTR( PanelOverlay );
	DECLARE_SMART_PTR( BorderPanelOverlay );
	DECLARE_SMART_PTR( TextOverlay );

	DECLARE_VECTOR( OverlaySPtr, OverlayPtr );
	DECLARE_MAP( Castor::String, OverlaySPtr, OverlayPtrStr );

	//@}
	/**@name Render */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Matrix modes enumeration
	\~french
	\brief		Enumération des types de matrices
	*/
	typedef enum eMTXMODE
	CASTOR_TYPE( uint8_t )
	{
		eMTXMODE_PROJECTION,	//<!\~english Eye to Clip transform	\~french Transformations de Vue vers Clip
		eMTXMODE_MODEL,			//!<\~english Object to World transform	\~french Transformations de Objet vers Monde
		eMTXMODE_VIEW,			//!<\~english World to Eye transform	\~french Transformations de Monde vers Vue
		eMTXMODE_TEXTURE0,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE1,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE2,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE3,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE4,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE5,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE6,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE7,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE8,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE9,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE10,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE11,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE12,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE13,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE14,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE15,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE16,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE17,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE18,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE19,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE20,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE21,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE22,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE23,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE24,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE25,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE26,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE27,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE28,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE29,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE30,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE31,		//!<\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_COUNT
	}	eMTXMODE;

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Matrix modes masks
	\~french
	\brief		Masques pour les types de matrices
	*/
	static const uint64_t MASK_MTXMODE_PROJECTION = uint64_t( 0x1 ) << eMTXMODE_PROJECTION;
	static const uint64_t MASK_MTXMODE_MODEL = uint64_t( 0x1 ) << eMTXMODE_MODEL;
	static const uint64_t MASK_MTXMODE_VIEW = uint64_t( 0x1 ) << eMTXMODE_VIEW;
	static const uint64_t MASK_MTXMODE_TEXTURE0 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE0;
	static const uint64_t MASK_MTXMODE_TEXTURE1 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE1;
	static const uint64_t MASK_MTXMODE_TEXTURE2 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE2;
	static const uint64_t MASK_MTXMODE_TEXTURE3 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE3;
	static const uint64_t MASK_MTXMODE_TEXTURE4 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE4;
	static const uint64_t MASK_MTXMODE_TEXTURE5 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE5;
	static const uint64_t MASK_MTXMODE_TEXTURE6 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE6;
	static const uint64_t MASK_MTXMODE_TEXTURE7 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE7;
	static const uint64_t MASK_MTXMODE_TEXTURE8 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE8;
	static const uint64_t MASK_MTXMODE_TEXTURE9 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE9;
	static const uint64_t MASK_MTXMODE_TEXTURE10 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE10;
	static const uint64_t MASK_MTXMODE_TEXTURE11 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE11;
	static const uint64_t MASK_MTXMODE_TEXTURE12 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE12;
	static const uint64_t MASK_MTXMODE_TEXTURE13 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE13;
	static const uint64_t MASK_MTXMODE_TEXTURE14 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE14;
	static const uint64_t MASK_MTXMODE_TEXTURE15 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE15;
	static const uint64_t MASK_MTXMODE_TEXTURE16 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE16;
	static const uint64_t MASK_MTXMODE_TEXTURE17 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE17;
	static const uint64_t MASK_MTXMODE_TEXTURE18 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE18;
	static const uint64_t MASK_MTXMODE_TEXTURE19 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE19;
	static const uint64_t MASK_MTXMODE_TEXTURE20 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE20;
	static const uint64_t MASK_MTXMODE_TEXTURE21 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE21;
	static const uint64_t MASK_MTXMODE_TEXTURE22 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE22;
	static const uint64_t MASK_MTXMODE_TEXTURE23 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE23;
	static const uint64_t MASK_MTXMODE_TEXTURE24 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE24;
	static const uint64_t MASK_MTXMODE_TEXTURE25 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE25;
	static const uint64_t MASK_MTXMODE_TEXTURE26 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE26;
	static const uint64_t MASK_MTXMODE_TEXTURE27 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE27;
	static const uint64_t MASK_MTXMODE_TEXTURE28 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE28;
	static const uint64_t MASK_MTXMODE_TEXTURE29 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE29;
	static const uint64_t MASK_MTXMODE_TEXTURE30 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE30;
	static const uint64_t MASK_MTXMODE_TEXTURE31 = uint64_t( 0x1 ) << eMTXMODE_TEXTURE31;

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Buffer lock modes enumeration
	\~french
	\brief		Enumération des modes de lock de buffer
	*/
	typedef enum eLOCK_FLAG
	CASTOR_TYPE( uint32_t )
	{
		eLOCK_FLAG_READ_ONLY = 1 << 0,
		   eLOCK_FLAG_WRITE_ONLY = 1 << 1,
	}	eLOCK_FLAG;
	/*!
	\author 	Sylvain DOREMUS
	\see		eBUFFER_ACCESS_NATURE
	\~english
	\brief		Buffer access types enumeration
	\remark		Made to be combined with eBUFFER_ACCESS_NATURE
	\~french
	\brief		Enumération des types d'accès de tampon
	\remark		Fait pour être combiné avec eBUFFER_ACCESS_NATURE
	*/
	typedef enum eBUFFER_ACCESS_TYPE
	CASTOR_TYPE( uint32_t )
	{
		eBUFFER_ACCESS_TYPE_STATIC = 1 << 0,	//!< Modified once and used many times
		eBUFFER_ACCESS_TYPE_DYNAMIC = 1 << 1,	//!< Modified many times and used many times
		eBUFFER_ACCESS_TYPE_STREAM = 1 << 2,	//!< Modified oncce and used at most a few times
	}	eBUFFER_ACCESS_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\see		eBUFFER_ACCESS_TYPE
	\~english
	\brief		Buffer access natures enumeration
	\remark		Made to be combined with eBUFFER_ACCESS_TYPE
	\~french
	\brief		Enumération des natures des accès de tampon
	\remark		Fait pour être combiné avec eBUFFER_ACCESS_TYPE
	*/
	typedef enum eBUFFER_ACCESS_NATURE
	CASTOR_TYPE( uint32_t )
	{
		eBUFFER_ACCESS_NATURE_DRAW = 1 << 4,	//!< Modified by CPU and used by GPU
		eBUFFER_ACCESS_NATURE_READ = 1 << 5,	//!< Modified by GPU and used by CPU
		eBUFFER_ACCESS_NATURE_COPY = 1 << 6,	//!< Modified by GPU and used by GPU
	}	eBUFFER_ACCESS_NATURE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Primitive draw types enumeration
	\~french
	\brief		Enumération des type de primitive dessinées
	*/
	typedef enum eTOPOLOGY
	CASTOR_TYPE( uint8_t )
	{
		eTOPOLOGY_POINTS,			//!< Display textured points
		eTOPOLOGY_LINES,			//!< Display textured edges
		eTOPOLOGY_LINE_LOOP,		//!< Display textured edge loops
		eTOPOLOGY_LINE_STRIP,		//!< Display textured edge strips
		eTOPOLOGY_TRIANGLES,		//!< Display textured triangles
		eTOPOLOGY_TRIANGLE_STRIPS,	//!< Display triangle strips
		eTOPOLOGY_TRIANGLE_FAN,		//!< Display triangle fan
		eTOPOLOGY_QUADS,			//!< Display quads
		eTOPOLOGY_QUAD_STRIPS,		//!< Display quad strips
		eTOPOLOGY_POLYGON,			//!< Display quad strips
		eTOPOLOGY_COUNT,
	}	eTOPOLOGY;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Depth functions enumeration
	\~french
	\brief		Enumération des fonctions de profondeur
	*/
	typedef enum eDEPTH_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eDEPTH_FUNC_NEVER,		//!< Never passes.
		eDEPTH_FUNC_LESS,		//!< Passes if the incoming depth value is less than the stored depth value.
		eDEPTH_FUNC_EQUAL,		//!< Passes if the incoming depth value is equal to the stored depth value.
		eDEPTH_FUNC_LEQUAL,		//!< Passes if the incoming depth value is less than or equal to the stored depth value.
		eDEPTH_FUNC_GREATER,	//!< Passes if the incoming depth value is greater than the stored depth value.
		eDEPTH_FUNC_NOTEQUAL,	//!< Passes if the incoming depth value is not equal to the stored depth value.
		eDEPTH_FUNC_GEQUAL,		//!< Passes if the incoming depth value is greater than or equal to the stored depth value.
		eDEPTH_FUNC_ALWAYS,		//!< Always passes.
		eDEPTH_FUNC_COUNT,		//!< Functions count.
	}	eDEPTH_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Wrinting masks enumeration
	\~french
	\brief		Enumération des masques d'écriture
	*/
	typedef enum eWRITING_MASK
	CASTOR_TYPE( uint8_t )
	{
		eWRITING_MASK_ZERO,		//!< Writing disabled
		eWRITING_MASK_ALL,		//!< Writing enabled
		eWRITING_MASK_COUNT,	//!< Masks count
	}	eWRITING_MASK;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Stencil functions enumeration
	\~french
	\brief		Enumération des fonctions de stencil
	*/
	typedef enum eSTENCIL_FUNC
	CASTOR_TYPE( uint8_t )
	{
		eSTENCIL_FUNC_NEVER,	//!< Never passes.
		eSTENCIL_FUNC_LESS,		//!< Passes if the incoming stencil value is less than the stored stencil value.
		eSTENCIL_FUNC_EQUAL,	//!< Passes if the incoming stencil value is equal to the stored stencil value.
		eSTENCIL_FUNC_LEQUAL,	//!< Passes if the incoming stencil value is less than or equal to the stored stencil value.
		eSTENCIL_FUNC_GREATER,	//!< Passes if the incoming stencil value is greater than the stored stencil value.
		eSTENCIL_FUNC_NOTEQUAL,	//!< Passes if the incoming stencil value is not equal to the stored stencil value.
		eSTENCIL_FUNC_GEQUAL,	//!< Passes if the incoming stencil value is greater than or equal to the stored stencil value.
		eSTENCIL_FUNC_ALWAYS,	//!< Always passes.
		eSTENCIL_FUNC_COUNT,	//!< Functions count.
	}	eSTENCIL_FUNC;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Stencil functions enumeration
	\~french
	\brief		Enumération des fonctions de stencil
	*/
	typedef enum eSTENCIL_OP
	CASTOR_TYPE( uint8_t )
	{
		eSTENCIL_OP_KEEP,		//!< Keeps the current value.
		eSTENCIL_OP_ZERO,		//!< Sets the stencil buffer value to 0.
		eSTENCIL_OP_REPLACE,	//!< Sets the stencil buffer value by the one given
		eSTENCIL_OP_INCR,		//!< Increments the current stencil buffer value. Clamps to the maximum representable unsigned value.
		eSTENCIL_OP_INCR_WRAP,	//!< Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value.
		eSTENCIL_OP_DECR,		//!< Decrements the current stencil buffer value. Clamps to 0.
		eSTENCIL_OP_DECR_WRAP,	//!< Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero.
		eSTENCIL_OP_INVERT,		//!< Bitwise inverts the current stencil buffer value.
		eSTENCIL_OP_COUNT,		//!< Ops count.
	}	eSTENCIL_OP;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Element usage enumeration
	\~french
	\brief		Enumération des utilisations d'éléments de tampon
	*/
	typedef enum eELEMENT_USAGE
	CASTOR_TYPE( uint8_t )
	{
		eELEMENT_USAGE_POSITION,		//!< Position coords
		eELEMENT_USAGE_NORMAL,			//!< Normal coords
		eELEMENT_USAGE_TANGENT,			//!< Tangent coords
		eELEMENT_USAGE_BITANGENT,		//!< Bitangent coords
		eELEMENT_USAGE_DIFFUSE,			//!< Diffuse colour
		eELEMENT_USAGE_TEXCOORDS0,		//!< Texture coordinates 0
		eELEMENT_USAGE_TEXCOORDS1,		//!< Texture coordinates 1
		eELEMENT_USAGE_TEXCOORDS2,		//!< Texture coordinates 2
		eELEMENT_USAGE_TEXCOORDS3,		//!< Texture coordinates 3
		eELEMENT_USAGE_BONE_IDS,		//!< Bone IDs
		eELEMENT_USAGE_BONE_WEIGHTS,	//!< Bone weights
		eELEMENT_USAGE_COUNT,
	}	eELEMENT_USAGE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Element type enumeration
	\~french
	\brief		Enumération des types pour les éléments de tampon
	*/
	typedef enum eELEMENT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eELEMENT_TYPE_1FLOAT,	//!< 1 float
		eELEMENT_TYPE_2FLOATS,	//!< 2 floats
		eELEMENT_TYPE_3FLOATS,	//!< 3 floats
		eELEMENT_TYPE_4FLOATS,	//!< 4 floats
		eELEMENT_TYPE_COLOUR,	//!< colour (uint32_t)
		eELEMENT_TYPE_1INT,		//!< 1 int (4 bytes each)
		eELEMENT_TYPE_2INTS,	//!< 2 ints (4 bytes each)
		eELEMENT_TYPE_3INTS,	//!< 3 ints (4 bytes each)
		eELEMENT_TYPE_4INTS,	//!< 4 ints (4 bytes each)
		eELEMENT_TYPE_COUNT,
	}	eELEMENT_TYPE;

	template< typename T > class CpuBuffer;
	class RenderSystem;
	class Context;
	class RenderWindow;
	class IPipelineImpl;
	class Pipeline;
	class DepthStencilState;
	class RasteriserState;
	class BlendState;

	struct BufferElementDeclaration;
	class BufferDeclaration;
	template< typename T > class GpuBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class MatrixBuffer;
	class GeometryBuffers;
	class TextureBuffer;

	DECLARE_SMART_PTR( GeometryBuffers );
	DECLARE_SMART_PTR( BufferElementDeclaration );
	DECLARE_SMART_PTR( BufferDeclaration );
	DECLARE_SMART_PTR( VertexBuffer );
	DECLARE_SMART_PTR( IndexBuffer );
	DECLARE_SMART_PTR( MatrixBuffer );
	DECLARE_SMART_PTR( TextureBuffer );
	DECLARE_SMART_PTR( Context );
	DECLARE_SMART_PTR( DepthStencilState );
	DECLARE_SMART_PTR( RasteriserState );
	DECLARE_SMART_PTR( BlendState );

	DECLARE_COLLECTION( DepthStencilState, Castor::String, DepthStencilState );
	DECLARE_COLLECTION( RasteriserState, Castor::String, RasteriserState );
	DECLARE_COLLECTION( BlendState, Castor::String, BlendState );

	DECLARE_SMART_PTR( DepthStencilStateCollection );
	DECLARE_SMART_PTR( RasteriserStateCollection );
	DECLARE_SMART_PTR( BlendStateCollection );

	DECLARE_MAP( RenderWindow *, ContextSPtr, ContextPtr );
	DECLARE_MAP( std::thread::id, ContextPtrMap, ContextPtrMapId );

	//@}
	/**@name Scene */
	//@{

	class Scene;
	class SceneLoader;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;

	DECLARE_SMART_PTR( SceneNode );
	DECLARE_SMART_PTR( Scene );
	DECLARE_SMART_PTR( SceneFileContext );
	DECLARE_SMART_PTR( SceneFileParser );

	//! SceneNode pointer array
	DECLARE_VECTOR( SceneNodeSPtr, SceneNodePtr );
	//! Scene pointer map, sorted by name
	DECLARE_MAP( Castor::String, SceneSPtr, ScenePtrStr );
	//! SceneNode pointer map, sorted by name
	DECLARE_MAP( Castor::String, SceneNodeSPtr, SceneNodePtrStr );

	//@}
	/**@name Shader */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Enumeration of existing shader models
	\~french
	\brief		Enumération des modèles de shader existants
	\remark
	*/
	typedef enum eSHADER_MODEL
	CASTOR_TYPE( uint8_t )
	{
		eSHADER_MODEL_1,		//!< Vertex shaders
		eSHADER_MODEL_2,		//!< Pixel shaders
		eSHADER_MODEL_3,		//!< Geometry shaders
		eSHADER_MODEL_4,		//!< Tessellation shaders
		eSHADER_MODEL_5,		//!< Compute shaders
		eSHADER_MODEL_COUNT,	//!< A count
	}	eSHADER_MODEL;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Enumeration of existing shader lnaguages
	\~french
	\brief		Enumération des langages de shader existants
	\remark
	*/
	typedef enum eSHADER_LANGUAGE
	CASTOR_TYPE( uint8_t )
	{
		eSHADER_LANGUAGE_GLSL,
		eSHADER_LANGUAGE_HLSL,
		eSHADER_LANGUAGE_AUTO,
		eSHADER_LANGUAGE_COUNT,
	}	eSHADER_LANGUAGE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Shader status enueration
	\~french
	\brief		Enumération des statuts de shader
	*/
	typedef enum eSHADER_STATUS
	CASTOR_TYPE( uint8_t )
	{
		eSHADER_STATUS_DONTEXIST,
		eSHADER_STATUS_NOTCOMPILED,
		eSHADER_STATUS_ERROR,
		eSHADER_STATUS_COMPILED,
	}	eSHADER_STATUS;
	/*!
	\author 	Sylvain DOREMUS
	\date 		22/08/2012
	\version	0.7.0.0
	\~english
	\brief		Program status enumeration
	\~french
	\brief		Enumération des statuts de programme
	*/
	typedef enum ePROGRAM_STATUS
	CASTOR_TYPE( uint8_t )
	{
		ePROGRAM_STATUS_NOTLINKED,
		ePROGRAM_STATUS_ERROR,
		ePROGRAM_STATUS_LINKED,
	}	ePROGRAM_STATUS;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types enumeration
	\~french
	\brief		Enumération des types de shader object
	*/
	typedef enum eSHADER_TYPE
	CASTOR_TYPE( int8_t )
	{
		eSHADER_TYPE_NONE = -1,
			 eSHADER_TYPE_VERTEX,
			 eSHADER_TYPE_HULL,
			 eSHADER_TYPE_DOMAIN,
			 eSHADER_TYPE_GEOMETRY,
			 eSHADER_TYPE_PIXEL,
			 eSHADER_TYPE_COMPUTE,
			 eSHADER_TYPE_COUNT,
	}	eSHADER_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Shader object types masks
	\~french
	\brief		Masques pour les types de shader object
	*/
	static const uint64_t MASK_SHADER_TYPE_VERTEX = uint64_t( 0x1 ) << eSHADER_TYPE_VERTEX;
	static const uint64_t MASK_SHADER_TYPE_HULL = uint64_t( 0x1 ) << eSHADER_TYPE_HULL;
	static const uint64_t MASK_SHADER_TYPE_DOMAIN = uint64_t( 0x1 ) << eSHADER_TYPE_DOMAIN;
	static const uint64_t MASK_SHADER_TYPE_GEOMETRY = uint64_t( 0x1 ) << eSHADER_TYPE_GEOMETRY;
	static const uint64_t MASK_SHADER_TYPE_PIXEL = uint64_t( 0x1 ) << eSHADER_TYPE_PIXEL;
	static const uint64_t MASK_SHADER_TYPE_COMPUTE = uint64_t( 0x1 ) << eSHADER_TYPE_COMPUTE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		20/11/13
	\version	0.7.0.0
	\~english
	\brief		Flags to use when looking for an automatically generated program
	\~french
	\brief		Indicateurs à utiliser lors de la recherche de programme généré automatiquement
	*/
	typedef enum ePROGRAM_FLAG
	CASTOR_TYPE( uint32_t )
	{
		ePROGRAM_FLAG_INSTANCIATION = 0x01,	//!<\~english Program using instanciation	\~french Programme utilisant l'instanciation
		ePROGRAM_FLAG_SKINNING = 0x02,		//!<\~english Program using skeleton animations	\~french Programme utilisant les animations par squelette
		ePROGRAM_FLAG_BILLBOARDS = 0x04,	//!<\~english Program used by billboards	\~french Programme utilisé par des billboards
	}	ePROGRAM_FLAG;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables dimensions enumeration
	\~french
	\brief		Enumération des dimensions de variable supportées
	*/
	typedef enum eVARIABLE_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eVARIABLE_TYPE_ONE,		//!< One variable
		eVARIABLE_TYPE_VEC1,	//!< 1 variable array
		eVARIABLE_TYPE_VEC2,	//!< 2 variables array
		eVARIABLE_TYPE_VEC3,	//!< 3 variables array
		eVARIABLE_TYPE_VEC4,	//!< 4 variables array
		eVARIABLE_TYPE_MAT1,	//!< 1x1 matrix
		eVARIABLE_TYPE_MAT2X2,	//!< 2x2 matrix
		eVARIABLE_TYPE_MAT2X3,	//!< 2x3 matrix
		eVARIABLE_TYPE_MAT2X4,	//!< 2x4 matrix
		eVARIABLE_TYPE_MAT3X2,	//!< 3x2 matrix
		eVARIABLE_TYPE_MAT3X3,	//!< 3x3 matrix
		eVARIABLE_TYPE_MAT3X4,	//!< 3x4 matrix
		eVARIABLE_TYPE_MAT4X2,	//!< 4x2 matrix
		eVARIABLE_TYPE_MAT4X3,	//!< 4x3 matrix
		eVARIABLE_TYPE_MAT4X4,	//!< 4x4 matrix
		eVARIABLE_TYPE_COUNT,
	}	eVARIABLE_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Supported variables full type enumeration
	\~french
	\brief		Enumération des types complets de variable supportées
	*/
	typedef enum eFRAME_VARIABLE_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eFRAME_VARIABLE_TYPE_BOOL,
		eFRAME_VARIABLE_TYPE_INT,
		eFRAME_VARIABLE_TYPE_UINT,
		eFRAME_VARIABLE_TYPE_FLOAT,
		eFRAME_VARIABLE_TYPE_DOUBLE,
		eFRAME_VARIABLE_TYPE_SAMPLER,
		eFRAME_VARIABLE_TYPE_VEC2B,
		eFRAME_VARIABLE_TYPE_VEC3B,
		eFRAME_VARIABLE_TYPE_VEC4B,
		eFRAME_VARIABLE_TYPE_VEC2I,
		eFRAME_VARIABLE_TYPE_VEC3I,
		eFRAME_VARIABLE_TYPE_VEC4I,
		eFRAME_VARIABLE_TYPE_VEC2UI,
		eFRAME_VARIABLE_TYPE_VEC3UI,
		eFRAME_VARIABLE_TYPE_VEC4UI,
		eFRAME_VARIABLE_TYPE_VEC2F,
		eFRAME_VARIABLE_TYPE_VEC3F,
		eFRAME_VARIABLE_TYPE_VEC4F,
		eFRAME_VARIABLE_TYPE_VEC2D,
		eFRAME_VARIABLE_TYPE_VEC3D,
		eFRAME_VARIABLE_TYPE_VEC4D,
		eFRAME_VARIABLE_TYPE_MAT2X2B,
		eFRAME_VARIABLE_TYPE_MAT2X3B,
		eFRAME_VARIABLE_TYPE_MAT2X4B,
		eFRAME_VARIABLE_TYPE_MAT3X2B,
		eFRAME_VARIABLE_TYPE_MAT3X3B,
		eFRAME_VARIABLE_TYPE_MAT3X4B,
		eFRAME_VARIABLE_TYPE_MAT4X2B,
		eFRAME_VARIABLE_TYPE_MAT4X3B,
		eFRAME_VARIABLE_TYPE_MAT4X4B,
		eFRAME_VARIABLE_TYPE_MAT2X2I,
		eFRAME_VARIABLE_TYPE_MAT2X3I,
		eFRAME_VARIABLE_TYPE_MAT2X4I,
		eFRAME_VARIABLE_TYPE_MAT3X2I,
		eFRAME_VARIABLE_TYPE_MAT3X3I,
		eFRAME_VARIABLE_TYPE_MAT3X4I,
		eFRAME_VARIABLE_TYPE_MAT4X2I,
		eFRAME_VARIABLE_TYPE_MAT4X3I,
		eFRAME_VARIABLE_TYPE_MAT4X4I,
		eFRAME_VARIABLE_TYPE_MAT2X2UI,
		eFRAME_VARIABLE_TYPE_MAT2X3UI,
		eFRAME_VARIABLE_TYPE_MAT2X4UI,
		eFRAME_VARIABLE_TYPE_MAT3X2UI,
		eFRAME_VARIABLE_TYPE_MAT3X3UI,
		eFRAME_VARIABLE_TYPE_MAT3X4UI,
		eFRAME_VARIABLE_TYPE_MAT4X2UI,
		eFRAME_VARIABLE_TYPE_MAT4X3UI,
		eFRAME_VARIABLE_TYPE_MAT4X4UI,
		eFRAME_VARIABLE_TYPE_MAT2X2F,
		eFRAME_VARIABLE_TYPE_MAT2X3F,
		eFRAME_VARIABLE_TYPE_MAT2X4F,
		eFRAME_VARIABLE_TYPE_MAT3X2F,
		eFRAME_VARIABLE_TYPE_MAT3X3F,
		eFRAME_VARIABLE_TYPE_MAT3X4F,
		eFRAME_VARIABLE_TYPE_MAT4X2F,
		eFRAME_VARIABLE_TYPE_MAT4X3F,
		eFRAME_VARIABLE_TYPE_MAT4X4F,
		eFRAME_VARIABLE_TYPE_MAT2X2D,
		eFRAME_VARIABLE_TYPE_MAT2X3D,
		eFRAME_VARIABLE_TYPE_MAT2X4D,
		eFRAME_VARIABLE_TYPE_MAT3X2D,
		eFRAME_VARIABLE_TYPE_MAT3X3D,
		eFRAME_VARIABLE_TYPE_MAT3X4D,
		eFRAME_VARIABLE_TYPE_MAT4X2D,
		eFRAME_VARIABLE_TYPE_MAT4X3D,
		eFRAME_VARIABLE_TYPE_MAT4X4D,
	}	eFRAME_VARIABLE_TYPE;

#if CASTOR_USE_DOUBLE
#	define eFRAME_VARIABLE_TYPE_REAL	eFRAME_VARIABLE_TYPE_DOUBLE
#	define eFRAME_VARIABLE_TYPE_VEC2R	eFRAME_VARIABLE_TYPE_VEC2D
#	define eFRAME_VARIABLE_TYPE_VEC3R	eFRAME_VARIABLE_TYPE_VEC3D
#	define eFRAME_VARIABLE_TYPE_VEC4R	eFRAME_VARIABLE_TYPE_VEC4D
#	define eFRAME_VARIABLE_TYPE_MAT2X2R	eFRAME_VARIABLE_TYPE_MAT2X2D
#	define eFRAME_VARIABLE_TYPE_MAT2X3R	eFRAME_VARIABLE_TYPE_MAT2X3D
#	define eFRAME_VARIABLE_TYPE_MAT2X4R	eFRAME_VARIABLE_TYPE_MAT2X4D
#	define eFRAME_VARIABLE_TYPE_MAT3X2R	eFRAME_VARIABLE_TYPE_MAT3X2D
#	define eFRAME_VARIABLE_TYPE_MAT3X3R	eFRAME_VARIABLE_TYPE_MAT3X3D
#	define eFRAME_VARIABLE_TYPE_MAT3X4R	eFRAME_VARIABLE_TYPE_MAT3X4D
#	define eFRAME_VARIABLE_TYPE_MAT4X2R	eFRAME_VARIABLE_TYPE_MAT4X2D
#	define eFRAME_VARIABLE_TYPE_MAT4X3R	eFRAME_VARIABLE_TYPE_MAT4X3D
#	define eFRAME_VARIABLE_TYPE_MAT4X4R	eFRAME_VARIABLE_TYPE_MAT4X4D
#else
#	define eFRAME_VARIABLE_TYPE_REAL	eFRAME_VARIABLE_TYPE_FLOAT
#	define eFRAME_VARIABLE_TYPE_VEC2R	eFRAME_VARIABLE_TYPE_VEC2F
#	define eFRAME_VARIABLE_TYPE_VEC3R	eFRAME_VARIABLE_TYPE_VEC3F
#	define eFRAME_VARIABLE_TYPE_VEC4R	eFRAME_VARIABLE_TYPE_VEC4F
#	define eFRAME_VARIABLE_TYPE_MAT2X2R	eFRAME_VARIABLE_TYPE_MAT2X2F
#	define eFRAME_VARIABLE_TYPE_MAT2X3R	eFRAME_VARIABLE_TYPE_MAT2X3F
#	define eFRAME_VARIABLE_TYPE_MAT2X4R	eFRAME_VARIABLE_TYPE_MAT2X4F
#	define eFRAME_VARIABLE_TYPE_MAT3X2R	eFRAME_VARIABLE_TYPE_MAT3X2F
#	define eFRAME_VARIABLE_TYPE_MAT3X3R	eFRAME_VARIABLE_TYPE_MAT3X3F
#	define eFRAME_VARIABLE_TYPE_MAT3X4R	eFRAME_VARIABLE_TYPE_MAT3X4F
#	define eFRAME_VARIABLE_TYPE_MAT4X2R	eFRAME_VARIABLE_TYPE_MAT4X2F
#	define eFRAME_VARIABLE_TYPE_MAT4X3R	eFRAME_VARIABLE_TYPE_MAT4X3F
#	define eFRAME_VARIABLE_TYPE_MAT4X4R	eFRAME_VARIABLE_TYPE_MAT4X4F
#endif

	class ShaderManager;
	class ShaderProgramBase;
	class ShaderObjectBase;
	class FrameVariable;
	class FrameVariableBuffer;
	class FrameVariableLink;
	class ProgramLinks;
	template < typename T > class OneFrameVariable;
	template < typename T, uint32_t Count > class PointFrameVariable;
	template < typename T, uint32_t Rows, uint32_t Columns > class MatrixFrameVariable;

	typedef OneFrameVariable< TextureBase * > OneTextureFrameVariable;
	typedef OneFrameVariable< bool > OneBoolFrameVariable;
	typedef OneFrameVariable< int > OneIntFrameVariable;
	typedef OneFrameVariable< uint32_t > OneUIntFrameVariable;
	typedef OneFrameVariable< float > OneFloatFrameVariable;
	typedef OneFrameVariable< double > OneDoubleFrameVariable;
	typedef OneFrameVariable< real > OneRealFrameVariable;
	typedef PointFrameVariable< bool, 2 > Point2bFrameVariable;
	typedef PointFrameVariable< bool, 3 > Point3bFrameVariable;
	typedef PointFrameVariable< bool, 4 > Point4bFrameVariable;
	typedef PointFrameVariable< int, 2 > Point2iFrameVariable;
	typedef PointFrameVariable< int, 3 > Point3iFrameVariable;
	typedef PointFrameVariable< int, 4 > Point4iFrameVariable;
	typedef PointFrameVariable< uint32_t, 2 > Point2uiFrameVariable;
	typedef PointFrameVariable< uint32_t, 3 > Point3uiFrameVariable;
	typedef PointFrameVariable< uint32_t, 4 > Point4uiFrameVariable;
	typedef PointFrameVariable< float, 2 > Point2fFrameVariable;
	typedef PointFrameVariable< float, 3 > Point3fFrameVariable;
	typedef PointFrameVariable< float, 4 > Point4fFrameVariable;
	typedef PointFrameVariable< double, 2 > Point2dFrameVariable;
	typedef PointFrameVariable< double, 3 > Point3dFrameVariable;
	typedef PointFrameVariable< double, 4 > Point4dFrameVariable;
	typedef PointFrameVariable< real, 2 > Point2rFrameVariable;
	typedef PointFrameVariable< real, 3 > Point3rFrameVariable;
	typedef PointFrameVariable< real, 4 > Point4rFrameVariable;
	typedef MatrixFrameVariable< bool, 2, 2 > Matrix2x2bFrameVariable;
	typedef MatrixFrameVariable< bool, 2, 3 > Matrix2x3bFrameVariable;
	typedef MatrixFrameVariable< bool, 2, 4 > Matrix2x4bFrameVariable;
	typedef MatrixFrameVariable< bool, 3, 2 > Matrix3x2bFrameVariable;
	typedef MatrixFrameVariable< bool, 3, 3 > Matrix3x3bFrameVariable;
	typedef MatrixFrameVariable< bool, 3, 4 > Matrix3x4bFrameVariable;
	typedef MatrixFrameVariable< bool, 4, 2 > Matrix4x2bFrameVariable;
	typedef MatrixFrameVariable< bool, 4, 3 > Matrix4x3bFrameVariable;
	typedef MatrixFrameVariable< bool, 4, 4 > Matrix4x4bFrameVariable;
	typedef MatrixFrameVariable< int, 2, 2 > Matrix2x2iFrameVariable;
	typedef MatrixFrameVariable< int, 2, 3 > Matrix2x3iFrameVariable;
	typedef MatrixFrameVariable< int, 2, 4 > Matrix2x4iFrameVariable;
	typedef MatrixFrameVariable< int, 3, 2 > Matrix3x2iFrameVariable;
	typedef MatrixFrameVariable< int, 3, 3 > Matrix3x3iFrameVariable;
	typedef MatrixFrameVariable< int, 3, 4 > Matrix3x4iFrameVariable;
	typedef MatrixFrameVariable< int, 4, 2 > Matrix4x2iFrameVariable;
	typedef MatrixFrameVariable< int, 4, 3 > Matrix4x3iFrameVariable;
	typedef MatrixFrameVariable< int, 4, 4 > Matrix4x4iFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 2, 2 > Matrix2x2uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 2, 3 > Matrix2x3uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 2, 4 > Matrix2x4uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 3, 2 > Matrix3x2uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 3, 3 > Matrix3x3uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 3, 4 > Matrix3x4uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 4, 2 > Matrix4x2uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 4, 3 > Matrix4x3uiFrameVariable;
	typedef MatrixFrameVariable< unsigned  int, 4, 4 > Matrix4x4uiFrameVariable;
	typedef MatrixFrameVariable< float, 2, 2 > Matrix2x2fFrameVariable;
	typedef MatrixFrameVariable< float, 2, 3 > Matrix2x3fFrameVariable;
	typedef MatrixFrameVariable< float, 2, 4 > Matrix2x4fFrameVariable;
	typedef MatrixFrameVariable< float, 3, 2 > Matrix3x2fFrameVariable;
	typedef MatrixFrameVariable< float, 3, 3 > Matrix3x3fFrameVariable;
	typedef MatrixFrameVariable< float, 3, 4 > Matrix3x4fFrameVariable;
	typedef MatrixFrameVariable< float, 4, 2 > Matrix4x2fFrameVariable;
	typedef MatrixFrameVariable< float, 4, 3 > Matrix4x3fFrameVariable;
	typedef MatrixFrameVariable< float, 4, 4 > Matrix4x4fFrameVariable;
	typedef MatrixFrameVariable< double, 2, 2 > Matrix2x2dFrameVariable;
	typedef MatrixFrameVariable< double, 2, 3 > Matrix2x3dFrameVariable;
	typedef MatrixFrameVariable< double, 2, 4 > Matrix2x4dFrameVariable;
	typedef MatrixFrameVariable< double, 3, 2 > Matrix3x2dFrameVariable;
	typedef MatrixFrameVariable< double, 3, 3 > Matrix3x3dFrameVariable;
	typedef MatrixFrameVariable< double, 3, 4 > Matrix3x4dFrameVariable;
	typedef MatrixFrameVariable< double, 4, 2 > Matrix4x2dFrameVariable;
	typedef MatrixFrameVariable< double, 4, 3 > Matrix4x3dFrameVariable;
	typedef MatrixFrameVariable< double, 4, 4 > Matrix4x4dFrameVariable;
	typedef MatrixFrameVariable< real, 2, 2 > Matrix2x2rFrameVariable;
	typedef MatrixFrameVariable< real, 2, 3 > Matrix2x3rFrameVariable;
	typedef MatrixFrameVariable< real, 2, 4 > Matrix2x4rFrameVariable;
	typedef MatrixFrameVariable< real, 3, 2 > Matrix3x2rFrameVariable;
	typedef MatrixFrameVariable< real, 3, 3 > Matrix3x3rFrameVariable;
	typedef MatrixFrameVariable< real, 3, 4 > Matrix3x4rFrameVariable;
	typedef MatrixFrameVariable< real, 4, 2 > Matrix4x2rFrameVariable;
	typedef MatrixFrameVariable< real, 4, 3 > Matrix4x3rFrameVariable;
	typedef MatrixFrameVariable< real, 4, 4 > Matrix4x4rFrameVariable;

	DECLARE_SMART_PTR( OneTextureFrameVariable );
	DECLARE_SMART_PTR( OneBoolFrameVariable );
	DECLARE_SMART_PTR( OneIntFrameVariable );
	DECLARE_SMART_PTR( OneUIntFrameVariable );
	DECLARE_SMART_PTR( OneFloatFrameVariable );
	DECLARE_SMART_PTR( OneDoubleFrameVariable );
	DECLARE_SMART_PTR( OneRealFrameVariable );
	DECLARE_SMART_PTR( Point2bFrameVariable );
	DECLARE_SMART_PTR( Point3bFrameVariable );
	DECLARE_SMART_PTR( Point4bFrameVariable );
	DECLARE_SMART_PTR( Point2iFrameVariable );
	DECLARE_SMART_PTR( Point3iFrameVariable );
	DECLARE_SMART_PTR( Point4iFrameVariable );
	DECLARE_SMART_PTR( Point2uiFrameVariable );
	DECLARE_SMART_PTR( Point3uiFrameVariable );
	DECLARE_SMART_PTR( Point4uiFrameVariable );
	DECLARE_SMART_PTR( Point2fFrameVariable );
	DECLARE_SMART_PTR( Point3fFrameVariable );
	DECLARE_SMART_PTR( Point4fFrameVariable );
	DECLARE_SMART_PTR( Point2dFrameVariable );
	DECLARE_SMART_PTR( Point3dFrameVariable );
	DECLARE_SMART_PTR( Point4dFrameVariable );
	DECLARE_SMART_PTR( Point2rFrameVariable );
	DECLARE_SMART_PTR( Point3rFrameVariable );
	DECLARE_SMART_PTR( Point4rFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2bFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3bFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4bFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2bFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3bFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4bFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2bFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3bFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4bFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2iFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3iFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4iFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2iFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3iFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4iFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2iFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3iFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4iFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2uiFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3uiFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4uiFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2uiFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3uiFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4uiFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2uiFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3uiFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4uiFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2fFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3fFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4fFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2fFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3fFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4fFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2fFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3fFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4fFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2dFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3dFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4dFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2dFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3dFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4dFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2dFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3dFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4dFrameVariable );
	DECLARE_SMART_PTR( Matrix2x2rFrameVariable );
	DECLARE_SMART_PTR( Matrix2x3rFrameVariable );
	DECLARE_SMART_PTR( Matrix2x4rFrameVariable );
	DECLARE_SMART_PTR( Matrix3x2rFrameVariable );
	DECLARE_SMART_PTR( Matrix3x3rFrameVariable );
	DECLARE_SMART_PTR( Matrix3x4rFrameVariable );
	DECLARE_SMART_PTR( Matrix4x2rFrameVariable );
	DECLARE_SMART_PTR( Matrix4x3rFrameVariable );
	DECLARE_SMART_PTR( Matrix4x4rFrameVariable );

	DECLARE_SMART_PTR( FrameVariableLink );
	DECLARE_SMART_PTR( ProgramLinks );
	DECLARE_SMART_PTR( ShaderManager );
	DECLARE_SMART_PTR( FrameVariableBuffer );
	DECLARE_SMART_PTR( FrameVariable );
	DECLARE_SMART_PTR( ShaderObjectBase );
	DECLARE_SMART_PTR( ShaderProgramBase );

	DECLARE_LIST( FrameVariableLinkSPtr, VariableLinkPtr );
	DECLARE_MAP( ShaderProgramBase *, ProgramLinksSPtr, LinksPtrListProgram );
	DECLARE_VECTOR( ShaderProgramBaseSPtr, ShaderProgramPtr );
	DECLARE_VECTOR( ShaderObjectBaseSPtr, ShaderObjectPtr );
	DECLARE_LIST( FrameVariableSPtr, FrameVariablePtr );
	DECLARE_LIST( FrameVariableBufferSPtr, FrameVariableBufferPtr );
	DECLARE_MAP( Castor::String, FrameVariableWPtr, FrameVariablePtrStr );
	DECLARE_MAP( Castor::String, FrameVariableBufferWPtr, FrameVariableBufferPtrStr );
	DECLARE_MAP( eSHADER_TYPE, FrameVariableBufferWPtr, FrameVariableBufferPtrShader );

	//@}
	/**@name General */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Movable object types enumerator
	\remark		There are four movable object types : camera, geometry, light and billboard.
	\~french
	\brief		Enumération des types de MovableObject
	\remark		Il y a quatre types d'objets déplaçables : caméra, géométrie, lumière et billboard
	*/
	typedef enum eMOVABLE_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eMOVABLE_TYPE_CAMERA,
		eMOVABLE_TYPE_GEOMETRY,
		eMOVABLE_TYPE_LIGHT,
		eMOVABLE_TYPE_BILLBOARD,
		eMOVABLE_TYPE_COUNT,
	}	eMOVABLE_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		RenderTarget supported types
	\~french
	\brief		Types de RenderTarget supportés
	*/
	typedef enum eTARGET_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eTARGET_TYPE_WINDOW,
		eTARGET_TYPE_TEXTURE,
		eTARGET_TYPE_COUNT,
	}	eTARGET_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Plugin types enumeration
	\~french
	\brief		Enumération des types de plugins
	*/
	typedef enum ePLUGIN_TYPE
	CASTOR_TYPE( uint8_t )
	{
		ePLUGIN_TYPE_RENDERER,	//!< Renderer plugin (OpenGl2, 3 or Direct3D)
		ePLUGIN_TYPE_IMPORTER,	//!< Importer plugin
		ePLUGIN_TYPE_DIVIDER,	//!< Mesh Divider plugin
		ePLUGIN_TYPE_GENERATOR,	//!< Procedural generator
		ePLUGIN_TYPE_TECHNIQUE,	//!< Render technique plugin
		ePLUGIN_TYPE_POSTFX,	//!< Post effect plugin
		ePLUGIN_TYPE_GENERIC,	//!< Generic plugin
		ePLUGIN_TYPE_COUNT,
	}	ePLUGIN_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		01/04/2011
	\~english
	\brief		Supported renderers enumeration
	\~french
	\brief		Enumération des renderers supportés
	*/
	typedef enum eRENDERER_TYPE
	CASTOR_TYPE( int8_t )
	{
		eRENDERER_TYPE_UNDEFINED = -1,
		eRENDERER_TYPE_OPENGL = 0,		//!< OpenGl Renderer
		eRENDERER_TYPE_DIRECT3D = 1,	//!< Direct3D11 Renderer
		eRENDERER_TYPE_COUNT = 2,
	}	eRENDERER_TYPE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer components enumeration
	\~french
	\brief		Enumération des composantes de tampon d'image
	*/
	typedef enum eBUFFER_COMPONENT
	CASTOR_TYPE( uint8_t )
	{
		eBUFFER_COMPONENT_COLOUR = 1,
		eBUFFER_COMPONENT_DEPTH = 2,
		eBUFFER_COMPONENT_STENCIL = 4,
	}	eBUFFER_COMPONENT;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer components enumeration
	\~french
	\brief		Enumération des composantes de tampon d'image
	*/
	typedef enum eBUFFER
	CASTOR_TYPE( uint8_t )
	{
		eBUFFER_NONE,
		eBUFFER_FRONT_LEFT,
		eBUFFER_FRONT_RIGHT,
		eBUFFER_BACK_LEFT,
		eBUFFER_BACK_RIGHT,
		eBUFFER_FRONT,
		eBUFFER_BACK,
		eBUFFER_LEFT,
		eBUFFER_RIGHT,
		eBUFFER_FRONT_AND_BACK,
		eBUFFER_COUNT,
	}	eBUFFER;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer binding targets enumeration
	\~french
	\brief		Enumération des cibles d'activation de tampon d'image
	*/
	typedef enum eFRAMEBUFFER_TARGET
	CASTOR_TYPE( uint8_t )
	{
		eFRAMEBUFFER_TARGET_DRAW,	//!<\~english Frame buffer is bound as a target for draws	\~french Le tampon d'image est activé en tant que cible pour les rendus
		eFRAMEBUFFER_TARGET_READ,	//!<\~english Frame buffer is bound as a target for reads	\~french Le tampon d'image est activé en tant que cible pour les lectures
		eFRAMEBUFFER_TARGET_BOTH,	//!<\~english Frame buffer is bound as a target for reads and draws	\~french Le tampon d'image est activé en tant que cible pour les lectures et les rendus
		eFRAMEBUFFER_TARGET_COUNT,
	}	eFRAMEBUFFER_TARGET;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer binding modes enumeration
	\~french
	\brief		Enumération des modes d'activation de tampon d'image
	*/
	typedef enum eFRAMEBUFFER_MODE
	CASTOR_TYPE( uint8_t )
	{
		eFRAMEBUFFER_MODE_CONFIG,		//!<\~english Frame buffer is bound for configuration	\~french Le tampon d'image est activé pour configuration
		eFRAMEBUFFER_MODE_AUTOMATIC,	//!<\~english Frame buffer is bound and FrameBuffer::SetDrawBuffers is called automatically	\~french Le tampon d'image est activé et FrameBuffer::SetDrawBuffers est appelée automatiquement
		eFRAMEBUFFER_MODE_MANUAL,		//!<\~english Frame buffer is bound and user must call FrameBuffer::SetDrawBuffers if he wants	\~french Le tampon d'image est activé et l'utilisateur doit appeler FrameBuffer::SetDrawBuffers s'il veut
		eFRAMEBUFFER_MODE_COUNT,
	}	eFRAMEBUFFER_MODE;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Frame buffer attachment points enumeration
	\~french
	\brief		Enumération des points d'attache pour un tampon d'image
	*/
	typedef enum eATTACHMENT_POINT
	CASTOR_TYPE( uint8_t )
	{
		eATTACHMENT_POINT_NONE,
		eATTACHMENT_POINT_COLOUR0,
		eATTACHMENT_POINT_COLOUR1,
		eATTACHMENT_POINT_COLOUR2,
		eATTACHMENT_POINT_COLOUR3,
		eATTACHMENT_POINT_COLOUR4,
		eATTACHMENT_POINT_COLOUR5,
		eATTACHMENT_POINT_COLOUR6,
		eATTACHMENT_POINT_COLOUR7,
		eATTACHMENT_POINT_COLOUR8,
		eATTACHMENT_POINT_COLOUR9,
		eATTACHMENT_POINT_COLOUR10,
		eATTACHMENT_POINT_COLOUR11,
		eATTACHMENT_POINT_COLOUR12,
		eATTACHMENT_POINT_COLOUR13,
		eATTACHMENT_POINT_COLOUR14,
		eATTACHMENT_POINT_COLOUR15,
		eATTACHMENT_POINT_DEPTH,
		eATTACHMENT_POINT_STENCIL,
		eATTACHMENT_POINT_COUNT,
	}	eATTACHMENT_POINT;
	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		Projection Directions enumeration
	\~french
	\brief		Enumération des directions de projection
	*/
	typedef enum ePROJECTION_DIRECTION
	CASTOR_TYPE( uint8_t )
	{
		ePROJECTION_DIRECTION_FRONT,	//!< camera looks at the scene from the front
		ePROJECTION_DIRECTION_BACK,		//!< camera looks at the scene from the back
		ePROJECTION_DIRECTION_LEFT,		//!< camera looks at the scene from the left
		ePROJECTION_DIRECTION_RIGHT,	//!< camera looks at the scene from the right
		ePROJECTION_DIRECTION_TOP,		//!< camera looks at the scene from the top
		ePROJECTION_DIRECTION_BOTTOM,	//!< camera looks at the scene from the bottom
		ePROJECTION_DIRECTION_COUNT,
	}	ePROJECTION_DIRECTION;
	/*!
	\author		Sylvain DOREMUS
	\~english
	\brief		The  viewport projection types enumeration
	\~french
	\brief		Enumération des types de projection de viewport
	*/
	typedef enum eVIEWPORT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eVIEWPORT_TYPE_3D,		//!< 3 Dimensions projection type
		eVIEWPORT_TYPE_2D,		//!< 2 Dimensions projection type
		eVIEWPORT_TYPE_COUNT,
	}	eVIEWPORT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\date 		10/06/2013
	\version	0.7.0
	\~english
	\brief		Frustum view planes enumeration
	\~french
	\brief		Enumération des plans du frustum de vue
	*/
	typedef enum eFRUSTUM_PLANE
	CASTOR_TYPE( uint8_t )
	{
		eFRUSTUM_PLANE_NEAR,	//!<\~english Near plane	\~french Plan lointain
		eFRUSTUM_PLANE_FAR,		//!<\~english Far plane	\~french Plan proche
		eFRUSTUM_PLANE_LEFT,	//!<\~english Left plane	\~french Plan gauche
		eFRUSTUM_PLANE_RIGHT,	//!<\~english Right plane	\~french Plan droit
		eFRUSTUM_PLANE_TOP,		//!<\~english Top plane	\~french Plan haut
		eFRUSTUM_PLANE_BOTTOM,	//!<\~english Bottom plane	\~french Plan bas
		eFRUSTUM_PLANE_COUNT,	//!<\~english Planes count	\~french Compte des plans
	}	eFRUSTUM_PLANE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Frame Event Type enumeration
	\~french
	\brief		Enumération des types d'évènement de frame
	*/
	typedef enum eEVENT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eEVENT_TYPE_PRE_RENDER,		//!<\~english This kind of event happens before any render, device context is active (so be fast !!)	\~french Ce type d'évènement est traité avant le rendu, le contexte de rendu est actif (donc soyez rapide !!)
		eEVENT_TYPE_QUEUE_RENDER,	//!<\~english This kind of event happens after the render, before buffers' swap, device context is active (so be fast !!)	\~french Ce type d'évènement est traité après le rendu, avant l'échange des buffers, le contexte de rendu est actif (donc soyez rapide !!)
		eEVENT_TYPE_POST_RENDER,	//!<\~english This kind of event happens after the buffer' swap	\~french Ce type d'évènement est traité après l'échange des tampons
		eEVENT_TYPE_COUNT,
	}	eEVENT_TYPE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		29/11/2012
	\~english
	\brief		Tweakable options enumeration
	\~french
	\brief		Enumération des options réglables dans le contexte de dessin
	*/
	typedef enum eTWEAK
	CASTOR_TYPE( uint8_t )
	{
		eTWEAK_DEPTH_TEST,			//!<\~english Depth test	\~french Test de profondeur
		eTWEAK_STENCIL_TEST,		//!<\~english Stencil test	\~french Test de découpe
		eTWEAK_BLEND,				//!<\~english Colour and alpha blend	\~french Mélange des couleurs et de l'alpha
		eTWEAK_LIGHTING,			//!<\~english Lighting	\~french Eclairage
		eTWEAK_ALPHA_TEST,			//!<\~english Alpha testing	\~french Test d'alpha
		eTWEAK_CULLING,				//!<\~english Polygons culling	\~french Sélection des faces à dessiner
		eTWEAK_DITHERING,			//!<\~english Dithering	\~french Tramage
		eTWEAK_FOG,					//!<\~english Fog	\~french Brouillard
		eTWEAK_DEPTH_WRITE,			//!<\~english Depth write	\~french Ecriture dans le tampon de profondeur
		eTWEAK_ALPHA_TO_COVERAGE,	//!<\~english Alpha to coverage	\~french Alpha to coverage
		eTWEAK_COUNT,
	}	eTWEAK;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		23/05/2013
	\~english
	\brief		Faces orientations enumeration
	\~french
	\brief		Enumération des orientations des faces
	*/
	typedef enum eFACE
	CASTOR_TYPE( uint8_t )
	{
		eFACE_NONE,				//!<\~english No face	\~french Aucune face
		eFACE_FRONT,			//!<\~english Front face	\~french Face avant
		eFACE_BACK,				//!<\~english Back face	\~french Face arrière
		eFACE_FRONT_AND_BACK,	//!<\~english Back and front faces	\~french Faces avant et arrière
		eFACE_COUNT,			//!<\~english A count	\~french Un compte
	}   eFACE;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0.0
	\date		23/05/2013
	\~english
	\brief		Polygon rasterisation modes enumeration
	\~french
	\brief		Enumération des mode de rastérisation des polygones
	*/
	typedef enum eFILL_MODE
	CASTOR_TYPE( uint8_t )
	{
		eFILL_MODE_POINT,	//!<\~english Polygon vertices that are marked as the start of a boundary edge are drawn as points	\~french Les vertices marquant les arêtes sont dessinés en tant que points
		eFILL_MODE_LINE,	//!<\~english Boundary edges of the polygon are drawn as line segments	\~french Les arêtes du polygone sont dessinées en tant que segments
		eFILL_MODE_SOLID,	//!<\~english The interior of the polygon is filled	\~french L'intérieur du polygone est rempli
		eFILL_MODE_COUNT,	//!<\~english Fill modes count	\~french Compte des modes de rastérisation
	}   eFILL_MODE;

	class WindowHandle;
	class RenderTarget;
	class RenderBuffer;
	class ColourRenderBuffer;
	class DepthStencilRenderBuffer;
	class FrameBufferAttachment;
	class RenderBufferAttachment;
	class TextureAttachment;
	class FrameBuffer;
	class RenderTechniqueBase;
	class RenderWindow;		//!< The render window handler class
	class IWindowHandle;
	class DebugOverlays;
	class Engine;			//!< The Castor3D core system
	class RendererServer;	//!< Manages graphics related stuff
	class PluginBase;		//!< The base plugin handler class
	class RendererPlugin;	//!< The base plugin handler class
	class ImporterPlugin;	//!< The base plugin handler class
	class DividerPlugin;	//!< The base plugin handler class
	class GenericPlugin;		//!< The base plugin handler class
	class FrameEvent;		//!< The frame event representation
	class FrameListener;	//!< The frame event listener
	class Version;			//!< Castor3D Version
	class PostEffect;
	class Parameters;
	class BillboardList;

	DECLARE_SMART_PTR( RenderWindow );
	DECLARE_SMART_PTR( RenderBuffer );
	DECLARE_SMART_PTR( ColourRenderBuffer );
	DECLARE_SMART_PTR( DepthStencilRenderBuffer );
	DECLARE_SMART_PTR( FrameBufferAttachment );
	DECLARE_SMART_PTR( RenderBufferAttachment );
	DECLARE_SMART_PTR( TextureAttachment );
	DECLARE_SMART_PTR( FrameBuffer );
	DECLARE_SMART_PTR( RenderTechniqueBase );
	DECLARE_SMART_PTR( PostEffect );
	DECLARE_SMART_PTR( Engine );
	DECLARE_SMART_PTR( RendererServer );
	DECLARE_SMART_PTR( PluginBase );
	DECLARE_SMART_PTR( RendererPlugin );
	DECLARE_SMART_PTR( ImporterPlugin );
	DECLARE_SMART_PTR( DividerPlugin );
	DECLARE_SMART_PTR( GenericPlugin );
	DECLARE_SMART_PTR( FrameEvent );
	DECLARE_SMART_PTR( FrameListener );
	DECLARE_SMART_PTR( IWindowHandle );
	DECLARE_SMART_PTR( BillboardList );

	class TechniqueFactory;
	DECLARE_SMART_PTR( TechniqueFactory );

	//! real array
	DECLARE_VECTOR( real, Real );
	//! RenderWindow pointer array
	DECLARE_VECTOR( RenderWindowSPtr, RenderWindowPtr );
	//! RenderBuffer pointer array
	DECLARE_VECTOR( RenderBufferSPtr, RenderBufferPtr );
	//! FrameEvent pointer array
	DECLARE_VECTOR( FrameEventSPtr, FrameEventPtr );
	//! FrameListener pointer array
	DECLARE_VECTOR( FrameListenerSPtr, FrameListenerPtr );
	//! RenderWindow pointer map, sorted by index
	DECLARE_MAP( uint32_t, RenderWindowSPtr, RenderWindow );
	//! Plugin map, sorted by name
	DECLARE_MAP( Castor::String, PluginBaseSPtr, PluginStr );
	DECLARE_MAP( int, Castor::String, StrInt );
	DECLARE_VECTOR( PostEffectSPtr, PostEffectPtr );
	DECLARE_VECTOR( BillboardListSPtr, BillboardList );

	DECLARE_COLLECTION( Scene, Castor::String, Scene );
	DECLARE_COLLECTION( Animation, Castor::String, Animation );
	DECLARE_COLLECTION( Mesh, Castor::String, Mesh );
	DECLARE_COLLECTION( Overlay, Castor::String, Overlay );
	DECLARE_COLLECTION( Material, Castor::String, Material );
	DECLARE_COLLECTION( Castor::Image, Castor::String, Image );
	DECLARE_COLLECTION( Castor::Font, Castor::String, Font );

	DECLARE_SMART_PTR( SceneCollection );
	DECLARE_SMART_PTR( AnimationCollection );
	DECLARE_SMART_PTR( MeshCollection );
	DECLARE_SMART_PTR( OverlayCollection );
	DECLARE_SMART_PTR( MaterialCollection );
	DECLARE_SMART_PTR( ImageCollection );
	DECLARE_SMART_PTR( FontCollection );

	DECLARE_ARRAY( RendererPluginSPtr, eRENDERER_TYPE_COUNT, RendererPtr );
	DECLARE_ARRAY( PluginStrMap, ePLUGIN_TYPE_COUNT, PluginStrMap );
	DECLARE_MAP( Castor::Path, Castor::DynamicLibrarySPtr, DynamicLibraryPtrPath );
	DECLARE_ARRAY( DynamicLibraryPtrPathMap, ePLUGIN_TYPE_COUNT, DynamicLibraryPtrPathMap );
	DECLARE_MAP( Castor::Path, ePLUGIN_TYPE, PluginTypePath );
	DECLARE_MAP( Castor::String, BillboardListSPtr, BillboardListStr );

	typedef std::map< Castor::String, RenderWindowSPtr > WindowPtrStrMap;

	//@}
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif
