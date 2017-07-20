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
#ifndef ___GLSL_PhongLighting_H___
#define ___GLSL_PhongLighting_H___

#include "GlslLighting.hpp"

namespace GLSL
{
	class PhongLightingModel
		: public LightingModel
	{
	public:
		GlslWriter_API PhongLightingModel( ShadowType shadows, GlslWriter & writer );
		GlslWriter_API static std::shared_ptr< LightingModel > Create( ShadowType shadows, GlslWriter & writer );
		GlslWriter_API void ComputeCombinedLighting( Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		GlslWriter_API void ComputeDirectionalLight( DirectionalLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		GlslWriter_API void ComputePointLight( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		GlslWriter_API void ComputeSpotLight( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		GlslWriter_API void ComputeOneDirectionalLight( DirectionalLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		GlslWriter_API void ComputeOnePointLight( PointLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		GlslWriter_API void ComputeOneSpotLight( SpotLight const & light
			, Vec3 const & worldEye
			, Float const & shininess
			, Int const & receivesShadows
			, FragmentInput const & fragmentIn
			, OutputComponents & output );

	protected:
		void DoDeclareModel();
		void Declare_ComputeDirectionalLight()override;
		void Declare_ComputePointLight()override;
		void Declare_ComputeSpotLight()override;
		void Declare_ComputeOneDirectionalLight()override;
		void Declare_ComputeOnePointLight()override;
		void Declare_ComputeOneSpotLight()override;

		void DoComputeLight( Light const & light
			, Vec3 const & worldEye
			, Vec3 const & lightDirection
			, Float const & shininess
			, Float const & shadowFactor
			, FragmentInput const & fragmentIn
			, OutputComponents & output );
		void DoDeclare_ComputeLight();

	public:
		GlslWriter_API static const Castor::String Name;
		Function< Void, InLight, InVec3, InVec3, InFloat, InFloat, FragmentInput, OutputComponents & > m_computeLight;
		Function< Void, DirectionalLight, InVec3, InFloat, InInt, FragmentInput, OutputComponents & > m_computeDirectional;
		Function< Void, PointLight, InVec3, InFloat, InInt, FragmentInput, OutputComponents & > m_computePoint;
		Function< Void, SpotLight, InVec3, InFloat, InInt, FragmentInput, OutputComponents & > m_computeSpot;
		Function< Void, DirectionalLight, InVec3, InFloat, InInt, FragmentInput, OutputComponents & > m_computeOneDirectional;
		Function< Void, PointLight, InVec3, InFloat, InInt, FragmentInput, OutputComponents & > m_computeOnePoint;
		Function< Void, SpotLight, InVec3, InFloat, InInt, FragmentInput, OutputComponents & > m_computeOneSpot;
	};
}

#endif
