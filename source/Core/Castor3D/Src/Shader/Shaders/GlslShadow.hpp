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
			C3D_API Shadow( glsl::GlslWriter & writer );
			C3D_API void declare( ShadowType type
				, uint32_t & index );
			C3D_API void declareDirectional( ShadowType type
				, uint32_t & index );
			C3D_API void declarePoint( ShadowType type
				, uint32_t & index );
			C3D_API void declareSpot( ShadowType type
				, uint32_t & index );
			C3D_API glsl::Float computeDirectionalShadow( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );
			C3D_API glsl::Float computeSpotShadow( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal
				, glsl::Int const & index );
			C3D_API glsl::Float computePointShadow( glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal
				, glsl::Float const & farPlane
				, glsl::Int const & index );
			C3D_API glsl::Float computeSpotShadow( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );
			C3D_API glsl::Float computePointShadow( glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal
				, glsl::Float const & farPlane );

		private:
			void doDeclareGetRandom();
			void doDeclareGetShadowOffset();
			void doDeclareChebyshevUpperBound();
			void doDeclareGetLightSpacePosition();
			void doDeclareComputeDirectionalShadow();
			void doDeclareComputeSpotShadow();
			void doDeclareComputePointShadow();
			void doDeclareComputeOneSpotShadow();
			void doDeclareComputeOnePointShadow();

		private:
			glsl::GlslWriter & m_writer;
			glsl::Function< glsl::Float
				, glsl::InVec4 > m_getRandom;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3 > m_getShadowOffset;
			glsl::Function < glsl::Float
				, glsl::InVec2
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat > m_chebyshevUpperBound;
			glsl::Function< glsl::Vec3
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_getLightSpacePosition;
			glsl::Function< glsl::Float
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeDirectional;
			glsl::Function< glsl::Float
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InInt > m_computeSpot;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt > m_computePoint;
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
		};
	}
}

#endif
