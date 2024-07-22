/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_AtmosphereBackgroundModel_H___
#define ___C3DAS_AtmosphereBackgroundModel_H___

#include "AtmosphereScattering/AtmosphereModel.hpp"
#include "AtmosphereScattering/AtmosphereCameraUbo.hpp"
#include "AtmosphereScattering/AtmosphereScatteringUbo.hpp"
#include "AtmosphereScattering/ScatteringModel.hpp"
#include "AtmosphereScattering/CloudsModel.hpp"
#include "AtmosphereScattering/CloudsUbo.hpp"

#include <Castor3D/Shader/Shaders/GlslBackground.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

#include <ShaderWriter/CompositeTypes/UniformBuffer.hpp>

namespace atmosphere_scattering
{
	class AtmosphereBackgroundModel
		: public castor3d::shader::BackgroundModel
	{
	public:
		AtmosphereBackgroundModel( castor3d::Engine const & engine
			, sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, VkExtent2D targetSize
			, bool needsForeground
			, uint32_t & binding
			, uint32_t set );

		static castor3d::shader::BackgroundModelPtr create( castor3d::Engine const & engine
			, sdw::ShaderWriter & writer
			, castor3d::shader::Utils & utils
			, VkExtent2D targetSize
			, bool needsForeground
			, uint32_t & binding
			, uint32_t set );

		sdw::RetVec3 computeSpecularReflections( sdw::Vec3 const & wsNormal
			, sdw::Vec3 const & wsPosition
			, sdw::Vec3 const & V
			, sdw::Float const & NdotV
			, sdw::Vec3 const & fresnel
			, sdw::Float const & roughness
			, castor3d::shader::BlendComponents & components
			, sdw::CombinedImage2DRgba32 const & brdf
			, castor3d::shader::DebugOutputCategory & debugOutput )override;

		void applyVolume( sdw::Vec2 const fragCoord
			, sdw::Float const linearDepth
			, sdw::Vec2 const targetSize
			, sdw::Vec2 const cameraPlanes
			, sdw::Vec4 & output )override;
		sdw::Vec3 getSunRadiance( sdw::Vec3 const & sunDir );

		castor3d::shader::RetRay getPixelTransLum( sdw::Vec2 const & fragPos
			, sdw::Vec2 const & fragSize
			, sdw::Float const & fragDepth
			, sdw::Vec4 & transmittance
			, sdw::Vec4 & luminance )
		{
			return scattering.getPixelTransLum( fragPos
				, fragSize
				, fragDepth
				, transmittance
				, luminance );
		}

	private:
		sdw::Float erfc( sdw::Float const x );
		sdw::RetFloat lambda( sdw::Float const cosTheta
			, sdw::Float const sigmaSq );
		sdw::RetFloat reflectedSunRadiance( sdw::Vec3 const L
			, sdw::Vec3 const V
			, sdw::Vec3 const N
			, sdw::Vec3 const Tx
			, sdw::Vec3 const Ty
			, sdw::Vec2 const sigmaSq );
		sdw::RetVec2 U( sdw::Vec2 const zeta
			, sdw::Vec3 const V
			, sdw::Vec3 const N
			, sdw::Vec3 const Tx
			, sdw::Vec3 const Ty );
		sdw::RetVec3 meanSkyRadiance( sdw::Vec3 const V
			, sdw::Vec3 const N
			, sdw::Vec3 const Tx
			, sdw::Vec3 const Ty
			, sdw::Vec2 const sigmaSq );
		sdw::RetVec3 getMultipleScattering( sdw::Vec3 const & scattering
			, sdw::Vec3 const & extinction
			, sdw::Vec3 const & worldPos
			, sdw::Float const & viewZenithCosAngle );

	public:
		static castor::String const Name;
		static castor::String const PluginType;
		static castor::MbString const PluginName;

		sdw::UniformBuffer cameraBuffer;
		CameraData cameraData;
		sdw::UniformBuffer atmosphereBuffer;
		AtmosphereData atmosphereData;
		sdw::UniformBuffer cloudsBuffer;
		CloudsData cloudsData;
		AtmosphereModel atmosphere;
		ScatteringModel scattering;
		sdw::CombinedImage2DRgba32 cloudsResult;

	private:
		sdw::Function< sdw::Vec3 > m_computeReflections;
		sdw::Function< sdw::Vec3 > m_computeRefractions;
		sdw::Function< sdw::Void > m_mergeReflRefr;
		sdw::Function< sdw::Void
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InOutVec4 > m_computeVolume;
		sdw::Function < sdw::Float
			, sdw::InFloat > m_erfc;
		sdw::Function < sdw::Float
			, sdw::InFloat
			, sdw::InFloat > m_lambda;
		sdw::Function < sdw::Float
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec2 > m_reflectedSunRadiance;
		sdw::Function < sdw::Vec2
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3 > m_U;
		sdw::Function < sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec2 > m_meanSkyRadiance;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3 > m_computeDiffuseReflections;
		sdw::Function< sdw::Vec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec3
			, sdw::InVec2 > m_computeSpecularReflections;
	};
}

#endif
