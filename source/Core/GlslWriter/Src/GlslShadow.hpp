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
		GlslWriter_API Float ComputeDirectionalShadow( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal );
		GlslWriter_API Float ComputeSpotShadow( Vec4 const & p_lightSpacePosition, Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index );
		GlslWriter_API Float ComputePointShadow( Vec3 const & p_lightDirection, Vec3 const & p_normal, Int const & p_index );
		GlslWriter_API Float GetRandom( Vec4 const & p_seed );

	private:
		void DoDeclare_ComputeDirectionalShadow( ShadowType p_type );
		void DoDeclare_ComputeSpotShadow( ShadowType p_type );
		void DoDeclare_ComputePointShadow( ShadowType p_type );
		void DoDeclare_GetRandom();

	private:
		GlslWriter & m_writer;
	};
}

#endif
