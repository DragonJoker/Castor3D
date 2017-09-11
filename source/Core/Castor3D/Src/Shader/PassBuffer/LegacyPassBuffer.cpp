#include "LegacyPassBuffer.hpp"

#include "Material/LegacyPass.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		LegacyPassBuffer::PassesData doBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.getData();
			auto diffAmb = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto specShin = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto reflRefr = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto transmittance = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			return
			{
				diffAmb,
				specShin,
				common,
				reflRefr,
				{
					transmittance,
				},
			};
		}

#else

		LegacyPassBuffer::PassesData doBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.getData();
			return makeArrayView( reinterpret_cast< LegacyPassBuffer::PassData * >( data )
				, reinterpret_cast< LegacyPassBuffer::PassData * >( data ) + count );
		}

#endif
	}

	//*********************************************************************************************

	LegacyPassBuffer::LegacyPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ doBindData( m_buffer, count ) }
	{
	}

	LegacyPassBuffer::~LegacyPassBuffer()
	{
	}

	void LegacyPassBuffer::visit( LegacyPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.diffAmb[index].r = pass.getDiffuse().red();
		m_data.diffAmb[index].g = pass.getDiffuse().green();
		m_data.diffAmb[index].b = pass.getDiffuse().blue();
		m_data.diffAmb[index].a = pass.getAmbient();
		m_data.specShin[index].r = pass.getSpecular().red();
		m_data.specShin[index].g = pass.getSpecular().green();
		m_data.specShin[index].b = pass.getSpecular().blue();
		m_data.specShin[index].a = pass.getShininess();
		m_data.common[index].r = pass.getOpacity();
		m_data.common[index].g = pass.getEmissive();
		m_data.common[index].b = pass.getAlphaValue();
		m_data.common[index].a = pass.needsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.getRefractionRatio();
		m_data.reflRefr[index].g = checkFlag( pass.getTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = checkFlag( pass.getTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = 1.0f;
		doVisitExtended( pass, m_data.extended );

#else

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

#endif
	}
}
