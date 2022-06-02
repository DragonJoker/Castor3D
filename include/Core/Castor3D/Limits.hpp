/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Limits_H___
#define ___C3D_Limits_H___

#include <cstdint>

namespace castor3d
{
	/**
	*\name
	*	Materials.
	*/
	//@{
	// Phong/Blinn-Phong
	static uint32_t constexpr MaxPassLayers = 8u;
	// Phong/Blinn-Phong
	static float constexpr MaxPhongShininess = 256.0f;
	// Edges (when displayed)
	static float constexpr MinMaterialEdgeWidth = 0.001f;
	static float constexpr MaxMaterialEdgeWidth = 1000.0f;
	// SSS transmittance factors count
	static uint32_t constexpr TransmittanceProfileSize = 10u;
	//@}
	/**
	*\name
	*	Render nodes.
	*/
	//@{
	// Maximum pipelines count.
	static VkDeviceSize constexpr MaxPipelines = 1'000ull;
	// Maximum nodes per Pipeline Nodes buffer.
	static VkDeviceSize constexpr MaxNodesPerPipeline = 25'000ull;
	// Maximum nodes per meshlet indirect commands buffer.
	static VkDeviceSize constexpr MaxSubmeshMeshletDrawIndirectCommand = MaxNodesPerPipeline;
	// Maximum nodes per indexed indirect commands buffer.
	static VkDeviceSize constexpr MaxSubmeshIdxDrawIndirectCommand = MaxNodesPerPipeline;
	// Maximum nodes per non-indexed indirect commands buffer.
	static VkDeviceSize constexpr MaxSubmeshNIdxDrawIndirectCommand = MaxNodesPerPipeline;
	// Maximum billboard nodes per indirect commands buffer.
	static VkDeviceSize constexpr MaxBillboardDrawIndirectCommand = MaxNodesPerPipeline;
	// Maximum objects nodes count in a scene (submesh or billboards).
	static VkDeviceSize constexpr MaxObjectNodesCount = 250'000ull;
	//@}
	/**
	*\name
	*	PBR.
	*/
	//@{
	static uint32_t constexpr RadianceMapSize = 32u;
	static uint32_t constexpr PrefilteredEnvironmentMapSize = 128u;
	static uint32_t constexpr PrefilteredBrdfMapSize = 512u;
	static uint32_t constexpr MaxIblReflectionLod = 4u;
	//@}
	/**
	*\name
	*	Environment maps.
	*/
	//@{
	static uint32_t constexpr MaxEnvironmentMapCount = 10u;
	static uint32_t constexpr EnvironmentMapSize = 128u;
	//@}
	/**
	*\name
	*	Light Propagation Volumes.
	*/
	//@{
	static uint32_t constexpr LpvMaxPropagationSteps = 8u;
	static uint32_t constexpr LpvMaxCascadesCount = 3u;
	//@}
	/**
	*\name
	*	Voxel Cone Tracing.
	*/
	//@{
	static uint32_t constexpr VctMaxTextureSize = 512u;
	static uint32_t constexpr VctMaxDiffuseCones = 16u;
	//@}
	/**
	*\name
	*	Directional Light Shadows.
	*/
	//@{
	static uint32_t constexpr ShadowMapDirectionalTextureSize = 2'048u;
	static uint32_t constexpr DirectionalMaxCascadesCount = 4u;
	//@}
	/**
	*\name
	*	Point Light Shadows.
	*/
	//@{
	static uint32_t constexpr ShadowMapPointTextureSize = 512u;
	static uint32_t constexpr PointShadowMapCount = 6u;
	//@}
	/**
	*\name
	*	Spot Light Shadows.
	*/
	//@{
	static uint32_t constexpr ShadowMapSpotTextureSize = 512u;
	static uint32_t constexpr SpotShadowMapCount = 10u;
	//@}
	/**
	*\name
	*	Shader buffers.
	*/
	//@{
	// Light Buffer.
	static uint32_t constexpr MaxLightsCount = 2'000u;
	// Pass Buffer.
	static uint32_t constexpr MaxMaterialsCount = 2'000u;
	static uint32_t constexpr MaxSssProfilesCount = 100u;
	static int constexpr MaxMaterialComponentsCount = 8;
	// Texture Configuration Buffer.
	static uint32_t constexpr MaxTextureConfigurationCount = 4'000u;
	static int constexpr MaxTextureConfigurationComponentsCount = 12;
	// Texture Animation Buffer.
	static uint32_t constexpr MaxTextureAnimationCount = MaxTextureConfigurationCount;
	static int constexpr MaxTextureAnimationComponentsCount = 4;
	//@}
	/**
	*\name
	*	Uniform/Storage buffers.
	*/
	//@{
	// Nodes IDs buffer
	static uint32_t constexpr MaxObjectIDNodesCount = 10'000u;
	// Morphing Data buffer
	static uint32_t constexpr MaxMorphingDataCount = 1'000u;
	// Skinning Data buffer
	static uint32_t constexpr MaxSkinningDataCount = 1'000u;
	//@}
	/**
	*\name
	*	Other.
	*/
	//@{
	// Max chars count for text overlay buffer pool
	static uint32_t constexpr MaxOverlayCharsPerBuffer = 600u;
	// Max count for (border)panel overlay buffer pool
	static uint32_t constexpr MaxOverlayPanelsPerBuffer = 100u;
	// Base count for objects buffers pool
	static uint32_t constexpr BaseObjectPoolBufferCount = 1'000'000u;
	// Max bones in a skeleton
	static uint32_t constexpr MaxBonesCount = 400u;
	// Max morph target in a mesh
	static uint32_t constexpr MaxMorphTargets = 100u;
	// Number of mip levels in the linearised depth buffer
	static uint32_t constexpr MaxLinearizedDepthMipLevel = 5u;
	// The picking area size
	static uint32_t constexpr PickingAreaWidth = 32u;
	static uint32_t constexpr MaxRsmRange = 800u;
	static uint32_t constexpr MaxBindlessResources = 16536u;
	// Max supported texture coordinates sets.
	static uint32_t constexpr MaxTextureCoordinatesSets = 4u;
	//@}
	/**
	*\name
	*	Meshlet.
	*/
	//@{
	static uint32_t MaxMeshletVertexCount = 64u;
	static uint32_t MaxMeshletTriangleCount = 124u;
	//@}
}

#endif
