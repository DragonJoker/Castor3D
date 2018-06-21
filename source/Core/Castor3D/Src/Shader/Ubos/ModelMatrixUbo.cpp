#include "ModelMatrixUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	uint32_t const ModelMatrixUbo::BindingPoint = 4u;
	String const ModelMatrixUbo::BufferModelMatrix = cuT( "ModelMatrices" );
	String const ModelMatrixUbo::PrvMtxModel = cuT( "c3d_prvMtxModel" );
	String const ModelMatrixUbo::PrvMtxNormal = cuT( "c3d_prvMtxNormal" );
	String const ModelMatrixUbo::CurMtxModel = cuT( "c3d_curMtxModel" );
	String const ModelMatrixUbo::CurMtxNormal = cuT( "c3d_curMtxNormal" );

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
		configuration.prvNormal = configuration.curNormal;
		configuration.prvModel = configuration.curModel;
		configuration.curNormal = castor::Matrix4x4r{ normal };
		configuration.curModel = model;
		m_ubo->upload();
	}
}
