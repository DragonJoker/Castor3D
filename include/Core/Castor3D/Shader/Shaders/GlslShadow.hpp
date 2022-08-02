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
			C3D_API sdw::Float computeDirectional( shader::Light const & light
				, Surface const & surface
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::UInt const & maxCascade );
			C3D_API sdw::Float computeDirectional( shader::Light const & light
				, sdw::Vec3 const & wsPosition
				, sdw::Vec3 const & wsNormal
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::UInt const & maxCascade );
			C3D_API sdw::Float computeSpot( shader::Light const & light
				, Surface const & surface
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection );
			C3D_API sdw::Float computePoint( shader::Light const & light
				, Surface const & surface
				, sdw::Vec3 const & lightDirection );
			C3D_API sdw::Float computeVolumetric( shader::Light const & light
				, Surface const & surface
				, sdw::Vec3 const & eyePosition
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::UInt const & maxCascade );
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
			sdw::RetFloat filterPCF( sdw::Vec4 const & lightSpacePosition
				, sdw::CombinedImage2DR32 const & shadowMap
				, sdw::Vec2 const & invTexDim
				, sdw::Float const & bias );
			sdw::RetFloat filterPCF( sdw::Vec4 const & lightSpacePosition
				, sdw::CombinedImage2DArrayR32 const & shadowMap
				, sdw::Vec2 const & invTexDim
				, sdw::UInt const & cascadeIndex
				, sdw::Float const & bias );
			sdw::RetFloat filterPCF( sdw::Vec4 const & lightSpacePosition
				, sdw::CombinedImage2DArrayR32 const & shadowMap
				, sdw::Vec2 const & invTexDim
				, sdw::Int const & index
				, sdw::Float const & depth
				, sdw::Float const & bias );
			sdw::RetFloat filterPCF( sdw::Vec3 const & lightToVertex
				, sdw::CombinedImageCubeArrayR32 const & shadowMap
				, sdw::Vec2 const & invTexDim
				, sdw::Int const & index
				, sdw::Float const & depth
				, sdw::Float const & bias );
			sdw::RetFloat chebyshevUpperBound( sdw::Vec2 const & moments
				, sdw::Float const & depth
				, sdw::Float const & minVariance
				, sdw::Float const & varianceBias );
			sdw::RetFloat filterVSM( sdw::Vec3 const & lightToVertex
				, sdw::CombinedImageCubeArrayRg32 const & shadowMap
				, sdw::Int const & index
				, sdw::Float const & depth
				, sdw::Float const & minVariance
				, sdw::Float const & varianceBias );

		private:
			sdw::ShaderWriter & m_writer;
			ShadowOptions m_shadowOptions;
			sdw::Function< sdw::Float
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat > m_getShadowOffset;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InCombinedImage2DArrayR32
				, sdw::InVec2
				, sdw::InInt
				, sdw::InFloat
				, sdw::InFloat > m_filterPCF;
			sdw::Function < sdw::Float
				, sdw::InVec2
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat > m_chebyshevUpperBound;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InCombinedImage2DR32
				, sdw::InVec2
				, sdw::InFloat > m_filterPCFNoCascade;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InCombinedImage2DArrayR32
				, sdw::InVec2
				, sdw::InUInt
				, sdw::InFloat > m_filterPCFCascade;
			sdw::Function< sdw::Float
				, sdw::InVec3
				, sdw::InCombinedImageCubeArrayR32
				, sdw::InVec2
				, sdw::InInt
				, sdw::InFloat
				, sdw::InFloat > m_filterPCFCube;
			sdw::Function < sdw::Float
				, sdw::InVec3
				, sdw::InCombinedImageCubeArrayRg32
				, sdw::InInt
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat > m_filterVSMCube;
			sdw::Function< sdw::Vec4
				, sdw::InMat4
				, sdw::InVec3 > m_getLightSpacePosition;
			sdw::Function< sdw::Float
				, InLight
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InUInt > m_computeDirectional;
			sdw::Function< sdw::Float
				, InLight
				, InSurface
				, sdw::InMat4
				, sdw::InVec3 > m_computeSpot;
			sdw::Function< sdw::Float
				, InLight
				, InSurface
				, sdw::InVec3 > m_computePoint;
			sdw::Function< sdw::Float
				, InLight
				, InSurface
				, sdw::InVec3
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InUInt > m_computeVolumetric;
		};
	}
}

#endif
