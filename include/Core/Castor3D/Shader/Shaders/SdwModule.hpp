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

#include <CastorUtils/Design/Factory.hpp>

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

namespace castor3d::shader
{
	/**@name Shader */
	//@{
	/**@name Sdw */
	//@{

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
	static uint32_t constexpr MaxTransmittanceProfileSize = 10u;
	static int constexpr MaxMaterialComponentsCount = MaxTransmittanceProfileSize + 9;
	// Texture Configuration Buffer.
	static uint32_t constexpr MaxTextureConfigurationCount = 4000u;
	static int constexpr MaxTextureConfigurationComponentsCount = 9;
	// Texture Animation Buffer.
	static uint32_t constexpr MaxTextureAnimationCount = MaxTextureConfigurationCount;
	static int constexpr MaxTextureAnimationComponentsCount = 3;

	struct DirectionalLight;
	struct Light;
	struct LightMaterial;
	struct LayeredLpvGridData;
	struct LpvGridData;
	struct LpvLightData;
	struct Material;
	struct OutputComponents;
	struct PointLight;
	struct SpotLight;
	struct TextureAnimData;
	struct TextureConfigData;
	struct TiledDirectionalLight;
	struct VoxelData;

	template< ast::var::Flag FlagT >
	struct SurfaceT;
	template< ast::var::Flag FlagT >
	struct FragmentSurfaceT;
	template< ast::var::Flag FlagT >
	struct VertexSurfaceT;

	using Surface = SurfaceT< ast::var::Flag::eNone >;

	class Fog;
	class Materials;
	class LightingModel;
	class ReflectionModel;
	class Shadow;
	class TextureAnimations;
	class TextureConfigurations;
	class Utils;

	struct PhongLightMaterial;
	class PhongLightingModel;
	class PhongReflectionModel;

	struct PbrLightMaterial;
	class PbrLightingModel;
	class PbrReflectionModel;

	using LightingModelPtr = std::unique_ptr< LightingModel >;
	using ReflectionModelPtr = std::unique_ptr< ReflectionModel >;
	using LightingModelCreator = std::function< LightingModelPtr( sdw::ShaderWriter & writer
		, Utils & utils
		, ShadowOptions shadowsOptions
		, bool isOpaqueProgram ) >;

	using LightingModelFactory = castor::Factory< LightingModel
		, castor::String
		, LightingModelPtr
		, LightingModelCreator >;

	Writer_Parameter( DirectionalLight );
	Writer_Parameter( LayeredLpvGridData );
	Writer_Parameter( Light );
	Writer_Parameter( LightMaterial );
	Writer_Parameter( LpvGridData );
	Writer_Parameter( LpvLightData );
	Writer_Parameter( PbrLightMaterial );
	Writer_Parameter( PhongLightMaterial );
	Writer_Parameter( PointLight );
	Writer_Parameter( SpotLight );
	Writer_Parameter( Surface );
	Writer_Parameter( TextureConfigData );
	Writer_Parameter( TiledDirectionalLight );
	Writer_Parameter( VoxelData );
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

	//@}
	//@}
}

namespace sdw
{
	template<>
	struct ParamTranslater< castor3d::shader::OutputComponents >
	{
		using Type = castor3d::shader::OutputComponents &;
	};
}

#endif
