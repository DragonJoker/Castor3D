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
			C3D_API sdw::Float computeSpot( shader::Light const & light
				, Surface const & surface
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection );
			C3D_API sdw::Float computePoint( shader::Light const & light
				, Surface const & surface
				, sdw::Vec3 const & lightDirection );
			C3D_API void computeVolumetric( shader::Light const & light
				, Surface const & surface
				, sdw::Vec3 const & eyePosition
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::UInt const & maxCascade
				, OutputComponents & parentOutput );
			C3D_API sdw::Vec4 getLightSpacePosition( sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition );

			bool isEnabled()const
			{
				return m_shadowOptions.type != SceneFlag::eNone;
			}

		private:
			void doDeclareGetRandom();
			void doDeclareGetShadowOffset();
			void doDeclareChebyshevUpperBound();
			void doDeclareTextureProj();
			void doDeclareFilterPCF();
			void doDeclareTextureProjNoCascade();
			void doDeclareFilterPCFNoCascade();
			void doDeclareTextureProjCascade();
			void doDeclareFilterPCFCascade();
			void doDeclareGetLightSpacePosition();
			void doDeclareComputeDirectionalShadow();
			void doDeclareComputeSpotShadow();
			void doDeclareComputePointShadow();
			void doDeclareComputeVolumetric();

		private:
			sdw::ShaderWriter & m_writer;
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
				, sdw::InSampledImage2DRgba32
				, sdw::InFloat > m_textureProjNoCascade;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InSampledImage2DRgba32
				, sdw::InVec2
				, sdw::InFloat > m_filterPCFNoCascade;
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
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InVec2
				, sdw::InSampledImage2DRgba32
				, sdw::InUInt
				, sdw::InFloat > m_textureProjTile;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InSampledImage2DRgba32
				, sdw::InVec2
				, sdw::InUInt
				, sdw::InFloat > m_filterPCFTile;
			sdw::Function< sdw::Vec4
				, sdw::InMat4
				, sdw::InVec3 > m_getLightSpacePosition;
			sdw::Function< sdw::Float
				, InLight
				, InSurface
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
			sdw::Function< sdw::Void
				, InLight
				, InSurface
				, sdw::InVec3
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InUInt
				, OutputComponents & > m_computeVolumetric;
		};
	}
}

#endif
