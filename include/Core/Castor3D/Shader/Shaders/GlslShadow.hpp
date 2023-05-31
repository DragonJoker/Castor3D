/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslShadow_H___
#define ___C3D_GlslShadow_H___

#include "Castor3D/Shader/Shaders/SdwModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include "Castor3D/Shader/Shaders/GlslBuffer.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

#define C3D_DebugSpotShadows 0
#define C3D_DebugCascades 0

namespace castor3d::shader
{
	struct ShadowData
		: public sdw::StructInstanceHelperT< "C3D_ShadowData"
			, sdw::type::MemoryLayout::eC
			, sdw::UIntField< "shadowType" >
			, sdw::UIntField< "cascadeCount" >
			, sdw::FloatField< "pcfFilterSize" >
			, sdw::UIntField< "pcfSampleCount" >
			, sdw::Vec2Field< "rawShadowOffsets" >
			, sdw::Vec2Field< "pcfShadowOffsets" >
			, sdw::FloatField< "vsmMinVariance" >
			, sdw::FloatField< "vsmLightBleedingReduction" >
			, sdw::UIntField< "volumetricSteps" >
			, sdw::FloatField< "volumetricScattering" > >
	{
		friend class ShadowsBuffer;
		friend struct DirectionalShadowData;
		friend struct PointShadowData;
		friend struct SpotShadowData;

		ShadowData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto shadowType()const { return getMember< "shadowType" >(); }
		auto pcfFilterSize()const { return getMember< "pcfFilterSize" >(); }
		auto pcfSampleCount()const { return getMember< "pcfSampleCount" >(); }
		auto rawShadowOffsets()const { return getMember< "rawShadowOffsets" >(); }
		auto pcfShadowOffsets()const { return getMember< "pcfShadowOffsets" >(); }
		auto vsmMinVariance()const { return getMember< "vsmMinVariance" >(); }
		auto vsmLightBleedingReduction()const { return getMember< "vsmLightBleedingReduction" >(); }
		auto volumetricSteps()const { return getMember< "volumetricSteps" >(); }
		auto volumetricScattering()const { return getMember< "volumetricScattering" >(); }

		C3D_API void updateShadowType( ShadowType type );

	private:
		auto cascadeCount()const { return getMember< "cascadeCount" >(); }
	};

