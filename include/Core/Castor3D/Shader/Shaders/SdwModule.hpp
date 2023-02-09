/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderSdwModule_H___
#define ___C3D_ShaderSdwModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/ShaderBuffersModule.hpp"

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

	struct DerivTex;
	struct DirectionalLight;
	struct Intersection;
	struct Light;
	struct LightData;
	struct LightSurface;
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

	struct BlendComponents;
	struct SurfaceBase;
	struct RasterizerSurfaceBase;

	template< typename TexcoordT >
	struct BlendComponentsT;
	struct Surface;
	template< typename TexcoordT, ast::var::Flag FlagT >
	struct RasterizerSurfaceT;
	template< ast::var::Flag FlagT >
	struct VertexSurfaceT;
	template< sdw::var::Flag FlagT >
	struct VoxelSurfaceT;

	template< ast::var::Flag FlagT >
	using FragmentSurfaceT = RasterizerSurfaceT< sdw::Vec3, FlagT >;
	using RasterizerSurface = RasterizerSurfaceT< sdw::Vec3, ast::var::Flag::eNone >;
	using Vec3BlendComponents = BlendComponentsT< sdw::Vec3 >;
	using DerivFragmentSurface = RasterizerSurfaceT< DerivTex, ast::var::Flag::eNone >;
	template< typename TexcoordT >
	using RasterSurfaceT = RasterizerSurfaceT< TexcoordT, ast::var::Flag::eNone >;
	using DerivBlendComponents = BlendComponentsT< DerivTex >;
	using VertexSurface = VertexSurfaceT< sdw::var::Flag::eNone >;
	using VoxelSurface = VoxelSurfaceT< sdw::var::Flag::eNone >;

	class BackgroundModel;
	class BufferBase;
	class BRDFHelpers;
	class Fog;
	class Lights;
	class LightingModel;
	class Materials;
	class PassShaders;
	class ReflectionModel;
	class Shadow;
	class SssProfiles;
	class SssTransmittance;
	class TextureAnimations;
	class TextureConfigurations;
	class Utils;

	class PhongLightingModel;
	class PhongReflectionModel;

	class PbrLightingModel;
	class PbrReflectionModel;

	template< typename DataT >
	class BufferT;

	CU_DeclareCUSmartPtr( castor3d::shader, LightingModel, C3D_API );

	using ReflectionModelPtr = std::unique_ptr< ReflectionModel >;
	using LightingModelCreator = std::function< LightingModelUPtr( LightingModelID lightingModelId
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric ) >;

	struct BackgroundModelEntry
	{
	};

	using BackgroundModelPtr = std::unique_ptr< BackgroundModel >;
	using BackgroundModelCreator = std::function< BackgroundModelPtr( Engine const & engine
		, sdw::ShaderWriter & writer
		, Utils & utils
		, VkExtent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set ) >;
	using BackgroundModelFactory = castor::Factory< BackgroundModel
		, castor::String
		, BackgroundModelPtr
		, BackgroundModelCreator
		, BackgroundModelID >;

	Writer_Parameter( BlendComponents );
	Writer_Parameter( DirectionalLight );
	Writer_Parameter( Intersection );
	Writer_Parameter( LayeredLpvGridData );
	Writer_Parameter( Light );
	Writer_Parameter( LightSurface );
	Writer_Parameter( LpvGridData );
	Writer_Parameter( LpvLightData );
	Writer_Parameter( Material );
	Writer_Parameter( PointLight );
	Writer_Parameter( Ray );
	Writer_Parameter( SpotLight );
	Writer_Parameter( Surface );
	Writer_Parameter( TextureAnimData );
	Writer_Parameter( TextureConfigData );
	Writer_Parameter( VertexSurface );
	Writer_Parameter( VoxelData );
	Writer_Parameter( VoxelSurface );

	C3D_API uint32_t getSpotShadowMapCount();
	C3D_API uint32_t getPointShadowMapCount();

	C3D_API castor::String concatModelNames( castor::String lhs
		, castor::String rhs );

	struct DerivTex
		: public sdw::StructInstanceHelperT< "C3D_DerivTex"
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

		DerivTex( sdw::Vec2 const & puv
			, sdw::Vec2 const & pdPdx
			, sdw::Vec2 const & pdPdy )
			: DerivTex{ sdw::findWriterMandat( puv, pdPdx, pdPdy )
				, sdw::makeAggrInit( makeType( puv.getType()->getCache() )
					, [&puv, &pdPdx, &pdPdy]()
					{
						sdw::expr::ExprList result;
						result.emplace_back( makeExpr( puv ) );
						result.emplace_back( makeExpr( pdPdx ) );
						result.emplace_back( makeExpr( pdPdy ) );
						return result;
					}() )
				, true }
		{
		}

		C3D_API sdw::Float computeMip( sdw::Vec2 const & texSize )const;

		auto uv()const { return getMember< "uv" >(); }
		auto dPdx()const { return getMember< "dPdx" >(); }
		auto dPdy()const { return getMember< "dPdy" >(); }
	};

	Writer_Parameter( DerivTex );

	CU_DeclareSmartPtr( Material );

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
