#include "ModelMatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const ModelMatrixUbo::BufferModelMatrix = cuT( "ModelMatrices" );
	String const ModelMatrixUbo::MtxModel = cuT( "c3d_mtxModel" );
	String const ModelMatrixUbo::MtxNormal = cuT( "c3d_mtxNormal" );

	ModelMatrixUbo::ModelMatrixUbo( Engine & engine )
		: m_engine{ engine }
	{
		if ( engine.getRenderSystem()->hasCurrentDevice() )
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
			auto & device = getCurrentDevice( m_engine );
			m_ubo = renderer::makeUniformBuffer< Configuration >( device
				, 1u
				, renderer::BufferTarget::eTransferDst
				, renderer::MemoryPropertyFlag::eHostVisible );
		}
	}

	void ModelMatrixUbo::cleanup()
	{
		m_ubo.reset();
	}

	void ModelMatrixUbo::update( castor::Matrix4x4r const & model )const
	{
		auto normal = Matrix3x3r{ model };
		normal.invert();
		normal.transpose();
		update( model, normal );
	}

	void ModelMatrixUbo::update( castor::Matrix4x4r const & model
		, castor::Matrix3x3r const & normal )const
	{
		auto & configuration = m_ubo->getData( 0u );
		configuration.normal = castor::Matrix4x4r{ normal };
		configuration.model = model;
		m_ubo->upload();
	}
}
