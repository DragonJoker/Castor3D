/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_SHADOW_H___
#define ___GLSL_SHADOW_H___

#include "Castor3DPrerequisites.hpp"

#include <GlslIntrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		struct OutputComponents
		{
			C3D_API explicit OutputComponents( glsl::GlslWriter & writer );
			C3D_API OutputComponents( glsl::InOutVec3 const & diffuse
				, glsl::InOutVec3 const & specular );
			glsl::InOutVec3 m_diffuse;
			glsl::InOutVec3 m_specular;
		};

		C3D_API castor::String paramToString( castor::String & sep
			, OutputComponents const & value );
		C3D_API castor::String toString( OutputComponents const & value );

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
			C3D_API explicit Shadow( glsl::GlslWriter & writer );
			C3D_API void declare( uint32_t & index );
			C3D_API void declareDirectional( ShadowType type
				, uint32_t & index );
			C3D_API void declarePoint( ShadowType type
				, uint32_t & index );
			C3D_API void declareSpot( ShadowType type
				, uint32_t & index );
			C3D_API glsl::Float computeDirectionalShadow( glsl::Int const & shadowType
				, glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );
			C3D_API glsl::Float computeSpotShadow( glsl::Int const & shadowType
				, glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal
				, glsl::Int const & index );
			C3D_API glsl::Float computePointShadow( glsl::Int const & shadowType
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal
				, glsl::Float const & farPlane
				, glsl::Int const & index );
			C3D_API void computeVolumetric( glsl::Int const & shadowType
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & eyePosition
				, glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & lightColour
				, glsl::Vec2 const & lightIntensity
				, OutputComponents & parentOutput );
			C3D_API glsl::Float computeDirectionalShadow( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );
			C3D_API glsl::Float computeSpotShadow( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );
			C3D_API glsl::Float computePointShadow( glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal
				, glsl::Float const & farPlane );
			C3D_API void computeVolumetric( glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & eyePosition
				, glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & lightColour
				, glsl::Vec2 const & lightIntensity
				, OutputComponents & parentOutput );

		private:
			glsl::Float chebyshevUpperBound( glsl::Vec2 const & moments
				, glsl::Float const & distance
				, glsl::Float const & minVariance
				, glsl::Float const & varianceBias );
			glsl::Float getShadowOffset( glsl::Vec3 const & normal
				, glsl::Vec3 const & lightDirection
				, glsl::Float const & minOffset
				, glsl::Float const & maxSlopeOffset );
			glsl::Float textureProj( glsl::Vec4 const & lightSpacePosition
				, glsl::Vec2 const & offset
				, glsl::Sampler2D const & shadowMap
				, glsl::Float const & bias );
			glsl::Float filterPCF( glsl::Vec4 const & lightSpacePosition
				, glsl::Sampler2D const & shadowMap
				, glsl::Float const & bias );
			glsl::Vec4 getLightSpacePosition( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition );

		private:
			void doDeclareGetRandom();
			void doDeclareGetShadowOffset();
			void doDeclareChebyshevUpperBound();
			void doDeclareTextureProj();
			void doDeclareFilterPCF();
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
			glsl::GlslWriter & m_writer;
			glsl::Function< glsl::Float
				, glsl::InVec4 > m_getRandom;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat > m_getShadowOffset;
			glsl::Function< glsl::Float
				, glsl::InVec4
				, glsl::InVec2
				, glsl::InSampler2D
				, glsl::InFloat > m_textureProj;
			glsl::Function< glsl::Float
				, glsl::InVec4
				, glsl::InSampler2D
				, glsl::InFloat > m_filterPCF;
			glsl::Function < glsl::Float
				, glsl::InVec2
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat > m_chebyshevUpperBound;
			glsl::Function< glsl::Vec4
				, glsl::InMat4
				, glsl::InVec3 > m_getLightSpacePosition;
			glsl::Function< glsl::Float
				, glsl::InInt
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeDirectional;
			glsl::Function< glsl::Float
				, glsl::InInt
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InInt > m_computeSpot;
			glsl::Function< glsl::Float
				, glsl::InInt
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt > m_computePoint;
			glsl::Function< glsl::Float
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeOneDirectional;
			glsl::Function< glsl::Float
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeOneSpot;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat > m_computeOnePoint;
			glsl::Function< glsl::Void
				, glsl::InInt
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec2
				, OutputComponents & > m_computeVolumetric;
			glsl::Function< glsl::Void
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec2
				, OutputComponents & > m_computeOneVolumetric;
		};
	}
}

#endif
