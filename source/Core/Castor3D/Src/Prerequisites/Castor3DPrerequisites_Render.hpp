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

	struct NonTexturedQuad
	{
		struct Vertex
		{
			castor::Point2f position;
		};

		Vertex vertex[6];
	};

	struct TexturedQuad
	{
		struct Vertex
		{
			castor::Point2f position;
			castor::Point2f texture;
		};

		Vertex vertex[6];
	};

	struct NonTexturedCube
	{
		struct Quad
		{
			struct Vertex
			{
				castor::Point3f position;
			};

			Vertex vertex[6];
		};

		Quad faces[6];
	};

	struct TexturedCube
	{
		struct Quad
		{
			struct Vertex
			{
				castor::Point3f position;
				castor::Point2f texture;
			};

			Vertex vertex[6];
		};

		Quad faces[6];
	};
	struct GeometryBuffers
	{
		ashes::BufferCRefArray vbo;
		ashes::UInt64Array vboOffsets;
		uint32_t vtxCount;
		ashes::VertexLayoutCRefArray layouts;
		ashes::BufferBase const * ibo{ nullptr };
		uint64_t iboOffset;
		uint32_t idxCount;
	};
	/*!
	\version	0.11.0
	\~english
	\brief		The picking node types.
	\~french
	\brief		Les types de noeud de picking.
	*/
	enum class PickNodeType
		: uint8_t
	{
		eNone,
		eStatic,
		eInstantiatedStatic,
		eSkinning,
		eInstantiatedSkinning,
		eMorphing,
		eBillboard
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
	CU_ImplementFlags( ElementUsage )
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
		BlendMode colourBlendMode;
		BlendMode alphaBlendMode;
		PassFlags passFlags;
		TextureChannels textureFlags;
		ProgramFlags programFlags;
		SceneFlags sceneFlags;
		ashes::PrimitiveTopology topology;
		ashes::CompareOp alphaFunc;
	};

	template< typename T >
	class CpuBuffer;

	class Context;
	class EnvironmentMap;
	class EnvironmentMapPass;
	class GeometryPassResult;
	class GpuInformations;
	class HdrConfig;
	class IblTextures;
	class PostEffect;
	class RenderColourCubeLayerToTexture;
	class RenderColourCubeToTexture;
	class RenderColourLayerToTexture;
	class RenderColourToCube;
	class RenderQuad;
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
	class RenderWindow;
	class SceneCuller;
	class ShadowMap;
	class ShadowMapPass;
	class TextureProjection;
	class ToneMapping;
	struct BillboardRenderNode;
	struct ParticleElementDeclaration;
	struct DistanceRenderNodeBase;
	struct MorphingRenderNode;
	struct PassRenderNode;
	struct PassRenderNodeUniforms;
	struct RenderInfo;
	struct SceneRenderNode;
	struct SkinningRenderNode;
	struct StaticRenderNode;

	CU_DeclareSmartPtr( ParticleElementDeclaration );
	CU_DeclareSmartPtr( Context );
	CU_DeclareSmartPtr( EnvironmentMap );
	CU_DeclareSmartPtr( EnvironmentMapPass );
	CU_DeclareSmartPtr( IblTextures );
	CU_DeclareSmartPtr( PostEffect );
	CU_DeclareSmartPtr( RenderColourCubeLayerToTexture );
	CU_DeclareSmartPtr( RenderColourCubeToTexture );
	CU_DeclareSmartPtr( RenderColourLayerToTexture );
	CU_DeclareSmartPtr( RenderColourToCube );
	CU_DeclareSmartPtr( RenderQuad );
	CU_DeclareSmartPtr( RenderDepthCubeLayerToTexture );
	CU_DeclareSmartPtr( RenderDepthCubeToTexture );
	CU_DeclareSmartPtr( RenderDepthLayerToTexture );
	CU_DeclareSmartPtr( RenderDepthToCube );
	CU_DeclareSmartPtr( RenderLoop );
	CU_DeclareSmartPtr( RenderPipeline );
	CU_DeclareSmartPtr( RenderSystem );
	CU_DeclareSmartPtr( RenderTarget );
	CU_DeclareSmartPtr( RenderTechnique );
	CU_DeclareSmartPtr( RenderWindow );
	CU_DeclareSmartPtr( SceneCuller );
	CU_DeclareSmartPtr( ShadowMap );
	CU_DeclareSmartPtr( ShadowMapPass );
	CU_DeclareSmartPtr( TextureProjection );
	CU_DeclareSmartPtr( ToneMapping );

	CU_DeclareMap( RenderWindow *, ContextSPtr, ContextPtr );
	CU_DeclareMap( std::thread::id, ContextPtrMap, ContextPtrMapId );
	using RenderQueueArray = std::vector< std::reference_wrapper< RenderQueue > >;
	using ShadowMapRefArray = std::vector< std::pair< std::reference_wrapper< ShadowMap >, UInt32Array > >;
	using ShadowMapLightTypeArray = std::array< ShadowMapRefArray, size_t( LightType::eCount ) >;

	//@}
}

#endif
