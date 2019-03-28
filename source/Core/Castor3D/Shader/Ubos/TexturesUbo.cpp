#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const TexturesUbo::BindingPoint = 6u;
	String const TexturesUbo::BufferTextures = cuT( "Textures" );
	String const TexturesUbo::TexturesConfig = cuT( "c3d_textureConfig" );

	TexturesUbo::TexturesUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	TexturesUbo::~TexturesUbo()
	{
	}

	void TexturesUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentDevice( m_engine );
			m_ubo = ashes::makeUniformBuffer< Configuration >( device
				, 1u
				, ashes::BufferTarget::eTransferDst
				, ashes::MemoryPropertyFlag::eHostVisible );
			device.debugMarkerSetObjectName(
				{
					ashes::DebugReportObjectType::eBuffer,
					&m_ubo->getUbo().getBuffer(),
					"TexturesUbo"
				} );
			m_ubo->upload();
		}
	}

	void TexturesUbo::cleanup()
	{
		m_ubo.reset();
	}

	void TexturesUbo::update( Pass const & pass )const
	{
		CU_Require( m_ubo );
		CU_Require( pass.getNonEnvTextureUnitsCount() <= 12u );
		auto & texturesData = m_ubo->getData();
		uint32_t index = 0u;

		for ( auto & unit : pass )
		{
			if ( unit->getConfiguration().environment == 0u )
			{
				texturesData.indices[index / 4u][index % 4] = unit->getId();
				++index;
			}
		}

		m_ubo->upload();
	}
}
