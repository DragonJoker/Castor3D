/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderSdwModule_H___
#define ___C3D_ShaderSdwModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/BaseTypes/Array.hpp>
#include <ShaderWriter/BaseTypes/Bool.hpp>
#include <ShaderWriter/BaseTypes/Double.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Image.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/SampledImage.hpp>
#include <ShaderWriter/BaseTypes/Sampler.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name Sdw */
	//@{

	namespace shader
	{
		struct ShadowOptions
		{
			SceneFlags type = SceneFlags( 0 );
			// Use RSM result
			bool rsm{ false };

			ShadowOptions()
				: type{ SceneFlag::eNone }
				, rsm{ false }
			{
			}

			explicit ShadowOptions( bool rsm )
				: type{ SceneFlag::eNone }
				, rsm{ rsm }
			{
			}

			ShadowOptions( bool enabled
				, LightType lightType
				, bool rsm )
				: type{ ( enabled
					? SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << int( lightType ) )
					: SceneFlag::eNone ) }
				, rsm{ rsm }
			{
			}

			ShadowOptions( SceneFlags sceneFlags
				, bool rsm )
				: type{ sceneFlags & SceneFlag::eShadowAny }
				, rsm{ rsm }
			{
			}
		};

		// Light Propagation Volumes Cascades.
		constexpr uint32_t LpvMaxCascadesCount = 3u;
		// Directional Shadow Cascades.
		static uint32_t constexpr DirectionalMaxCascadesCount = ShadowMapDirectionalTileCountX * ShadowMapDirectionalTileCountY;
		// Pass Buffer.
		static uint32_t constexpr MaxMaterialsCount = 2000u;
		static int constexpr MaxMaterialComponentsCount = 15;
		static uint32_t constexpr MaxTransmittanceProfileSize = 10u;
		// Texture Configuration Buffer.
		static uint32_t constexpr MaxTextureConfigurationCount = 4000u;
		static int constexpr MaxTextureConfigurationComponentsCount = 9;

		class Utils;
		class Shadow;
		class Fog;
		struct Light;
		struct DirectionalLight;
		struct TiledDirectionalLight;
		struct PointLight;
		struct SpotLight;
		class Materials;
		class PhongMaterials;
		class PbrMRMaterials;
		class PbrSGMaterials;
		class LightingModel;
		class PhongReflectionModel;
		class PbrReflectionModel;
		class PhongLightingModel;
		class MetallicBrdfLightingModel;
		class SpecularBrdfLightingModel;
		struct BaseMaterial;
		struct PhongMaterial;
		struct MetallicRoughnessMaterial;
		struct SpecularGlossinessMaterial;
		struct TextureConfigData;
		class TextureConfigurations;
		struct OutputComponents;
		class LpvGridData;
		class LayeredLpvGridData;
		class LpvLightData;
		class VoxelData;
		struct Surface;
		struct VertexSurface;
		struct OutFragmentSurface;
		struct InFragmentSurface;

		Writer_Parameter( Light );
		Writer_Parameter( DirectionalLight );
		Writer_Parameter( TiledDirectionalLight );
		Writer_Parameter( PointLight );
		Writer_Parameter( SpotLight );
		Writer_Parameter( TextureConfigData );
		Writer_Parameter( LayeredLpvGridData );
		Writer_Parameter( LpvLightData );
		Writer_Parameter( LpvGridData );
		Writer_Parameter( VoxelData );
		Writer_Parameter( Surface );
		/**
		 *\~english
		 *\brief		Creates the appropriate GLSL materials buffer.
		 *\param[in]	writer		The GLSL writer.
		 *\param		passFlags	The pass flags.
		 *\~french
		 *\brief		Crée le tampon de matériaux GLSL approprié.
		 *\param[in]	writer		Le writer GLSL.
		 *\param		passFlags	Les indicateurs de passe.
		 */
		C3D_API std::unique_ptr< Materials > createMaterials( sdw::ShaderWriter & writer
			, PassFlags const & passFlags );
		C3D_API uint32_t getSpotShadowMapCount();
		C3D_API uint32_t getPointShadowMapCount();
		C3D_API uint32_t getBaseLightComponentsCount();
		C3D_API uint32_t getMaxLightComponentsCount();
	}

	//@}
	//@}
}

#endif