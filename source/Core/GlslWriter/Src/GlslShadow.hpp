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

namespace GLSL
{
	class Shadow
	{
	public:
		GlslWriter_API static Castor::String const MapShadowDirectional;
		GlslWriter_API static Castor::String const MapShadowSpot;
		GlslWriter_API static Castor::String const MapShadowPoint;

	public:
		GlslWriter_API Shadow( GlslWriter & p_writer );
		GlslWriter_API void Declare( ShadowType p_type );
		GlslWriter_API void DeclareDirectional( ShadowType p_type );
		GlslWriter_API void DeclarePoint( ShadowType p_type );
		GlslWriter_API void DeclareSpot( ShadowType p_type );
		GlslWriter_API Float ComputeDirectionalShadow( Mat4 const & p_lightMatrix, Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );
		GlslWriter_API Float ComputeSpotShadow( Mat4 const & p_lightMatrix, Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index );
		GlslWriter_API Float ComputePointShadow( Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index );
		GlslWriter_API Float ComputeSpotShadow( Mat4 const & p_lightMatrix, Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );
		GlslWriter_API Float ComputePointShadow( Vec3 const & p_worldSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );

	private:
		void DoDeclare_GetRandom();
		void DoDeclare_GetShadowOffset();
		void DoDeclare_GetLightSpacePosition();
		void DoDeclare_ComputeDirectionalShadow();
		void DoDeclare_ComputeSpotShadow();
		void DoDeclare_ComputePointShadow();
		void DoDeclare_ComputeOneSpotShadow();
		void DoDeclare_ComputeOnePointShadow();
		void DoDeclare_PcfSample();
		void DoDeclare_PcfSampleDirectional();
		void DoDeclare_PcfSampleSpot();
		void DoDeclare_PcfSamplePoint();

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
		Function< Float, InVec3 > m_sampleDirectional;
		Function< Float, InVec3, InInt > m_sampleSpot;
		Function< Float, InVec3, InFloat, InInt > m_samplePoint;
		Function< Float, InVec3 > m_sampleOneSpot;
		Function< Float, InVec3, InFloat > m_sampleOnePoint;
	};
}

#endif
