/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderUbosModule_H___
#define ___C3D_ShaderUbosModule_H___

#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include "Castor3D/Limits.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name UBO */
	//@{

	namespace shader
	{
		struct BillboardData;
		struct CameraData;
		struct ClustersData;
		struct HdrConfigData;
		struct LayeredLpvGridData;
		struct LpvGridData;
		struct LpvLightData;
		struct ModelData;
		struct ModelInstancesData;
		struct MorphTargetData;
		struct MorphTargetsData;
		struct MorphingWeightsData;
		struct OverlayData;
		struct PickingData;
		struct RsmConfigData;
		struct SceneData;
		struct ShadowMapData;
		struct SkinningData;
		struct SsaoConfigData;
		struct TextureData;
		struct VoxelData;

		Writer_Parameter( BillboardData );
		Writer_Parameter( HdrConfigData );
		Writer_Parameter( LayeredLpvGridData );
		Writer_Parameter( LpvGridData );
		Writer_Parameter( LpvLightData );
		Writer_Parameter( CameraData );
		Writer_Parameter( ModelData );
		Writer_Parameter( ModelInstancesData );
		Writer_Parameter( MorphTargetData );
		Writer_Parameter( MorphTargetsData );
		Writer_Parameter( MorphingWeightsData );
		Writer_Parameter( OverlayData );
		Writer_Parameter( PickingData );
		Writer_Parameter( RsmConfigData );
		Writer_Parameter( SceneData );
		Writer_Parameter( ShadowMapData );
		Writer_Parameter( SsaoConfigData );
		Writer_Parameter( TextureData );
		Writer_Parameter( VoxelData );
	}
	/**
	*\~english
	*\brief
	*	Billboards list UBO.
	*\~french
	*\brief
	*	UBO pour les billboards.
	*/
	class BillboardUbo;
	/**
	*\~english
	*\brief
	*	Billboards list data.
	*\~french
	*\brief
	*	Données pour les billboards.
	*/
	struct BillboardUboConfiguration
	{
		castor::Point2f dimensions{};
		uint32_t isSpherical{};
		uint32_t isFixedSize{};
	};
	/**
	*\~english
	*\brief
	*	Debug switchs.
	*\~french
	*\brief
	*	Les switchs de debug.
	*/
	struct DebugConfig;
	/**
	*\~english
	*\brief
	*	Meshlet draw parameters.
	*\~french
	*\brief
	*	Paramètres de dessin de meshlets.
	*/
	struct DrawParamsConfiguration
	{
		uint32_t nodeId{};
		uint32_t instanceCount{};
		uint32_t instanceOffset{};
	};
	/**
	*\~english
	*\brief
	*	Light propagation data.
	*\~french
	*\brief
	*	Données de light propagation.
	*/
	struct LpvGridConfigUboConfiguration
	{
		castor::Point4f minVolumeCorner{};
		castor::Point4f gridSizeAtt{};
		castor::Point4f cameraPos{};
	};
	/**
	*\~english
	*\brief
	*	Light propagation UBO.
	*\~french
	*\brief
	*	UBO de light propagation.
	*/
	class LpvLightConfigUbo;
	/**
	*\~english
	*\brief
	*	Light propagation data.
	*\~french
	*\brief
	*	Données de light propagation.
	*/
	struct LpvLightConfigUboConfiguration
	{
		castor::Matrix4x4f lightView{};
		float texelAreaModifier = 1.0f;
		float tanFovXHalf = 1.0f;
		float tanFovYHalf = 1.0f;
		float lightOffset = 0.0f;
	};
	/**
	*\~english
	*\brief
	*	Light propagation data.
	*\~french
	*\brief
	*	Données de light propagation.
	*/
	struct LayeredLpvGridConfigUboConfiguration
	{
		std::array< castor::Point4f, LpvMaxCascadesCount > allMinVolumeCorners{};
		castor::Point4f allCellSizes{};
		castor::Point4f gridSizeAtt{};
	};
	/**
	*\brief
	*	HDR configuration data UBO.
	*\~french
	*\brief
	*	UBO des données de configuration HDR.
	*/
	class HdrConfigUbo;
	/**
	*\~english
	*\brief
	*	Global matrices UBO.
	*\~french
	*\brief
	*	UBO des matrices globales.
	*/
	class CameraUbo;
	/**
	*\~english
	*\brief
	*	Global matrices data.
	*\~french
	*\brief
	*	Données des matrices globales.
	*/
	struct CameraUboConfiguration
	{
		std::array< castor::Point4f, 6u > frustumPlanes{};
		castor::Matrix4x4f projection{};
		castor::Matrix4x4f invProjection{};
		castor::Matrix4x4f curView{};
		castor::Matrix4x4f invCurView{};
		castor::Matrix4x4f prvView{};
		castor::Matrix4x4f invPrvView{};
		castor::Matrix4x4f curViewProj{};
		castor::Matrix4x4f invCurViewProj{};
		castor::Matrix4x4f prvViewProj{};
		castor::Matrix4x4f invPrvViewProj{};
		castor::Point2ui size{};
		castor::Point2f jitter{};
		castor::Point3f position{};
		float gamma{};
		uint32_t debugIndex{};
		float pad{};
		float nearPlane{};
		float farPlane{};
	};
	/**
	*\~english
	*\brief
	*	Clusters data UBO.
	*\~french
	*\brief
	*	UBO des données des clusters.
	*/
	class ClustersUbo;
	/**
	*\~english
	*\brief
	*	Clusters data.
	*\~french
	*\brief
	*	Données des clusters.
	*/
	struct ClustersUboConfiguration
	{
		// The 3D dimensions of the cluster grid.
		castor::Point3ui gridDim{};
		// The clusters Z split scheme.
		uint32_t splitScheme{};
		// The size of a cluster in screen space (pixels).
		castor::Point2ui clusterSize{};
		// The distance to the near and far clipping plane. (Used for computing the index in the cluster grid)
		castor::Point2f viewNearFar{};
		// The number of BVH levels for point lights.
		uint32_t pointLightLevelsCount{};
		// The number of BVH levels for spot lights.
		uint32_t spotLightLevelsCount{};
		// The number of point lights.
		uint32_t pointLightsCount{};
		// The number of spot lights.
		uint32_t spotLightsCount{};
		// The biased exponential split scheme bias.
		float bias{};
		// If clustered lights processing uses wave intrinsics.
		uint32_t enableWaveIntrinsics{};
		float pad0{};
		float pad1{};
	};
	/**
	*\~english
	*\brief
	*	Meshlet draw informations.
	*\~french
	*\brief
	*	Informations de dessin de meshlets.
	*/
	struct MeshInfoConfiguration
	{
		uint32_t meshletCount{};
		uint32_t meshletOffset{};
	};
	/**
	*\~english
	*\brief
	*	Model data UBO.
	*\~french
	*\brief
	*	UBO des données modèle.
	*/
	class ModelUbo;
	/**
	*\~english
	*\brief
	*	Model index data.
	*\~french
	*\brief
	*	Données indices de modèle.
	*/
	struct ModelBufferConfiguration
	{
		castor::Matrix4x4f prvModel{};
		castor::Matrix4x4f curModel{};
		castor::Matrix4x4f normal{};
		uint32_t materialId{};
		uint32_t shadowReceiver{};
		uint32_t envMapId{};
		uint32_t vertexOffset{};
		castor::Point3f scale{};
		uint32_t meshletCount{};
		uint32_t indexOffset{};
		uint32_t meshletOffset{};
		uint32_t pad1{};
		uint32_t pad2{};
	};
	/**
	*\~english
	*\brief
	*	Morphing data (time) UBO.
	*\~french
	*\brief
	*	UBO des données de morphing (temps).
	*/
	class MorphingUbo;
	/**
	*\~english
	*\brief
	*	Morphing data.
	*\~french
	*\brief
	*	Données de morphing (temps).
	*/
	struct MorphingWeightsConfiguration
	{
		// Contains the actual number of weights
		castor::Point4ui morphTargetsData{};
		// x4 because the shader stores a uvec4[MaxMorphTargets]
		std::array< uint32_t, MaxMorphTargets * 4u > morphTargetsIndices{};
		// x4 because the shader stores a vec4[MaxMorphTargets]
		std::array< float, MaxMorphTargets * 4u > morphTargetsWeights{};
	};
	/**
	*\~english
	*\brief
	*	Objects ID data.
	*\~french
	*\brief
	*	Données des ID d'objets.
	*/
	struct ObjectIdsConfiguration
	{
		uint32_t nodeId{};
		uint32_t morphingId{};
		uint32_t skinningId{};
		uint32_t pad{};
	};
	/**
	*\~english
	*\brief
	*	Overlay data UBO.
	*\~french
	*\brief
	*	UBO des données d'overlays.
	*/
	class OverlayUbo;
	/**
	*\~english
	*\brief
	*	Overlay data.
	*\~french
	*\brief
	*	Données d'overlays.
	*/
	struct OverlayUboConfiguration
	{
		castor::Point4f uv{};
		castor::Point4f parentRect{};
		castor::Point4f renderArea{};
		castor::Point2f relativePosition{};
		castor::Point2f relativeSize{};
		castor::Point4f border{};
		castor::Point4f borderInnerUV{};
		castor::Point4f borderOuterUV{};
		uint32_t vertexOffset{};
		uint32_t materialId{};
		uint32_t borderPosition{};
		uint32_t textTexturingMode{};
		uint32_t textWordOffset{};
		uint32_t textLineOffset{};
		float textTopOffset{};
		uint32_t pad1{};
	};
	/**
	*\~english
	*\brief
	*	Reflective Shadow Maps based GI UBO
	*\~french
	*\brief
	*	UBO du GI basé sur les Reflective Shadow Maps.
	*/
	class RsmConfigUbo;
	/**
	*\~english
	*\brief
	*	Reflective Shadow Maps based GI UBO configuration.
	*\~french
	*\brief
	*	Configuration de l'UBO du GI basé sur les Reflective Shadow Maps.
	*/
	struct RsmUboConfiguration
	{
		float intensity{};
		float maxRadius{};
		uint32_t sampleCount{};
		int32_t index{};
	};
	/**
	*\~english
	*\brief
	*	Scene data UBO.
	*\~french
	*\brief
	*	UBO des données de scène.
	*/
	class SceneUbo;
	/**
	*\~english
	*\brief
	*	Scene data.
	*\~french
	*\brief
	*	données de scène.
	*/
	struct SceneUboConfiguration
	{
		castor::Point3f ambientLight{};
		uint32_t fogType{};
		castor::Point3f backgroundColour{};
		float fogDensity{};
	};
	/**
	*\~english
	*\brief
	*	Shadow map data UBO.
	*\~french
	*\brief
	*	UBO des données de shadow map.
	*/
	class ShadowMapUbo;
	/**
	*\~english
	*\brief
	*	Shadow map data.
	*\~french
	*\brief
	*	Données de shadow map.
	*/
	struct ShadowMapUboConfiguration
	{
		castor::Matrix4x4f lightProjection{};
		castor::Matrix4x4f lightView{};
		castor::Point4f lightPosFarPlane{};
		uint32_t lightOffset{};
	};
	/**
	*\~english
	*\brief
	*	Skinning data UBO.
	*\~french
	*\brief
	*	UBO des données de skinning.
	*/
	class SkinningUbo;
	/**
	*\~english
	*\brief
	*	Skinning data.
	*\~french
	*\brief
	*	Données de skinning.
	*/
	struct SkinningTransformsConfiguration
	{
		std::array< castor::Matrix4x4f, MaxBonesCount > bonesMatrix{};
	};
	/**
	*\~english
	*\brief
	*	Voxelizer configuration data.
	*\~french
	*\brief
	*	Données de configuration du voxelizer.
	*/
	struct VoxelizerUboConfiguration;

	CU_DeclareSmartPtr( castor3d, LayeredLpvGridConfigUbo, C3D_API );
	CU_DeclareSmartPtr( castor3d, LpvGridConfigUbo, C3D_API );
	CU_DeclareSmartPtr( castor3d, LpvLightConfigUbo, C3D_API );
	CU_DeclareSmartPtr( castor3d, SceneUbo, C3D_API );

	CU_DeclareVector( LpvLightConfigUbo, LpvLightConfigUbo );
	CU_DeclareVector( LpvGridConfigUbo, LpvGridConfigUbo );

	// Needs to be last, because of namespace changes...
	CU_DeclareSmartPtr( castor3d::shader, ClustersData, C3D_API );
	//@}
	//@}
}

#endif
