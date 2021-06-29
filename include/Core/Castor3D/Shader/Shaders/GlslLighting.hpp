/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLighting_H___
#define ___C3D_GlslLighting_H___

#include "SdwModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		class LightingModel
		{
		public:
			C3D_API LightingModel( sdw::ShaderWriter & writer
				, Utils & utils
				, ShadowOptions shadowOptions
				, bool isOpaqueProgram );
			C3D_API void declareModel( uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet );
			C3D_API void declareDiffuseModel( uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet );
			C3D_API void declareDirectionalModel( bool lightUbo
				, uint32_t lightUboBinding
				, uint32_t lightUboSet
				, uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet );
			C3D_API void declarePointModel( bool lightUbo
				, uint32_t lightUboBinding
				, uint32_t lightUboSet
				, uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet );
			C3D_API void declareSpotModel( bool lightUbo
				, uint32_t lightUboBinding
				, uint32_t lightUboSet
				, uint32_t lightsBufBinding
				, uint32_t lightsBufSet
				, uint32_t & shadowMapBinding
				, uint32_t shadowMapSet );
			// Calls
			C3D_API DirectionalLight getDirectionalLight( sdw::Int const & index )const;
			C3D_API TiledDirectionalLight getTiledDirectionalLight( sdw::Int const & index )const;
			C3D_API PointLight getPointLight( sdw::Int const & index )const;
			C3D_API SpotLight getSpotLight( sdw::Int const & index )const;
			static C3D_API sdw::Vec3 computeF0( sdw::Vec3 const & albedo
				, sdw::Float const & metalness );
			static C3D_API sdw::Float computeMetalness( sdw::Vec3 const & albedo
				, sdw::Vec3 const & specular );

			inline Shadow const & getShadowModel()const
			{
				return *m_shadowModel;
			}

		protected:
			C3D_API Light getBaseLight( sdw::Int const & value )const;
			C3D_API void doDeclareLightsBuffer( uint32_t binding
				, uint32_t set );
			C3D_API void doDeclareDirectionalLightUbo( uint32_t binding
				, uint32_t set );
			C3D_API void doDeclarePointLightUbo( uint32_t binding
				, uint32_t set );
			C3D_API void doDeclareSpotLightUbo( uint32_t binding
				, uint32_t set );
			C3D_API void doDeclareGetBaseLight();
			C3D_API void doDeclareGetDirectionalLight();
			C3D_API void doDeclareGetPointLight();
			C3D_API void doDeclareGetSpotLight();
			C3D_API void doDeclareGetCascadeFactors();

			virtual void doDeclareModel() = 0;
			virtual void doDeclareComputeDirectionalLight() = 0;
			virtual void doDeclareComputePointLight() = 0;
			virtual void doDeclareComputeSpotLight() = 0;
			virtual void doDeclareDiffuseModel() = 0;
			virtual void doDeclareComputeDirectionalLightDiffuse() = 0;
			virtual void doDeclareComputePointLightDiffuse() = 0;
			virtual void doDeclareComputeSpotLightDiffuse() = 0;

		protected:
			sdw::ShaderWriter & m_writer;
			Utils & m_utils;
			bool m_isOpaqueProgram;
			std::shared_ptr< Shadow > m_shadowModel;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec4
				, sdw::InUInt > m_getCascadeFactors;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InParam< sdw::Array< sdw::Vec4 > >
				, sdw::InUInt > m_getTileFactors;
			sdw::Function< shader::Light
				, sdw::InInt > m_getBaseLight;
			sdw::Function< shader::DirectionalLight
				, sdw::InInt > m_getDirectionalLight;
			sdw::Function< shader::TiledDirectionalLight
				, sdw::InInt > m_getTiledDirectionalLight;
			sdw::Function< shader::PointLight
				, sdw::InInt > m_getPointLight;
			sdw::Function< shader::SpotLight
				, sdw::InInt > m_getSpotLight;
		};
	}
}

#endif
