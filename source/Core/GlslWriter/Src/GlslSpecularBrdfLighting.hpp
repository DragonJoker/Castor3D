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
#ifndef ___GLSL_SpecularBrdfLighting_H___
#define ___GLSL_SpecularBrdfLighting_H___

#include "GlslLighting.hpp"

namespace glsl
{
	class SpecularBrdfLightingModel
		: public LightingModel
	{
	public:
		GlslWriter_API SpecularBrdfLightingModel( ShadowType shadows, GlslWriter & writer );
		GlslWriter_API static std::shared_ptr< LightingModel > create( ShadowType shadows, GlslWriter & writer );
		GlslWriter_API Vec3 computeCombinedLighting( Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );
		GlslWriter_API Vec3 computeDirectionalLight( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );
		GlslWriter_API Vec3 computePointLight( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );
		GlslWriter_API Vec3 computeSpotLight( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );
		GlslWriter_API Vec3 computeOneDirectionalLight( DirectionalLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );
		GlslWriter_API Vec3 computeOnePointLight( PointLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );
		GlslWriter_API Vec3 computeOneSpotLight( SpotLight const & light
			, Vec3 const & worldEye
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn );

	protected:
		void doDeclareModel();
		void doDeclareComputeDirectionalLight()override;
		void doDeclareComputePointLight()override;
		void doDeclareComputeSpotLight()override;
		void doDeclareComputeOneDirectionalLight()override;
		void doDeclareComputeOnePointLight()override;
		void doDeclareComputeOneSpotLight()override;

		Vec3 doComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & direction
			, Vec3 const & diffuse
			, Vec3 const & specular
			, Float const & glossiness
			, Float const & shadowFactor
			, FragmentInput const & fragmentIn );

		void doDeclareDistribution();
		void doDeclareGeometry();
		void doDeclareFresnelShlick();
		void doDeclareComputeLight();

	public:
		GlslWriter_API static const castor::String Name;
		Function< Float, InFloat, InFloat > m_distributionGGX;
		Function< Float, InFloat, InFloat > m_geometrySchlickGGX;
		Function< Float, InFloat, InFloat, InFloat > m_geometrySmith;
		Function< Float, InVec3, InVec3, InVec3, InFloat > m_smith;
		Function< Vec3, InFloat, InVec3 > m_schlickFresnel;
		Function< Vec3, InFloat, InFloat, InFloat, InFloat, InVec3 > m_cookTorrance;
		Function< Vec3, DirectionalLight, InVec3, InVec3, InVec3, InFloat, InInt, FragmentInput > m_computeDirectional;
		Function< Vec3, PointLight, InVec3, InVec3, InVec3, InFloat, InInt, FragmentInput > m_computePoint;
		Function< Vec3, SpotLight, InVec3, InVec3, InVec3, InFloat, InInt, FragmentInput > m_computeSpot;
		Function< Vec3, DirectionalLight, InVec3, InVec3, InVec3, InFloat, InInt, FragmentInput > m_computeOneDirectional;
		Function< Vec3, PointLight, InVec3, InVec3, InVec3, InFloat, InInt, FragmentInput > m_computeOnePoint;
		Function< Vec3, SpotLight, InVec3, InVec3, InVec3, InFloat, InInt, FragmentInput > m_computeOneSpot;
		Function< Vec3, InLight, InVec3, InVec3, InVec3, InVec3, InFloat, InFloat, FragmentInput > m_computeLight;
	};
}

#endif
