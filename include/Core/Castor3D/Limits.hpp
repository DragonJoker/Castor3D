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
	*	Shaders.
	*/
	//@{
	// Maximum different lighting models.
	static uint32_t constexpr MaxLightingModels = 64u;
	// Maximum different background models.
	static uint32_t constexpr MaxBackgroundModels = 64u;
	// The number of vec4s in the random buffer.
	static uint32_t constexpr RandomDataCount = 1024u;
	//@}
	/**
	*\name
	*	Materials.
	*/
	//@{
	// Maximum textures in a pass.
	static uint32_t constexpr MaxPassTextures = 8u;
	// Maximum passes in a material.
	static uint32_t constexpr MaxPassLayers = 16u;
	// Phong/Blinn-Phong
	static float constexpr MaxPhongShininess = 256.0f;
	// SSS transmittance factors count
	static uint32_t constexpr TransmittanceProfileSize = 10u;
	//@}
	/**
	*\name
	*	Render nodes.
	*/
	//@{
	// Maximum pipelines and buffer count.
	static uint64_t constexpr MaxPipelines = 25'000ull;
	// Maximum nodes per Pipeline Nodes buffer.
	static uint64_t constexpr MaxNodesPerPipeline = 1'000ull;
	// Maximum nodes per meshlet indirect commands buffer.
	static uint64_t constexpr MaxSubmeshMeshletDrawIndirectCommand = MaxPipelines;
	// Maximum nodes per indexed indirect commands buffer.
	static uint64_t constexpr MaxSubmeshIdxDrawIndirectCommand = MaxPipelines;
	// Maximum nodes per non-indexed indirect commands buffer.
	static uint64_t constexpr MaxSubmeshNIdxDrawIndirectCommand = MaxPipelines;
	// Maximum billboard nodes per indirect commands buffer.
	static uint64_t constexpr MaxBillboardDrawIndirectCommand = MaxPipelines;
	// Maximum objects nodes count in a scene (submesh or billboards).
	static uint64_t constexpr MaxObjectNodesCount = 250'000ull;
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
	static uint32_t constexpr EnvironmentMipLevels = 8u;
	static uint32_t constexpr EnvironmentMapSize = 0x0001u << ( EnvironmentMipLevels - 1u );
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
	*	PCF Filtering.
	*/
	//@{
	static uint32_t constexpr MaxPcfFilterSize = 64u;
	static uint32_t constexpr MaxPcfSampleCount = 64u;
	//@}
	/**
	*\name
	*	Directional Light Shadows.
	*/
	//@{
	static uint32_t constexpr ShadowMapDirectionalTextureSize = 2048u;
	static uint32_t constexpr MaxDirectionalCascadesCount = 6u;
	//@}
	/**
	*\name
	*	Point Light Shadows.
	*/
	//@{
	static uint32_t constexpr ShadowMapPointTextureSize = 256u;
	static uint32_t constexpr MaxPointShadowMapCount = 8u;
	//@}
	/**
	*\name
	*	Spot Light Shadows.
	*/
	//@{
	static uint32_t constexpr ShadowMapSpotTextureSize = 256u;
	static uint32_t constexpr MaxSpotShadowMapCount = 16u;
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
	static int32_t constexpr MaxMaterialComponentsCount = 8;
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
	// Max counts for text overlays
	static uint32_t constexpr MaxCharsPerOverlay = 600u;
	static uint32_t constexpr MaxLinesPerOverlay = 600u;
	// Max counts for text overlays text buffer
	static uint32_t constexpr MaxCharsPerBuffer = 65'536u;
	// Max counts for text buffers
	static uint32_t constexpr MaxTextsContsPerOverlay = MaxCharsPerOverlay;
	static uint32_t constexpr MaxTextsOverlaysPerBuffer = 1'000u;
	static uint32_t constexpr MaxTextsCharsPerWord = 60u;
	static uint32_t constexpr MaxTextsCharsPerLine = 600u;
	// Max count for (border)panel overlay buffer pool
	static uint32_t constexpr MaxOverlayPanelsPerBuffer = 100u;
	// Base count for objects buffers pool
	static uint32_t constexpr BaseObjectPoolBufferCount = 1'000'000u;
	// Max bones affecting a vertex
	static uint32_t constexpr MaxBonesPerVertex = 8;
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
	static uint32_t constexpr MaxMeshletVertexCount = 64u;
	static uint32_t constexpr MaxMeshletTriangleCount = 124u;
	//@}
}

#endif
