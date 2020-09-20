#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Buffer/UniformBufferPools.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

namespace castor3d
{
	uint32_t const ModelMatrixUbo::BindingPoint = 5u;
	castor::String const ModelMatrixUbo::BufferModelMatrix = cuT( "ModelMatrices" );
	castor::String const ModelMatrixUbo::PrvMtxModel = cuT( "c3d_prvMtxModel" );
	castor::String const ModelMatrixUbo::PrvMtxNormal = cuT( "c3d_prvMtxNormal" );
	castor::String const ModelMatrixUbo::CurMtxModel = cuT( "c3d_curMtxModel" );
	castor::String const ModelMatrixUbo::CurMtxNormal = cuT( "c3d_curMtxNormal" );

	ModelMatrixUbo::ModelMatrixUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentRenderDevice() )
		{
			initialise();
		}
	}

	ModelMatrixUbo::~ModelMatrixUbo()
	{
	}

	void ModelMatrixUbo::initialise()
	{
		if ( !m_ubo )
		{
			auto & device = getCurrentRenderDevice( m_engine );
			m_ubo = device.uboPools->getBuffer< Configuration >( VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT );
		}
	}

	void ModelMatrixUbo::cleanup()
	{
		if ( m_ubo )
		{
			auto & device = getCurrentRenderDevice( m_engine );
			device.uboPools->putBuffer( m_ubo );
		}
	}

	void ModelMatrixUbo::cpuUpdate( castor::Matrix4x4f const & model )
	{
		auto normal = castor::Matrix3x3f{ model };
		normal.invert();
		normal.transpose();
		cpuUpdate( model, normal );
	}

	void ModelMatrixUbo::cpuUpdate( castor::Matrix4x4f const & model
		, castor::Matrix3x3f const & normal )
	{
		auto & configuration = m_ubo.getData();
		configuration.prvNormal = configuration.curNormal;
		configuration.prvModel = configuration.curModel;
		configuration.curNormal = castor::Matrix4x4f{ normal };
		configuration.curModel = model;
	}
}
