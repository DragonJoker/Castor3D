#include "MetallicRoughnessPassBuffer.hpp"

#include "Material/LegacyPass.hpp"
#include "Material/MetallicRoughnessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		MetallicRoughnessPassBuffer::PassesData doBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.getData();
			auto albRough = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto metDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto reflRefr = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			return
			{
				albRough,
				metDiv,
				common,
				reflRefr,
			};
		}

#else

		MetallicRoughnessPassBuffer::PassesData doBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.getData();
			return makeArrayView( reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data )
				, reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data ) + count );
		}

#endif
	}

	//*********************************************************************************************

	MetallicRoughnessPassBuffer::MetallicRoughnessPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ doBindData( m_buffer, count ) }
	{
	}

	MetallicRoughnessPassBuffer::~MetallicRoughnessPassBuffer()
	{
	}

	void MetallicRoughnessPassBuffer::visit( LegacyPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.albRough[index].r = pass.getDiffuse().red();
		m_data.albRough[index].g  =pass.getDiffuse().green();
		m_data.albRough[index].b = pass.getDiffuse().blue();
		m_data.albRough[index].a = ( 255.0f - pass.getShininess() ) / 255.0f;
		m_data.metDiv[index].r = float( point::length( toRGBFloat( pass.getSpecular() ) ) / point::length( Point3r{ 1, 1, 1 } ) );
		m_data.common[index].r = pass.getOpacity();
		m_data.common[index].g = pass.getEmissive();
		m_data.common[index].b = pass.getAlphaValue();
		m_data.common[index].a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.getRefractionRatio();
		m_data.reflRefr[index].g = checkFlag( pass.getTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = checkFlag( pass.getTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = 1.0f;

#else

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

#endif
	}

	void MetallicRoughnessPassBuffer::visit( MetallicRoughnessPbrPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.albRough[index].r = pass.getAlbedo().red();
		m_data.albRough[index].g = pass.getAlbedo().green();
		m_data.albRough[index].b = pass.getAlbedo().blue();
		m_data.albRough[index].a = pass.getRoughness();
		m_data.metDiv[index].r = pass.getMetallic();
		m_data.common[index].r = pass.getOpacity();
		m_data.common[index].g = pass.getEmissive();
		m_data.common[index].b = pass.getAlphaValue();
		m_data.common[index].a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.getRefractionRatio();
		m_data.reflRefr[index].g = checkFlag( pass.getTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = checkFlag( pass.getTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = 1.0f;

#else

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

#endif
	}
}
