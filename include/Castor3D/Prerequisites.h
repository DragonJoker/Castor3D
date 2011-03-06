/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___C3D_Prerequisites___
#define ___C3D_Prerequisites___

//#define CASTOR_GTK

#undef RGB

#ifndef CASTOR_GTK
#  ifdef _WIN32
#       define CASTOR_MSW
#  else
#       define CASTOR_X11
#      endif
#endif

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <list>
#include <stdarg.h>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <stack>
#include <time.h>
#include <typeinfo>
#include <locale>

#ifndef WIN32
#	define sprintf_s snprintf
#	define Sleep usleep
#	define C3D_API
#else
// disable: "<type> needs to have dll-interface to be used by clients'
// Happens on STL member variables which are not public therefore is ok
#   pragma warning (disable : 4251)

// disable: "non dll-interface class used as base for dll-interface class"
// Happens when deriving from Singleton because bug in compiler ignores
// template export
#   pragma warning (disable : 4275)

#	ifdef Castor3D_EXPORTS
#		define C3D_API __declspec(dllexport)
#	else
#		define C3D_API __declspec(dllimport)
#	endif

#endif

#define CASTOR_VERSION_MAJOR		0
#define CASTOR_VERSION_MINOR		6

#include <CastorUtils/Module_Utils.h>
#include <CastorUtils/Value.h>

#include <CastorUtils/Angle.h>
#include <CastorUtils/Buffer.h>
#include <CastorUtils/CastorString.h>
#include <CastorUtils/Colour.h>
#include <CastorUtils/Container.h>
#include <CastorUtils/DynamicLibrary.h>
#include <CastorUtils/FastMath.h>
#include <CastorUtils/File.h>
#include <CastorUtils/Font.h>
#include <CastorUtils/Image.h>
#include <CastorUtils/Line.h>
#include <CastorUtils/List.h>
#include <CastorUtils/Manager.h>
#include <CastorUtils/Map.h>
#include <CastorUtils/Matrix.h>
#include <CastorUtils/Multimap.h>
#include <CastorUtils/Mutex.h>
#include <CastorUtils/NonCopyable.h>
#include <CastorUtils/Plane.h>
#include <CastorUtils/Point.h>
#include <CastorUtils/PreciseTimer.h>
#include <CastorUtils/Quaternion.h>
#include <CastorUtils/Resource.h>
#include <CastorUtils/ResourceLoader.h>
#include <CastorUtils/Serialisable.h>
#include <CastorUtils/Set.h>
#include <CastorUtils/SphericalVertex.h>
#include <CastorUtils/TransformationMatrix.h>
#include <CastorUtils/Thread.h>
#include <CastorUtils/Vector.h>

using namespace Castor;
using namespace Castor::Resources;
using namespace Castor::Utils;
using namespace Castor::Math;
using namespace Castor::Templates;

#if CHECK_MEMORYLEAKS
#	include <CastorUtils/Memory.h>
#endif

#include "Log.h"

#include <Cg/cg.h>

namespace Castor3D
{
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Animation
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class MovingObject;			//!< Class which represents the moving objects
	class KeyFrame;				//!< Key frames class
	class Animation;			//!< The class which manages key frames
	class AnimationManager;		//!< Animation manager class
	class AnimatedObject;		//!< Class which represents the animated objects
	class AnimatedObjectGroup;	//!< Class which represents the animated object groups

	typedef shared_ptr<	KeyFrame				>	KeyFramePtr;
	typedef shared_ptr<	AnimatedObject			>	AnimatedObjectPtr;
	typedef shared_ptr<	MovingObject			>	MovingObjectPtr;
	typedef shared_ptr<	Animation				>	AnimationPtr;
	typedef shared_ptr<	AnimatedObjectGroup		>	AnimatedObjectGroupPtr;

