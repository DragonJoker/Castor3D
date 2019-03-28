/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_SHADOW_H___
#define ___GLSL_SHADOW_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

#define C3D_DebugSpotShadows 0

namespace castor3d
{
	namespace shader
	{
		struct OutputComponents
		{
			C3D_API explicit OutputComponents( sdw::ShaderWriter & writer );
			C3D_API OutputComponents( sdw::InOutVec3 const & diffuse
				, sdw::InOutVec3 const & specular );

			C3D_API ast::expr::Expr * getExpr()const;
			C3D_API sdw::Shader * getShader()const;
			C3D_API void setVar( ast::var::VariableList::const_iterator & var );

			sdw::InOutVec3 m_diffuse;
			sdw::InOutVec3 m_specular;

		private:
			ast::expr::ExprPtr m_expr;
		};

		C3D_API ast::expr::ExprList makeFnArg( sdw::Shader & shader
			, OutputComponents const & value );

		class Shadow
		{
		public:
			C3D_API static castor::String const MapShadowDirectional;
			C3D_API static castor::String const MapShadowSpot;
			C3D_API static castor::String const MapShadowPoint;
			C3D_API static castor::String const MapDepthDirectional;
			C3D_API static castor::String const MapDepthSpot;
			C3D_API static castor::String const MapDepthPoint;

		public:
			C3D_API Shadow( sdw::ShaderWriter & writer
				, Utils & utils );
			C3D_API void declare( uint32_t & index
				, uint32_t maxCascades );
			C3D_API void declareDirectional( ShadowType type
				, uint32_t & index
				, uint32_t maxCascades );
			C3D_API void declarePoint( ShadowType type
				, uint32_t & index );
			C3D_API void declareSpot( ShadowType type
				, uint32_t & index );
			C3D_API sdw::Float computeDirectionalShadow( sdw::Int const & shadowType
				, sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::Vec3 const & normal )const;
			C3D_API sdw::Float computeSpotShadow( sdw::Int const & shadowType
				, sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::Vec3 const & normal
				, sdw::Int const & index )const;
			C3D_API sdw::Float computePointShadow( sdw::Int const & shadowType
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
				, sdw::Vec3 const & lightColour
				, sdw::Vec2 const & lightIntensity
				, sdw::UInt const & lightVolumetricSteps
				, sdw::Float const & lightVolumetricScattering
				, OutputComponents & parentOutput )const;
			C3D_API sdw::Float computeDirectionalShadow( sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::Vec3 const & normal )const;
			C3D_API sdw::Float computeSpotShadow( sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::Vec3 const & normal
				, sdw::Int const & index )const;
			C3D_API sdw::Float computePointShadow( sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & lightDirection
				, sdw::Vec3 const & normal
				, sdw::Float const & farPlane
				, sdw::Int const & index )const;
			C3D_API void computeVolumetric( sdw::Vec2 const & shadowOffsets
				, sdw::Vec2 const & shadowVariance
				, sdw::Vec2 const & clipSpacePosition
				, sdw::Vec3 const & worldSpacePosition
				, sdw::Vec3 const & eyePosition
				, sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & lightDirection
				, sdw::UInt const & cascadeIndex
				, sdw::Vec3 const & lightColour
				, sdw::Vec2 const & lightIntensity
				, sdw::UInt const & lightVolumetricSteps
				, sdw::Float const & lightVolumetricScattering
				, OutputComponents & parentOutput )const;
			C3D_API sdw::Vec4 getLightSpacePosition( sdw::Mat4 const & lightMatrix
				, sdw::Vec3 const & worldSpacePosition )const;

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
			void doDeclareComputeOneDirectionalShadow( ShadowType type );
			void doDeclareComputeOneSpotShadow( ShadowType type );
			void doDeclareComputeOnePointShadow( ShadowType type );
			void doDeclareOneVolumetric( ShadowType type );

		private:
			sdw::ShaderWriter & m_writer;
			Utils & m_utils;
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
				, sdw::InSampledImage2DArrayR32
				, sdw::InInt
				, sdw::InFloat > m_textureProj;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InSampledImage2DArrayR32
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
				, sdw::InSampledImage2DArrayR32
				, sdw::InUInt
				, sdw::InFloat > m_textureProjCascade;
			sdw::Function< sdw::Float
				, sdw::InVec4
				, sdw::InSampledImage2DArrayR32
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
				, sdw::InVec3
				, sdw::InVec2
				, sdw::InUInt
				, sdw::InFloat
				, OutputComponents & > m_computeVolumetric;
			sdw::Function< sdw::Float
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InVec3 > m_computeOneDirectional;
			sdw::Function< sdw::Float
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InInt > m_computeOneSpot;
			sdw::Function< sdw::Float
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InInt > m_computeOnePoint;
			sdw::Function< sdw::Void
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec2
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InMat4
				, sdw::InVec3
				, sdw::InUInt
				, sdw::InVec3
				, sdw::InVec2
				, sdw::InUInt
				, sdw::InFloat
				, OutputComponents & > m_computeOneVolumetric;
		};
	}
}

#endif
