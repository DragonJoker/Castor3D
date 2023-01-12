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

	public:
		static castor::String const Name;
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
	};
}

#endif
