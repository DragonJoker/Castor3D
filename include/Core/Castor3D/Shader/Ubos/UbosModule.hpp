/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderUbosModule_H___
#define ___C3D_ShaderUbosModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ShaderWriter/ShaderWriterPrerequisites.hpp>

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name UBO */
	//@{

	namespace shader
	{
		struct BillboardData;
		struct GpInfoData;
		struct HdrConfigData;
		struct LayeredLpvGridData;
		struct LpvGridData;
		struct LpvLightData;
		struct MatrixData;
		struct ModelInstancesData;
		struct ModelData;
		struct MorphingData;
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
		Writer_Parameter( GpInfoData );
		Writer_Parameter( HdrConfigData );
		Writer_Parameter( LayeredLpvGridData );
		Writer_Parameter( LpvGridData );
		Writer_Parameter( LpvLightData );
		Writer_Parameter( MatrixData );
		Writer_Parameter( ModelInstancesData );
		Writer_Parameter( ModelData );
		Writer_Parameter( MorphingData );
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
		castor::Point2f dimensions;
		castor::Point2f dummy;
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
	*	Geometry pass UBO.
	*\~french
	*\brief
	*	UBO de la Geometry pass.
	*/
	class GpInfoUbo;
	/**
	*\~english
	*\brief
	*	Geometry pass data.
	*\~french
	*\brief
	*	Données de la Geometry pass.
	*/
	struct GpInfoUboConfiguration
	{
		castor::Matrix4x4f invViewProj;
		castor::Matrix4x4f invView;
		castor::Matrix4x4f invProj;
		castor::Matrix4x4f gView;
		castor::Matrix4x4f gProj;
		castor::Point2f renderSize;
	};
	/**
	*\~english
	*\brief
	*	Light propagation UBO.
	*\~french
	*\brief
	*	UBO de light propagation.
	*/
	class LpvGridConfigUbo;
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
		castor::Point4f minVolumeCorner;
		castor::Point4f gridSizeAtt;
		castor::Point4f cameraPos;
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
		castor::Matrix4x4f lightView;
		float texelAreaModifier = 1.0f;
		float tanFovXHalf = 1.0f;
		float tanFovYHalf = 1.0f;
		float lightIndex = 0.0f;
	};
	/**
	*\~english
	*\brief
	*	Light propagation UBO.
	*\~french
	*\brief
	*	UBO de light propagation.
	*/
	class LayeredLpvGridConfigUbo;
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
		std::array< castor::Point4f, shader::LpvMaxCascadesCount > allMinVolumeCorners;
		castor::Point4f allCellSizes;
		castor::Point4f gridSizeAtt;
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
	class MatrixUbo;
	/**
	*\~english
	*\brief
	*	Global matrices data.
	*\~french
	*\brief
	*	Données des matrices globales.
	*/
	struct MatrixUboConfiguration
	{
		castor::Matrix4x4f projection;
		castor::Matrix4x4f invProjection;
		castor::Matrix4x4f curView;
		castor::Matrix4x4f prvView;
		castor::Matrix4x4f curViewProj;
		castor::Matrix4x4f prvViewProj;
		castor::Point4f jitter;
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
		castor::Matrix4x4f prvModel;
		castor::Matrix4x4f curModel;
		castor::Matrix4x4f normal;
		castor::Point4ui textures0;
		castor::Point4ui textures1;
		castor::Point4i countsIDs;
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
	struct MorphingBufferConfiguration
	{
		castor::Point4f time;
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
		castor::Point4f positionRatio;
		castor::Point4i renderSizeIndex;
	};
	/**
	*\~english
	*\brief
	*	Picking data UBO.
	*\~french
	*\brief
	*	UBO des données de picking.
	*/
	class PickingUbo;
	/**
	*\~english
	*\brief
	*	Picking data.
	*\~french
	*\brief
	*	Données de picking.
	*/
	struct PickingUboConfiguration
	{
		uint32_t drawIndex;
		uint32_t nodeIndex;
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
		float intensity;
		float maxRadius;
		uint32_t sampleCount;
		uint32_t index;
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
		castor::Point4f ambientLight;
		castor::Point4f backgroundColour;
		castor::Point4f lightsCount;
		castor::Point4f cameraPos;
		castor::Point4f clipInfo;
		castor::Point4f fogInfo;
	};
	/**
	*\~english
	*\brief
	*	Shadow map data UBO for directional sources.
	*\~french
	*\brief
	*	UBO des données de shadow map pour les sources directionnelles.
	*/
	class ShadowMapDirectionalUbo;
	/**
	*\~english
	*\brief
	*	Shadow map data for directional sources.
	*\~french
	*\brief
	*	Données de shadow map pour les sources directionnelles.
	*/
	struct ShadowMapDirectionalUboConfiguration
	{
		std::array< castor::Matrix4x4f, 6u > lightProjections;
		std::array< castor::Matrix4x4f, 6u > lightViews;
		castor::Point4f lightTiles;
		uint32_t lightIndex;
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
		castor::Matrix4x4f lightProjection;
		castor::Matrix4x4f lightView;
		castor::Point4f lightPosFarPlane;
		uint32_t lightIndex;
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
		castor::Matrix4x4f bonesMatrix[400];
	};
	/**
	*\~english
	*\brief
	*	Voxelizer configuration data UBO.
	*\~french
	*\brief
	*	UBO des données de configuration du voxelizer.
	*/
	class VoxelizerUbo;
	/**
	*\~english
	*\brief
	*	Voxelizer configuration data.
	*\~french
	*\brief
	*	Données de configuration du voxelizer.
	*/
	struct VoxelizerUboConfiguration;

	CU_DeclareCUSmartPtr( castor3d, LpvGridConfigUbo, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LpvLightConfigUbo, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LayeredLpvGridConfigUbo, C3D_API );
	CU_DeclareVector( LpvLightConfigUbo, LpvLightConfigUbo );
	CU_DeclareVector( LpvGridConfigUbo, LpvGridConfigUbo );

	//@}
	//@}
}

#endif
