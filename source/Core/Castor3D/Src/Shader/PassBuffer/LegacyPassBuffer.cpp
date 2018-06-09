#include "LegacyPassBuffer.hpp"

#include "Material/LegacyPass.hpp"

using namespace castor;

namespace castor3d
{
	LegacyPassBuffer::LegacyPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ makeArrayView( reinterpret_cast< LegacyPassBuffer::PassData * >( m_buffer.getPtr() )
			, reinterpret_cast< LegacyPassBuffer::PassData * >( m_buffer.getPtr() ) + count ) }
	{
	}

	LegacyPassBuffer::~LegacyPassBuffer()
	{
	}

	void LegacyPassBuffer::visit( LegacyPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

		m_data[index].diffAmb.r = pass.getDiffuse().red();
		m_data[index].diffAmb.g = pass.getDiffuse().green();
		m_data[index].diffAmb.b = pass.getDiffuse().blue();
		m_data[index].diffAmb.a = pass.getAmbient();
		m_data[index].specShin.r = pass.getSpecular().red();
		m_data[index].specShin.g = pass.getSpecular().green();
		m_data[index].specShin.b = pass.getSpecular().blue();
		m_data[index].specShin.a = pass.getShininess();
		m_data[index].common.r = pass.getOpacity();
		m_data[index].common.g = pass.getEmissive();
		m_data[index].common.b = pass.getAlphaValue();
		m_data[index].common.a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data[index].reflRefr.r = pass.getRefractionRatio();
		m_data[index].reflRefr.g = checkFlag( pass.getTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.b = checkFlag( pass.getTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.a = 1.0f;
		doVisitExtended( pass, m_data[index].extended );
	}
}
