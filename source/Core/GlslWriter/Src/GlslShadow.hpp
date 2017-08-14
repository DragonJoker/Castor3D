/*
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

#include "GlslIntrinsics.hpp"

namespace glsl
{
	class Shadow
	{
	public:
		GlslWriter_API static castor::String const MapShadowDirectional;
		GlslWriter_API static castor::String const MapShadowSpot;
		GlslWriter_API static castor::String const MapShadowPoint;

	public:
		GlslWriter_API Shadow( GlslWriter & p_writer );
		GlslWriter_API void declare( ShadowType p_type );
		GlslWriter_API void declareDirectional( ShadowType p_type );
		GlslWriter_API void declarePoint( ShadowType p_type );
		GlslWriter_API void declareSpot( ShadowType p_type );
		GlslWriter_API Float computeDirectionalShadow( Mat4 const & p_lightMatrix, Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );
		GlslWriter_API Float computeSpotShadow( Mat4 const & p_lightMatrix, Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index );
		GlslWriter_API Float computePointShadow( Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index );
		GlslWriter_API Float computeSpotShadow( Mat4 const & p_lightMatrix, Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );
		GlslWriter_API Float computePointShadow( Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );

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
		GlslWriter & m_writer;
		Function< Float, InVec4 > m_getRandom;
		Function< Float, InVec3, InVec3 > m_getShadowOffset;
		Function< Vec3, InParam< Mat4 >, InVec3, InVec3, InVec3 > m_getLightSpacePosition;
		Function< Float, InParam< Mat4 >, InVec3, InVec3, InVec3 > m_computeDirectional;
		Function< Float, InParam< Mat4 >, InVec3, InVec3, InVec3, InInt > m_computeSpot;
		Function< Float, InVec3, InVec3, InVec3, InInt > m_computePoint;
		Function< Float, InParam< Mat4 >, InVec3, InVec3, InVec3 > m_computeOneSpot;
		Function< Float, InVec3, InVec3, InVec3 > m_computeOnePoint;
		Function< Float, InVec3, InFloat, InParam< Sampler2DShadow > > m_sample2D;
		Function< Float, InVec3, InFloat, InParam< SamplerCubeShadow > > m_sampleCube;
	};
}

#endif
