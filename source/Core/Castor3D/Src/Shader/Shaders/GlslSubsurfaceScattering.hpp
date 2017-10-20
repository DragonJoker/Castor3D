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
#ifndef ___GLSL_SubsurfaceScattering_H___
#define ___GLSL_SubsurfaceScattering_H___

#include "Castor3DPrerequisites.hpp"

#include <GlslIntrinsics.hpp>

#define C3D_DEBUG_SSS_TRANSMITTANCE 0

namespace castor3d
{
	namespace shader
	{
		class SubsurfaceScattering
		{
		public:
			static castor::String const TransmittanceProfile;
			static castor::String const ProfileFactorsCount;

		public:
			C3D_API SubsurfaceScattering( glsl::GlslWriter & writer
				, bool shadowMap );
			C3D_API void declare( LightType type );

			C3D_API glsl::Vec3 compute( shader::BaseMaterial const & material
				, shader::DirectionalLight const & light
				, glsl::Vec2 const & uv
				, glsl::Vec3 const & position
				, glsl::Vec3 const & normal
				, glsl::Float const & translucency )const;
			C3D_API glsl::Vec3 compute( shader::BaseMaterial const & material
				, shader::PointLight const & light
				, glsl::Vec2 const & uv
				, glsl::Vec3 const & position
				, glsl::Vec3 const & normal
				, glsl::Float const & translucency )const;
			C3D_API glsl::Vec3 compute( shader::BaseMaterial const & material
				, shader::SpotLight const & light
				, glsl::Vec2 const & uv
				, glsl::Vec3 const & position
				, glsl::Vec3 const & normal
				, glsl::Float const & translucency )const;

		private:
			void doDeclareGetTransformedPosition();
			void doDeclareComputeDirectionalTransmittance();
			void doDeclareComputePointTransmittance();
			void doDeclareComputeSpotTransmittance();
			
			glsl::Vec3 doComputeTransmittance( shader::DirectionalLight const & light
				, glsl::Vec2 const & uv
				, glsl::Int const & transmittanceProfileSize
				, glsl::Array< glsl::Vec4 > const & transmittanceProfile
				, glsl::Float const & width
				, glsl::Vec3 const & position
				, glsl::Vec3 const & normal
				, glsl::Float const & translucency )const;
			glsl::Vec3 doComputeTransmittance( shader::PointLight const & light
				, glsl::Vec2 const & uv
				, glsl::Int const & transmittanceProfileSize
				, glsl::Array< glsl::Vec4 > const & transmittanceProfile
				, glsl::Float const & width
				, glsl::Vec3 const & position
				, glsl::Vec3 const & normal
				, glsl::Float const & translucency )const;
			glsl::Vec3 doComputeTransmittance( shader::SpotLight const & light
				, glsl::Vec2 const & uv
				, glsl::Int const & transmittanceProfileSize
				, glsl::Array< glsl::Vec4 > const & transmittanceProfile
				, glsl::Float const & width
				, glsl::Vec3 const & position
				, glsl::Vec3 const & normal
				, glsl::Float const & translucency )const;

		private:
			glsl::GlslWriter & m_writer;
			bool m_shadowMap;

			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InMat4 > m_getTransformedPosition;
			glsl::Function< glsl::Vec3
				, glsl::InParam< shader::DirectionalLight >
				, glsl::InVec2
				, glsl::InInt
				, glsl::InArrayParam< glsl::Vec4 >
				, glsl::InFloat
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat > m_computeDirectionalTransmittance;
			glsl::Function< glsl::Vec3
				, glsl::InParam< shader::PointLight >
				, glsl::InVec2
				, glsl::InInt
				, glsl::InArrayParam< glsl::Vec4 >
				, glsl::InFloat
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat > m_computePointTransmittance;
			glsl::Function< glsl::Vec3
				, glsl::InParam< shader::SpotLight >
				, glsl::InVec2
				, glsl::InInt
				, glsl::InArrayParam< glsl::Vec4 >
				, glsl::InFloat
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat > m_computeSpotTransmittance;
		};
	}
}

#endif
