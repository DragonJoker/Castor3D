/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderModule_H___
#define ___C3D_RenderModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Image/ImageView.hpp>

#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>

namespace castor3d
{
	/**@name Render */
	//@{

	castor::String const RenderTypeUndefined = cuT( "Undefined" );

	/**
	*\~english
	*\brief
	*	A quad without UV.
	*\~french
	*\brief
	*	Un quad sans UV.
	*/
	struct NonTexturedQuad
	{
		struct Vertex
		{
			castor::Point2f position;
		};

		Vertex vertex[6];
	};
	/**
	*\~english
	*\brief
	*	A quad with UV.
	*\~french
	*\brief
	*	Un quad avec UV.
	*/
	struct TexturedQuad
	{
		struct Vertex
		{
			castor::Point2f position;
			castor::Point2f texture;
		};

		Vertex vertex[6];
	};
	/**
	*\~english
	*\brief
	*	A cube without UV.
	*\~french
	*\brief
	*	Un cube sans UV.
	*/
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
	/**
	*\~english
	*\brief
	*	A cube with UV.
	*\~french
	*\brief
	*	Un cube avec UV.
	*/
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
	/**
	*\~english
	*\brief
	*	RenderGraph's image view binding.
	*\~french
	*\brief
	*	Binding d'une image RenderGraph.
	*/
	struct Texture
	{
		C3D_API Texture();
		C3D_API Texture( RenderDevice const & device
			, crg::ResourceHandler & handler
			, castor::String const & name
			, VkImageCreateFlags createFlags
			, VkExtent3D const & size
			, uint32_t layerCount
			, uint32_t mipLevels
			, VkFormat format
			, VkImageUsageFlags usageFlags
			, VkBorderColor const & borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK
			, bool createSubviews = true );

		C3D_API void create();

