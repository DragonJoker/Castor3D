#include "ToonMaterial/Shaders/GlslToonProfile.hpp"

#include "ToonMaterial/EdgesComponent.hpp"

#include <Castor3D/Limits.hpp>
#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
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

	void ToonProfiles::update( c3d::ShaderBuffer const & buffer
		, c3d::Pass const & pass )
	{
		if ( auto edges = pass.getComponent< EdgesComponent >() )
		{
			auto profiles = c3d::makeArrayView( reinterpret_cast< ToonProfileData * >( buffer.getPtr() ), c3d::MaxMaterialsCount );
			auto & data = profiles[pass.getId() - 1u];
			edges->fillProfileBuffer( data );
		}
	}

	c3d::ShaderBufferUPtr ToonProfiles::create( c3d::RenderDevice const & device )
	{
		return c3d::makeUnique< c3d::ShaderBuffer >( device
			, device.renderSystem.getEngine()->getGraphResourceCache()
			, c3d::MaxMaterialsCount * sizeof( ToonProfileData )
			, cuT( "ToonProfilesBuffer" ) );
	}

	c3ds::BufferBaseUPtr ToonProfiles::declare( sdw::ShaderWriter & writer
		, uint32_t binding
		, uint32_t set )
	{
		return c3d::makeUniqueDerived< c3ds::BufferBase, ToonProfiles >( writer
			, binding
			, set
			, true );
	}
}
