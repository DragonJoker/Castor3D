#include "SpecularGlossinessPassBuffer.hpp"

#include "Material/LegacyPass.hpp"
#include "Material/SpecularGlossinessPbrPass.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		SpecularGlossinessPassBuffer::PassesData doBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.getData();
			auto diffDiv = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto specGloss = makeArrayView( reinterpret_cast< PassBuffer::RgbaColour * >( data )
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
				diffDiv,
				specGloss,
				common,
				reflRefr,
				{
					transmittance,
				},
			};
		}

#else

		SpecularGlossinessPassBuffer::PassesData doBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.getData();
			return makeArrayView( reinterpret_cast< SpecularGlossinessPassBuffer::PassData * >( data )
				, reinterpret_cast< SpecularGlossinessPassBuffer::PassData * >( data ) + count );
		}

#endif
	}

	//*********************************************************************************************

	SpecularGlossinessPassBuffer::SpecularGlossinessPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ doBindData( m_buffer, count ) }
	{
	}

	SpecularGlossinessPassBuffer::~SpecularGlossinessPassBuffer()
	{
	}

	void SpecularGlossinessPassBuffer::visit( LegacyPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.diffDiv[index].r = pass.getDiffuse().red();
		m_data.diffDiv[index].g = pass.getDiffuse().green();
		m_data.diffDiv[index].b = pass.getDiffuse().blue();
		m_data.specGloss[index].r = pass.getSpecular().red();
		m_data.specGloss[index].g = pass.getSpecular().green();
		m_data.specGloss[index].b = pass.getSpecular().blue();
		m_data.specGloss[index].a = pass.getShininess();
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

#endif
	}

	void SpecularGlossinessPassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		REQUIRE( pass.getId() > 0 );
		auto index = pass.getId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.diffDiv[index].r = pass.getDiffuse().red();
		m_data.diffDiv[index].g = pass.getDiffuse().green();
		m_data.diffDiv[index].b = pass.getDiffuse().blue();
		m_data.specGloss[index].r = pass.getSpecular().red();
		m_data.specGloss[index].g = pass.getSpecular().green();
		m_data.specGloss[index].b = pass.getSpecular().blue();
		m_data.specGloss[index].a = pass.getGlossiness();
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

#endif
	}
}
