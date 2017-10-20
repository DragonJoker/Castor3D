/*
See LICENSE file in root folder
*/
#ifndef ___GLSL_PhongLighting_H___
#define ___GLSL_PhongLighting_H___

#include "GlslLighting.hpp"

namespace castor3d
{
	namespace shader
	{
		class PhongLightingModel
			: public LightingModel
		{
		public:
			C3D_API PhongLightingModel( ShadowType shadows
				, glsl::GlslWriter & writer );
			C3D_API void computeCombined( glsl::Vec3 const & worldEye
				, glsl::Float const & shininess
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( DirectionalLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Float const & shininess
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( PointLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Float const & shininess
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API void compute( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, glsl::Float const & shininess
				, glsl::Int const & receivesShadows
				, FragmentInput const & fragmentIn
				, OutputComponents & output )const;
			C3D_API glsl::Vec3 computeBackLit( DirectionalLight const & light
				, glsl::Vec3 const & worldEye
				, FragmentInput const & fragmentIn )const;
			C3D_API glsl::Vec3 computeBackLit( PointLight const & light
				, glsl::Vec3 const & worldEye
				, FragmentInput const & fragmentIn )const;
			C3D_API glsl::Vec3 computeBackLit( SpotLight const & light
				, glsl::Vec3 const & worldEye
				, FragmentInput const & fragmentIn )const;

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

			void doComputeLight( Light const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & lightDirection
				, glsl::Float const & shininess
				, glsl::Float const & shadowFactor
				, FragmentInput const & fragmentIn
				, OutputComponents & output );
			void doDeclareComputeLight();

			glsl::Vec3 doComputeLightBackLit( Light const & light
				, glsl::Vec3 const & worldEye
				, glsl::Vec3 const & lightDirection
				, FragmentInput const & fragmentIn );
			void doDeclareComputeLightBackLit();

		public:
			C3D_API static const castor::String Name;
			glsl::Function< glsl::Void
				, InLight
				, glsl::InVec3
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InFloat
				, FragmentInput
				, OutputComponents & > m_computeLight;
			glsl::Function< glsl::Void
				, DirectionalLight
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeDirectional;
			glsl::Function< glsl::Void
				, PointLight
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computePoint;
			glsl::Function< glsl::Void
				, SpotLight
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeSpot;
			glsl::Function< glsl::Void
				, PointLight
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeOnePoint;
			glsl::Function< glsl::Void
				, SpotLight
				, glsl::InVec3
				, glsl::InFloat
				, glsl::InInt
				, FragmentInput
				, OutputComponents & > m_computeOneSpot;
			glsl::Function< glsl::Vec3
				, InLight
				, glsl::InVec3
				, glsl::InVec3
				, FragmentInput > m_computeLightBackLit;
			glsl::Function< glsl::Vec3
				, DirectionalLight
				, glsl::InVec3
				, FragmentInput > m_computeDirectionalBackLit;
			glsl::Function< glsl::Vec3
				, PointLight
				, glsl::InVec3
				, FragmentInput > m_computePointBackLit;
			glsl::Function< glsl::Vec3
				, SpotLight
				, glsl::InVec3
				, FragmentInput > m_computeSpotBackLit;
		};
	}
}

#endif