	struct DirectionalShadowData
		: public sdw::StructInstanceHelperT< "C3D_DirectionalShadowData"
		, sdw::type::MemoryLayout::eStd140
		, sdw::StructFieldT< ShadowData, "base" >
		, sdw::Vec4ArrayField< "splitDepths", ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) / 4u >
		, sdw::Vec4ArrayField< "splitScales", ashes::getAlignedSize( MaxDirectionalCascadesCount, 4u ) / 4u >
		, sdw::Mat4ArrayField< "transforms", MaxDirectionalCascadesCount > >
	{
		DirectionalShadowData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}
		
		auto base()const { return getMember< "base" >(); }
		auto cascadeCount()const { return base().cascadeCount(); }
		auto splitDepths()const { return getMember< "splitDepths" >(); }
		auto splitScales()const { return getMember< "splitScales" >(); }
		auto transforms()const { return getMember< "transforms" >(); }
	};

	struct PointShadowData
		: public sdw::StructInstanceHelperT< "C3D_PointShadowData"
		, sdw::type::MemoryLayout::eStd140
		, sdw::StructFieldT< ShadowData, "base" > >
	{
		PointShadowData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}
		
		auto base()const { return getMember< "base" >(); }
	};

	struct SpotShadowData
		: public sdw::StructInstanceHelperT< "C3D_SpotShadowData"
		, sdw::type::MemoryLayout::eStd140
		, sdw::StructFieldT< ShadowData, "base" >
		, sdw::Mat4Field< "transform" > >
	{
		SpotShadowData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto base()const { return getMember< "base" >(); }
		auto transform()const { return getMember< "transform" >(); }
	};

	struct AllShadowData
		: public sdw::StructInstanceHelperT< "C3D_AllShadowData"
		, sdw::type::MemoryLayout::eStd140
		, sdw::StructFieldT< DirectionalShadowData, "directional" >
		, sdw::StructFieldArrayT< PointShadowData, "point", MaxPointShadowMapCount >
		, sdw::StructFieldArrayT< SpotShadowData, "spot", MaxSpotShadowMapCount > >
	{
		AllShadowData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto directional()const { return getMember< "directional" >(); }
		auto point()const { return getMember< "point" >(); }
		auto spot()const { return getMember< "spot" >(); }
	};

	class ShadowsBuffer
	{
	public:
		C3D_API ShadowsBuffer( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set
			, bool enable = true );

		C3D_API DirectionalShadowData getDirectionalShadows();
		C3D_API PointShadowData getPointShadows( sdw::Int const & index );
		C3D_API SpotShadowData getSpotShadows( sdw::Int const & index );

	private:
		std::unique_ptr< AllShadowData > m_data;
	};

	class Shadow
	{
	public:
		C3D_API static castor::String const MapDepthDirectional;
		C3D_API static castor::String const MapDepthSpot;
		C3D_API static castor::String const MapDepthPoint;
		C3D_API static castor::String const MapDepthCmpDirectional;
		C3D_API static castor::String const MapDepthCmpSpot;
		C3D_API static castor::String const MapDepthCmpPoint;
		C3D_API static castor::String const MapVarianceDirectional;
		C3D_API static castor::String const MapVarianceSpot;
		C3D_API static castor::String const MapVariancePoint;
		C3D_API static castor::String const MapNormalDirectional;
		C3D_API static castor::String const MapNormalSpot;
		C3D_API static castor::String const MapNormalPoint;
		C3D_API static castor::String const MapPositionDirectional;
		C3D_API static castor::String const MapPositionSpot;
		C3D_API static castor::String const MapPositionPoint;
		C3D_API static castor::String const MapFluxDirectional;
		C3D_API static castor::String const MapFluxSpot;
		C3D_API static castor::String const MapFluxPoint;
		C3D_API static castor::String const RandomBuffer;

	public:
		C3D_API Shadow( ShadowOptions shadowOptions
			, sdw::ShaderWriter & writer );

		C3D_API void declare( uint32_t & index
			, uint32_t set );
		C3D_API void declareDirectional( uint32_t & index
			, uint32_t set );
		C3D_API void declarePoint( uint32_t & index
			, uint32_t set );
		C3D_API void declareSpot( uint32_t & index
			, uint32_t set );

		C3D_API DirectionalShadowData getDirectionalShadows();
		C3D_API PointShadowData getPointShadows( sdw::Int const & index );
		C3D_API SpotShadowData getSpotShadows( sdw::Int const & index );

		C3D_API sdw::Float computeDirectional( shader::ShadowData const & shadows
			, sdw::Vec3 const & wsVertexToLight
			, sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade );
		C3D_API sdw::Float computeDirectional( shader::ShadowData const & light
			, LightSurface const & lightSurface
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade );
		C3D_API sdw::Float computeSpot( shader::ShadowData const & light
			, sdw::Int const & shadowMapIndex
			, LightSurface const & lightSurface
			, sdw::Mat4 const & lightMatrix
			, sdw::Float const & depth );
		C3D_API sdw::Float computePoint( shader::ShadowData const & light
			, sdw::Int const & shadowMapIndex
			, LightSurface const & lightSurface
			, sdw::Float const & depth );
		C3D_API sdw::Float computeVolumetric( shader::ShadowData const & light
			, LightSurface const & lightSurface
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade );
		C3D_API sdw::Float computeVolumetric( shader::ShadowData const & light
			, LightSurface const & lightSurface
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & scattering );
		C3D_API sdw::Float computeVolumetric( shader::ShadowData const & light
			, LightSurface const & lightSurface
			, Ray const & ray
			, sdw::Float const & stepLength
			, sdw::Mat4 const & lightMatrix
			, sdw::UInt const & cascadeIndex
			, sdw::UInt const & maxCascade
			, sdw::Float const & scattering );
		C3D_API sdw::Vec4 getLightSpacePosition( sdw::Mat4 const & lightMatrix
			, sdw::Vec3 const & worldSpacePosition );

		bool isEnabled()const
		{
			return m_shadowOptions.type != SceneFlag::eNone;
		}

	private:
		sdw::RetFloat getShadowOffset( sdw::Vec3 const & pnormal
			, sdw::Vec3 const & lightDirection
			, sdw::Float const & minOffset
			, sdw::Float const & maxSlopeOffset );
		sdw::RetFloat filterPCF( sdw::Vec3 const & lightSpacePosition
			, sdw::CombinedImage2DArrayShadowR32 const & shadowMap
			, sdw::Vec2 const & invTexDim
			, sdw::UInt const & arrayIndex
			, sdw::Float const & depthBias
			, sdw::UInt const & sampleCount
			, sdw::Float const & filterSize );
		sdw::RetFloat filterPCF( sdw::Vec3 const & lightToVertex
			, sdw::CombinedImageCubeArrayShadowR32 const & shadowMap
			, sdw::Vec2 const & invTexDim
			, sdw::UInt const & cubeIndex
			, sdw::Float const & depth
			, sdw::Float const & depthBias
			, sdw::UInt const & sampleCount
			, sdw::Float const & filterSize );
		sdw::RetFloat chebyshevUpperBound( sdw::Vec2 const & moments
			, sdw::Float const & depth
			, sdw::Float const & minVariance
			, sdw::Float const & varianceBias );

	private:
		sdw::ShaderWriter & m_writer;
		ShadowOptions m_shadowOptions;
		sdw::Vec2Array m_poissonSamples;
		ShadowsBufferUPtr m_shadowsBuffer;
		std::unique_ptr< sdw::Vec4Array > m_randomData;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat > m_getShadowOffset;
		sdw::Function < sdw::Float
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_chebyshevUpperBound;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InCombinedImage2DArrayShadowR32
			, sdw::InVec2
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InFloat > m_filterPCFArray;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InCombinedImageCubeArrayShadowR32
			, sdw::InVec2
			, sdw::InUInt
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InUInt
			, sdw::InFloat > m_filterPCFCube;
		sdw::Function< sdw::Vec4
			, sdw::InMat4
			, sdw::InVec3 > m_getLightSpacePosition;
		sdw::Function< sdw::Float
			, InShadowData
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InMat4
			, sdw::InUInt
			, sdw::InUInt > m_computeDirectional;
		sdw::Function< sdw::Float
			, InShadowData
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InMat4 > m_computeSpot;
		sdw::Function< sdw::Float
			, InShadowData
			, sdw::InInt
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3 > m_computePoint;
		sdw::Function < sdw::Float
			, InShadowData
			, sdw::InVec3
			, sdw::InVec3
			, InRay
			, sdw::InFloat
			, sdw::InMat4
			, sdw::InUInt
			, sdw::InUInt
			, sdw::InFloat > m_computeVolumetric;

	};
}

#endif