	typedef KeyedContainer<	String,	AnimatedObjectGroupPtr	>::Map	AnimatedObjectGroupPtrStrMap;	//!< AnimatedObjectGroup pointer map, sorted by name
	typedef KeyedContainer<	String,	AnimationPtr			>::Map	AnimationPtrStrMap;				//!< Animation pointer map, sorted by name
	typedef KeyedContainer<	real,	KeyFramePtr				>::Map	KeyFramePtrRealMap;				//!< KeyFrame pointer map, sorted by time index
	typedef KeyedContainer<	String,	AnimatedObjectPtr		>::Map	AnimatedObjectPtrStrMap;		//!< AnimatedObject pointer map, sorted by name
	typedef KeyedContainer<	String,	MovingObjectPtr			>::Map	MovingObjectPtrStrMap;			//!< MovingObject pointer map, sorted by name

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Camera
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class Camera;
	class Viewport;
	class Ray;

	typedef shared_ptr<	Camera		>	CameraPtr;
	typedef shared_ptr<	Viewport	>	ViewportPtr;
	typedef shared_ptr<	Ray			>	RayPtr;

	typedef Container<		CameraPtr				>::Vector	CameraPtrArray;		//!< Camera pointer array
	typedef KeyedContainer<	String,		CameraPtr	>::Map		CameraPtrStrMap;	//!< Camera pointer map, sorted by name

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Geometry
	////////////////////////////////////////////////////////////////////////////////////////////////////
	//! The mesh types enumerator
	/*!
	Actually, there are 8 mesh types defined : custom, cone, cylinder, sphere, cube, torus, plane and icosaedron
	*/
	typedef enum
	{
		eCustom,		//!< Custom mesh type => User defined vertex...
		eCone,			//!< Cone mesh type
		eCylinder,		//!< Cylinder mesh type
		eSphere,		//!< Rectangular faces sphere mesh type
		eCube,			//!< Cube mesh type
		eTorus,			//!< Torus mesh type
		ePlane,			//!< Plane mesh type
		eIcosaedron,	//!< Triangular faces sphere mesh type
		eProjection,	//!< Projection mesh type
	}
	eMESH_TYPE;

	template <typename T, size_t Count> class IdPoint;
	typedef IdPoint<	int,	4>	IdPoint4i;
	typedef IdPoint<	int,	3>	IdPoint3i;
	typedef IdPoint<	int,	2>	IdPoint2i;
	typedef IdPoint<	real,	4>	IdPoint4r;
	typedef IdPoint<	real,	3>	IdPoint3r;
	typedef IdPoint<	real,	2>	IdPoint2r;
	typedef IdPoint<	float,	4>	IdPoint4f;
	typedef IdPoint<	float,	3>	IdPoint3f;
	typedef IdPoint<	float,	2>	IdPoint2f;
	typedef IdPoint<	double,	4>	IdPoint4d;
	typedef IdPoint<	double,	3>	IdPoint3d;
	typedef IdPoint<	double,	2>	IdPoint2d;
	typedef shared_ptr<	IdPoint3r	>	IdPoint3rPtr;

	class Vertex;
	typedef shared_ptr<Vertex>		VertexPtr;

	template <typename T> class Pattern;
	typedef Pattern<	Vertex			>	VertexPattern;
	typedef Pattern<	Point3r			>	Point3rPattern;
	typedef Pattern<	IdPoint3r		>	IdPoint3rPattern;
	typedef Pattern<	VertexPtr		>	VertexPtrPattern;
	typedef Pattern<	Point3rPtr		>	Point3rPtrPattern;
	typedef Pattern<	IdPoint3rPtr	>	IdPoint3rPtrPattern;

	struct FaceGroup;
	class Face;
	class Graph;
	class SmoothingGroup;
	class MeshManager;
	class Mesh;
	class Submesh;
	class Cone;
	class Cylinder;
	class Icosaedron;
	class Cube;
	class Plane;
	class Sphere;
	class Torus;
	class Geometry;
	class MovableObject;
	class Subdivider;
	class PnTrianglesDivider;

