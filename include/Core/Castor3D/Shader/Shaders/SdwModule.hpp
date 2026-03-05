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

#include <ShaderWriter/Writer.hpp>
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
#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace c3d::shader
{
	/**@name Shader */
	//@{

	struct ShadowOptions
	{
		SceneFlags type = SceneFlags( 0 );
		// Reserve binding IDs even if shadows are disabled
		bool reserveIds{ false };

		explicit ShadowOptions( SceneFlags sceneFlags
			, bool reserveIds = false )noexcept
			: type{ sceneFlags & SceneFlag::eShadowAny }
			, reserveIds{ reserveIds }
		{
		}

		explicit ShadowOptions()noexcept
			: ShadowOptions{ SceneFlag::eNone, false }
		{
		}
	};

	struct AABB;
	struct Cone;
	struct DirectionalLight;
	struct DirectionalShadowData;
	struct DirectLighting;
	struct Frustum;
	struct IndirectLighting;
	struct Intersection;
	struct Light;
	struct LightData;
	struct LightSurface;
	struct LayeredLpvGridData;
	struct LpvGridData;
	struct LpvLightData;
	struct Material;
	struct Meshlet;
	struct ObjectIds;
	struct Plane;
	struct PointLight;
	struct PointShadowData;
	struct Ray;
	struct ReflectionRefraction;
	struct RenderData;
	struct ShadowData;
	struct SpotLight;
	struct SpotShadowData;
	struct SssProfile;
	struct TextureTransformData;
	struct TextureConfigData;
	struct VoxelData;

	struct BlendComponents;

	template< typename Position3T, typename Position4T, typename NormalT >
	struct SurfaceBaseT;

	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T >
	struct RasterizerSurfaceBaseT;

	template< typename Position3T, typename Position4T, typename NormalT >
	struct SurfaceT;
	template< typename Position3T, typename Position4T, typename Normal3T, typename Normal4T, typename TexcoordT, ast::var::Flag FlagT >
	struct RasterizerSurfaceT;
	template< ast::var::Flag FlagT >
	struct MeshVertexT;
	template< sdw::var::Flag FlagT >
	struct VoxelSurfaceT;
	template< ast::var::Flag FlagT >
	struct OverlaySurfaceT;
	template< typename ColTypeT, sdw::var::Flag FlagT >
	struct ColourT;
	template< typename PosTypeT, sdw::var::Flag FlagT >
	struct PositionT;
	template< typename PosTypeT, sdw::var::Flag FlagT >
	struct PosUvT;
	template< typename UvTypeT, sdw::var::Flag FlagT >
	struct UvT;
	template< typename UvTypeT, sdw::var::Flag FlagT >
	struct TwoUvT;
	template< typename ValueT, sdw::StringLiteralT StructNameT >
	struct DerivativeValueT;

	using DerivFloat = DerivativeValueT< sdw::Float, "C3D_DerivFloat" >;
	using DerivVec2 = DerivativeValueT< sdw::Vec2, "C3D_DerivVec2" >;
	using DerivVec3 = DerivativeValueT< sdw::Vec3, "C3D_DerivVec3" >;
	using DerivVec4 = DerivativeValueT< sdw::Vec4, "C3D_DerivVec4" >;
	using DerivTex = DerivVec2;

	using SurfaceBase = SurfaceBaseT< sdw::Vec3, sdw::Vec4, sdw::Vec3 >;
	using Surface = SurfaceT< sdw::Vec3, sdw::Vec4, sdw::Vec3 >;
	using DerivSurfaceBase = SurfaceBaseT< DerivVec3, DerivVec4, DerivVec3 >;
	using DerivSurface = SurfaceT< DerivVec3, DerivVec4, DerivVec3 >;
	using RasterizerSurfaceBase = RasterizerSurfaceBaseT< sdw::Vec3, sdw::Vec4, sdw::Vec3, sdw::Vec4 >;
	using DerivRasterizerSurfaceBase = RasterizerSurfaceBaseT< DerivVec3, DerivVec4, DerivVec3, DerivVec4 >;

	template< ast::var::Flag FlagT >
	using FragmentSurfaceT = RasterizerSurfaceT< sdw::Vec3, sdw::Vec4, sdw::Vec3, sdw::Vec4, sdw::Vec3, FlagT >;
	using RasterizerSurface = RasterizerSurfaceT< sdw::Vec3, sdw::Vec4, sdw::Vec3, sdw::Vec4, sdw::Vec3, ast::var::Flag::eNone >;
	using DerivFragmentSurface = RasterizerSurfaceT< sdw::Vec3, sdw::Vec4, sdw::Vec3, sdw::Vec4, DerivTex, ast::var::Flag::eNone >;
	using AllDerivFragmentSurface = RasterizerSurfaceT< DerivVec3, DerivVec4, DerivVec3, DerivVec4, DerivTex, ast::var::Flag::eNone >;
	template< typename TexcoordT >
	using RasterSurfaceT = RasterizerSurfaceT< sdw::Vec3, sdw::Vec4, sdw::Vec3, sdw::Vec4, TexcoordT, ast::var::Flag::eNone >;
	using MeshVertex = MeshVertexT< sdw::var::Flag::eNone >;
	using VoxelSurface = VoxelSurfaceT< sdw::var::Flag::eNone >;
	using OverlaySurface = OverlaySurfaceT< sdw::var::Flag::eNone >;

	template< sdw::var::Flag FlagT >
	using Colour1FT = ColourT< sdw::Float, FlagT >;
	template< sdw::var::Flag FlagT >
	using Colour3FT = ColourT< sdw::Vec3, FlagT >;
	template< sdw::var::Flag FlagT >
	using Colour4FT = ColourT< sdw::Vec4, FlagT >;
	template< sdw::var::Flag FlagT >
	using Position2FT = PositionT< sdw::Vec2, FlagT >;
	template< sdw::var::Flag FlagT >
	using Position3FT = PositionT< sdw::Vec3, FlagT >;
	template< sdw::var::Flag FlagT >
	using Position4FT = PositionT< sdw::Vec4, FlagT >;
	template< sdw::var::Flag FlagT >
	using PosUv2FT = PosUvT< sdw::Vec2, FlagT >;
	template< sdw::var::Flag FlagT >
	using PosUv3FT = PosUvT< sdw::Vec3, FlagT >;
	template< sdw::var::Flag FlagT >
	using PosUv4FT = PosUvT< sdw::Vec4, FlagT >;
	template< sdw::var::Flag FlagT >
	using Uv2FT = UvT< sdw::Vec2, FlagT >;
	template< sdw::var::Flag FlagT >
	using Uv3FT = UvT< sdw::Vec3, FlagT >;
	template< sdw::var::Flag FlagT >
	using TwoUv2FT = TwoUvT< sdw::Vec2, FlagT >;
	template< sdw::var::Flag FlagT >
	using TwoUv3FT = TwoUvT< sdw::Vec3, FlagT >;

	class BackgroundModel;
	class BufferBase;
	class BRDFHelpers;
	class CookTorranceBRDF;
	class ClearcoatBRDF;
	class ClusteredLights;
	class DebugOutput;
	class DebugOutputCategory;
	class DiffuseBRDF;
	class Fog;
	class Lights;
	class LightsBuffer;
	class LightingModel;
	class Materials;
	class PassShaders;
	class ReflectionModel;
	class ScatteringModel;
	class Shadow;
	class ShadowsBuffer;
	class SheenBRDF;
	class SpecularBRDF;
	class SssProfiles;
	class SssTransmittance;
	class SubmeshShaders;
	class TextureAnimations;
	class TextureConfigurations;
	class Utils;

	class PhongLightingModel;
	class PhongReflectionModel;

	class PbrLightingModel;
	class PbrReflectionModel;

	template< typename DataT >
	class BufferT;

	template< typename BrdfCreatorT >
	struct BrdfDescT
	{
		String name;
		BrdfCreatorT create;
	};

	template< typename BrdfCreatorT >
	using BrdfArrayT = Vector< BrdfDescT< BrdfCreatorT > >;

	using BackgroundModelPtr = RawUniquePtr< BackgroundModel >;
	using ReflectionModelPtr = RawUniquePtr< ReflectionModel >;
	using LightingModelPtr = UniquePtr < LightingModel >;
	using ClearcoatBRDFPtr = UniquePtr< ClearcoatBRDF >;
	using DiffuseBRDFPtr = UniquePtr < DiffuseBRDF >;
	using SheenBRDFPtr = UniquePtr < SheenBRDF >;
	using SpecularBRDFPtr = UniquePtr < SpecularBRDF >;
	using ScatteringModelPtr = UniquePtr < ScatteringModel >;

	using DiffuseBrdfCreator = Function< DiffuseBRDFPtr( sdw::ShaderWriter &, BRDFHelpers & ) >;
	using SpecularBrdfCreator = Function< SpecularBRDFPtr( sdw::ShaderWriter &, BRDFHelpers & ) >;
	using SheenBrdfCreator = Function< SheenBRDFPtr( sdw::ShaderWriter &, BRDFHelpers & ) >;
	using ClearcoatBrdfCreator = Function< ClearcoatBRDFPtr( sdw::ShaderWriter &, BRDFHelpers & ) >;
	using ScatteringModelCreator = Function< ScatteringModelPtr( sdw::ShaderWriter & ) >;

	using DiffuseBrdfDesc = BrdfDescT< DiffuseBrdfCreator >;
	using SpecularBrdfDesc = BrdfDescT< SpecularBrdfCreator >;
	using SheenBrdfDesc = BrdfDescT< SheenBrdfCreator >;
	using ClearcoatBrdfDesc = BrdfDescT< ClearcoatBrdfCreator >;
	using ScatteringModelDesc = BrdfDescT< ScatteringModelCreator >;

	using DiffuseBrdfArray = BrdfArrayT< DiffuseBrdfCreator >;
	using SpecularBrdfArray = BrdfArrayT< SpecularBrdfCreator >;
	using SheenBrdfArray = BrdfArrayT< SheenBrdfCreator >;
	using ClearcoatBrdfArray = BrdfArrayT< ClearcoatBrdfCreator >;
	using ScatteringModelArray = BrdfArrayT< ScatteringModelCreator >;

	/** @cond !Doxygen */
	CU_DeclareDeleter( c3d::shader, ClearcoatBRDF, C3D_API );
	CU_DeclareDeleter( c3d::shader, DiffuseBRDF, C3D_API );
	CU_DeclareDeleter( c3d::shader, LightingModel, C3D_API );
	CU_DeclareDeleter( c3d::shader, SheenBRDF, C3D_API );
	CU_DeclareDeleter( c3d::shader, SpecularBRDF, C3D_API );
	CU_DeclareDeleter( c3d::shader, ScatteringModel, C3D_API );

	CU_DeclareSmartPtr( c3d::shader, LightsBuffer, C3D_API );
	CU_DeclareSmartPtr( c3d::shader, Material, C3D_API );
	CU_DeclareSmartPtr( c3d::shader, Shadow, C3D_API );
	CU_DeclareSmartPtr( c3d::shader, ShadowsBuffer, C3D_API );
	CU_DeclareSmartPtr( c3d::shader, SssTransmittance, C3D_API );

	Writer_Parameter( AABB );
	Writer_Parameter( AllDerivFragmentSurface );
	Writer_Parameter( BlendComponents );
	Writer_Parameter( Cone );
	Writer_Parameter( DerivFloat );
	Writer_Parameter( DerivTex );
	Writer_Parameter( DerivVec2 );
	Writer_Parameter( DerivVec3 );
	Writer_Parameter( DerivVec4 );
	Writer_Parameter( DerivFragmentSurface );
	Writer_Parameter( DirectionalLight );
	Writer_Parameter( DirectionalShadowData );
	Writer_Parameter( DirectLighting );
	Writer_Parameter( Frustum );
	Writer_Parameter( Intersection );
	Writer_Parameter( LayeredLpvGridData );
	Writer_Parameter( Light );
	Writer_Parameter( LightSurface );
	Writer_Parameter( LpvGridData );
	Writer_Parameter( LpvLightData );
	Writer_Parameter( Material );
	Writer_Parameter( Meshlet );
	Writer_Parameter( Plane );
	Writer_Parameter( PointLight );
	Writer_Parameter( PointShadowData );
	Writer_Parameter( Ray );
	Writer_Parameter( ReflectionRefraction );
	Writer_Parameter( ShadowData );
	Writer_Parameter( SpotLight );
	Writer_Parameter( SpotShadowData );
	Writer_Parameter( Surface );
	Writer_Parameter( TextureTransformData );
	Writer_Parameter( TextureConfigData );
	Writer_Parameter( MeshVertex );
	Writer_Parameter( VoxelData );
	Writer_Parameter( VoxelSurface );
	/** @endcond */

	struct LightingModelSpec
	{
		DiffuseBRDFPtr diffuse;
		SpecularBRDFPtr specular;
		SheenBRDFPtr sheen;
		ClearcoatBRDFPtr clearcoat;
		ScatteringModelPtr scattering;
	};

	struct LightingModelDesc
	{
		DiffuseBrdfDesc diffuse;
		SpecularBrdfDesc specular;
		SheenBrdfDesc sheen;
		ClearcoatBrdfDesc clearcoat;
		ScatteringModelDesc scattering;
	};

	struct LightingModelNames
	{
		String diffuse;
		String specular;
		String sheen;
		String clearcoat;
		String scattering;
	};

	using LightingModelCreator = Function< LightingModelPtr( LightingModelID lightingModelId
		, LightingModelDesc const & desc
		, sdw::ShaderWriter & writer
		, Materials const & materials
		, Utils & utils
		, BRDFHelpers & brdf
		, Shadow & shadowModel
		, Lights & lights
		, bool enableVolumetric ) >;

	using BackgroundModelCreator = Function< BackgroundModelPtr( Engine const & engine
		, sdw::ShaderWriter & writer
		, Utils & utils
		, Extent2D targetSize
		, bool needsForeground
		, uint32_t & binding
		, uint32_t set ) >;
	using BackgroundModelFactory = Factory< BackgroundModel
		, String
		, BackgroundModelPtr
		, BackgroundModelCreator
		, BackgroundModelID >;

	struct BufferData
		: public sdw::StructInstanceHelperT < "C3D_BufferData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec4Field< "data" > >
	{
		SDW_DeclStructInstance( C3D_INL_API, BufferData );

		BufferData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
		{
		}

		auto data()const
		{
			return getMember< "data" >();
		}
	};

	C3D_API uint32_t getSpotShadowMapCount();
	C3D_API uint32_t getPointShadowMapCount();
	C3D_API void groupMemoryBarrierWithGroupSync( sdw::ShaderWriter & writer );

	C3D_API String concatModelNames( String lhs
		, String rhs );

	template< typename ... ExprT >
	inline sdw::expr::ExprList makeExprList( ExprT && ... expr )
	{
		sdw::expr::ExprList result;
		( result.emplace_back( c3d::forward< ExprT >( expr ) ), ... );
		return result;
	}

	template< typename OutComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::Int const & in )
	{
		return writer.template cast< OutComponentT >( in );
	}

	template< typename OutComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::UInt const & in )
	{
		return writer.template cast< OutComponentT >( in );
	}

	template< typename OutComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::Float const & in )
	{
		return writer.template cast< OutComponentT >( in );
	}

	template< typename OutComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::Double const & in )
	{
		return writer.template cast< OutComponentT >( in );
	}

	template< typename OutComponentT, typename InComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::Vec2T< InComponentT > const & in )
	{
		return writer.template cast< OutComponentT >( in.x() );
	}

	template< typename OutComponentT, typename InComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::Vec3T< InComponentT > const & in )
	{
		return writer.template cast< OutComponentT >( in.x() );
	}

	template< typename OutComponentT, typename InComponentT >
	OutComponentT makeVec1T( sdw::ShaderWriter & writer, sdw::Vec4T< InComponentT > const & in )
	{
		return writer.template cast< OutComponentT >( in.x() );
	}

	template< typename OutComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::Int const & in )
	{
		return sdw::vec2T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::UInt const & in )
	{
		return sdw::vec2T( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::Float const & in )
	{
		return sdw::vec2T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::Double const & in )
	{
		return sdw::vec2T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::Vec2T< InComponentT > const & in )
	{
		return sdw::vec2T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::Vec3T< InComponentT > const & in )
	{
		return sdw::vec2T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec2T< OutComponentT > makeVec2T( sdw::ShaderWriter & writer, sdw::Vec4T< InComponentT > const & in )
	{
		return sdw::vec2T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() ) );
	}

	template< typename OutComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::Int const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::UInt const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::Float const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::Double const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::Vec2T< InComponentT > const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() )
			, writer.template cast< OutComponentT >( 0 ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::Vec3T< InComponentT > const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() )
			, writer.template cast< OutComponentT >( in.z() ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec3T< OutComponentT > makeVec3T( sdw::ShaderWriter & writer, sdw::Vec4T< InComponentT > const & in )
	{
		return sdw::vec3T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() )
			, writer.template cast< OutComponentT >( in.z() ) );
	}

	template< typename OutComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::Int const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::UInt const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::Float const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::Double const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::Vec2T< InComponentT > const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() )
			, writer.template cast< OutComponentT >( 0 )
			, writer.template cast< OutComponentT >( 1 ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::Vec3T< InComponentT > const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() )
			, writer.template cast< OutComponentT >( in.z() )
			, writer.template cast< OutComponentT >( 1 ) );
	}

	template< typename OutComponentT, typename InComponentT >
	sdw::Vec4T< OutComponentT > makeVec4T( sdw::ShaderWriter & writer, sdw::Vec4T< InComponentT > const & in )
	{
		return sdw::vec4T< OutComponentT >( writer.template cast< OutComponentT >( in.x() )
			, writer.template cast< OutComponentT >( in.y() )
			, writer.template cast< OutComponentT >( in.z() )
			, writer.template cast< OutComponentT >( in.w() ) );
	}

	template< sdw::type::ImageFormat FormatT, typename InputT >
	sdw::ImageFetchT< FormatT > makeFetchT( sdw::ShaderWriter & writer, InputT const & in )
	{
		using TypeT = sdw::ImageFetchT< FormatT >;
		using ComponentT = sdw::TypeTraits< TypeT >::ComponentType;
		if constexpr ( sdw::TypeTraits< TypeT >::ComponentCount == 1u )
			return makeVec1T< ComponentT >( writer, in );
		else if constexpr ( sdw::TypeTraits< TypeT >::ComponentCount == 2u )
			return makeVec2T< ComponentT >( writer, in );
		else if constexpr ( sdw::TypeTraits< TypeT >::ComponentCount == 3u )
			return makeVec3T< ComponentT >( writer, in );
		else
			return makeVec4T< ComponentT >( writer, in );
	}

	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Int const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::IVec2 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::IVec3 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::IVec4 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UInt const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UVec2 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UVec3 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::UVec4 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Float const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Vec2 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Vec3 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Vec4 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::Double const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::DVec2 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::DVec3 const & in );
	C3D_API sdw::Vec3 makeVec3( sdw::ShaderWriter & writer, sdw::DVec4 const & in );

	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Int const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::IVec2 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::IVec3 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::IVec4 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UInt const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UVec2 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UVec3 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::UVec4 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Float const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Vec2 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Vec3 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Vec4 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::Double const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::DVec2 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::DVec3 const & in );
	C3D_API sdw::Vec4 makeVec4( sdw::ShaderWriter & writer, sdw::DVec4 const & in );

	//@}
}

#endif
