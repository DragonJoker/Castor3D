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
			C3D_API static castor::String const MapVarianceDirectional;
			C3D_API static castor::String const MapVarianceSpot;
			C3D_API static castor::String const MapVariancePoint;
			C3D_API static castor::String const MapNormalDepthDirectional;
			C3D_API static castor::String const MapNormalDepthSpot;
			C3D_API static castor::String const MapNormalDepthPoint;
			C3D_API static castor::String const MapPositionDirectional;
			C3D_API static castor::String const MapPositionSpot;
			C3D_API static castor::String const MapPositionPoint;
			C3D_API static castor::String const MapFluxDirectional;
			C3D_API static castor::String const MapFluxSpot;
			C3D_API static castor::String const MapFluxPoint;

		public:
			C3D_API Shadow( ShadowOptions shadowOptions
				, sdw::ShaderWriter & writer
				, Utils & utils );
			C3D_API void declare( uint32_t & index );
			C3D_API void declareDirectional( uint32_t & index );
			C3D_API void declarePoint( uint32_t & index );
			C3D_API void declareSpot( uint32_t & index );
			C3D_API sdw::Float computeDirectional( sdw::Int const & shadowType
				, sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::UInt const & maxCascade
				, sdw::Vec3 const & normal )const;
			C3D_API sdw::Float computeSpot( sdw::Int const & shadowType
				, sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::Vec3 const & normal
				, sdw::Int const & index )const;
			C3D_API sdw::Float computePoint( sdw::Int const & shadowType
				, sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::Vec3 const & normal
				, sdw::Float const & farPlane
				, sdw::Int const & index )const;
			C3D_API void computeVolumetric( sdw::Int const & shadowType
				, sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Vec2 const & clipSpacePosition
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & eyePosition
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::UInt const & maxCascade
				, sdw::Vec3 const & lightColour
				, sdw::Vec2 const & lightIntensity
				, sdw::UInt const & lightVolumetricSteps
				, sdw::Float const & lightVolumetricScattering
				, OutputComponents & parentOutput )const;
			C3D_API sdw::Vec4 getLightSpacePosition( sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition )const;

			bool isEnabled()const
			{
				return m_shadowOptions.enabled;
			}

		private:
			void doDeclareGetRandom();
			void doDeclareGetShadowOffset();
			void doDeclareChebyshevUpperBound();
			void doDeclareTextureProj();
			void doDeclareFilterPCF();
			void doDeclareTextureProjCascade();
			void doDeclareFilterPCFCascade();
			void doDeclareGetLightSpacePosition();
			void doDeclareComputeDirectionalShadow();
			void doDeclareComputeSpotShadow();
			void doDeclareComputePointShadow();
			void doDeclareVolumetric();

		private:
			sdw::ShaderWriter & m_writer;
			Utils & m_utils;
			ShadowOptions m_shadowOptions;
			sdw::Function< sdw::Float
				, sdw::InVec4 > m_getRandom;
			sdw::Function< sdw::Float
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat > m_getShadowOffset;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InVec2
				, sdw::InSampledImage2DArrayRgba32
				, sdw::InInt
				, sdw::InFloat > m_textureProj;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InSampledImage2DArrayRgba32
				, sdw::InInt
				, sdw::InVec2
				, sdw::InFloat > m_filterPCF;
			sdw::Function < sdw::Float
				, sdw::InVec2
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InFloat > m_chebyshevUpperBound;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InVec2
				, sdw::InSampledImage2DArrayRgba32
				, sdw::InUInt
				, sdw::InFloat > m_textureProjCascade;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InSampledImage2DArrayRgba32
				, sdw::InVec2
				, sdw::InUInt
				, sdw::InFloat > m_filterPCFCascade;
			sdw::Function< sdw::Vec4
				, sdw::InMat4
				, sdw::InVec3 > m_getLightSpacePosition;
			sdw::Function< sdw::Float
				, sdw::InInt
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InUInt
				, sdw::InVec3 > m_computeDirectional;
			sdw::Function< sdw::Float
				, sdw::InInt
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InInt > m_computeSpot;
			sdw::Function< sdw::Float
				, sdw::InInt
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt > m_computePoint;
			sdw::Function< sdw::Void
				, sdw::InInt
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InVec2
				, sdw::InUInt
				, sdw::InFloat
				, OutputComponents & > m_computeVolumetric;
		};
	}
}

#endif
