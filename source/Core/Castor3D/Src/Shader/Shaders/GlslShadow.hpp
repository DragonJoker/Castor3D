﻿/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

		public:
			C3D_API Shadow( glsl::GlslWriter & writer );
			C3D_API void declare( ShadowType type );
			C3D_API void declareDirectional( ShadowType type );
			C3D_API void declarePoint( ShadowType type );
			C3D_API void declareSpot( ShadowType type );
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
				, glsl::Int const & index );
			C3D_API glsl::Float computeSpotShadow( glsl::Mat4 const & lightMatrix
				, glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );
			C3D_API glsl::Float computePointShadow( glsl::Vec3 const & worldSpacePosition
				, glsl::Vec3 const & lightDirection
				, glsl::Vec3 const & normal );

		private:
			void doDeclareGetRandom();
			void doDeclareGetShadowOffset();
			void doDeclareGetLightSpacePosition();
			void doDeclareComputeDirectionalShadow();
			void doDeclareComputeSpotShadow();
			void doDeclareComputePointShadow();
			void doDeclareComputeOneSpotShadow();
			void doDeclareComputeOnePointShadow();
			void doDeclarePcfSample2D();
			void doDeclarePcfSampleCube();

		private:
			glsl::GlslWriter & m_writer;
			glsl::Function< glsl::Float
				, glsl::InVec4 > m_getRandom;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3 > m_getShadowOffset;
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
				, glsl::InInt > m_computePoint;
			glsl::Function< glsl::Float
				, glsl::InMat4
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeOneSpot;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3 > m_computeOnePoint;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InParam< glsl::Sampler2DShadow > > m_sample2D;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InParam< glsl::SamplerCubeShadow > > m_sampleCube;
		};
	}
}

#endif