	typedef shared_ptr<	VertexPattern			>	VertexPatternPtr;
	typedef shared_ptr<	Point3rPattern			>	Point3rPatternPtr;
	typedef shared_ptr<	IdPoint3rPattern		>	IdPoint3rPatternPtr;
	typedef shared_ptr<	VertexPtrPattern		>	VertexPtrPatternPtr;
	typedef shared_ptr<	Point3rPtrPattern		>	Point3rPtrPatternPtr;
	typedef shared_ptr<	IdPoint3rPtrPattern		>	IdPoint3rPtrPatternPtr;
	typedef shared_ptr<	Face					>	FacePtr;
	typedef shared_ptr<	Graph					>	GraphPtr;
	typedef shared_ptr<	SmoothingGroup			>	SmoothingGroupPtr;
	typedef shared_ptr<	Mesh					>	MeshPtr;
	typedef shared_ptr<	Submesh					>	SubmeshPtr;
	typedef shared_ptr<	Cone					>	ConePtr;
	typedef shared_ptr<	Cylinder				>	CylinderPtr;
	typedef shared_ptr<	Icosaedron				>	IcosaedronPtr;
	typedef shared_ptr<	Cube					>	CubePtr;
	typedef shared_ptr<	Plane					>	PlanePtr;
	typedef shared_ptr<	Sphere					>	SpherePtr;
	typedef shared_ptr<	Torus					>	TorusPtr;
	typedef shared_ptr<	Geometry				>	GeometryPtr;
	typedef shared_ptr<	MovableObject			>	MovableObjectPtr;
	typedef shared_ptr<	Subdivider				>	SubdividerPtr;
	typedef shared_ptr<	PnTrianglesDivider		>	PnTrianglesDividerPtr;
	typedef shared_ptr<	FaceGroup				>	FaceGroupPtr;

	//! The normals mode enumerator
	/*!
	Actually, 2 Normals mode are implemented : Face and Smoothgroups
	*/
	typedef enum
	{
		eFace		= 0,	//!< Face normals mode sets to each face vertex the same face normal
		eSmooth		= 1		//!< Smoothgroup normals mode sets to each face vertex a normal computed from it's position relative to his neighbour's one
	}
	eNORMALS_MODE;

	typedef Container<		IdPoint3r								>::Vector	IdPoint3rArray;			//! IdPoint3r array
	typedef Container<		IdPoint3rPtr							>::Vector	IdPoint3rPtrArray;		//! IdPoint3r pointer array
	typedef Container<		Vertex									>::Vector	VertexArray;			//!< Vertex array
	typedef Container<		VertexPtr								>::Vector	VertexPtrArray;			//!< Vertex pointer array
	typedef Container<		IdPoint3r								>::Vector	IdPoint3rArray;			//!< IdPoint3r array
	typedef Container<		Face									>::Vector	FaceArray;				//!< Face array
	typedef Container<		FacePtr									>::Vector	FacePtrArray;			//!< Face pointer array
	typedef Container<		FaceGroupPtr							>::Vector	FaceGroupPtrArray;		//!< FaceGroup pointer array
	typedef Container<		SmoothingGroupPtr						>::Vector	SmoothGroupPtrArray;	//!< Smoothing group pointer array
	typedef Container<		SmoothingGroup							>::Vector	SmoothGroupArray;		//!< Smoothing group array
	typedef Container<		SubmeshPtr								>::Vector	SubmeshPtrArray;		//!< Submesh pointer array
	typedef Container<		GeometryPtr								>::Vector	GeometryPtrArray;		//!< Geometry pointer array
	typedef Container<		VertexPtr								>::List		VertexPtrList;			//!< Vertex pointer list
	typedef KeyedContainer<	String,				MeshPtr				>::Map		MeshPtrStrMap;			//!< Mesh pointer array
	typedef KeyedContainer<	String,				MovableObjectPtr	>::Map		MovableObjectPtrStrMap;	//!< MovableObject pointer map, sorted by name
	typedef KeyedContainer<	String,				GeometryPtr			>::Map		GeometryPtrStrMap;		//!< Geometry pointer map, sorted by name
	typedef KeyedContainer<	size_t,				SmoothingGroup		>::Map		SmoothGroupUIntMap;		//!< Smoothing group int map
	typedef KeyedContainer<	size_t,				SmoothingGroupPtr	>::Map		SmoothGroupPtrUIntMap;	//!< Smoothing group pointer int map
	typedef KeyedContainer<	String,				SmoothingGroup		>::Map		SmoothGroupStrMap;		//!< Smoothing group string map
	typedef KeyedContainer<	String,				SmoothingGroupPtr	>::Map		SmoothGroupPtrStrMap;	//!< Smoothing group pointer string map
	typedef KeyedContainer<	size_t,				IdPoint3rPtr		>::Map		VertexPtrUIntMap;
	typedef KeyedContainer<	size_t,				int					>::Map		IntUIntMap;
	typedef KeyedContainer<	String,				SubmeshPtr			>::Map		SubmeshPtrStrMap;
	typedef KeyedContainer<	String,				int					>::Map		IntStrMap;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Importer
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class Importer;

