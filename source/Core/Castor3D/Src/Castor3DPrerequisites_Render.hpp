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
#ifndef ___C3D_PREREQUISITES_RENDER_H___
#define ___C3D_PREREQUISITES_RENDER_H___

namespace Castor3D
{
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
		: uint8_t
	{
		//<\~english Eye to Clip transform	\~french Transformations de Vue vers Clip
		eMTXMODE_PROJECTION,
		//!\~english Object to World transform	\~french Transformations de Objet vers Monde
		eMTXMODE_MODEL,
		//!\~english World to Eye transform	\~french Transformations de Monde vers Vue
		eMTXMODE_VIEW,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE0,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE1,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE2,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE3,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE4,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE5,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE6,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE7,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE8,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE9,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE10,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE11,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE12,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE13,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE14,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE15,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE16,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE17,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE18,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE19,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE20,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE21,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE22,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE23,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE24,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE25,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE26,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE27,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE28,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE29,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE30,
		//!\~english Texture transforms	\~french Transformations de texture
		eMTXMODE_TEXTURE31,
		CASTOR_ENUM_BOUNDS( eMTXMODE, eMTXMODE_PROJECTION )
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
	\version	0.8.0
	\date		16/10/2015
	\~english
	\brief		Access types enumeration.
	\~french
	\brief		Enumération des types d'accès.
	*/
	typedef enum eACCESS_TYPE
		: uint8_t
	{
		//! Read access type.
		eACCESS_TYPE_READ = 1 << 0,
		//! Write access type.
		eACCESS_TYPE_WRITE = 1 << 1,
	}	eACCESS_TYPE;
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
		: uint32_t
	{
		//! Modified once and used many times
		eBUFFER_ACCESS_TYPE_STATIC = 1 << 0,
		//! Modified many times and used many times
		eBUFFER_ACCESS_TYPE_DYNAMIC = 1 << 1,
		//! Modified oncce and used at most a few times
		eBUFFER_ACCESS_TYPE_STREAM = 1 << 2,
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
		: uint32_t
	{
		//! Modified by CPU and used by GPU
		eBUFFER_ACCESS_NATURE_DRAW = 1 << 4,
		//! Modified by GPU and used by CPU
		eBUFFER_ACCESS_NATURE_READ = 1 << 5,
		//! Modified by GPU and used by GPU
		eBUFFER_ACCESS_NATURE_COPY = 1 << 6,
	}	eBUFFER_ACCESS_NATURE;
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Primitive draw types enumeration
	\~french
	\brief		Enumération des type de primitive dessinées
	*/
	typedef enum eTOPOLOGY
		: uint8_t
	{
		//! Display textured points
		eTOPOLOGY_POINTS,
		//! Display textured edges
		eTOPOLOGY_LINES,
		//! Display textured edge loops
		eTOPOLOGY_LINE_LOOP,
		//! Display textured edge strips
		eTOPOLOGY_LINE_STRIP,
		//! Display textured triangles
		eTOPOLOGY_TRIANGLES,
		//! Display triangle strips
		eTOPOLOGY_TRIANGLE_STRIPS,
		//! Display triangle fan
		eTOPOLOGY_TRIANGLE_FAN,
		//! Display quads
		eTOPOLOGY_QUADS,
		//! Display quad strips
		eTOPOLOGY_QUAD_STRIPS,
		//! Display quad strips
		eTOPOLOGY_POLYGON,
		CASTOR_ENUM_BOUNDS( eTOPOLOGY, eTOPOLOGY_POINTS )
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
		: uint8_t
	{
		//! Never passes.
		eDEPTH_FUNC_NEVER,
		//! Passes if the incoming depth value is less than the stored depth value.
		eDEPTH_FUNC_LESS,
		//! Passes if the incoming depth value is equal to the stored depth value.
		eDEPTH_FUNC_EQUAL,
		//! Passes if the incoming depth value is less than or equal to the stored depth value.
		eDEPTH_FUNC_LEQUAL,
		//! Passes if the incoming depth value is greater than the stored depth value.
		eDEPTH_FUNC_GREATER,
		//! Passes if the incoming depth value is not equal to the stored depth value.
		eDEPTH_FUNC_NOTEQUAL,
		//! Passes if the incoming depth value is greater than or equal to the stored depth value.
		eDEPTH_FUNC_GEQUAL,
		//! Always passes.
		eDEPTH_FUNC_ALWAYS,
		CASTOR_ENUM_BOUNDS( eDEPTH_FUNC, eDEPTH_FUNC_NEVER )
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
		: uint8_t
	{
		//! Writing disabled
		eWRITING_MASK_ZERO,
		//! Writing enabled
		eWRITING_MASK_ALL,
		CASTOR_ENUM_BOUNDS( eWRITING_MASK, eWRITING_MASK_ZERO )
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
		: uint8_t
	{
		//! Never passes.
		eSTENCIL_FUNC_NEVER,
		//! Passes if the incoming stencil value is less than the stored stencil value.
		eSTENCIL_FUNC_LESS,
		//! Passes if the incoming stencil value is equal to the stored stencil value.
		eSTENCIL_FUNC_EQUAL,
		//! Passes if the incoming stencil value is less than or equal to the stored stencil value.
		eSTENCIL_FUNC_LEQUAL,
		//! Passes if the incoming stencil value is greater than the stored stencil value.
		eSTENCIL_FUNC_GREATER,
		//! Passes if the incoming stencil value is not equal to the stored stencil value.
		eSTENCIL_FUNC_NOTEQUAL,
		//! Passes if the incoming stencil value is greater than or equal to the stored stencil value.
		eSTENCIL_FUNC_GEQUAL,
		//! Always passes.
		eSTENCIL_FUNC_ALWAYS,
		CASTOR_ENUM_BOUNDS( eSTENCIL_FUNC, eSTENCIL_FUNC_NEVER )
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
		: uint8_t
	{
		//! Keeps the current value.
		eSTENCIL_OP_KEEP,
		//! Sets the stencil buffer value to 0.
		eSTENCIL_OP_ZERO,
		//! Sets the stencil buffer value by the one given
		eSTENCIL_OP_REPLACE,
		//! Increments the current stencil buffer value. Clamps to the maximum representable unsigned value.
		eSTENCIL_OP_INCR,
		//! Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value.
		eSTENCIL_OP_INCR_WRAP,
		//! Decrements the current stencil buffer value. Clamps to 0.
		eSTENCIL_OP_DECR,
		//! Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero.
		eSTENCIL_OP_DECR_WRAP,
		//! Bitwise inverts the current stencil buffer value.
		eSTENCIL_OP_INVERT,
		CASTOR_ENUM_BOUNDS( eSTENCIL_OP, eSTENCIL_OP_KEEP )
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
		: uint32_t
	{
		//! Position coords
		eELEMENT_USAGE_UNKNOWN = 0x000,
		//! Position coords
		eELEMENT_USAGE_POSITION = 0x001,
		//! Normal coords
		eELEMENT_USAGE_NORMAL = 0x002,
		//! Tangent coords
		eELEMENT_USAGE_TANGENT = 0x004,
		//! Bitangent coords
		eELEMENT_USAGE_BITANGENT = 0x008,
		//! Diffuse colour
		eELEMENT_USAGE_DIFFUSE = 0x010,
		//! Texture coordinates
		eELEMENT_USAGE_TEXCOORDS = 0x020,
		//! Bone IDs 0
		eELEMENT_USAGE_BONE_IDS0 = 0x040,
		//! Bone IDs 1
		eELEMENT_USAGE_BONE_IDS1 = 0x080,
		//! Bone weights 0
		eELEMENT_USAGE_BONE_WEIGHTS0 = 0x100,
		//! Bone weights 1
		eELEMENT_USAGE_BONE_WEIGHTS1 = 0x200,
		//! Instantiation matrix
		eELEMENT_USAGE_TRANSFORM = 0x400,
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
		: uint8_t
	{
		//! 1 float (GLSL float)
		eELEMENT_TYPE_1FLOAT,
		//! 2 floats (GLSL vec2)
		eELEMENT_TYPE_2FLOATS,
		//! 3 floats (GLSL vec3)
		eELEMENT_TYPE_3FLOATS,
		//! 4 floats (GLSL vec4)
		eELEMENT_TYPE_4FLOATS,
		//! colour (uint32_t)
		eELEMENT_TYPE_COLOUR,
		//! 1 int (4 bytes each, GLSL int)
		eELEMENT_TYPE_1INT,
		//! 2 ints (4 bytes each, GLSL ivec2)
		eELEMENT_TYPE_2INTS,
		//! 3 ints (4 bytes each, GLSL ivec3)
		eELEMENT_TYPE_3INTS,
		//! 4 ints (4 bytes each, GLSL ivec4)
		eELEMENT_TYPE_4INTS,
		//! 2x2 floats (GLSL mat2)
		eELEMENT_TYPE_2x2FLOATS,
		//! 3x3 floats (GLSL mat3)
		eELEMENT_TYPE_3x3FLOATS,
		//! 4x4 floats (GLSL mat4)
		eELEMENT_TYPE_4x4FLOATS,
		CASTOR_ENUM_BOUNDS( eELEMENT_TYPE, eELEMENT_TYPE_1FLOAT )
	}	eELEMENT_TYPE;

	inline uint32_t GetSize( eELEMENT_TYPE p_type )
	{
		switch ( p_type )
		{
		case eELEMENT_TYPE_1FLOAT:
			return uint32_t( 1u * sizeof( real ) );
			break;

		case eELEMENT_TYPE_2FLOATS:
			return uint32_t( 2u * sizeof( real ) );
			break;

		case eELEMENT_TYPE_3FLOATS:
			return uint32_t( 3u * sizeof( real ) );
			break;

		case eELEMENT_TYPE_4FLOATS:
			return uint32_t( 4u * sizeof( real ) );
			break;

		case eELEMENT_TYPE_COLOUR:
			return uint32_t( sizeof( uint32_t ) );
			break;

		case eELEMENT_TYPE_1INT:
			return uint32_t( 1u * sizeof( int32_t ) );
			break;

		case eELEMENT_TYPE_2INTS:
			return uint32_t( 2u * sizeof( int32_t ) );
			break;

		case eELEMENT_TYPE_3INTS:
			return uint32_t( 3u * sizeof( int32_t ) );
			break;

		case eELEMENT_TYPE_4INTS:
			return uint32_t( 4u * sizeof( int32_t ) );
			break;

		case eELEMENT_TYPE_2x2FLOATS:
			return uint32_t( 2u * 2u * sizeof( real ) );
			break;

		case eELEMENT_TYPE_3x3FLOATS:
			return uint32_t( 3u * 3u * sizeof( real ) );
			break;

		case eELEMENT_TYPE_4x4FLOATS:
			return uint32_t( 4u * 4u * sizeof( real ) );
			break;

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			break;
		}

		return 0;
	}

	template< typename T > class CpuBuffer;
	class GpuInformations;
	class RenderSystem;
	class RenderTechnique;
	class Context;
	class RenderWindow;
	class IPipelineImpl;
	class Pipeline;
	class DepthStencilState;
	class RasteriserState;
	class BlendState;
	class ToneMapping;
	class ToneMappingFactory;
	struct RenderNode;
	struct SceneRenderNode;
	struct StaticGeometryRenderNode;
	struct AnimatedGeometryRenderNode;
	struct BillboardRenderNode;

	struct BufferElementDeclaration;
	class BufferDeclaration;
	template< typename T > class GpuBuffer;
	class VertexBuffer;
	class IndexBuffer;
	class GeometryBuffers;
	class TextureBuffer;

	DECLARE_SMART_PTR( GeometryBuffers );
	DECLARE_SMART_PTR( BufferElementDeclaration );
	DECLARE_SMART_PTR( BufferDeclaration );
	DECLARE_SMART_PTR( VertexBuffer );
	DECLARE_SMART_PTR( IndexBuffer );
	DECLARE_SMART_PTR( TextureBuffer );
	DECLARE_SMART_PTR( Context );
	DECLARE_SMART_PTR( DepthStencilState );
	DECLARE_SMART_PTR( RasteriserState );
	DECLARE_SMART_PTR( BlendState );
	DECLARE_SMART_PTR( IPipelineImpl );
	DECLARE_SMART_PTR( ToneMapping );

	DECLARE_MAP( RenderWindow *, ContextSPtr, ContextPtr );
	DECLARE_MAP( std::thread::id, ContextPtrMap, ContextPtrMapId );
	//!\~english Multimap of RenderNodes sorted by distance.	\~french Multimap de RenderNodes triés par distance.
	DECLARE_MULTIMAP( double, StaticGeometryRenderNode, StaticGeometryRenderNodeByDistance );
	DECLARE_MULTIMAP( double, AnimatedGeometryRenderNode, AnimatedGeometryRenderNodeByDistance );
	DECLARE_MULTIMAP( double, BillboardRenderNode, BillboardRenderNodeByDistance );

	//@}
}

#endif
