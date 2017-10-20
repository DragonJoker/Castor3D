#include "MetallicRoughnessPassBuffer.hpp"

#include "Material/LegacyPass.hpp"
#include "Material/MetallicRoughnessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		MetallicRoughnessPassBuffer::PassesData doBindData( uint8_t * data
			, uint32_t count )
		{
			return makeArrayView( reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data )
				, reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data ) + count );
		}
	}

	//*********************************************************************************************

	MetallicRoughnessPassBuffer::MetallicRoughnessPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ doBindData( m_buffer.ptr(), count ) }
	{
	}

	MetallicRoughnessPassBuffer::~MetallicRoughnessPassBuffer()
	{
	}

	void MetallicRoughnessPassBuffer::visit( LegacyPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

		m_data[index].albRough.r = pass.getDiffuse().red();
		m_data[index].albRough.g = pass.getDiffuse().green();
		m_data[index].albRough.b = pass.getDiffuse().blue();
		m_data[index].albRough.a = ( 255.0f - pass.getShininess() ) / 255.0f;
		m_data[index].metDiv.r = float( point::length( toRGBFloat( pass.getSpecular() ) ) / point::length( Point3r{ 1, 1, 1 } ) );
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

	void MetallicRoughnessPassBuffer::visit( MetallicRoughnessPbrPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

		m_data[index].albRough.r = pass.getAlbedo().red();
		m_data[index].albRough.g = pass.getAlbedo().green();
		m_data[index].albRough.b = pass.getAlbedo().blue();
		m_data[index].albRough.a = pass.getRoughness();
		m_data[index].metDiv.r = pass.getMetallic();
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