	typedef shared_ptr<Importer>	ImporterPtr;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Light
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class Light;
	class DirectionalLight;
	class PointLight;
	class SpotLight;

	typedef shared_ptr<	Light				>	LightPtr;
	typedef shared_ptr<	DirectionalLight	>	DirectionalLightPtr;
	typedef shared_ptr<	PointLight			>	PointLightPtr;
	typedef shared_ptr<	SpotLight			>	SpotLightPtr;

	typedef Container<		LightPtr				>::Vector	LightPtrArray;	//!< Array of lights
	typedef KeyedContainer<	String,		LightPtr	>::Map		LightPtrStrMap;	//!< Map of lights, sorted by name

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Material
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class TextureEnvironment;	//!< The texture environment class
	class TextureUnit;			//!< The texture unit class
	class Material;				//!< The material class
	class MaterialManager;		//!< The Material manager class
	class Pass;					//!< The material pass class (for multipass materials)

	//! Light indexes enumerator
	/*!
	The height available light indexes
	*/
	typedef enum
	{
		LILight0 = 0,
		LILight1 = 1,
		LILight2 = 2,
		LILight3 = 3,
		LILight4 = 4,
		LILight5 = 5,
		LILight6 = 6,
		LILight7 = 7
	}
	eLIGHT_INDEXES;

	typedef shared_ptr<	TextureEnvironment	>	TextureEnvironmentPtr;
	typedef shared_ptr<	TextureUnit			>	TextureUnitPtr;
	typedef shared_ptr<	MaterialManager		>	MaterialManagerPtr;
	typedef shared_ptr<	Material			>	MaterialPtr;
	typedef shared_ptr<	Pass				>	PassPtr;

	typedef Container<		MaterialPtr					>::Vector	MaterialPtrArray;		//!< Material pointer array
	typedef Container<		TextureUnit					>::Vector	TextureUnitArray;		//!< TextureUnit array
	typedef Container<		TextureUnitPtr				>::Vector	TextureUnitPtrArray;	//!< TextureUnit pointer array
	typedef Container<		Pass						>::Vector	PassArray;				//!< Pass array
	typedef Container<		PassPtr						>::Vector	PassPtrArray;			//!< Pass pointer array
	typedef Container<		unsigned int				>::Vector	UIntArray;				//!< unsigned int array
	typedef KeyedContainer<	String,			MaterialPtr	>::Map		MaterialPtrStrMap;		//!< Material pointer map, sorted by name
	typedef KeyedContainer<	size_t,			MaterialPtr	>::Map		MaterialPtrUIntMap;		//!< Material pointer map

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Overlay
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class Overlay;
	class PanelOverlay;
	class BorderPanelOverlay;
	class TextOverlay;
	class OverlayManager;

	typedef shared_ptr<	Overlay				>	OverlayPtr;
	typedef shared_ptr<	PanelOverlay		>	PanelOverlayPtr;
	typedef shared_ptr<	BorderPanelOverlay	>	BorderPanelOverlayPtr;
	typedef shared_ptr<	TextOverlay			>	TextOverlayPtr;

	typedef KeyedContainer< int,	OverlayPtr	>::Map	OverlayPtrIntMap;
	typedef KeyedContainer< String,	OverlayPtr	>::Map	OverlayPtrStrMap;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Render
	////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class T, class Y> class Renderer;
	template <class T, class Y> class Renderable;
	template <typename T> class Buffer3D;
	class RendererBase;
	class RenderSystem;
	class SubmeshRenderer;
	class TextureEnvironmentRenderer;
	class TextureRenderer;
	class PassRenderer;
	class LightRenderer;
	class WindowRenderer;
	class CameraRenderer;
	class OverlayRenderer;
	class Context;
	class RenderWindow;

