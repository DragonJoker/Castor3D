#include "BillboardUbo.hpp"

#include "Engine.hpp"
#include "Render/RenderSystem.hpp"

#include <Buffer/UniformBuffer.hpp>

using namespace castor;

namespace castor3d
{
	String const BillboardUbo::BufferBillboard = cuT( "Billboard" );
	String const BillboardUbo::Dimensions = cuT( "c3d_dimensions" );
	String const BillboardUbo::WindowSize = cuT( "c3d_windowSize" );

	BillboardUbo::BillboardUbo( Engine & engine )
		: m_engine{ engine }
	{
	}

	BillboardUbo::~BillboardUbo()
	{
	}

	void BillboardUbo::initialise()
	{
		auto & device = *m_engine.getRenderSystem()->getCurrentDevice();
		m_ubo = renderer::makeUniformBuffer< Configuration >( device
			, 1u
			, renderer::BufferTarget::eTransferDst
			, renderer::MemoryPropertyFlag::eHostVisible );
	}

	void BillboardUbo::cleanup()
	{
		m_ubo.reset();
	}

	void BillboardUbo::update( Point2f const & dimensions )const
	{
		m_ubo->getData( 0u ).dimensions = dimensions;
		m_ubo->upload();
	}

	void BillboardUbo::setWindowSize( Size const & window )const
	{
		m_ubo->getData( 0u ).windowSize = castor::Point2i{ window[0], window[1] };
	}
}
