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
#include <ShaderWriter/BaseTypes/Boolean.hpp>
#include <ShaderWriter/BaseTypes/Double.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>
#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/BaseTypes/CombinedImage.hpp>
#include <ShaderWriter/BaseTypes/StorageImage.hpp>
#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>

namespace castor3d::shader
{
	/**@name Shader */
	//@{
	/**@name Sdw */
	//@{

	struct ShadowOptions
	{
		SceneFlags type = SceneFlags( 0 );
		// Use VSM result
		bool vsm{ false };
		// Use RSM result
		bool rsm{ false };

		ShadowOptions()
			: type{ SceneFlag::eNone }
			, vsm{ false }
			, rsm{ false }
		{
		}

		explicit ShadowOptions( bool vsm
			, bool rsm )
			: type{ SceneFlag::eNone }
			, vsm{ rsm }
			, rsm{ rsm }
		{
		}

		ShadowOptions( bool enabled
			, LightType lightType
			, bool vsm
			, bool rsm )
			: type{ ( enabled
				? SceneFlag( uint8_t( SceneFlag::eShadowBegin ) << int( lightType ) )
				: SceneFlag::eNone ) }
			, vsm{ vsm }
			, rsm{ rsm }
		{
		}

		ShadowOptions( SceneFlags sceneFlags
			, bool vsm
			, bool rsm )
			: type{ sceneFlags & SceneFlag::eShadowAny }
			, vsm{ vsm }
			, rsm{ rsm }
		{
		}
	};

	struct DirectionalLight;
	struct Intersection;
	struct Light;
	struct LightData;
	struct LightMaterial;
	struct LayeredLpvGridData;
	struct LpvGridData;
	struct LpvLightData;
	struct Material;
	struct OutputComponents;
	struct PointLight;
	struct Ray;
	struct SpotLight;
	struct SssProfile;
	struct TextureAnimData;
	struct TextureConfigData;
	struct VoxelData;

	template< ast::var::Flag FlagT >
	struct SurfaceT;
	template< typename TexcoordT, ast::var::Flag FlagT >
	struct FragmentSurfaceUvT;
	template< ast::var::Flag FlagT >
	struct VertexSurfaceT;

	using Surface = SurfaceT< ast::var::Flag::eNone >;

	class BackgroundModel;
	class Fog;
	class Materials;
	class LightingModel;
	class ReflectionModel;
	class Shadow;
	class SssProfiles;
	class SssTransmittance;
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
		, SssProfiles const * sssProfiles
		, bool enableVolumetric ) >;
	using LightingModelFactory = castor::Factory< LightingModel
		, castor::String
		, LightingModelPtr
		, LightingModelCreator >;

	using BackgroundModelPtr = std::unique_ptr< BackgroundModel >;
	using BackgroundModelCreator = std::function< BackgroundModelPtr( sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, uint32_t & binding
		, uint32_t set ) >;
	using BackgroundModelFactory = castor::Factory< BackgroundModel
		, castor::String
		, BackgroundModelPtr
		, BackgroundModelCreator >;

	Writer_Parameter( DirectionalLight );
	Writer_Parameter( Intersection );
	Writer_Parameter( LayeredLpvGridData );
	Writer_Parameter( Light );
	Writer_Parameter( LightMaterial );
	Writer_Parameter( LpvGridData );
	Writer_Parameter( LpvLightData );
	Writer_Parameter( Material );
	Writer_Parameter( PbrLightMaterial );
	Writer_Parameter( PhongLightMaterial );
	Writer_Parameter( PointLight );
	Writer_Parameter( Ray );
	Writer_Parameter( SpotLight );
	Writer_Parameter( Surface );
	Writer_Parameter( TextureAnimData );
	Writer_Parameter( TextureConfigData );
	Writer_Parameter( VoxelData );

	C3D_API uint32_t getSpotShadowMapCount();
	C3D_API uint32_t getPointShadowMapCount();
	C3D_API uint32_t getBaseLightComponentsCount();
	C3D_API uint32_t getMaxLightComponentsCount();
	C3D_API castor::String concatModelNames( castor::String lhs
		, castor::String rhs );

	struct DerivTex
		: public sdw::StructInstanceHelperT< "DerivTex"
		, sdw::type::MemoryLayout::eC
		, sdw::Vec2Field< "uv" >
		, sdw::Vec2Field< "dPdx" >
		, sdw::Vec2Field< "dPdy" > >
	{
		DerivTex( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API sdw::Float computeMip( sdw::Vec2 const & texSize )const;

		auto uv()const { return getMember< "uv" >(); }
		auto dPdx()const { return getMember< "dPdx" >(); }
		auto dPdy()const { return getMember< "dPdy" >(); }
	};

	Writer_Parameter( DerivTex );

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