	template <typename T> class VertexAttribsBuffer;
	struct BufferElementDeclaration;
	class BufferDeclaration;
	class VertexBuffer;
	class IndexBuffer;
	class TextureBufferObject;

	typedef VertexAttribsBuffer<	bool			>	VertexAttribsBufferBool;
	typedef VertexAttribsBuffer<	int				>	VertexAttribsBufferInt;
	typedef VertexAttribsBuffer<	unsigned int	>	VertexAttribsBufferUInt;
	typedef VertexAttribsBuffer<	float			>	VertexAttribsBufferFloat;
	typedef VertexAttribsBuffer<	double			>	VertexAttribsBufferDouble;

	typedef shared_ptr<	RendererBase				>	RendererPtr;
	typedef shared_ptr<	SubmeshRenderer				>	SubmeshRendererPtr;
	typedef shared_ptr<	TextureEnvironmentRenderer	>	TextureEnvironmentRendererPtr;
	typedef shared_ptr<	TextureRenderer				>	TextureRendererPtr;
	typedef shared_ptr<	PassRenderer				>	PassRendererPtr;
	typedef shared_ptr<	LightRenderer				>	LightRendererPtr;
	typedef shared_ptr<	WindowRenderer				>	WindowRendererPtr;
	typedef shared_ptr<	CameraRenderer				>	CameraRendererPtr;
	typedef shared_ptr<	OverlayRenderer				>	OverlayRendererPtr;
	typedef shared_ptr<	BufferElementDeclaration	>	BufferElementPtr;
	typedef shared_ptr<	BufferDeclaration			>	BufferDeclarationPtr;
	typedef shared_ptr<	VertexBuffer				>	VertexBufferPtr;
	typedef shared_ptr<	IndexBuffer					>	IndexBufferPtr;
	typedef shared_ptr<	VertexAttribsBufferBool		>	VertexAttribsBufferBoolPtr;
	typedef shared_ptr<	VertexAttribsBufferInt		>	VertexAttribsBufferIntPtr;
	typedef shared_ptr<	VertexAttribsBufferUInt		>	VertexAttribsBufferUIntPtr;
	typedef shared_ptr<	VertexAttribsBufferFloat	>	VertexAttribsBufferFloatPtr;
	typedef shared_ptr<	VertexAttribsBufferDouble	>	VertexAttribsBufferDoublePtr;
	typedef shared_ptr<	TextureBufferObject			>	TextureBufferObjectPtr;

	typedef Container< 		SubmeshRendererPtr								>::Vector	SubmeshRendererPtrArray;
	typedef Container< 		TextureEnvironmentRendererPtr					>::Vector	TextureEnvironmentRendererPtrArray;
	typedef Container< 		TextureRendererPtr								>::Vector	TextureRendererPtrArray;
	typedef Container< 		PassRendererPtr									>::Vector	PassRendererPtrArray;
	typedef Container< 		LightRendererPtr								>::Vector	LightRendererPtrArray;
	typedef Container< 		WindowRendererPtr								>::Vector	WindowRendererPtrArray;
	typedef Container< 		CameraRendererPtr								>::Vector	CameraRendererPtrArray;
	typedef Container<		OverlayRendererPtr								>::Vector	OverlayRendererPtrArray;
	typedef KeyedContainer<	RenderWindow *,					Context *		>::Map		ContextPtrMap;
	/*!
	Enumerates the different draw types : Triangles, Lines, Points...
	*/
	typedef enum
	{
		eTriangles		= 0,	//!< Triangles : Show textured triangles
		eLines			= 1,	//!< Lines : Show textured edges
		ePoints			= 2,	//!< Lines : Show textured points
		eTriangleStrips	= 3,
		eTriangleFan	= 4,
		eQuads			= 5,
		eQuadStrip		= 6,
		eNbDrawTypes,
	}
	ePRIMITIVE_TYPE;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Scene
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class Scene;
	class SceneManager;
	class SceneNode;
	class SceneFileContext;
	class SceneFileParser;

	typedef shared_ptr<	SceneNode			>	SceneNodePtr;
	typedef shared_ptr<	Scene				>	ScenePtr;
	typedef shared_ptr<	SceneFileContext	>	SceneFileContextPtr;
	typedef shared_ptr<	SceneFileParser		>	SceneFileParserPtr;

