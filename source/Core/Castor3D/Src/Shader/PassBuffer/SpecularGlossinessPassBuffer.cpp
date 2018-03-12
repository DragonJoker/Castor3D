#include "SpecularGlossinessPassBuffer.hpp"

#include "Material/LegacyPass.hpp"
#include "Material/SpecularGlossinessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	SpecularGlossinessPassBuffer::SpecularGlossinessPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ makeArrayView( reinterpret_cast< SpecularGlossinessPassBuffer::PassData * >( m_buffer.getPtr() )
			, reinterpret_cast< SpecularGlossinessPassBuffer::PassData * >( m_buffer.getPtr() ) + count ) }
	{
	}

	SpecularGlossinessPassBuffer::~SpecularGlossinessPassBuffer()
	{
	}

	void SpecularGlossinessPassBuffer::visit( LegacyPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

		m_data[index].diffDiv.r = pass.getDiffuse().red();
		m_data[index].diffDiv.g = pass.getDiffuse().green();
		m_data[index].diffDiv.b = pass.getDiffuse().blue();
		m_data[index].specGloss.r = pass.getSpecular().red();
		m_data[index].specGloss.g = pass.getSpecular().green();
		m_data[index].specGloss.b = pass.getSpecular().blue();
		m_data[index].specGloss.a = pass.getShininess();
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

	void SpecularGlossinessPassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

		m_data[index].diffDiv.r = pass.getDiffuse().red();
		m_data[index].diffDiv.g = pass.getDiffuse().green();
		m_data[index].diffDiv.b = pass.getDiffuse().blue();
		m_data[index].specGloss.r = pass.getSpecular().red();
		m_data[index].specGloss.g = pass.getSpecular().green();
		m_data[index].specGloss.b = pass.getSpecular().blue();
		m_data[index].specGloss.a = pass.getGlossiness();
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
