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
		// Reserve binding IDs even if shadows are disabled
		bool reserveIds{ false };

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
			, bool rsm
			, bool reserveIds = false )
			: type{ sceneFlags & SceneFlag::eShadowAny }
			, vsm{ vsm }
			, rsm{ rsm }
			, reserveIds{ reserveIds }
		{
		}
	};

	struct AABB;
	struct Cone;
	struct DirectionalLight;
	struct DirectionalShadowData;
	struct DirectLighting;
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

	class BackgroundModel;
	class BufferBase;
	class BRDFHelpers;
	class CookTorranceBRDF;
	class ClusteredLights;
	class DebugOutput;
	class DebugOutputCategory;
	class Fog;
	class Lights;
	class LightsBuffer;
	class LightingModel;
	class Materials;
	class PassShaders;
	class ReflectionModel;
	class Shadow;
	class ShadowsBuffer;
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

	CU_DeclareSmartPtr( castor3d::shader, LightsBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d::shader, LightingModel, C3D_API );
	CU_DeclareSmartPtr( castor3d::shader, Material, C3D_API );
	CU_DeclareSmartPtr( castor3d::shader, Shadow, C3D_API );
	CU_DeclareSmartPtr( castor3d::shader, ShadowsBuffer, C3D_API );
	CU_DeclareSmartPtr( castor3d::shader, SssTransmittance, C3D_API );

	using ReflectionModelPtr = castor::RawUniquePtr< ReflectionModel >;
	using LightingModelCreator = castor::Function< LightingModelUPtr( LightingModelID lightingModelId
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

	using BackgroundModelPtr = castor::RawUniquePtr< BackgroundModel >;
	using BackgroundModelCreator = castor::Function< BackgroundModelPtr( Engine const & engine
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
	Writer_Parameter( Intersection );
	Writer_Parameter( LayeredLpvGridData );
	Writer_Parameter( Light );
	Writer_Parameter( DirectLighting );
	Writer_Parameter( LightSurface );
	Writer_Parameter( LpvGridData );
	Writer_Parameter( LpvLightData );
	Writer_Parameter( Material );
	Writer_Parameter( Meshlet );
	Writer_Parameter( Plane );
	Writer_Parameter( PointLight );
	Writer_Parameter( PointShadowData );
	Writer_Parameter( Ray );
	Writer_Parameter( ShadowData );
	Writer_Parameter( SpotLight );
	Writer_Parameter( SpotShadowData );
	Writer_Parameter( Surface );
	Writer_Parameter( TextureTransformData );
	Writer_Parameter( TextureConfigData );
	Writer_Parameter( MeshVertex );
	Writer_Parameter( VoxelData );
	Writer_Parameter( VoxelSurface );

	C3D_API uint32_t getSpotShadowMapCount();
	C3D_API uint32_t getPointShadowMapCount();
	C3D_API void groupMemoryBarrierWithGroupSync( sdw::ShaderWriter & writer );

	C3D_API castor::String concatModelNames( castor::String lhs
		, castor::String rhs );

	struct BufferData
		: public sdw::StructInstanceHelperT < "C3D_BufferData"
		, sdw::type::MemoryLayout::eStd430
		, sdw::Vec4Field< "data" > >
	{
		BufferData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		auto data()const {
			return getMember< "data" >();
		}
	};

	template< typename ... ExprT >
	inline sdw::expr::ExprList makeExprList( ExprT && ... expr )
	{
		sdw::expr::ExprList result;
		( result.emplace_back( castor::forward< ExprT >( expr ) ), ... );
		return result;
	}

	//@}
	//@}
}

#endif
