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
	\version	0.8.0
	\date		16/10/2015
	\~english
	\brief		Access types enumeration.
	\~french
	\brief		Enumération des types d'accès.
	*/
	typedef enum eACCESS_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eACCESS_TYPE_READ = 1 << 0,		//!< Read access type.
		eACCESS_TYPE_WRITE = 1 << 1,	//!< Write access type.
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
	\brief		Element type enumeration
	\~french
	\brief		Enumération des types pour les éléments de tampon
	*/
	typedef enum eELEMENT_TYPE
	CASTOR_TYPE( uint8_t )
	{
		eELEMENT_TYPE_1FLOAT,		//!< 1 float (GLSL float)
		eELEMENT_TYPE_2FLOATS,		//!< 2 floats (GLSL vec2)
		eELEMENT_TYPE_3FLOATS,		//!< 3 floats (GLSL vec3)
		eELEMENT_TYPE_4FLOATS,		//!< 4 floats (GLSL vec4)
		eELEMENT_TYPE_COLOUR,		//!< colour (uint32_t)
		eELEMENT_TYPE_1INT,			//!< 1 int (4 bytes each, GLSL int)
		eELEMENT_TYPE_2INTS,		//!< 2 ints (4 bytes each, GLSL ivec2)
		eELEMENT_TYPE_3INTS,		//!< 3 ints (4 bytes each, GLSL ivec3)
		eELEMENT_TYPE_4INTS,		//!< 4 ints (4 bytes each, GLSL ivec4)
		eELEMENT_TYPE_COUNT,
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

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			break;
		}

		return 0;
	}


	template< typename T > class CpuBuffer;
	class RenderSystem;
	class RenderTechniqueBase;
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
	DECLARE_SMART_PTR( IPipelineImpl );

	DECLARE_MAP( RenderWindow *, ContextSPtr, ContextPtr );
	DECLARE_MAP( std::thread::id, ContextPtrMap, ContextPtrMapId );

	//@}
}

#endif
