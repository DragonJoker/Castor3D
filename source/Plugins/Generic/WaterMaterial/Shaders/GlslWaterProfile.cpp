#include "WaterMaterial/Shaders/GlslWaterProfile.hpp"

#include "WaterMaterial/WaterComponent.hpp"

#include <Castor3D/Limits.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace water::shader
{
	WaterProfiles::WaterProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< WaterProfile >{ writer
		, "C3D_WaterProfiles"
		, "c3d_waterProfiles"
		, binding
		, set
		, enable }
	{
	}

	void WaterProfiles::update( castor3d::ShaderBuffer & buffer
		, castor3d::Pass const & pass )
	{
		if ( auto edges = pass.getComponent< WaterComponent >() )
		{
			auto profiles = castor::makeArrayView( reinterpret_cast< WaterProfileData * >( buffer.getPtr() ), castor3d::MaxMaterialsCount );
			auto & data = profiles[pass.getId() - 1u];
			edges->fillProfileBuffer( data );
		}
	}

	castor3d::ShaderBufferUPtr WaterProfiles::create( castor3d::RenderDevice const & device )
	{
		return castor::makeUnique< castor3d::ShaderBuffer >( *device.renderSystem.getEngine()
			, device
			, castor3d::MaxMaterialsCount * sizeof( WaterProfileData )
			, cuT( "WaterProfilesBuffer" ) );
	}

	c3d::BufferBaseUPtr WaterProfiles::declare( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set )
	{
		return castor::makeUniqueDerived< c3d::BufferBase, WaterProfiles >( writer
			, binding
			, set
			, true );
	}
}
