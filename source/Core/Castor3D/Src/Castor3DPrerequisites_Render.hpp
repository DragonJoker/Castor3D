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
#ifndef ___C3D_PREREQUISITES_RENDER_H___
#define ___C3D_PREREQUISITES_RENDER_H___

#include <Design/Factory.hpp>

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
	enum class MatrixMode
		: uint8_t
	{
		//!\~english	Eye to Clip transform.
		//!\~french		Transformations de Vue vers Clip.
		Projection,
		//!\~english	Object to World transform.
		//!\~french		Transformations de Objet vers Monde.
		Model,
		//!\~english	World to Eye transform.
		//!\~french Transformations de Monde vers Vue.
		View,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture0,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture1,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture2,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture3,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture4,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture5,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture6,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture7,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture8,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture9,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture10,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture11,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture12,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture13,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture14,
		//!\~english	Texture transforms.
		//!\~french		Transformations de texture.
		Texture15,
		CASTOR_ENUM_CLASS_BOUNDS( Projection )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Matrix modes masks
	\~french
	\brief		Masques pour les types de matrices
	*/
	static const uint64_t MASK_MTXMODE_PROJECTION = uint64_t( 0x1 ) << int( MatrixMode::Projection );
	static const uint64_t MASK_MTXMODE_MODEL = uint64_t( 0x1 ) << int( MatrixMode::Model );
	static const uint64_t MASK_MTXMODE_VIEW = uint64_t( 0x1 ) << int( MatrixMode::View );
	static const uint64_t MASK_MTXMODE_TEXTURE0 = uint64_t( 0x1 ) << int( MatrixMode::Texture0 );
	static const uint64_t MASK_MTXMODE_TEXTURE1 = uint64_t( 0x1 ) << int( MatrixMode::Texture1 );
	static const uint64_t MASK_MTXMODE_TEXTURE2 = uint64_t( 0x1 ) << int( MatrixMode::Texture2 );
	static const uint64_t MASK_MTXMODE_TEXTURE3 = uint64_t( 0x1 ) << int( MatrixMode::Texture3 );
	static const uint64_t MASK_MTXMODE_TEXTURE4 = uint64_t( 0x1 ) << int( MatrixMode::Texture4 );
	static const uint64_t MASK_MTXMODE_TEXTURE5 = uint64_t( 0x1 ) << int( MatrixMode::Texture5 );
	static const uint64_t MASK_MTXMODE_TEXTURE6 = uint64_t( 0x1 ) << int( MatrixMode::Texture6 );
	static const uint64_t MASK_MTXMODE_TEXTURE7 = uint64_t( 0x1 ) << int( MatrixMode::Texture7 );
	static const uint64_t MASK_MTXMODE_TEXTURE8 = uint64_t( 0x1 ) << int( MatrixMode::Texture8 );
	static const uint64_t MASK_MTXMODE_TEXTURE9 = uint64_t( 0x1 ) << int( MatrixMode::Texture9 );
	static const uint64_t MASK_MTXMODE_TEXTURE10 = uint64_t( 0x1 ) << int( MatrixMode::Texture10 );
	static const uint64_t MASK_MTXMODE_TEXTURE11 = uint64_t( 0x1 ) << int( MatrixMode::Texture11 );
	static const uint64_t MASK_MTXMODE_TEXTURE12 = uint64_t( 0x1 ) << int( MatrixMode::Texture12 );
	static const uint64_t MASK_MTXMODE_TEXTURE13 = uint64_t( 0x1 ) << int( MatrixMode::Texture13 );
	static const uint64_t MASK_MTXMODE_TEXTURE14 = uint64_t( 0x1 ) << int( MatrixMode::Texture14 );
	static const uint64_t MASK_MTXMODE_TEXTURE15 = uint64_t( 0x1 ) << int( MatrixMode::Texture15 );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		16/10/2015
	\~english
	\brief		Access types enumeration.
	\~french
	\brief		Enumération des types d'accès.
	*/
	enum class AccessType
		: uint8_t
	{
		//!\~english	No access.
		//!\~french		Aucun accès.
		None = 0,
		//!\~english	Read only access type.
		//!\~french		Accès lecture seule.
		Read = 1 << 0,
		//!\~english	Write only access type.
		//!\~french		Accèes écriture seule.
		Write = 1 << 1,
		//!\~english	Read and write access type.
		//!\~french		Accès lecture et écriture.
		ReadWrite = Read | Write,
	};
	/**
	 *\~english
	 *\brief		Bitwise OR on AccessType.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		OU binaire sur des AccessType.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	inline AccessType operator|( AccessType p_lhs, AccessType p_rhs )
	{
		return AccessType( uint8_t( p_lhs ) | uint8_t( p_rhs ) );
	}
	/**
	 *\~english
	 *\brief		Bitwise AND on AccessType.
	 *\param[in]	p_lhs, p_rhs	The operands.
	 *\~french
	 *\brief		ET binaire sur des AccessType.
	 *\param[in]	p_lhs, p_rhs	Les opérandes.
	 */
	inline AccessType operator&( AccessType p_lhs, AccessType p_rhs )
	{
		return AccessType( uint8_t( p_lhs ) & uint8_t( p_rhs ) );
	}
	/*!
	\author 	Sylvain DOREMUS
	\see		BufferAccessNature
	\~english
	\brief		Buffer access types enumeration
	\remark		Made to be combined with BufferAccessNature
	\~french
	\brief		Enumération des types d'accès de tampon
	\remark		Fait pour être combiné avec BufferAccessNature
	*/
	enum class BufferAccessType
		: uint32_t
	{
		//! Modified once and used many times
		Static = 1 << 0,
		//! Modified many times and used many times
		Dynamic = 1 << 1,
		//! Modified oncce and used at most a few times
		Stream = 1 << 2,
	};
	/*!
	\author 	Sylvain DOREMUS
	\see		BufferAccessType
	\~english
	\brief		Buffer access natures enumeration
	\remark		Made to be combined with BufferAccessType
	\~french
	\brief		Enumération des natures des accès de tampon
	\remark		Fait pour être combiné avec BufferAccessType
	*/
	enum class BufferAccessNature
		: uint32_t
	{
		//! Modified by CPU and used by GPU
		Draw = 1 << 4,
		//! Modified by GPU and used by CPU
		Read = 1 << 5,
		//! Modified by GPU and used by GPU
		Copy = 1 << 6,
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Primitive draw types enumeration
	\~french
	\brief		Enumération des type de primitive dessinées
	*/
	enum class Topology
		: uint8_t
	{
		//! Display textured points
		Points,
		//! Display textured edges
		Lines,
		//! Display textured edge loops
		LineLoop,
		//! Display textured edge strips
		LineStrip,
		//! Display textured triangles
		Triangles,
		//! Display triangle strips
		TriangleStrips,
		//! Display triangle fan
		TriangleFan,
		//! Display quads
		Quads,
		//! Display quad strips
		QuadStrips,
		//! Display quad strips
		Polygon,
		CASTOR_ENUM_CLASS_BOUNDS( Points )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Depth functions enumeration
	\~french
	\brief		Enumération des fonctions de profondeur
	*/
	enum class DepthFunc
		: uint8_t
	{
		//! Never passes.
		Never,
		//! Passes if the incoming depth value is less than the stored depth value.
		Less,
		//! Passes if the incoming depth value is equal to the stored depth value.
		Equal,
		//! Passes if the incoming depth value is less than or equal to the stored depth value.
		LEqual,
		//! Passes if the incoming depth value is greater than the stored depth value.
		Greater,
		//! Passes if the incoming depth value is not equal to the stored depth value.
		NEqual,
		//! Passes if the incoming depth value is greater than or equal to the stored depth value.
		GEqual,
		//! Always passes.
		Always,
		CASTOR_ENUM_CLASS_BOUNDS( Never )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Wrinting masks enumeration
	\~french
	\brief		Enumération des masques d'écriture
	*/
	enum class WritingMask
		: uint8_t
	{
		//! Writing disabled
		Zero,
		//! Writing enabled
		All,
		CASTOR_ENUM_CLASS_BOUNDS( Zero )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Stencil functions enumeration
	\~french
	\brief		Enumération des fonctions de stencil
	*/
	enum class StencilFunc
		: uint8_t
	{
		//! Never passes.
		Never,
		//! Passes if the incoming stencil value is less than the stored stencil value.
		Less,
		//! Passes if the incoming stencil value is equal to the stored stencil value.
		Equal,
		//! Passes if the incoming stencil value is less than or equal to the stored stencil value.
		LEqual,
		//! Passes if the incoming stencil value is greater than the stored stencil value.
		Greater,
		//! Passes if the incoming stencil value is not equal to the stored stencil value.
		NEqual,
		//! Passes if the incoming stencil value is greater than or equal to the stored stencil value.
		GEqual,
		//! Always passes.
		Always,
		CASTOR_ENUM_CLASS_BOUNDS( Never )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		02/06/2013
	\~english
	\brief		Stencil functions enumeration
	\~french
	\brief		Enumération des fonctions de stencil
	*/
	enum class StencilOp
		: uint8_t
	{
		//! Keeps the current value.
		Keep,
		//! Sets the stencil buffer value to 0.
		Zero,
		//! Sets the stencil buffer value by the one given
		Replace,
		//! Increments the current stencil buffer value. Clamps to the maximum representable unsigned value.
		Increment,
		//! Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value.
		IncrWrap,
		//! Decrements the current stencil buffer value. Clamps to 0.
		Decrement,
		//! Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero.
		DecrWrap,
		//! Bitwise inverts the current stencil buffer value.
		Invert,
		CASTOR_ENUM_CLASS_BOUNDS( Keep )
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Element usage enumeration
	\~french
	\brief		Enumération des utilisations d'éléments de tampon
	*/
	enum class ElementUsage
		: uint32_t
	{
		//! Position coords
		Unknown = 0x000,
		//! Position coords
		Position = 0x001,
		//! Normal coords
		Normal = 0x002,
		//! Tangent coords
		Tangent = 0x004,
		//! Bitangent coords
		Bitangent = 0x008,
		//! Diffuse colour
		Diffuse = 0x010,
		//! Texture coordinates
		TexCoords = 0x020,
		//! Bone IDs 0
		BoneIds0 = 0x040,
		//! Bone IDs 1
		BoneIds1 = 0x080,
		//! Bone weights 0
		BoneWeights0 = 0x100,
		//! Bone weights 1
		BoneWeights1 = 0x200,
		//! Instantiation matrix
		Transform = 0x400,
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Element type enumeration
	\~french
	\brief		Enumération des types pour les éléments de tampon
	*/
	enum class ElementType
		: uint8_t
	{
		//! 1 float (GLSL float)
		Float,
		//! 2 floats (GLSL vec2)
		Vec2,
		//! 3 floats (GLSL vec3)
		Vec3,
		//! 4 floats (GLSL vec4)
		Vec4,
		//! colour (uint32_t)
		Colour,
		//! 1 int (4 bytes each, GLSL int)
		Int,
		//! 2 ints (4 bytes each, GLSL ivec2)
		IVec2,
		//! 3 ints (4 bytes each, GLSL ivec3)
		IVec3,
		//! 4 ints (4 bytes each, GLSL ivec4)
		IVec4,
		//! 2x2 floats (GLSL mat2)
		Mat2,
		//! 3x3 floats (GLSL mat3)
		Mat3,
		//! 4x4 floats (GLSL mat4)
		Mat4,
		CASTOR_ENUM_CLASS_BOUNDS( Float )
	};

	inline uint32_t GetSize( ElementType p_type )
	{
		switch ( p_type )
		{
		case ElementType::Float:
			return uint32_t( 1u * sizeof( real ) );
			break;

		case ElementType::Vec2:
			return uint32_t( 2u * sizeof( real ) );
			break;

		case ElementType::Vec3:
			return uint32_t( 3u * sizeof( real ) );
			break;

		case ElementType::Vec4:
			return uint32_t( 4u * sizeof( real ) );
			break;

		case ElementType::Colour:
			return uint32_t( sizeof( uint32_t ) );
			break;

		case ElementType::Int:
			return uint32_t( 1u * sizeof( int32_t ) );
			break;

		case ElementType::IVec2:
			return uint32_t( 2u * sizeof( int32_t ) );
			break;

		case ElementType::IVec3:
			return uint32_t( 3u * sizeof( int32_t ) );
			break;

		case ElementType::IVec4:
			return uint32_t( 4u * sizeof( int32_t ) );
			break;

		case ElementType::Mat2:
			return uint32_t( 2u * 2u * sizeof( real ) );
			break;

		case ElementType::Mat3:
			return uint32_t( 3u * 3u * sizeof( real ) );
			break;

		case ElementType::Mat4:
			return uint32_t( 4u * 4u * sizeof( real ) );
			break;

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			break;
		}

		return 0;
	}

	struct PipelineFlags
	{
		BlendMode m_colourBlendMode;
		BlendMode m_alphaBlendMode;
		uint16_t m_textureFlags;
		uint16_t m_programFlags;
		uint8_t m_sceneFlags;
	};

	class GpuInformations;
	class RenderSystem;
	class RenderPass;
	class RenderTechnique;
	class Context;
	class RenderWindow;
	class Pipeline;
	class DepthStencilState;
	class RasteriserState;
	class BlendState;
	class MultisampleState;
	class ToneMapping;
	class PostEffect;
	class ShadowMapPass;
	struct PassRenderNode;
	struct SceneRenderNode;
	struct StaticGeometryRenderNode;
	struct AnimatedGeometryRenderNode;
	struct BillboardRenderNode;

	template< typename T >
	class GpuBuffer;
	template< typename T >
	class CpuBuffer;
	template< typename T >
	class CpuTransformBuffer;
	class GpuTransformBuffer;

	struct BufferElementDeclaration;
	class BufferDeclaration;
	struct TransformBufferElementDeclaration;
	class TransformBufferDeclaration;
	class VertexBuffer;
	class IndexBuffer;
	class GeometryBuffers;
	class TextureBuffer;

	DECLARE_SMART_PTR( RenderSystem );
	DECLARE_SMART_PTR( GeometryBuffers );
	DECLARE_SMART_PTR( BufferElementDeclaration );
	DECLARE_SMART_PTR( BufferDeclaration );
	DECLARE_SMART_PTR( TransformBufferElementDeclaration );
	DECLARE_SMART_PTR( TransformBufferDeclaration );
	DECLARE_SMART_PTR( VertexBuffer );
	DECLARE_SMART_PTR( IndexBuffer );
	DECLARE_SMART_PTR( TextureBuffer );
	DECLARE_SMART_PTR( GpuTransformBuffer );
	DECLARE_SMART_PTR( Context );
	DECLARE_SMART_PTR( DepthStencilState );
	DECLARE_SMART_PTR( RasteriserState );
	DECLARE_SMART_PTR( BlendState );
	DECLARE_SMART_PTR( MultisampleState );
	DECLARE_SMART_PTR( Pipeline );
	DECLARE_SMART_PTR( ToneMapping );
	DECLARE_SMART_PTR( PostEffect );
	DECLARE_SMART_PTR( ShadowMapPass );

	DECLARE_MAP( RenderWindow *, ContextSPtr, ContextPtr );
	DECLARE_MAP( std::thread::id, ContextPtrMap, ContextPtrMapId );
	DECLARE_MULTIMAP( double, StaticGeometryRenderNode, StaticGeometryRenderNodeByDistance );
	DECLARE_MULTIMAP( double, AnimatedGeometryRenderNode, AnimatedGeometryRenderNodeByDistance );
	DECLARE_MULTIMAP( double, BillboardRenderNode, BillboardRenderNodeByDistance );

	//@}
}

#endif
