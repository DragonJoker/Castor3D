/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslLighting_H___
#define ___C3D_GlslLighting_H___

#include "GlslShadow.hpp"

#include <GlslIntrinsics.hpp>

namespace castor3d
{
	namespace shader
	{
		struct FragmentInput
		{
			C3D_API explicit FragmentInput( glsl::GlslWriter & writer );
			C3D_API FragmentInput( glsl::InVec3 const & vertex
				, glsl::InVec3 const & normal );
			glsl::InVec3 m_vertex;
			glsl::InVec3 m_normal;
		};

		C3D_API castor::String paramToString( castor::String & sep
			, FragmentInput const & value );

		C3D_API castor::String toString( FragmentInput const & value );

		class LightingModel
		{
		public:
			C3D_API LightingModel( glsl::GlslWriter & writer );
			C3D_API void declareModel( uint32_t & index );
			C3D_API void declareDirectionalModel( ShadowType shadows
				, bool volumetric
				, uint32_t & index );
			C3D_API void declarePointModel( ShadowType shadows
				, bool volumetric
				, uint32_t & index );
			C3D_API void declareSpotModel( ShadowType shadows
				, bool volumetric
				, uint32_t & index );
			// Calls
			C3D_API DirectionalLight getDirectionalLight( glsl::Int const & index )const;
			C3D_API PointLight getPointLight( glsl::Int const & index )const;
			C3D_API SpotLight getSpotLight( glsl::Int const & index )const;

		protected:
			C3D_API Light getBaseLight( glsl::Type const & value )const;
			C3D_API void doDeclareLight();
			C3D_API void doDeclareDirectionalLight();
			C3D_API void doDeclarePointLight();
			C3D_API void doDeclareSpotLight();
			C3D_API void doDeclareDirectionalLightUbo();
			C3D_API void doDeclarePointLightUbo();
			C3D_API void doDeclareSpotLightUbo();
			C3D_API void doDeclareGetBaseLight();
			C3D_API void doDeclareGetDirectionalLight();
			C3D_API void doDeclareGetPointLight();
			C3D_API void doDeclareGetSpotLight();

			virtual void doDeclareModel() = 0;
			virtual void doDeclareComputeDirectionalLight() = 0;
			virtual void doDeclareComputePointLight() = 0;
			virtual void doDeclareComputeSpotLight() = 0;
			virtual void doDeclareComputeOneDirectionalLight( ShadowType shadowType
				, bool volumetric ) = 0;
			virtual void doDeclareComputeOnePointLight( ShadowType shadowType
				, bool volumetric ) = 0;
			virtual void doDeclareComputeOneSpotLight( ShadowType shadowType
				, bool volumetric ) = 0;

		public:
			C3D_API static uint32_t const UboBindingPoint;

		protected:
			glsl::GlslWriter & m_writer;
			std::shared_ptr< Shadow > m_shadowModel;
			glsl::Function< shader::DirectionalLight
				, glsl::InInt > m_getDirectionalLight;
			glsl::Function< shader::PointLight
				, glsl::InInt > m_getPointLight;
			glsl::Function< shader::SpotLight
				, glsl::InInt > m_getSpotLight;
		};
	}
}

#endif
