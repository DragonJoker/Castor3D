﻿#include "MetallicRoughnessPassBuffer.hpp"

#include "Material/LegacyPass.hpp"
#include "Material/MetallicRoughnessPbrPass.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		MetallicRoughnessPassBuffer::PassesData DoBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.GetData();
			auto albRough = make_array_view( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto metDiv = make_array_view( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto common = make_array_view( reinterpret_cast< PassBuffer::RgbaColour * >( data )
				, reinterpret_cast< PassBuffer::RgbaColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbaColour ) * count;
			auto reflRefr = make_array_view( reinterpret_cast< PassBuffer::RgbaColour * >( data )
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

		MetallicRoughnessPassBuffer::PassesData DoBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.GetData();
			return make_array_view( reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data )
				, reinterpret_cast< MetallicRoughnessPassBuffer::PassData * >( data ) + count );
		}

#endif
	}

	//*********************************************************************************************

	MetallicRoughnessPassBuffer::MetallicRoughnessPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ DoBindData( m_buffer, count ) }
	{
	}

	MetallicRoughnessPassBuffer::~MetallicRoughnessPassBuffer()
	{
	}

	void MetallicRoughnessPassBuffer::Visit( LegacyPass const & pass )
	{
		REQUIRE( pass.GetId() > 0 );
		auto index = pass.GetId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.albRough[index].r = pass.GetDiffuse().red();
		m_data.albRough[index].g  =pass.GetDiffuse().green();
		m_data.albRough[index].b = pass.GetDiffuse().blue();
		m_data.albRough[index].a = ( 255.0f - pass.GetShininess() ) / 255.0f;
		m_data.metDiv[index].r = float( point::length( rgb_float( pass.GetSpecular() ) ) / point::length( Point3r{ 1, 1, 1 } ) );
		m_data.common[index].r = pass.GetOpacity();
		m_data.common[index].g = pass.GetEmissive();
		m_data.common[index].b = pass.GetAlphaValue();
		m_data.common[index].a = pass.NeedsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.GetRefractionRatio();
		m_data.reflRefr[index].g = CheckFlag( pass.GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = CheckFlag( pass.GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = 1.0f;

#else

		m_data[index].albRough.r = pass.GetDiffuse().red();
		m_data[index].albRough.g = pass.GetDiffuse().green();
		m_data[index].albRough.b = pass.GetDiffuse().blue();
		m_data[index].albRough.a = ( 255.0f - pass.GetShininess() ) / 255.0f;
		m_data[index].metDiv.r = float( point::length( rgb_float( pass.GetSpecular() ) ) / point::length( Point3r{ 1, 1, 1 } ) );
		m_data[index].common.r = pass.GetOpacity();
		m_data[index].common.g = pass.GetEmissive();
		m_data[index].common.b = pass.GetAlphaValue();
		m_data[index].common.a = pass.NeedsGammaCorrection() ? 2.2f : 1.0f;
		m_data[index].reflRefr.r = pass.GetRefractionRatio();
		m_data[index].reflRefr.g = CheckFlag( pass.GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.b = CheckFlag( pass.GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.a = 1.0f;

#endif
	}

	void MetallicRoughnessPassBuffer::Visit( MetallicRoughnessPbrPass const & pass )
	{
		REQUIRE( pass.GetId() > 0 );
		auto index = pass.GetId() - 1;

#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		m_data.albRough[index].r = pass.GetAlbedo().red();
		m_data.albRough[index].g = pass.GetAlbedo().green();
		m_data.albRough[index].b = pass.GetAlbedo().blue();
		m_data.albRough[index].a = pass.GetRoughness();
		m_data.metDiv[index].r = pass.GetMetallic();
		m_data.common[index].r = pass.GetOpacity();
		m_data.common[index].g = pass.GetEmissive();
		m_data.common[index].b = pass.GetAlphaValue();
		m_data.common[index].a = pass.NeedsGammaCorrection() ? 2.2f : 1.0f;
		m_data.reflRefr[index].r = pass.GetRefractionRatio();
		m_data.reflRefr[index].g = CheckFlag( pass.GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].b = CheckFlag( pass.GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data.reflRefr[index].a = 1.0f;

#else

		m_data[index].albRough.r = pass.GetAlbedo().red();
		m_data[index].albRough.g = pass.GetAlbedo().green();
		m_data[index].albRough.b = pass.GetAlbedo().blue();
		m_data[index].albRough.a = pass.GetRoughness();
		m_data[index].metDiv.r = pass.GetMetallic();
		m_data[index].common.r = pass.GetOpacity();
		m_data[index].common.g = pass.GetEmissive();
		m_data[index].common.b = pass.GetAlphaValue();
		m_data[index].common.a = pass.NeedsGammaCorrection() ? 2.2f : 1.0f;
		m_data[index].reflRefr.r = pass.GetRefractionRatio();
		m_data[index].reflRefr.g = CheckFlag( pass.GetTextureFlags(), TextureChannel::eRefraction ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.b = CheckFlag( pass.GetTextureFlags(), TextureChannel::eReflection ) ? 1.0f : 0.0f;
		m_data[index].reflRefr.a = 1.0f;

#endif
	}
}
