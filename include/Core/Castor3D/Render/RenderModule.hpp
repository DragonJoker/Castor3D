/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderModule_H___
#define ___C3D_RenderModule_H___

#include "Castor3D/Buffer/BufferModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <RenderGraph/Attachment.hpp>
#include <RenderGraph/BufferData.hpp>
#include <RenderGraph/BufferViewData.hpp>
#include <RenderGraph/ImageData.hpp>
#include <RenderGraph/ImageViewData.hpp>

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Descriptor/WriteDescriptorSet.hpp>

#include <functional>

namespace c3d
{
	class CameraUbo;

	/**@name Render */
	//@{

	String const RenderTypeUndefined = cuT( "Undefined" );

	using RenderPassTypeID = uint16_t;

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
			Point2f position;
		};

		Array< Vertex, 6u > vertex;
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
			Point2f position;
			Point2f texture;
		};

		Array< Vertex, 6u > vertex;
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
				Point3f position;
			};

			Array< Vertex, 6u > vertex;
		};

		Array< Quad, 6u > faces;
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
				Point3f position;
				Point2f texture;
			};

			Array< Vertex, 6u > vertex;
		};

		Array< Quad, 6u > faces;
	};
	/**
	*\~english
	*\brief
	*	Draw constants for one render node.
	*\~french
	*\brief
	*	Données de dessin pour un noeud de rendu.
	*/
	struct DrawConstants
	{
		uint32_t pipelineId;
		int32_t drawId;
	};
	/**
	*\~english
	*\brief
	*	Draw constants for one overlay.
	*\~french
	*\brief
	*	Données de dessin pour un overlay.
	*/
	struct OverlayDrawConstants
	{
		uint32_t pipelineId;
		int32_t drawId;
	};
	/**
	*\~english
	*\brief
	*	The render pass UBOs binding index.
	*\~french
	*\brief
	*	L'index de binding des UBOs des passes de rendu.
	*/
	enum class GlobalBuffersIdx
		: uint32_t
	{
		eCamera = 0,
		eRender = 1,
		eScene = 2,
		eObjectsNodeID = 3,
		eModelsData = 4,
		eMaterials = 5,
		eSssProfiles = 6,
		eSssDiffusionProfiles = 7,
		eTexConfigs = 8,
		eTexAnims = 9,
		eBillboardsData = 10,
		CU_ScopedEnumBounds( eCamera, eBillboardsData ),
	};
	/**
	*\~english
	*\brief
	*	The render pass mesh buffers binding index.
	*\~french
	*\brief
	*	L'index de binding des buffers de mesh des passes de rendu.
	*/
	enum class MeshBuffersIdx
		: uint32_t
	{
		eCullData = 0,
		eMeshlets = 1,
		ePosition = 2,
		eNormal = 3,
		eTangent = 4,
		eBitangent = 5,
		eTexcoord0 = 6,
		eTexcoord1 = 7,
		eTexcoord2 = 8,
		eTexcoord3 = 9,
		eColour = 10,
		ePassMasks = 11,
		eVelocity = 12,
		eInstances = 13,
		CU_ScopedEnumBounds( eMeshlets, eInstances ),
	};
	/**
	*\~english
	*\brief
	*	Frustum corners enumeration
	*\~french
	*\brief
	*	Enumération des coins d'un frustum
	*/
	enum class Corner
	{
		//!\~english	Far left bottom corner.
		//!\~french		Coin éloigné bas gauche.
		eFarLeftBottom = 0,
		//!\~english	Far left top corner.
		//!\~french		Coin éloigné haut gauche.
		eFarLeftTop = 1,
		//!\~english	Far right top corner.
		//!\~french		Coin éloigné haut droit.
		eFarRightTop = 2,
		//!\~english	Far right bottom corner.
		//!\~french		Coin éloigné bas droit.
		eFarRightBottom = 3,
		//!\~english	Near left bottom corner.
		//!\~french		Coin proche bas gauche.
		eNearLeftBottom = 4,
		//!\~english	Near left top corner.
		//!\~french		Coin proche haut gauche.
		eNearLeftTop = 5,
		//!\~english	Near right top corner.
		//!\~french		Coin proche haut droit.
		eNearRightTop = 6,
		//!\~english	Near right bottom corner.
		//!\~french		Coin proche bas droit.
		eNearRightBottom = 7,
		CU_ScopedEnumBounds( eFarLeftBottom, eNearRightBottom )
	};
	C3D_API String getName( Corner value );
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
		eNear = 0,
		//!\~english	Far plane.
		//!\~french		Plan éloigné.
		eFar = 1,
		//!\~english	Left plane.
		//!\~french		Plan gauche.
		eLeft = 2,
		//!\~english	Right plane.
		//!\~french		Plan droit.
		eRight = 3,
		//!\~english	Top plane.
		//!\~french		Plan haut.
		eTop = 4,
		//!\~english	Bottom plane.
		//!\~french		Plan bas.
		eBottom = 5,
		CU_ScopedEnumBounds( eNear, eBottom )
	};
	C3D_API String getName( FrustumPlane value );
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
		eNone = 0,
		eSubmesh = 1,
		eBillboard = 2,
		CU_ScopedEnumBounds( eNone, eBillboard )
	};
	C3D_API String getName( PickNodeType value );
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
		eWindow = 0,
		eTexture = 1,
		CU_ScopedEnumBounds( eWindow, eTexture )
	};
	C3D_API String getName( TargetType value );
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
		eUndefined = 0,
		eOrtho = 1,
		ePerspective = 2,
		eInfinitePerspective = 3,
		eFrustum = 4,
		CU_ScopedEnumBounds( eUndefined, eFrustum )
	};
	C3D_API String getName( ViewportType value );
	/**
	*\~english
	*\brief
	*	The render pass shader flags.
	*\~french
	*\brief
	*	Les indicateurs de shader des passes.
	*/
	enum class ShaderFlag
		: uint32_t
	{
		eNone = 0x00000000,
		//!\~english	Shader using normals.
		//\~french		Shader utilisant les normales.
		eNormal = 0x00000001,
		//!\~english	Shader using tangents.
		//\~french		Shader utilisant les tangentes.
		eTangent = 0x00000002,
		//!\~english	Shader using tangent space.
		//\~french		Shader utilisant l'espace tangent.
		eTangentSpace = eTangent | eNormal,
		//!\~english	Shader using velocity.
		//\~french		Shader utilisant la vélocité.
		eVelocity = 0x00000004,
		//!\~english	Shader using world space positions.
		//\~french		Shader utilisant les positions en espace monde.
		eWorldSpace = 0x00000008,
		//!\~english	Shader using view space positions.
		//\~french		Shader utilisant les positions en espace vue.
		eViewSpace = 0x00000010,
		//!\~english	Shader for the depth pre-pass.
		//\~french		Shader pour la pré-passe de profondeur.
		eDepth = 0x00000020,
		//!\~english	Shader for the visibility pre-pass.
		//\~french		Shader pour la pré-passe de visibilité.
		eVisibility = 0x00000040,
		//!\~english	Shader for the picking pass.
		//\~french		Shader pour la passe de picking.
		ePicking = 0x00000080,
		//!\~english	Shader supporting lighting.
		//\~french		Shader supportant les éclairages.
		eLighting = 0x00000100,
		//!\~english	Shader used to render a shadow map for directional light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière directionnalle.
		eShadowMapDirectional = 0x00000200,
		//!\~english	Shader used to render a shadow map for spot light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière projecteur.
		eShadowMapSpot = 0x00000400,
		//!\~english	Shader used to render a shadow map for point light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière omnidirectionnelle.
		eShadowMapPoint = 0x00000800,
		//!\~english	Writes to Variance shadow map.
		//\~french		Ecrit dans la Variance shadow map.
		eVsmShadowMap = 0x00001000,
		//!\~english	Writes to Reflective shadow map.
		//\~french		Ecrit dans la Reflective shadow map.
		eRsmShadowMap = 0x00002000,
		//!\~english	Shader used to render an environment map.
		//\~french		Shader utilisé pour dessiner une texture d'environnement.
		eEnvironmentMapping = 0x00004000,
		//!\~english	Shader using opacity.
		//\~french		Shader utilisant l'opacité.
		eOpacity = 0x00008000,
		//!\~english	Uses a geometry shader.
		//\~french		Utilise un geometry shader.
		eGeometry = 0x00010000,
		//!\~english	Uses tessellation shaders.
		//\~french		Utilise des tessellation shaders.
		eTessellation = 0x00020000,
		//!\~english	Forces texcoords binding.
		//\~french		Force le binding des UV.
		eForceTexCoords = 0x00040000,
		//!\~english	Shader uses the colour component.
		//\~french		Le shader utilise la composante couleur.
		eColour = 0x00080000,
		//!\~english	All shader flags.
		//\~french		Tous les indicateurs de shader.
		eAll = 0x000FFFFF,
	};
	CU_ImplementFlags( ShaderFlag )
	/**
	*\~english
	*\brief
	*	Determines what kind of lighting the pass handles.
	*\~french
	*\brief
	*	Détermine le type d'éclairage géré par la passe.
	*/
	enum class DeferredLightingFilter
		: uint8_t
	{
		eIgnore = 0,
		//!\~english	Defers lighting to a next pass.
		//\~french		Diffère l'éclairage à une autre passe.
		eDeferLighting = 1,
		//!\~english	Only processes deferred lighting.
		//\~french		N'effectue que l'éclairage différé.
		eDeferredOnly = 2,
		CU_ScopedEnumBounds( eIgnore, eDeferredOnly )
	};
	/**
	*\~english
	*\brief
	*	Determines what kind of lighting the pass handles.
	*\~french
	*\brief
	*	Détermine le type d'éclairage géré par la passe.
	*/
	enum class ParallaxOcclusionFilter
		: uint8_t
	{
		eIgnore = 0,
		//!\~english	Only process nodes without parallax occlusion.
		//\~french		Prend en compte uniquement les noeuds sans parallax occlusion.
		eDisabled = 1,
		//!\~english	Only process nodes with parallax occlusion.
		//\~french		Prend en compte uniquement les noeuds avec parallax occlusion.
		eEnabled = 2,
		CU_ScopedEnumBounds( eIgnore, eEnabled )
	};
	/**
	*\~english
	*\brief
	*	Filters out the opacity channels a render pass doesn't use.
	*\~french
	*\brief
	*	Filte les canaux d'opacité que la render pass n'utilise pas.
	*/
	enum class RenderFilter
		: uint8_t
	{
		eNone = 0x00,
		//!\~english	Alpha blended opacity.
		//\~french		Opacité de mélange d'alpha.
		eAlphaBlend = 0x01 << 0u,
		//!\~english	Alpha test opacity.
		//\~french		Opacité de test d'alpha.
		eAlphaTest = 0x01 << 1u,
		//!\~english	Materials with refraction.
		//\~french		Matériaux avec refraction.
		eTransmission = 0x01 << 2u,
		//!\~english	Fully opaque (no opacity at all).
		//\~french		Complètement opaque (pas d'opacité du tout).
		eOpaque = 0x01 << 3u,
	};
	CU_ImplementFlags( RenderFilter )

	C3D_API String getName( RenderFilter value );
	/**
	*\~english
	*\brief
	*	The 3D objects counts.
	*\~french
	*\brief
	*	Les comptes des objets 3D.
	*/
	struct RenderCounts
	{
		//!\~english	The vertex count.
		//!\~french		Le nombre de sommets.
		uint32_t vertexCount{};
		//!\~english	The face count.
		//!\~french		Le nombre de faces.
		uint32_t faceCount{};
		//!\~english	The object count.
		//!\~french		Le nombre d'objets.
		uint32_t objectCount{};
		//!\~english	The billboards count.
		//!\~french		Le nombre de billboards.
		uint32_t billboardCount{};
		//!\~english	The light sources count.
		//!\~french		Le nombre de source lumineuses.
		uint32_t lightsCount{};
	};
	/**
	*\~english
	*\brief
	*	RenderGraph's buffer view binding base class.
	*\~french
	*\brief
	*	Classe de base d'un binding d'un buffer RenderGraph.
	*/
	struct BufferBase;
	/**
	*\~english
	*\brief
	*	Child class of c3d::BufferBase, for untyped buffers.
	*\~french
	*\brief
	*	Classe fille de c3d::BufferBase, pour les buffers non typés.
	*/
	struct Buffer;
	/**
	*\~english
	*\brief
	*	Child class of c3d::BufferBase, for typed buffers.
	*\~french
	*\brief
	*	Classe fille de c3d::BufferBase, pour les buffers typés.
	*/
	template< typename DataT >
	struct BufferT;
	/**
	*\~english
	*\brief
	*	RenderGraph's image view binding.
	*\~french
	*\brief
	*	Binding d'une image RenderGraph.
	*/
	struct Texture;
	/**
	*\~english
	*\brief
	*	Holds minimal data for an intermediate view.
	*\~french
	*\brief
	*	Contient les données minimales pour une vue intermédiaire.
	*/
	struct IntermediateView;
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
	*	Holds instance and physical devices.
	*\~french
	*\brief
	*	Contient l'instance et les physical devices.
	*/
	struct Renderer;
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
	*	Render technique class
	*\remarks
	*	A render technique is the description of a way to render a render target
	*\~french
	*\brief
	*	Classe de technique de rendu
	*\remarks
	*	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class RenderTechnique;
	/**
	\~english
	\brief		Render technique pass base class.
	\~french
	\brief		Classe de base d'une passe de technique de rendu.
	*/
	class RenderTechniquePass;
	/**
	\~english
	\brief		Render technique nodes pass base class.
	\~french
	\brief		Classe de base d'une passe de technique de rendu de noeuds.
	*/
	class RenderTechniqueNodesPass;
	/**
	*\~english
	*\brief
	*	Render technique effect visitor base class.
	*\~french
	*\brief
	*	Classe de base d'un visiteur de technique de rendu.
	*/
	class RenderTechniqueVisitor;
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
	*	RenderNodesPass creation data.
	*\~french
	*\brief
	*	Données de création d'une RenderNodesPass.
	*/
	struct RenderNodesPassDesc;
	/**
	*\~english
	*\brief
	*	Nodes pass base class.
	*\~french
	*\brief
	*	Classe de base d'une passe de noeuds.
	*/
	class NodesPass;
	/**
	*\~english
	*\brief
	*	Rasterized nodes pass base class.
	*\~french
	*\brief
	*	Classe de base d'une passe de rastérisation de noeuds.
	*/
	class RenderNodesPass;
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
	/**
	*\~english
	*\brief
	*	Node used to render billboards.
	*\~french
	*\brief
	*	Noeud utilisé pour le dessin de billboards.
	*/
	struct BillboardRenderNode;
	/**
	*\~english
	*\brief
	*	Node used to render a submesh.
	*\~french
	*\brief
	*	Noeud utilisé pour le dessin un submesh.
	*/
	struct SubmeshRenderNode;
	/**
	*\~english
	*\brief
	*	Allocation counters for device buffers.
	*\~french
	*\brief
	*	Compteurs d'allocation pour les buffers du device.
	*/
	struct DeviceCounts
	{
		AllocationStats bufferAllocated;
		AllocationStats vertexAllocated;
		AllocationStats indexAllocated;
		AllocationStats geometryAllocated;
		AllocationStats uboAllocated;
		Vector< Pair< MemChunk, String > > uboAllocations;

		C3D_API explicit DeviceCounts( RenderDevice const & device );
	};

	using RenderQueueArray = Vector< ReferenceWrapper< RenderQueue > >;
	using TextureArray = Vector< Texture >;
	using TexturePtrArray = Vector< Texture * >;

	using ShadowMapRefIds = Pair< ReferenceWrapper< ShadowMap >, UInt32Array >;
	using ShadowMapRefArray = Vector< ShadowMapRefIds >;
	using ShadowMapLightTypeArray = Array< ShadowMapRefArray, size_t( LightType::eCount ) >;
	using LightIdArray = Vector< Pair< LightInstance *, uint32_t > >;

	template< typename NodeT >
	struct CulledNodeT
	{
		explicit CulledNodeT( NodeT const * pnode = {}
			, uint32_t pinstanceCount = {}
			, bool pvisible = {} )
			: node{ pnode }
			, instanceCount{ pinstanceCount }
			, visible{ pvisible }

		{
		}

		NodeT const * node;
		uint32_t instanceCount;
		uint32_t indexCount;
		uint32_t vertexCount;
		bool visible;
	};

	template< typename NodeT >
	using CulledNodePtrT = RawUniquePtr< CulledNodeT< NodeT > >;

	template< typename NodeT, template< typename NodeU > typename NodeWrapperT = CulledNodeT >
	using NodeArrayT = Vector< NodeWrapperT< NodeT > >;

	struct PipelineAndID
	{
		RenderPipeline * pipeline;
		uint16_t id;
	};

	struct ShadowMapLightIds
	{
		explicit ShadowMapLightIds( ReferenceWrapper< ShadowMap > shadowMap
			, LightIdArray ids = {} )
			: shadowMap{ c3d::move( shadowMap ) }
			, ids{ c3d::move( ids ) }
		{
		}

		ReferenceWrapper< ShadowMap > shadowMap;
		LightIdArray ids;
	};
	using ShadowMapLightIdArray = Vector< ShadowMapLightIds >;
	using ShadowMapLightArray = Array< ShadowMapLightIdArray, size_t( LightType::eCount ) >;

	struct TechniqueQueues
	{
		RenderQueueArray queues;
		ShadowMapLightTypeArray shadowMaps;
		ShadowBuffer const * shadowBuffer;
	};

	enum class TechniquePassEvent
	{
		eBeforeDepth = 0,
		eBeforeBackground = 1,
		eBeforeOpaque = 2,
		eBeforeTransparent = 3,
		eBeforePostEffects = 4,
		CU_ScopedEnumBounds( eBeforeDepth, eBeforePostEffects )
	};

	using NodesPassChangeSignalFunction = Function< void( NodesPass const & ) >;
	using NodesPassChangeSignal = SignalT< NodesPassChangeSignalFunction >;
	using NodesPassChangeSignalConnection = ConnectionT< NodesPassChangeSignal >;

	using TechniquePassVector = Vector< RenderTechniqueNodesPass * >;
	using TechniquePasses = Array< TechniquePassVector, size_t( TechniquePassEvent::eCount ) >;

	struct RenderPassRegisterInfo
	{
		using Creator = Function< crg::FramePassArray( RenderDevice const &
			, RenderTechnique &
			, TechniquePasses &
			, crg::ResourcesCache & ) >;

		RenderPassRegisterInfo( String pname
			, Creator pcreate
			, TechniquePassEvent pevent
			, RenderPassTypeID pid = {} )
			: name{ c3d::move( pname ) }
			, create{ c3d::move( pcreate ) }
			, event{ c3d::move( pevent ) }
			, id{ pid }
		{
		}

		String name;
		Creator create;
		TechniquePassEvent event;
		RenderPassTypeID id;
	};

	struct CpuUpdater
	{
		CpuUpdater() = default;

		RenderQueueArray * queues{ nullptr };
		Scene * scene{ nullptr };
		Camera * camera{ nullptr };
		SceneNode const * node{ nullptr };
		LightInstance * light{ nullptr };
		Viewport * viewport{ nullptr };
		uint32_t index{ 0u };
		uint32_t combineIndex{ 0u };
		uint32_t debugIndex{ 0u };
		Point2f jitter;
		bool voxelConeTracing{ false };
		Point3f gridCenter{};
		float cellSize{ 0.0f };
		Milliseconds tslf{};
		Milliseconds time{};
		Milliseconds total{};
		Vector< TechniqueQueues > techniquesQueues{};
		Point2f bandRatio{};
		Matrix4x4f bgMtxModl{};
		Matrix4x4f bgMtxView{};
		Matrix4x4f bgMtxProj{};
		bool isSafeBanded{ true };
		Texture * targetImage{};
		Size renderSize{};

		struct DirtyObjects
		{
			DirtyObjects() = default;

			bool isEmpty()const
			{
				return dirtyNodes.empty()
					&& dirtyGeometries.empty()
					&& dirtyBillboards.empty()
					&& dirtyLights.empty()
					&& dirtyCameras.empty();
			}

			Vector< SceneNode * > dirtyNodes{};
			Vector< Geometry * > dirtyGeometries{};
			Vector< BillboardBase * > dirtyBillboards{};
			Vector< LightInstance * > dirtyLights{};
			Vector< Camera * > dirtyCameras{};
		};
		Map< Scene const *, DirtyObjects > dirtyScenes;
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
		Point2f jitter{};
		Scene * scene{ nullptr };
		Camera * camera{ nullptr };
		LightInstance * light{ nullptr };
		uint32_t index{ 0u };
		bool voxelConeTracing{ false };
		FramePassTimer * timer{ nullptr };
		Milliseconds time{};
		Milliseconds total{};
		Size renderSize{};
	};

	struct TargetDebugConfig
	{
		uint32_t intermediateImageIndex{ 0u };

		void resetImages()
		{
			intermediateImageIndex = 0u;
			m_intermediateImageNames.clear();
		}

		uint32_t registerImage( String name )
		{
			auto it = std::find( m_intermediateImageNames.begin()
				, m_intermediateImageNames.end()
				, name );

			if ( it == m_intermediateImageNames.end() )
			{
				m_intermediateImageNames.emplace_back( name );
				it = std::next( m_intermediateImageNames.begin(), ptrdiff_t( m_intermediateImageNames.size() - 1u ) );
			}

			return uint32_t( std::distance( m_intermediateImageNames.begin(), it ) );
		}

		StringArray const & getIntermediateImages()const noexcept
		{
			return m_intermediateImageNames;
		}

	private:
		StringArray m_intermediateImageNames;
	};
	//@}

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Frustum, C3D_API );
	CU_DeclareSmartPtr( c3d, NodesPass, C3D_API );
	CU_DeclareSmartPtr( c3d, Picking, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderDevice, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderLoop, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderNodesPass, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderPipeline, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderQueue, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderSystem, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderTarget, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderTechnique, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderTechniquePass, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderWindow, C3D_API );
	CU_DeclareSmartPtr( c3d, Viewport, C3D_API );
	CU_DeclareSmartPtr( c3d, Texture, C3D_API );
	CU_DeclareSmartPtr( c3d, Buffer, C3D_API );

	CU_DeclareVector( IntermediateView, IntermediateView );
	/** @endcond */

	struct RenderWindowDesc
	{
		String name;
		RenderTargetRPtr renderTarget{};
		bool enableVSync{};
		bool fullscreen{};
		bool allowHdr{};
	};

	C3D_API uint32_t getSafeBandsSize( Size const & size );
	C3D_API uint32_t getSafeBandSize( Size const & size );
	C3D_API Size getSafeBandedSize( Size const & size );
	C3D_API Extent3D getSafeBandedExtent3D( Size const & size );
	C3D_API Angle getSafeBandedFovY( Angle const & fovY
		, Size const & size );
	C3D_API float getSafeBandedAspect( float aspect
		, Size const & size );
	C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, ImageSubresourceRange const & range
		, ImageLayout sourceLayout
		, ImageLayout destinationLayout
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily );
	C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, ImageSubresourceRange const & range
		, ImageLayout srcLayout
		, ImageLayout dstLayout
		, AccessFlags srcAccessFlags
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily );
	C3D_API VkImageMemoryBarrier makeLayoutTransition( VkImage image
		, ImageSubresourceRange const & range
		, ImageLayout srcLayout
		, ImageLayout dstLayout
		, AccessFlags srcAccessFlags
		, AccessFlags dstAccessMask
		, uint32_t srcQueueFamily
		, uint32_t dstQueueFamily );
	C3D_API void memoryBarrier( crg::RecordContext & context
		, VkCommandBuffer commandBuffer
		, BufferBase const & buffer
		, AccessState after
		, AccessState before );
	C3D_API ashes::Image makeImage( ashes::Device const & device
		, VkImage image
		, crg::ImageId data );
	C3D_API ashes::ImageView makeImageView( ashes::Image const & image
		, VkImageView view
		, crg::ImageViewId data );
	C3D_API ashes::ImageView makeTargetImageView( Texture const & texture );
	C3D_API ashes::ImageView makeSampledImageView( Texture const & texture );
	C3D_API ashes::ImageView makeWholeImageView( Texture const & texture );
	C3D_API void printGraph( crg::RunnableGraph const & graph );
	C3D_API VkSampler getSampler( ashes::Sampler const & sampler )noexcept;
	C3D_API VkImageView getImageView( ashes::ImageView const & view )noexcept;
	C3D_API VkBufferView getBufferView( ashes::BufferView const & view )noexcept;
	C3D_API DeviceSize getAlignedSize( ashes::UniformBuffer const & buffer )noexcept;
	C3D_API VkBuffer getBuffer( ashes::BufferBase const & buffer )noexcept;
	C3D_API VkBuffer getBuffer( ashes::UniformBuffer const & buffer )noexcept;
	C3D_API VkBuffer getBuffer( BufferBase const & buffer )noexcept;
	C3D_API DeviceSize getOffset( ashes::BufferView const & view )noexcept;
	C3D_API DeviceSize getRange( ashes::BufferView const & view )noexcept;
	C3D_API VkBufferUsageFlags getUsageFlags( ashes::BufferBase const & buffer )noexcept;
	C3D_API BufferSubresourceRange const & getSubresourceRange( BufferBase const & buffer )noexcept;
	using crg::getSubresourceRange;
	/**
	*\~english
	*\brief
	*	Writes the storage image view descriptor to the given writes.
	*\~french
	*\brief
	*	Ecrit le descripteur de la storage image dans les writes donnés.
	*/
	C3D_API void bindImage( VkImageView view
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	C3D_API void bindImage( ashes::ImageView const & view
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	C3D_API void bindImage( crg::RunnableGraph & graph
		, crg::ImageViewId const & view
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	/**
	*\~english
	*\brief
	*	Writes the image view and sampler descriptor to the given writes.
	*\~french
	*\brief
	*	Ecrit le descripteur de la vue et du sampler dans les writes donnés.
	*/
	C3D_API void bindTexture( VkImageView view
		, VkSampler sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	C3D_API void bindTexture( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	C3D_API void bindTexture( crg::RunnableGraph & graph
		, crg::ImageViewId const & view
		, VkSampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	/**
	*\~english
	*\brief
	*	Writes the buffer descriptor to the given writes.
	*\~french
	*\brief
	*	Ecrit le descripteur de buffer dans les writes donnés.
	*/
	C3D_API void bindBuffer( VkBuffer buffer
		, VkDeviceSize offset
		, VkDeviceSize range
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	C3D_API void bindBuffer( ashes::BufferBase const & buffer
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for storage image.
	*\param[in] view
	*	The image view.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour une storage image.
	*\param[in] view
	*	La vue sur l'image.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeStorageImageDescriptorWrite( VkImageView view
		, BindingT dstBinding
		, uint32_t dstArrayElement = 0u )
	{
		auto result = ashes::WriteDescriptorSet{ uint32_t( dstBinding )
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE };
		result.imageInfo.emplace_back() = { VkSampler{}, view, VK_IMAGE_LAYOUT_GENERAL };
		return result;
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for combined image sampler.
	*\param[in] view
	*	The image view.
	*\param[in] sampler
	*	The sampler.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un sampler et une image combinés.
	*\param[in] view
	*	La vue sur l'image.
	*\param[in] sampler
	*	Le sampler.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeImageViewDescriptorWrite( VkImageView view
		, VkSampler sampler
		, BindingT dstBinding
		, uint32_t dstArrayElement = 0u )
	{
		auto result = ashes::WriteDescriptorSet{ uint32_t( dstBinding )
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };
		result.imageInfo.emplace_back() = { sampler, view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL };
		return result;
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for uniform buffer.
	*\param[in] buffer
	*	The uniform buffer.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] elemOffset
	*	The offset, expressed in element count.
	*\param[in] elemRange
	*	The range, expressed in element count.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un uniform buffer.
	*\param[in] buffer
	*	L'uniform buffer.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] elemOffset
	*	L'offset, exprimé en nombre d'éléments.
	*\param[in] elemRange
	*	L'intervalle, exprimé en nombre d'éléments.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeUniformBufferDescriptorWrite( ashes::UniformBuffer const & buffer
		, BindingT dstBinding
		, VkDeviceSize elemOffset
		, VkDeviceSize elemRange
		, uint32_t dstArrayElement = 0u )
	{
		auto result = ashes::WriteDescriptorSet{ uint32_t( dstBinding )
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER };
		result.bufferInfo.emplace_back() = { getBuffer( buffer )
			, getAlignedSize( buffer ) * elemOffset
			, getAlignedSize( buffer ) * elemRange };
		return result;
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for storage buffer.
	*\param[in] storageBuffer
	*	The storage buffer.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] byteOffset
	*	The offset, expressed in bytes.
	*\param[in] byteRange
	*	The range, expressed in bytes.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un storage buffer.
	*\param[in] storageBuffer
	*	Le storage buffer.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] byteOffset
	*	L'offset, exprimé en octets.
	*\param[in] byteRange
	*	L'intervalle, exprimé en octets.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeStorageBufferDescriptorWrite( VkBuffer storageBuffer
		, BindingT dstBinding
		, VkDeviceSize byteOffset
		, VkDeviceSize byteRange
		, uint32_t dstArrayElement = 0u )
	{
		auto result = ashes::WriteDescriptorSet{ uint32_t( dstBinding )
			, dstArrayElement
			, 1u
			, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER };
		result.bufferInfo.emplace_back() = { storageBuffer
			, byteOffset
			, byteRange };
		return result;
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for storage buffer.
	*\param[in] storageBuffer
	*	The storage buffer.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] byteOffset
	*	The offset, expressed in bytes.
	*\param[in] byteRange
	*	The range, expressed in bytes.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un storage buffer.
	*\param[in] storageBuffer
	*	Le storage buffer.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] byteOffset
	*	L'offset, exprimé en octets.
	*\param[in] byteRange
	*	L'intervalle, exprimé en octets.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeStorageBufferDescriptorWrite( ashes::BufferBase const & storageBuffer
		, BindingT dstBinding
		, VkDeviceSize byteOffset
		, VkDeviceSize byteRange
		, uint32_t dstArrayElement = 0u )
	{
		return makeStorageBufferDescriptorWrite( getBuffer( storageBuffer )
			, dstBinding
			, byteOffset, byteRange
			, dstArrayElement );
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for storage buffer.
	*\param[in] storageBuffer
	*	The storage buffer.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un storage buffer.
	*\param[in] storageBuffer
	*	Le storage buffer.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeStorageBufferDescriptorWrite( BufferBase const & storageBuffer
		, BindingT dstBinding
		, uint32_t dstArrayElement = 0u )
	{
		auto & range = getSubresourceRange( storageBuffer );
		return makeStorageBufferDescriptorWrite( getBuffer( storageBuffer )
			, uint32_t( dstBinding )
			, range.offset, range.size
			, dstArrayElement );
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for storage buffer.
	*\param[in] storageBuffer
	*	The storage buffer.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] elemOffset
	*	The offset, expressed in element count.
	*\param[in] elemRange
	*	The range, expressed in element count.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour un storage buffer.
	*\param[in] storageBuffer
	*	Le storage buffer.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] elemOffset
	*	L'offset, exprimé en nombre d'éléments.
	*\param[in] elemRange
	*	L'intervalle, exprimé en nombre d'éléments.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename DataT, typename BindingT >
	ashes::WriteDescriptorSet makeStorageBufferDescriptorWrite( ashes::Buffer< DataT > const & storageBuffer
		, BindingT dstBinding
		, VkDeviceSize elemOffset
		, VkDeviceSize elemRange
		, uint32_t dstArrayElement = 0u )
	{
		return makeStorageBufferDescriptorWrite( storageBuffer.getBuffer()
			, dstBinding
			, elemOffset * sizeof( DataT )
			, elemRange * sizeof( DataT )
			, dstArrayElement );
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for buffer texel view.
	*\param[in] buffer
	*	The buffer.
	*\param[in] view
	*	The texel view.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour une texel view sur un buffer.
	*\param[in] buffer
	*	Le buffer.
	*\param[in] view
	*	La texel view.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename BindingT >
	ashes::WriteDescriptorSet makeTexelBufferDescriptorWrite( ashes::BufferBase const & buffer
		, ashes::BufferView const & view
		, BindingT dstBinding
		, uint32_t dstArrayElement = 0u )
	{
		auto result = ashes::WriteDescriptorSet{ uint32_t( dstBinding )
			, dstArrayElement
			, 1u
			, ( ( getUsageFlags( buffer ) & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
				? VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER
				: VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER ), };
		result.bufferInfo.emplace_back() = { getBuffer( buffer )
			, getOffset( view )
			, getRange( view ) };
		result.texelBufferView.push_back( getBufferView( view ) );
		return result;
	}
	/**
	*\~english
	*\brief
	*	Creates a descriptor write for buffer texel view.
	*\param[in] buffer
	*	The buffer.
	*\param[in] view
	*	The texel view.
	*\param[in] dstBinding
	*	The binding inside the descriptor set.
	*\param[in] dstArrayElement
	*	The array element index.
	*\~french
	*\brief
	*	Crée un descriptor write pour une texel view sur un buffer.
	*\param[in] buffer
	*	Le buffer.
	*\param[in] view
	*	La texel view.
	*\param[in] dstBinding
	*	Le binding dans le descriptor set.
	*\param[in] dstArrayElement
	*	L'indice dans le tableau d'éléments.
	*/
	template< typename DataT, typename BindingT >
	ashes::WriteDescriptorSet makeTexelBufferDescriptorWrite( ashes::Buffer< DataT > const & buffer
		, ashes::BufferView const & view
		, BindingT dstBinding
		, uint32_t dstArrayElement = 0u )
	{
		return makeTexelBufferDescriptorWrite( buffer.getBuffer()
			, view
			, dstBinding
			, dstArrayElement );
	}
}

CU_DeclareExportedOwnedBy( C3D_API, RenderSystem, RenderSystem )
CU_DeclareExportedOwnedBy( C3D_API, RenderDevice, RenderDevice )

#include "PipelineFlags.hpp"
#include "Texture.hpp"

#endif
