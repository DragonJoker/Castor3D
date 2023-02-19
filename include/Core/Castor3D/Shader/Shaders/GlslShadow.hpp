/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslShadow_H___
#define ___C3D_GlslShadow_H___

#include "SdwModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

#define C3D_DebugSpotShadows 0
#define C3D_DebugCascades 0

namespace castor3d
{
	namespace shader
	{
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
				, LightSurface const & lightSurface
				, sdw::Mat4 const & lightMatrix
				, sdw::Float const & depth );
			C3D_API sdw::Float computePoint( shader::ShadowData const & light
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
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InMat4 > m_computeSpot;
			sdw::Function< sdw::Float
				, InShadowData
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
}

#endif