	typedef Container<		SceneNodePtr					>::Vector	SceneNodePtrArray;		//!< SceneNode pointer array
	typedef KeyedContainer<	String,			ScenePtr		>::Map		ScenePtrStrMap;			//!< Scene pointer map, sorted by name
	typedef KeyedContainer<	String,			SceneNodePtr	>::Map		SceneNodePtrStrMap;		//!< SceneNode pointer map, sorted by name

	typedef bool ParserFunction( String & p_strParams, SceneFileContextPtr p_pContext);
	typedef ParserFunction * PParserFunction;
	typedef KeyedContainer<	String,	PParserFunction>::Map AttributeParserMap;

	#define DECLARE_ATTRIBUTE_PARSER( X) bool C3D_API X( String & p_strParams, SceneFileContextPtr p_pContext);
	#define IMPLEMENT_ATTRIBUTE_PARSER( X) bool Castor3D :: X( String & p_strParams, SceneFileContextPtr p_pContext)
	#define PARSING_ERROR( p_strError) SceneFileParser::ParseError( p_strError, p_pContext)
	#define PARSING_WARNING( p_strWarning) SceneFileParser::ParseWarning( p_strWarning, p_pContext)
	#define PARSE_V2( T, V) SceneFileParser::ParseVector<T, 2>( p_strParams, V, p_pContext)
	#define PARSE_V3( T, V) SceneFileParser::ParseVector<T, 3>( p_strParams, V, p_pContext)
	#define PARSE_V4( T, V) SceneFileParser::ParseVector<T, 4>( p_strParams, V, p_pContext)
	#define PARSE_PT2( T, line, V) SceneFileParser::ParseVector<T, 2>( line, V)
	#define PARSE_PT3( T, line, V) SceneFileParser::ParseVector<T, 3>( line, V)
	#define PARSE_PT4( T, line, V) SceneFileParser::ParseVector<T, 4>( line, V)

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Shader
	////////////////////////////////////////////////////////////////////////////////////////////////////
	template <class ShObj> class ShaderProgram;
	template <class ShObj> class ShaderObject;
	class ShaderManager;
	class ShaderProgramBase;
	class ShaderObjectBase;
	class GlslShaderObject;
	class GlslShaderProgram;
	class HlslShaderObject;
	class HlslShaderProgram;
	class CgShaderObject;
	class CgShaderProgram;
	class FrameVariable;
	class CgFrameVariable;
	template <	typename T									> class OneFrameVariable;
	template <	typename T									> class CgOneFrameVariable;
	template <	typename T, size_t Count					> class PointFrameVariable;
	template <	typename T, size_t Count					> class CgPointFrameVariable;
	template <	typename T,	size_t Rows,	size_t Columns	> class MatrixFrameVariable;
	template <	typename T,	size_t Rows,	size_t Columns	> class CgMatrixFrameVariable;

	typedef shared_ptr<	FrameVariable		>	FrameVariablePtr;
	typedef shared_ptr<	ShaderObjectBase	>	ShaderObjectPtr;
	typedef shared_ptr<	ShaderProgramBase	>	ShaderProgramPtr;
	typedef shared_ptr<	CgFrameVariable		>	CgFrameVariablePtr;
	typedef shared_ptr<	CgShaderProgram		>	CgShaderProgramPtr;
	typedef shared_ptr<	CgShaderObject		>	CgShaderObjectPtr;
	typedef shared_ptr<	GlslShaderProgram	>	GlslShaderProgramPtr;
	typedef shared_ptr<	GlslShaderObject	>	GlslShaderObjectPtr;
	typedef shared_ptr<	HlslShaderProgram	>	HlslShaderProgramPtr;
	typedef shared_ptr<	HlslShaderObject	>	HlslShaderObjectPtr;