		C3D_API VkImageMemoryBarrier makeGeneralLayout( VkImageLayout srcLayout
			, VkAccessFlags dstAccessFlags
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferDestination( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeTransferSource( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeShaderInputResource( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilReadOnly( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeColourAttachment( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeDepthStencilAttachment( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makePresentSource( VkImageLayout srcLayout
			, uint32_t srcQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, uint32_t dstQueueFamily = VK_QUEUE_FAMILY_IGNORED
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImageLayout sourceLayout
			, VkImageLayout destinationLayout
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImageLayout srcLayout
			, VkImageLayout dstLayout
			, VkAccessFlags srcAccessFlags
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;
		C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImageLayout srcLayout
			, VkImageLayout dstLayout
			, VkAccessFlags srcAccessFlags
			, VkAccessFlags dstAccessMask
			, uint32_t srcQueueFamily
			, uint32_t dstQueueFamily
			, bool target = false )const;

		uint32_t getMipLevels()const
		{
			return imageId.data->info.mipLevels;
		}

		VkFormat getFormat()const
		{
			return imageId.data->info.format;
		}

		VkExtent3D getExtent()const
		{
			return imageId.data->info.extent;
		}

		crg::ResourceHandler * handler{};
		RenderDevice const * device{};
		crg::ImageId imageId{};
		VkImage image{};
		crg::ImageViewId wholeViewId{};
		crg::ImageViewId targetViewId{};
		crg::ImageViewId sampledViewId{};
		VkImageView wholeView{};
		VkImageView targetView{};
		VkImageView sampledView{};
		crg::ImageViewIdArray subViewsId{};
		ashes::Sampler const * sampler{};
	};
	C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout sourceLayout
		, VkImageLayout destinationLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily );
	C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily );
	C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, VkImageSubresourceRange const & range
		, VkImageLayout srcLayout
		, VkImageLayout dstLayout
		, VkAccessFlags srcAccessFlags
		, VkAccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily );
	using TextureArray = std::vector< Texture >;
	/**
	*\~english
	*\brief
	*	Holds minimal data for an intermediate view.
	*\~french
	*\brief
	*	Contient les données minimales pour une vue intermédiaire.
	*/
	struct IntermediateView
	{
		C3D_API IntermediateView() = default;

		IntermediateView( castor::String name
			, crg::ImageViewId const & viewId
			, VkImageLayout layout
			, TextureFactors factors = {} )
			: name{ std::move( name ) }
			, viewId{ viewId }
			, layout{ layout }
			, factors{ std::move( factors ) }
		{
		}
		
		IntermediateView( castor::String name
			, Texture const & texture
			, VkImageLayout layout
			, TextureFactors factors = {} )
			: IntermediateView{ std::move( name )
				, texture.sampledViewId
				, layout
				, std::move( factors ) }
		{
		}

		castor::String name;
		crg::ImageViewId viewId;
		VkImageLayout layout;
		TextureFactors factors;
	};
	/**
	*\~english
	*\brief
	*	The render nodes UBOs binding index.
	*\~french
	*\brief
	*	L'index de binding des UBOs des noeuds de rendu.
	*/
	enum class NodeUboIdx
		: uint32_t
	{
		eMaterials,
		eTextures,
		eModel,
		eBillboard,
		eSkinningUbo,
		eSkinningSsbo,
		eMorphing,
		eModelInstances,
		eLpvGridConfig,
		eLayeredLpvGridConfig,
		eVoxelData = eLpvGridConfig,
	};
	/**
	*\~english
	*\brief
	*	The render pass UBOs binding index.
	*\~french
	*\brief
	*	L'index de binding des UBOs des passes de rendu.
	*/
	enum class PassUboIdx
		: uint32_t
	{
		eMatrix,
		eScene,
		CU_ScopedEnumBounds( eMatrix ),
	};
	/**
	*\~english
	*\brief
	*	Frustum corners enumeration
	*\~french
	*\brief
	*	Enumération des coins d'un frustum
	*/
	enum class FrustumCorner
	{
		//!\~english	Far left bottom corner.
		//!\~french		Coin éloigné bas gauche.
		eFarLeftBottom,
		//!\~english	Far left top corner.
		//!\~french		Coin éloigné haut gauche.
		eFarLeftTop,
		//!\~english	Far right top corner.
		//!\~french		Coin éloigné haut droit.
		eFarRightTop,
		//!\~english	Far right bottom corner.
		//!\~french		Coin éloigné bas droit.
		eFarRightBottom,
		//!\~english	Near left bottom corner.
		//!\~french		Coin proche bas gauche.
		eNearLeftBottom,
		//!\~english	Near left top corner.
		//!\~french		Coin proche haut gauche.
		eNearLeftTop,
		//!\~english	Near right top corner.
		//!\~french		Coin proche haut droit.
		eNearRightTop,
		//!\~english	Near right bottom corner.
		//!\~french		Coin proche bas droit.
		eNearRightBottom,
		CU_ScopedEnumBounds( eFarLeftBottom )
	};
	C3D_API castor::String getName( FrustumCorner value );
	/**
	*\~english
	*\brief
	*	Frustum planes enumeration
	*\~french
	*\brief
	*	Enumération des plans d'un frustum.
	*/
	enum class FrustumPlane
	{
		//!\~english	Near plane.
		//!\~french		Plan proche.
		eNear,
		//!\~english	Far plane.
		//!\~french		Plan éloigné.
		eFar,
		//!\~english	Left plane.
		//!\~french		Plan gauche.
		eLeft,
		//!\~english	Right plane.
		//!\~french		Plan droit.
		eRight,
		//!\~english	Top plane.
		//!\~french		Plan haut.
		eTop,
		//!\~english	Bottom plane.
		//!\~french		Plan bas.
		eBottom,
		CU_ScopedEnumBounds( eNear )
	};
	C3D_API castor::String getName( FrustumPlane value );
	/**
	*\~english
	*\brief
	*	The picking node types.
	*\~french
	*\brief
	*	Les types de noeud de picking.
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
	C3D_API castor::String getName( PickNodeType value );
	/**
	*\~english
	*\brief
	*	RenderTarget supported types
	*\~french
	*\brief
	*	Types de RenderTarget supportés
	*/
	enum class TargetType
		: uint8_t
	{
		eWindow,
		eTexture,
		CU_ScopedEnumBounds( eWindow )
	};
	C3D_API castor::String getName( TargetType value );
	/**
	*\~english
	*\brief
	*	The  viewport projection types enumeration
	*\~french
	*\brief
	*	Enumération des types de projection de viewport
	*/
	enum class ViewportType
		: uint8_t
	{
		eOrtho,
		ePerspective,
		eFrustum,
		CU_ScopedEnumBounds( eOrtho )
	};
	C3D_API castor::String getName( ViewportType value );
	/**
	*\~english
	*\brief
	*	The render pass shader flags.
	*\~french
	*\brief
	*	Les indicateurs de shader des passes.
	*/
	enum class ShaderFlag
		: uint8_t
	{
		eNone = 0x00,
		eNormal = 0x01,
		eTangentSpace = 0x02 | eNormal,
		eVelocity = 0x04,
		eWorldSpace = 0x08,
		eViewSpace = 0x10,
	};
	CU_ImplementFlags( ShaderFlag )
	/**
	*\~english
	*\brief
	*	The render pass modes, regarding transparency.
	*\~french
	*\brief
	*	Les modes de rendu des passes, par rapport à la transparence.
	*/
	enum class RenderMode
		: uint8_t
	{
		eOpaqueOnly,
		eTransparentOnly,
		eBoth,
		CU_ScopedEnumBounds( eOpaqueOnly )
	};
	C3D_API castor::String getName( RenderMode value );

	C3D_API FilteredTextureFlags::const_iterator checkFlags( FilteredTextureFlags const & flags, TextureFlag flag );
	C3D_API TextureFlagsArray::const_iterator checkFlags( TextureFlagsArray const & flags, TextureFlag flag );
	C3D_API TextureFlags merge( TextureFlagsArray const & flags );
	/**
	 *\~english
	 *\brief		Filters the given textures flags using the given mask.
	 *\param[in]	textures	The textures flags.
	 *\param[in]	mask		The mask used to filter out textures.
	 *\return		The filtered flags.
	 *\~french
	 *\brief		Filtre les indicateurs de textures donnés en utilisant le masque donné.
	 *\param[in]	textures	Les indicateurs de textures.
	 *\param[in]	mask		Le masque utilisé pour le filtre.
	 *\return		Les indicateurs filtrés.
	 */
	C3D_API FilteredTextureFlags filterTexturesFlags( TextureFlagsArray const & textures
		, TextureFlags mask );
	/**
	*\~english
	*\brief
	*	Pipeline flags.
	*\~french
	*\brief
	*	Indicateurs de pipeline.
	*/
	struct PipelineFlags
	{
		PipelineFlags( BlendMode colourBlendMode = BlendMode::eNoBlend
			, BlendMode alphaBlendMode = BlendMode::eNoBlend
			, PassFlags passFlags = PassFlag::eNone
			, PassTypeID passType = 0u
			, uint32_t heightMapIndex = InvalidIndex
			, ProgramFlags programFlags = ProgramFlag::eNone
			, SceneFlags sceneFlags = SceneFlag::eNone
			, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
			, VkCompareOp alphaFunc = VK_COMPARE_OP_ALWAYS
			, VkCompareOp blendAlphaFunc = VK_COMPARE_OP_ALWAYS
			, TextureFlagsArray textures = {} )
			: colourBlendMode{ colourBlendMode }
			, alphaBlendMode{ alphaBlendMode }
			, passFlags{ passFlags }
			, passType{ passType }
			, heightMapIndex{ heightMapIndex }
			, programFlags{ programFlags }
			, sceneFlags{ sceneFlags }
			, topology{ topology }
			, alphaFunc{ alphaFunc }
			, blendAlphaFunc{ blendAlphaFunc }
			, textures{ textures }
			, texturesFlags{ merge( textures ) }
		{
		}

		BlendMode colourBlendMode{ BlendMode::eNoBlend };
		BlendMode alphaBlendMode{ BlendMode::eNoBlend };
		PassFlags passFlags{ PassFlag::eNone };
		PassTypeID passType{ 0u };
		uint32_t heightMapIndex{ InvalidIndex };
		ProgramFlags programFlags{ ProgramFlag::eNone };
		SceneFlags sceneFlags{ SceneFlag::eNone };
		VkPrimitiveTopology topology{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
		VkCompareOp alphaFunc{ VK_COMPARE_OP_ALWAYS };
		VkCompareOp blendAlphaFunc{ VK_COMPARE_OP_ALWAYS };
		TextureFlagsArray textures;
		TextureFlags texturesFlags;
	};
	C3D_API bool operator==( PipelineFlags const & lhs, PipelineFlags const & rhs );
	/**
	*\~english
	*\brief
	*	Implements a frustum and the checks related to frustum culling.
	*\~french
	*\brief
	*	Implémente un frustum et les vérifications relatives au frustum culling.
	*/
	class Frustum;
	/**
	*\~english
	*\brief
	*	Holds render buffers of any geometry buffers pass.
	*\~french
	*\brief
	*	Contient les buffer de rendu de n'importe quelle passe de geometry buffers.
	*/
	class GBuffer;
	/**
	*\~english
	*\brief
	*	Picking pass, using FBO.
	*\~french
	*\brief
	*	Passe de picking, utilisant les FBO.
	*/
	class Picking;
	/**
	*\~english
	*\brief
	*	Queue and command pool.
	*\~french
	*\brief
	*	Une queue et un command pool.
	*/
	struct QueueData;
	/**
	*\~english
	*\brief
	*	The culled render nodes for a specific scene.
	*\~french
	*\brief
	*	Les noeuds de rendu culled pour une scène spécifique.
	*/
	struct QueueCulledRenderNodes;
	/**
	*\~english
	*\brief
	*	The render nodes for a specific scene.
	*\~french
	*\brief
	*	Les noeuds de rendu pour une scène spécifique.
	*/
	struct QueueRenderNodes;
	/**
	*\~english
	*\brief
	*	The queue data for all queues of a specific family index.
	*\~french
	*\brief
	*	Les données de file pour toutes celles d'un family index spécifique.
	*/
	struct QueuesData;
	/**
	*\~english
	*\brief
	*	Ray representation
	*\remarks
	*	A ray is an origin and a direction in 3D.
	*\~french
	*\brief
	*	Représentation d'un rayon.
	*\remarks
	*	Un rayon est représentaté par une origine et une direction.
	*/
	class Ray;
	/**
	*\~english
	*\brief
	*	Holds data for device rendering.
	*\~french
	*\brief
	*	Contient les données pour le rendu sur device.
	*/
	struct RenderDevice;
	/**
	*\~english
	*\brief
	*	Holds render informations.
	*\~french
	*\brief
	*	Contient les informations de rendu.
	*/
	struct RenderInfo;
	/**
	*\~english
	*\brief
	*	Render loop base class.
	*\~french
	*\brief
	*	Classe de base d'une boucle de rendu.
	*/
	class RenderLoop;
	/**
	*\~english
	*\brief
	*	Asynchronous render loop.
	*\~french
	*\brief
	*	Boucle de rendu asynchrone.
	*/
	class RenderLoopAsync;
	/**
	*\~english
	*\brief
	*	Synchronous render loop.
	*\~french
	*\brief
	*	Boucle de rendu synchrone.
	*/
	class RenderLoopSync;
	/**
	*\~english
	*\brief
	*	The render nodes for a specific scene.
	*\~french
	*\brief
	*	Les noeuds de rendu pour une scène spécifique.
	*/
	template< typename NodeType, typename MapType >
	struct RenderNodesT;
	/**
	*\~english
	*\brief
	*	A pipeline to render specific nodes from a render pass.
	*\~french
	*\brief
	*	Un pipeline pour le rendu de noeuds d'une passe de rendu.
	*/
	class RenderPipeline;
	/**
	*\~english
	*\brief
	*	A render queue, processing render nodes.
	*\~french
	*\brief
	*	Une file de rendu, agissant sur des noeuds de rendu.
	*/
	class RenderQueue;
	/**
	*\~english
	*\brief
	*	Links Castor3D to the rendering API.
	*\~french
	*\brief
	*	Fait le lien entre Castor3D et l'API de rendu.
	*/
	class RenderSystem;
	/**
	*\~english
	*\brief
	*	Render target class
	*\remarks
	*	A render target draws a scene in a frame buffer that can then be used by a window to have a direct render, or a texture to have offscreen rendering
	*\~french
	*\brief
	*	Classe de cible de rendu (render target)
	*\remarks
	*	Une render target dessine une scène dans un tampon d'image qui peut ensuite être utilisé dans une fenêtre pour un rendu direct, ou une texture pour un rendu hors écran
	*/
	class RenderTarget;
	/**
	*\~english
	*\brief
	*	Render window representation.
	*\remark
	*	Manages a window where you can render a scene.
	*\~french
	*\brief
	*	Implémentation d'une fenêtre de rendu.
	*\remark
	*	Gère une fenêtre dans laquelle une scène peut être rendue
	*/
	class RenderWindow;
	/**
	*\~english
	*\brief
	*	Render pass base class.
	*\~french
	*\brief
	*	Classe de base d'une passe de rendu.
	*/
	class SceneRenderPass;
	/**
	*\~english
	*\brief
	*	Shadow mapping implementation.
	*\~french
	*\brief
	*	Implémentation du mappage d'ombres.
	*/
	class ShadowMap;
	/**
	*\~english
	*\brief
	*	A render viewport.
	*\~french
	*\brief
	*	Un viewport de rendu.
	*/
	class Viewport;

	CU_DeclareCUSmartPtr( castor3d, Picking, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderDevice, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderLoop, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderPipeline, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderQueue, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderSystem, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderTarget, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, Viewport, C3D_API );

	using RenderWindowPtr = std::unique_ptr< RenderWindow >;

	CU_DeclareVector( IntermediateView, IntermediateView );

	using RenderQueueArray = std::vector< std::reference_wrapper< RenderQueue > >;

	using ShadowMapRefIds = std::pair< std::reference_wrapper< ShadowMap >, UInt32Array >;
	using ShadowMapRefArray = std::vector< ShadowMapRefIds >;
	using ShadowMapLightTypeArray = std::array< ShadowMapRefArray, size_t( LightType::eCount ) >;

	struct TechniqueQueues
	{
		RenderQueueArray queues;
		ShadowMapLightTypeArray shadowMaps;
	};

	struct CpuUpdater
	{
		CpuUpdater()
		{
		}

		RenderQueueArray * queues{ nullptr };
		Camera * camera{ nullptr };
		SceneNode const * node{ nullptr };
		Light * light{ nullptr };
		Viewport * viewport{ nullptr };
		uint32_t index{ 0u };
		uint32_t combineIndex{ 0u };
		castor::Point2f jitter;
		bool voxelConeTracing{ false };
		castor::Point3f gridCenter{};
		float cellSize{ 0.0f };
		castor::Milliseconds tslf;
		castor::Milliseconds time;
		castor::Milliseconds total;
		std::vector< TechniqueQueues > techniquesQueues;
		castor::Point2f bandRatio;
		castor::Matrix4x4f bgMtxModl;
		castor::Matrix4x4f bgMtxView;
		castor::Matrix4x4f bgMtxProj;
	};

	struct GpuUpdater
	{
		GpuUpdater( RenderDevice const & device
			, RenderInfo & info )
			: device{ device }
			, info{ info }
		{
		}

		RenderDevice const & device;
		RenderInfo & info;
		castor::Point2f jitter;
		Scene * scene{ nullptr };
		Camera * camera{ nullptr };
		uint32_t index{ 0u };
		bool voxelConeTracing{ false };
		FramePassTimer * timer{ nullptr };
		castor::Milliseconds time;
		castor::Milliseconds total;
	};
	//@}

	C3D_API uint32_t getSafeBandsSize( castor::Size const & size );
	C3D_API uint32_t getSafeBandSize( castor::Size const & size );
	C3D_API castor::Size getSafeBandedSize( castor::Size const & size );
	C3D_API VkExtent3D getSafeBandedExtent3D( castor::Size const & size );
	C3D_API castor::Angle getSafeBandedFovY( castor::Angle const & fovY
		, castor::Size const & size );
	C3D_API float getSafeBandedAspect( float aspect
		, castor::Size const & size );

}

CU_DeclareExportedOwnedBy( C3D_API, castor3d::RenderSystem, RenderSystem )
CU_DeclareExportedOwnedBy( C3D_API, castor3d::RenderDevice, RenderDevice )

#endif
