#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <Ashes/Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ModelUbo::BindingPoint = 7u;
	String const ModelUbo::BufferModel = cuT( "Model" );
	String const ModelUbo::ShadowReceiver = cuT( "c3d_shadowReceiver" );
	String const ModelUbo::MaterialIndex = cuT( "c3d_materialIndex" );
	String const ModelUbo::EnvironmentIndex = cuT( "c3d_envMapIndex" );

	ModelUbo::ModelUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
		{
			initialise();
		}
	}

	ModelUbo::~ModelUbo()
	{
	}

	void ModelUbo::initialise()
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
					"ModelUbo"
				} );
		}
	}

	void ModelUbo::cleanup()
	{
		m_ubo.reset();
	}

	void ModelUbo::setEnvMapIndex( uint32_t value )
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.environmentIndex = int32_t( value );
	}

	void ModelUbo::update( bool shadowReceiver
		, uint32_t materialIndex )const
	{
		CU_Require( m_ubo );
		auto & configuration = m_ubo->getData( 0u );
		configuration.shadowReceiver = shadowReceiver ? 1 : 0;
		configuration.materialIndex = materialIndex - 1;
		m_ubo->upload();
	}
}