	typedef Container<		ShaderProgramPtr						>::Vector	ShaderProgramPtrArray;
	typedef Container<		ShaderObjectPtr							>::Vector	ShaderObjectPtrArray;
	typedef KeyedContainer<	String,				FrameVariablePtr	>::Map		FrameVariablePtrStrMap;
	typedef Container<		CgShaderProgramPtr						>::Vector	CgShaderProgramPtrArray;
	typedef Container<		CgShaderObjectPtr						>::Vector	CgShaderObjectPtrArray;
	typedef KeyedContainer<	String,				CgFrameVariablePtr	>::Map		CgFrameVariablePtrStrMap;
	typedef Container<		GlslShaderProgramPtr					>::Vector	GlslShaderProgramPtrArray;
	typedef Container<		GlslShaderObjectPtr						>::Vector	GlslShaderObjectPtrArray;
	typedef Container<		HlslShaderProgramPtr					>::Vector	HlslShaderProgramPtrArray;
	typedef Container<		HlslShaderObjectPtr						>::Vector	HlslShaderObjectPtrArray;

	//! Shader object type
	/*!
	The different shader object types : vertex, fragment, geometry
	*/
	typedef enum
	{
		eShaderNone		= -1,
		eVertexShader	= 0,
		ePixelShader	= 1,
		eGeometryShader	= 2,
		eNbShaderTypes	= 3
	}
	eSHADER_PROGRAM_TYPE;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Main
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class RenderWindow;		//!< The render window handler class
	class Root;				//!< The Castor3D core system
	class RendererDriver;	//!< Engine graphics driver
	class RendererServer;	//!< Manages graphics related stuff
	class PluginBase;		//!< The base plugin handler class
	class RendererPlugin;	//!< The base plugin handler class
	class ImporterPlugin;	//!< The base plugin handler class
	class DividerPlugin;	//!< The base plugin handler class
	class FrameEvent;		//!< The frame event representation
	class FrameListener;	//!< The frame event listener
	class Version;			//!< Castor3D Version

	//! Projection Directions enumerator
	/*!
	The different 2 dimensions projection directions : front, back, left, right, top and bottom
	*/
	typedef enum
	{
		pdLookToFront,		//!< camera looks at the scene from the front
		pdLookToBack,		//!< camera looks at the scene from the back
		pdLookToLeft,		//!< camera looks at the scene from the left
		pdLookToRight,		//!< camera looks at the scene from the right
		pdLookToTop,		//!< camera looks at the scene from the top
		pdLookToBottom		//!< camera looks at the scene from the bottom
	}
	ePROJECTION_DIRECTION;

	typedef shared_ptr<	RenderWindow	>	RenderWindowPtr;
	typedef shared_ptr<	Root			>	RootPtr;
	typedef shared_ptr<	RendererDriver	>	RendererDriverPtr;
	typedef shared_ptr<	RendererServer	>	RendererServerPtr;
	typedef shared_ptr<	PluginBase		>	PluginPtr;
	typedef shared_ptr<	RendererPlugin	>	RendererPluginPtr;
	typedef shared_ptr<	ImporterPlugin	>	ImporterPluginPtr;
	typedef shared_ptr<	DividerPlugin	>	DividerPluginPtr;
	typedef shared_ptr<	FrameEvent		>	FrameEventPtr;
	typedef shared_ptr<	FrameListener	>	FrameListenerPtr;

	typedef Container<		RenderWindowPtr							>::Vector	RenderWindowArray;			//!< RenderWindow pointer array
	typedef Container<		unsigned int							>::Vector	UIntArray;					//!< unsigned int array
	typedef Container<		real									>::Vector	FloatArray;					//!< real array
	typedef Container<		FrameEventPtr							>::Vector	FrameEventArray;			//!< FrameEvent pointer array
	typedef Container<		FrameListenerPtr						>::Vector	FrameListenerArray;			//!< FrameListener pointer array
	typedef KeyedContainer<	int,				RendererDriverPtr	>::Map		RendererDriverPtrIntMap;	//!< RendererDriver pointer map ordered by renderer type
	typedef KeyedContainer<	size_t,				RenderWindowPtr		>::Map		RenderWindowMap;			//!< RenderWindow pointer map, sorted by index
	typedef KeyedContainer<	String,				PluginPtr			>::Map		PluginStrMap;				//!< Plugin map, sorted by name
	typedef KeyedContainer<	int,				String				>::Map		StrIntMap;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Text
	////////////////////////////////////////////////////////////////////////////////////////////////////
	class TextFont;

	typedef shared_ptr<	TextFont	>	TextFontPtr;
}

#endif
