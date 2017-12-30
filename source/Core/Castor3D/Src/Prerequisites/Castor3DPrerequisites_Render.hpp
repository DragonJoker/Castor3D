/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_RENDER_H___
#define ___C3D_PREREQUISITES_RENDER_H___

#include <Design/Factory.hpp>

namespace castor3d
{
	/**@name Render */
	//@{

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
		eNone = 0,
		//!\~english	read only access type.
		//!\~french		Accès lecture seule.
		eRead = 1 << 0,
		//!\~english	write only access type.
		//!\~french		Accèes écriture seule.
		eWrite = 1 << 1
	};
	IMPLEMENT_FLAGS( AccessType )
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
		eStatic = 1 << 0,
		//! Modified many times and used many times
		eDynamic = 1 << 1,
		//! Modified oncce and used at most a few times
		eStream = 1 << 2,
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
		eDraw = 1 << 4,
		//! Modified by GPU and used by CPU
		eRead = 1 << 5,
		//! Modified by GPU and used by GPU
		eCopy = 1 << 6,
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
		ePoints,
		//! Display textured edges
		eLines,
		//! Display textured edge loops
		eLineLoop,
		//! Display textured edge strips
		eLineStrip,
		//! Display textured triangles
		eTriangles,
		//! Display triangle strips
		eTriangleStrips,
		//! Display triangle fan
		eTriangleFan,
		CASTOR_SCOPED_ENUM_BOUNDS( ePoints )
	};
	/*!
	\author 	Sylvain DOREMUS
	\~english
	\brief		Helper structure to get a topology name.
	\~french
	\brief		Structure d'aide pour récupérer le nom d'une topologie.
	*/
	template< Topology Topo >
	struct TopologyNamer
	{
		C3D_API static castor::String const Name;
	};
	/**
	 *\~english
	 *\brief		gets the name of the given topology.
	 *\param[in]	p_topology	The topology.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom de la topologie donnée.
	 *\param[in]	p_topology	La topologie.
	 *\return		Le nom.
	 */
	static inline castor::String const getTopologyName( Topology p_topology )
	{
		switch ( p_topology )
		{
		case Topology::ePoints:
			return TopologyNamer< Topology::ePoints >::Name;

		case Topology::eLines:
			return TopologyNamer< Topology::eLines >::Name;

		case Topology::eLineLoop:
			return TopologyNamer< Topology::eLineLoop >::Name;

		case Topology::eLineStrip:
			return TopologyNamer< Topology::eLineStrip >::Name;

		case Topology::eTriangles:
			return TopologyNamer< Topology::eTriangles >::Name;

		case Topology::eTriangleStrips:
			return TopologyNamer< Topology::eTriangleStrips >::Name;

		case Topology::eTriangleFan:
			return TopologyNamer< Topology::eTriangleFan >::Name;

		default:
			FAILURE( "Topology type unknown" );
			CASTOR_EXCEPTION( "Topology type unknown" );
			break;
		}
	}
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
		eNever,
		//! Passes if the incoming depth value is less than the stored depth value.
		eLess,
		//! Passes if the incoming depth value is equal to the stored depth value.
		eEqual,
		//! Passes if the incoming depth value is less than or equal to the stored depth value.
		eLEqual,
		//! Passes if the incoming depth value is greater than the stored depth value.
		eGreater,
		//! Passes if the incoming depth value is not equal to the stored depth value.
		eNEqual,
		//! Passes if the incoming depth value is greater than or equal to the stored depth value.
		eGEqual,
		//! Always passes.
		eAlways,
		CASTOR_SCOPED_ENUM_BOUNDS( eNever )
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
		eZero,
		//! Writing enabled
		eAll,
		CASTOR_SCOPED_ENUM_BOUNDS( eZero )
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
		eNever,
		//! Passes if the incoming stencil value is less than the stored stencil value.
		eLess,
		//! Passes if the incoming stencil value is equal to the stored stencil value.
		eEqual,
		//! Passes if the incoming stencil value is less than or equal to the stored stencil value.
		eLEqual,
		//! Passes if the incoming stencil value is greater than the stored stencil value.
		eGreater,
		//! Passes if the incoming stencil value is not equal to the stored stencil value.
		eNEqual,
		//! Passes if the incoming stencil value is greater than or equal to the stored stencil value.
		eGEqual,
		//! Always passes.
		eAlways,
		CASTOR_SCOPED_ENUM_BOUNDS( eNever )
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
		eKeep,
		//! sets the stencil buffer value to 0.
		eZero,
		//! sets the stencil buffer value by the one given
		eReplace,
		//! Increments the current stencil buffer value. Clamps to the maximum representable unsigned value.
		eIncrement,
		//! Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value.
		eIncrWrap,
		//! Decrements the current stencil buffer value. Clamps to 0.
		eDecrement,
		//! Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero.
		eDecrWrap,
		//! Bitwise inverts the current stencil buffer value.
		eInvert,
		CASTOR_SCOPED_ENUM_BOUNDS( eKeep )
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
		eUnknown = 0x000,
		//! Position coords
		ePosition = 0x001,
		//! Normal coords
		eNormal = 0x002,
		//! Tangent coords
		eTangent = 0x004,
		//! Bitangent coords
		eBitangent = 0x008,
		//! Diffuse colour
		eDiffuse = 0x010,
		//! Texture coordinates
		eTexCoords = 0x020,
		//! Bone IDs 0
		eBoneIds0 = 0x040,
		//! Bone IDs 1
		eBoneIds1 = 0x080,
		//! Bone weights 0
		eBoneWeights0 = 0x100,
		//! Bone weights 1
		eBoneWeights1 = 0x200,
		//! Instantiation matrix
		eTransform = 0x400,
		//! Instantiation material index
		eMatIndex = 0x800,
	};
	IMPLEMENT_FLAGS( ElementUsage )
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
		eFloat,
		//! 2 floats (GLSL vec2)
		eVec2,
		//! 3 floats (GLSL vec3)
		eVec3,
		//! 4 floats (GLSL vec4)
		eVec4,
		//! colour (uint32_t)
		eColour,
		//! 1 int (4 bytes each, GLSL int)
		eInt,
		//! 2 ints (4 bytes each, GLSL ivec2)
		eIVec2,
		//! 3 ints (4 bytes each, GLSL ivec3)
		eIVec3,
		//! 4 ints (4 bytes each, GLSL ivec4)
		eIVec4,
		//! 1 uint (4 bytes each, GLSL uint)
		eUInt,
		//! 2 uints (4 bytes each, GLSL uivec2)
		eUIVec2,
		//! 3 uints (4 bytes each, GLSL uivec3)
		eUIVec3,
		//! 4 uints (4 bytes each, GLSL uivec4)
		eUIVec4,
		//! 2x2 floats (GLSL mat2)
		eMat2,
		//! 3x3 floats (GLSL mat3)
		eMat3,
		//! 4x4 floats (GLSL mat4)
		eMat4,
		CASTOR_SCOPED_ENUM_BOUNDS( eFloat )
	};
	/**
	 *\~english
	 *\brief		gets the byte size of the given element type.
	 *\param[in]	p_type	The element type.
	 *\return		The size.
	 *\~french
	 *\brief		Récupère la taille en octets du type d'élément donné.
	 *\param[in]	p_type	Le type d'élément.
	 *\return		La taille.
	 */
	inline uint32_t getSize( ElementType p_type )
	{
		switch ( p_type )
		{
		case ElementType::eFloat:
			return uint32_t( 1u * sizeof( real ) );
			break;

		case ElementType::eVec2:
			return uint32_t( 2u * sizeof( real ) );
			break;

		case ElementType::eVec3:
			return uint32_t( 3u * sizeof( real ) );
			break;

		case ElementType::eVec4:
			return uint32_t( 4u * sizeof( real ) );
			break;

		case ElementType::eColour:
			return uint32_t( sizeof( uint32_t ) );
			break;

		case ElementType::eInt:
			return uint32_t( 1u * sizeof( int32_t ) );
			break;

		case ElementType::eIVec2:
			return uint32_t( 2u * sizeof( int32_t ) );
			break;

		case ElementType::eIVec3:
			return uint32_t( 3u * sizeof( int32_t ) );
			break;

		case ElementType::eIVec4:
			return uint32_t( 4u * sizeof( int32_t ) );
			break;

		case ElementType::eUInt:
			return uint32_t( 1u * sizeof( uint32_t ) );
			break;

		case ElementType::eUIVec2:
			return uint32_t( 2u * sizeof( uint32_t ) );
			break;

		case ElementType::eUIVec3:
			return uint32_t( 3u * sizeof( uint32_t ) );
			break;

		case ElementType::eUIVec4:
			return uint32_t( 4u * sizeof( uint32_t ) );
			break;

		case ElementType::eMat2:
			return uint32_t( 2u * 2u * sizeof( real ) );
			break;

		case ElementType::eMat3:
			return uint32_t( 3u * 3u * sizeof( real ) );
			break;

		case ElementType::eMat4:
			return uint32_t( 4u * 4u * sizeof( real ) );
			break;

		default:
			assert( false && "Unsupported vertex buffer attribute type." );
			break;
		}

		return 0;
	}
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		12/12/2016
	\~english
	\brief		GPU buffers types enumeration.
	\~french
	\brief		Enumération des types de tampons.
	*/
	enum class BufferType
		: uint32_t
	{
		//!\~english	Data array => Vertex buffers.
		//!\~french		Tableau de données => Tampons de sommets.
		eArray,
		//!\~english	Element array => Index buffers.
		//!\~french		Tableau d'éléments => Tampons d'indices.
		eElementArray,
		//!\~english	Uniform buffer.
		//!\~french		Tampn de variables uniformes.
		eUniform,
		//!\~english	Atomic counters buffer.
		//!\~french		Tampon de compteurs atomiques.
		eAtomicCounter,
		//!\~english	Shader storage buffer.
		//!\~french		Tampon stockage pour shader.
		eShaderStorage,
	};
	/**
	 *\~english
	 *\brief		gets the name of the given element type.
	 *\param[in]	p_type	The element type.
	 *\return		The name.
	 *\~french
	 *\brief		Récupère le nom du type d'élément donné.
	 *\param[in]	p_type	Le type d'élément.
	 *\return		Le nom.
	 */
	C3D_API castor::String getName( ElementType p_type );
	/*!
	\author 	Sylvain DOREMUS
	\version	0.9.0
	\date		03/09/2016
	\~english
	\brief		Pipeline flags.
	\~french
	\brief		Indicateurs de pipeline.
	*/
	struct PipelineFlags
	{
		BlendMode m_colourBlendMode;
		BlendMode m_alphaBlendMode;
		PassFlags m_passFlags;
		TextureChannels m_textureFlags;
		ProgramFlags m_programFlags;
		SceneFlags m_sceneFlags;
	};

	template< typename T >
	class CpuBuffer;

	class BlendState;
	class BufferDeclaration;
	class Context;
	class DepthStencilState;
	class EnvironmentMap;
	class EnvironmentMapPass;
	class GeometryBuffers;
	class GpuBuffer;
	class GpuInformations;
	class HdrConfig;
	class IblTextures;
	class IndexBuffer;
	class MultisampleState;
	class PostEffect;
	class RasteriserState;
	class RenderColourCubeLayerToTexture;
	class RenderColourCubeToTexture;
	class RenderColourLayerToTexture;
	class RenderColourToCube;
	class RenderColourToTexture;
	class RenderDepthCubeLayerToTexture;
	class RenderDepthCubeToTexture;
	class RenderDepthLayerToTexture;
	class RenderDepthToCube;
	class RenderLoop;
	class RenderLoopAsync;
	class RenderLoopSync;
	class RenderPass;
	class RenderPipeline;
	class RenderQueue;
	class RenderSystem;
	class RenderTarget;
	class RenderTechnique;
	class RenderTechnique;
	class RenderWindow;
	class RenderWindow;
	class ShadowMap;
	class ShadowMapPass;
	class TextureBuffer;
	class TextureProjection;
	class ToneMapping;
	class VertexBuffer;
	struct BillboardRenderNode;
	struct BufferElementDeclaration;
	struct DistanceRenderNodeBase;
	struct MorphingRenderNode;
	struct PassRenderNode;
	struct PassRenderNodeUniforms;
	struct SceneRenderNode;
	struct SkinningRenderNode;
	struct StaticRenderNode;

	DECLARE_SMART_PTR( BlendState );
	DECLARE_SMART_PTR( BufferDeclaration );
	DECLARE_SMART_PTR( BufferElementDeclaration );
	DECLARE_SMART_PTR( Context );
	DECLARE_SMART_PTR( DepthStencilState );
	DECLARE_SMART_PTR( EnvironmentMap );
	DECLARE_SMART_PTR( EnvironmentMapPass );
	DECLARE_SMART_PTR( GeometryBuffers );
	DECLARE_SMART_PTR( GpuBuffer );
	DECLARE_SMART_PTR( IblTextures );
	DECLARE_SMART_PTR( IndexBuffer );
	DECLARE_SMART_PTR( MultisampleState );
	DECLARE_SMART_PTR( PostEffect );
	DECLARE_SMART_PTR( RasteriserState );
	DECLARE_SMART_PTR( RenderColourCubeLayerToTexture );
	DECLARE_SMART_PTR( RenderColourCubeToTexture );
	DECLARE_SMART_PTR( RenderColourLayerToTexture );
	DECLARE_SMART_PTR( RenderColourToCube );
	DECLARE_SMART_PTR( RenderColourToTexture );
	DECLARE_SMART_PTR( RenderDepthCubeLayerToTexture );
	DECLARE_SMART_PTR( RenderDepthCubeToTexture );
	DECLARE_SMART_PTR( RenderDepthLayerToTexture );
	DECLARE_SMART_PTR( RenderDepthToCube );
	DECLARE_SMART_PTR( RenderLoop );
	DECLARE_SMART_PTR( RenderPipeline );
	DECLARE_SMART_PTR( RenderSystem );
	DECLARE_SMART_PTR( RenderTarget );
	DECLARE_SMART_PTR( RenderTechnique );
	DECLARE_SMART_PTR( RenderWindow );
	DECLARE_SMART_PTR( ShadowMap );
	DECLARE_SMART_PTR( ShadowMapPass );
	DECLARE_SMART_PTR( TextureBuffer );
	DECLARE_SMART_PTR( TextureProjection );
	DECLARE_SMART_PTR( ToneMapping );
	DECLARE_SMART_PTR( VertexBuffer );

	DECLARE_MAP( RenderWindow *, ContextSPtr, ContextPtr );
	DECLARE_MAP( std::thread::id, ContextPtrMap, ContextPtrMapId );
	using VertexBufferArray = std::vector< std::reference_wrapper< VertexBuffer > >;
	using RenderQueueArray = std::vector< std::reference_wrapper< RenderQueue > >;
	using ShadowMapRefArray = std::vector< std::reference_wrapper< ShadowMap > >;
	using ShadowMapLightTypeArray = std::array< ShadowMapRefArray, size_t( LightType::eCount ) >;

	//@}
}

#endif
