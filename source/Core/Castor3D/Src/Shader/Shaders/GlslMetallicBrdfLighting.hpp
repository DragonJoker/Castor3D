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
#ifndef ___C3D_GlslMetallicBrdfLighting_H___
#define ___C3D_GlslMetallicBrdfLighting_H___

#include "GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class MetallicBrdfLightingModel
			: public LightingModel
		{
		public:
			C3D_API MetallicBrdfLightingModel( ShadowType shadows
				, glsl::GlslWriter & writer );
			C3D_API glsl::Vec3 computeCombinedLighting( glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeDirectionalLight( DirectionalLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computePointLight( PointLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeSpotLight( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeOneDirectionalLight( DirectionalLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeOnePointLight( PointLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeOneSpotLight( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn );

		protected:
			void doDeclareModel();
			void doDeclareComputeDirectionalLight()override;
			void doDeclareComputePointLight()override;
			void doDeclareComputeSpotLight()override;
			void doDeclareComputeOneDirectionalLight()override;
			void doDeclareComputeOnePointLight()override;
			void doDeclareComputeOneSpotLight()override;

			glsl::Vec3 doComputeLight( Light const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & direction
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Float const & shadowFactor
				, FragmentInput const & fragmentIn );

			void doDeclareDistribution();
			void doDeclareGeometry();
			void doDeclareFresnelShlick();
			void doDeclareComputeLight();

		public:
			C3D_API static const castor::String Name;
			glsl::Function< glsl::Float
				, glsl::InFloat
				, glsl::InFloat > m_distributionGGX;
			glsl::Function< glsl::Float
				, glsl::InFloat
				, glsl::InFloat > m_geometrySchlickGGX;
			glsl::Function< glsl::Float
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat > m_geometrySmith;
			glsl::Function< glsl::Float
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat > m_smith;
			glsl::Function< glsl::Vec3
				, glsl::InFloat
				, glsl::InVec3 > m_schlickFresnel;
			glsl::Function< glsl::Vec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InVec3 > m_cookTorrance;
			glsl::Function< glsl::Vec3
				, DirectionalLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput > m_computeDirectional;
			glsl::Function< glsl::Vec3
				, PointLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput > m_computePoint;
			glsl::Function< glsl::Vec3
				, SpotLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput > m_computeSpot;
			glsl::Function< glsl::Vec3
				, DirectionalLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput > m_computeOneDirectional;
			glsl::Function< glsl::Vec3
				, PointLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput > m_computeOnePoint;
			glsl::Function< glsl::Vec3
				, SpotLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput > m_computeOneSpot;
			glsl::Function< glsl::Vec3
				, InLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeLight;
		};
	}
}

#endif
