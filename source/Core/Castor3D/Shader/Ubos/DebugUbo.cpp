#include "Castor3D/Shader/Ubos/DebugUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

#include <ashespp/Buffer/StagingBuffer.hpp>

namespace castor3d
{
	uint32_t const DebugUbo::BindingPoint = 13u;
	castor::String const DebugUbo::BufferDebug = cuT( "Debug" );
	castor::String const DebugUbo::DebugLightEye = cuT( "c3d_debugLightEye" );
	castor::String const DebugUbo::DebugLightVSPosition = cuT( "c3d_debugLightVSPosition" );
	castor::String const DebugUbo::DebugLightWSPosition = cuT( "c3d_debugLightWSPosition" );
	castor::String const DebugUbo::DebugLightWSNormal = cuT( "c3d_debugLightWSNormal" );
	castor::String const DebugUbo::DebugLightTexCoord = cuT( "c3d_debugLightTexCoord" );
	castor::String const DebugUbo::DebugDeferredDiffuseLighting = cuT( "c3d_debugDeferredDiffuseLighting" );
	castor::String const DebugUbo::DebugDeferredSpecularLighting = cuT( "c3d_debugDeferredSpecularLighting" );
	castor::String const DebugUbo::DebugDeferredOcclusion = cuT( "c3d_debugDeferredOcclusion" );
	castor::String const DebugUbo::DebugDeferredSSSTransmittance = cuT( "c3d_debugDeferredSSSTransmittance" );
	castor::String const DebugUbo::DebugDeferredIBL = cuT( "c3d_debugDeferredIBL" );
	castor::String const DebugUbo::DebugDeferredNormals = cuT( "c3d_debugDeferredNormals" );
	castor::String const DebugUbo::DebugDeferredWorldPos = cuT( "c3d_debugDeferredWorldPos" );
	castor::String const DebugUbo::DebugDeferredViewPos = cuT( "c3d_debugDeferredViewPos" );
	castor::String const DebugUbo::DebugDeferredDepth = cuT( "c3d_debugDeferredDepth" );
	castor::String const DebugUbo::DebugDeferredData1 = cuT( "c3d_debugDeferredData1" );
	castor::String const DebugUbo::DebugDeferredData2 = cuT( "c3d_debugDeferredData2" );
	castor::String const DebugUbo::DebugDeferredData3 = cuT( "c3d_debugDeferredData3" );
	castor::String const DebugUbo::DebugDeferredData4 = cuT( "c3d_debugDeferredData4" );
	castor::String const DebugUbo::DebugDeferredData5 = cuT( "c3d_debugDeferredData5" );

	DebugUbo::DebugUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( m_engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	DebugUbo::~DebugUbo()
	{
	}

	void DebugUbo::initialise()
	{
		if ( !m_ubo )
		{
			m_ubo = makeUniformBuffer< Configuration >( *m_engine.getRenderSystem()
				, 1u
				, VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, "Debug" );
		}
	}

	void DebugUbo::cleanup()
	{
		m_ubo.reset();
	}

	void DebugUbo::update( DebugConfig const & config )
	{
		CU_Require( m_ubo );
		m_ubo->getData() = config;
		m_ubo->upload();
	}
}
