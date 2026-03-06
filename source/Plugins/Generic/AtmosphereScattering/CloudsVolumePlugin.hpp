/*
See LICENSE file in root folder
*/
#ifndef ___C3DAS_CloudsVolumePlugin_H___
#define ___C3DAS_CloudsVolumePlugin_H___

#include "AtmosphereModel.hpp"
#include "CloudsModel.hpp"
#include "ScatteringModel.hpp"

#include <Castor3D/Render/Volumetric/VolumeComponentPlugin.hpp>
#include <Castor3D/Shader/Shaders/GlslUtils.hpp>

namespace atmosphere_scattering
{
	class AtmosphereVolumeTraversal
	{
	public:
		AtmosphereVolumeTraversal( sdw::ShaderWriter & writer
			, c3d::shader::VolumeShaders const & volumeShaders
			, AtmosphereModel & atmosphere );

		void fillType( sdw::type::BaseStruct & type )const;
		void fillInit( sdw::type::BaseStruct & type
			, sdw::expr::ExprList & inits )const;
		void registerVolumes( c3d::shader::Volumes & volumes );
		void initialise( sdw::Vec2 const & pixelCoord, c3d::shader::Ray & ray
			, c3d::shader::Volumes & volumes, c3d::shader::VolumesTraversalResult & result
			, sdw::Vec4 const & depthObj );
		void finalise( c3d::shader::Ray const & ray, c3d::shader::VolumesTraversalResult & result );

		c3d::shader::VolumeTraversalFunc const & getTraversalFunc();
		c3d::shader::VolumeStepFunc const & getStepFunc();
		sdw::RetFloat getSamplesCount( sdw::Float const & range )const;
		sdw::RetFloat listVolumes( c3d::shader::Ray const & ray
			, sdw::Float const & objectId
			, sdw::Float const & linearDepth
			, sdw::Vec2 const & pixPos
			, c3d::shader::Volumes & volumes );

		uint32_t getVolumeType()const noexcept
		{
			return m_volumeType;
		}

	private:
		sdw::ShaderWriter & m_writer;
		c3d::shader::VolumeShaders const & m_volumeShaders;
		AtmosphereModel & m_atmosphere;
		uint32_t m_volumeType{};
		c3d::shader::VolumeTraversalFunc m_traverse;
		c3d::shader::VolumeStepFunc m_step;
		sdw::Function< sdw::Float
			, c3d::shader::InOutRay, sdw::InFloat, sdw::InFloat, sdw::InVec2
			, sdw::InOutParam< sdw::Array< c3d::shader::Volume > >, sdw::InOutUInt > m_listVolumes;
	};

	class CloudsVolumePlugin
		: public c3d::VolumeComponentPlugin
	{
	private:
		class Shader
			: public c3d::shader::VolumeComponentShader
		{
		public:
			Shader( sdw::ShaderWriter & writer
				, c3d::shader::VolumeShaders const & volumeShaders
				, c3d::Extent2D targetExtent
				, bool hasDepth
				, uint32_t & binding );

			void fillType( sdw::type::BaseStruct & type )const override;
			void fillInit( sdw::type::BaseStruct & type
				, sdw::expr::ExprList & inits )const override;
			void registerVolumes( c3d::shader::Volumes & volumes )override;
			void initialise( sdw::Vec2 const & pixelCoord, c3d::shader::Ray & ray
				, c3d::shader::Volumes & volumes, c3d::shader::VolumesTraversalResult & result )override;
			void finalise( c3d::shader::Ray const & ray, c3d::shader::VolumesTraversalResult & result )override;

		private:
			sdw::CombinedImage2DRgba32 m_depthMap;
			AtmosphereData m_atmosphereData;
			CameraData m_cameraData;
			AtmosphereModel m_atmosphere;
			ScatteringModel m_scattering;
			AtmosphereVolumeTraversal m_atmosphereTraversal;
		};

	public:
		using VolumeComponentPlugin::VolumeComponentPlugin;

		c3d::shader::VolumeComponentShaderPtr createComponentsShader( sdw::ShaderWriter & writer
			, c3d::shader::VolumeShaders const & volumeShaders
			, c3d::Extent2D const & targetExtent
			, bool hasDepth
			, uint32_t & bindingId )const override
		{
			return c3d::makeRawUnique< Shader >( writer, volumeShaders, targetExtent, hasDepth, bindingId );
		}

		static c3d::VolumeComponentPluginUPtr createPlugin( c3d::VolumeComponentRegister const & volumeComponents )
		{
			return c3d::makeUniqueDerived< c3d::VolumeComponentPlugin, CloudsVolumePlugin >( volumeComponents );
		}

	public:
		static c3d::String const TypeName;
	};
}

#endif
