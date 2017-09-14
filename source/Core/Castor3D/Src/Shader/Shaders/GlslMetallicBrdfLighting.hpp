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
			C3D_API void computeCombinedLighting( glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			C3D_API void computeDirectionalLight( DirectionalLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			C3D_API void computePointLight( PointLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			C3D_API void computeSpotLight( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			C3D_API void computeOnePointLight( PointLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			C3D_API void computeOneSpotLight( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			C3D_API glsl::Vec3 computeOneDirectionalLightBackLit( DirectionalLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeOnePointLightBackLit( PointLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, FragmentInput const & fragmentIn );
			C3D_API glsl::Vec3 computeOneSpotLightBackLit( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, FragmentInput const & fragmentIn );

		protected:
			void doDeclareModel();
			void doDeclareComputeDirectionalLight()override;
			void doDeclareComputePointLight()override;
			void doDeclareComputeSpotLight()override;
			void doDeclareComputeOnePointLight()override;
			void doDeclareComputeOneSpotLight()override;
			void doDeclareComputeDirectionalLightBackLit()override;
			void doDeclareComputePointLightBackLit()override;
			void doDeclareComputeSpotLightBackLit()override;
			void doDeclareComputeOneDirectionalLightBackLit()override;
			void doDeclareComputeOnePointLightBackLit ()override;
			void doDeclareComputeOneSpotLightBackLit ()override;

			void doComputeLight( Light const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & direction
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, glsl::Float const & shadowFactor
				, FragmentInput const & fragmentIn
				, OutputComponents & output );

			glsl::Vec3 doComputeLightBackLit( Light const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & direction
				, glsl::Vec3 const & albedo
				, glsl::Float const & metallic
				, glsl::Float const & roughness
				, FragmentInput const & fragmentIn );

			void doDeclareDistribution();
			void doDeclareGeometry();
			void doDeclareFresnelShlick();
			void doDeclareComputeLight();
			void doDeclareComputeLightBackLit();

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
			glsl::Function< glsl::Void
				, DirectionalLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeDirectional;
			glsl::Function< glsl::Void
				, PointLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computePoint;
			glsl::Function< glsl::Void
				, SpotLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeSpot;
			glsl::Function< glsl::Void
				, PointLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeOnePoint;
			glsl::Function< glsl::Void
				, SpotLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeOneSpot;
			glsl::Function< glsl::Void
				, InLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput
				, OutputComponents & > m_computeLight;
			glsl::Function< glsl::Vec3
				, DirectionalLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeDirectionalBackLit;
			glsl::Function< glsl::Vec3
				, PointLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computePointBackLit;
			glsl::Function< glsl::Vec3
				, SpotLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeSpotBackLit;
			glsl::Function< glsl::Vec3
				, DirectionalLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeOneDirectionalBackLit;
			glsl::Function< glsl::Vec3
				, PointLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeOnePointBackLit;
			glsl::Function< glsl::Vec3
				, SpotLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeOneSpotBackLit;
			glsl::Function< glsl::Vec3
				, InLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput > m_computeLightBackLit;
		};
	}
}

#endif
