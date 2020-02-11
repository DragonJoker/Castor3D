#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	uint32_t const TexturesUbo::BindingPoint = 6u;
	String const TexturesUbo::BufferTextures = cuT( "Textures" );
	String const TexturesUbo::TexturesConfig = cuT( "c3d_textureConfig" );

	TexturesUbo::TexturesUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
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
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "TexturesUbo" );
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
