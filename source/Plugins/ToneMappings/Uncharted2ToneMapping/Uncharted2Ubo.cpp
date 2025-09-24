#include "Uncharted2ToneMapping/Uncharted2Ubo.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Buffer/UniformBufferPool.hpp>

#include <CastorUtils/Graphics/Size.hpp>

#include <ShaderWriter/Source.hpp>

namespace Uncharted2
{
	//*********************************************************************************************

	const c3d::String Uncharted2Ubo::Buffer = cuT( "Uncharted2" );
	const c3d::String Uncharted2Ubo::Data = cuT( "Uncharted2Data" );

	Uncharted2Ubo::Uncharted2Ubo( c3d::RenderDevice const & device )
		: m_device{ device }
		, m_ubo{ device.uboPool->getBuffer< Configuration >( c3d::MemoryPropertyFlags::eNone ) }
	{
		auto & data = m_ubo.getData();
		data = Configuration{};
	}

	Uncharted2Ubo::~Uncharted2Ubo()
	{
		m_device.uboPool->putBuffer( m_ubo );
	}

	//************************************************************************************************
}
