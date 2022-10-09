#include "ToonMaterial/Shaders/GlslToonProfile.hpp"

#include "ToonMaterial/EdgesComponent.hpp"

#include <Castor3D/Limits.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Render/RenderDevice.hpp>
#include <Castor3D/Render/RenderSystem.hpp>
#include <Castor3D/Shader/ShaderBuffer.hpp>

#include <ShaderWriter/Source.hpp>

namespace toon::shader
{
	ToonProfiles::ToonProfiles( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set
		, bool enable )
		: BufferT< ToonProfile >{ writer
			, "C3D_ToonProfiles"
			, "c3d_toonProfiles"
			, binding
			, set
			, enable }
	{
	}

	void ToonProfiles::update( castor3d::ShaderBuffer & buffer
		, castor3d::Pass const & pass )
	{
		if ( auto edges = pass.getComponent< EdgesComponent >() )
		{
			auto profiles = castor::makeArrayView( reinterpret_cast< ToonProfileData * >( buffer.getPtr() ), castor3d::MaxMaterialsCount );
			auto & data = profiles[pass.getId() - 1u];
			edges->fillProfileBuffer( data );
		}
	}

	castor3d::ShaderBufferUPtr ToonProfiles::create( castor3d::RenderDevice const & device )
	{
		return castor::makeUnique< castor3d::ShaderBuffer >( *device.renderSystem.getEngine()
			, device
			, castor3d::MaxMaterialsCount * sizeof( ToonProfileData )
			, cuT( "ToonProfilesBuffer" ) );
	}

	c3d::BufferBaseUPtr ToonProfiles::declare( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set )
	{
		return castor::makeUniqueDerived< c3d::BufferBase, ToonProfiles >( writer
			, binding
			, set
			, true );
	}
}
