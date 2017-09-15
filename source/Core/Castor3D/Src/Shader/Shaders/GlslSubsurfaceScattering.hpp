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

namespace castor3d
{
	namespace shader
	{
		class SubsurfaceScattering
		{
		public:
			C3D_API SubsurfaceScattering( glsl::GlslWriter & writer );
			C3D_API void declare();
			C3D_API void declareDirectional();
			C3D_API void declarePoint();
			C3D_API void declareSpot();
			C3D_API glsl::Float computeLightDist( shader::DirectionalLight const & light
				, glsl::Vec3 const & position )const;
			C3D_API glsl::Float computeLightDist( shader::PointLight const & light
				, glsl::Vec3 const & position )const;
			C3D_API glsl::Float computeLightDist( shader::SpotLight const & light
				, glsl::Vec3 const & position )const;
			C3D_API glsl::Float computeOneLightDist( shader::DirectionalLight const & light
				, glsl::Vec3 const & position )const;
			C3D_API glsl::Float computeOneLightDist( shader::PointLight const & light
				, glsl::Vec3 const & position )const;
			C3D_API glsl::Float computeOneLightDist( shader::SpotLight const & light
				, glsl::Vec3 const & position )const;

		private:
			void doDeclareGetTransformedPosition();
			void doDeclareComputeDirectionalLightDist();
			void doDeclareComputePointLightDist();
			void doDeclareComputeSpotLightDist();
			void doDeclareComputeOneDirectionalLightDist();
			void doDeclareComputeOnePointLightDist();
			void doDeclareComputeOneSpotLightDist();

		private:
			glsl::GlslWriter & m_writer;

			glsl::Function< glsl::Vec3
				, glsl::InVec3
				, glsl::InMat4 > m_getTransformedPosition;
			glsl::Function< glsl::Float
				, glsl::InParam< shader::DirectionalLight >
				, glsl::InVec3 > m_computeDirectionalLightDist;
			glsl::Function< glsl::Float
				, glsl::InParam< shader::PointLight >
				, glsl::InVec3 > m_computePointLightDist;
			glsl::Function< glsl::Float
				, glsl::InParam< shader::SpotLight >
				, glsl::InVec3 > m_computeSpotLightDist;
			glsl::Function< glsl::Float
				, glsl::InParam< shader::DirectionalLight >
				, glsl::InVec3 > m_computeOneDirectionalLightDist;
			glsl::Function< glsl::Float
				, glsl::InParam< shader::PointLight >
				, glsl::InVec3 > m_computeOnePointLightDist;
			glsl::Function< glsl::Float
				, glsl::InParam< shader::SpotLight >
				, glsl::InVec3 > m_computeOneSpotLightDist;
		};
	}
}

#endif
