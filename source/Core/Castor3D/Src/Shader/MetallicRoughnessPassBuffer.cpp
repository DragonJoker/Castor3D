#include "MetallicRoughnessPassBuffer.hpp"

#include "Material/LegacyPass.hpp"
#include "Material/MetallicRoughnessPbrPass.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		MetallicRoughnessPassBuffer::PassesData DoBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.GetData();
			auto albedo = make_array_view( reinterpret_cast< PassBuffer::RgbColour * >( data )
				, reinterpret_cast< PassBuffer::RgbColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbColour ) * count;
			auto roughness = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto metallic = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto opacity = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto emissive = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto gamma = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto exposure = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto alphaRef = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto refractionRatio = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto hasRefraction = make_array_view( reinterpret_cast< int * >( data )
				, reinterpret_cast< int * >( data ) + count );
			data += sizeof( int ) * count;
			auto hasReflection = make_array_view( reinterpret_cast< int * >( data )
				, reinterpret_cast< int * >( data ) + count );
			data += sizeof( int ) * count;
			return
			{
				albedo,
				roughness,
				metallic,
				opacity,
				emissive,
				gamma,
				exposure,
				alphaRef,
				refractionRatio,
				hasReflection,
				hasRefraction
			};
		}
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
		m_data.albedo[index].r = pass.GetDiffuse().red();
		m_data.albedo[index].g  =pass.GetDiffuse().green();
		m_data.albedo[index].b = pass.GetDiffuse().blue();
		m_data.roughness[index] = ( 255.0f - pass.GetShininess() ) / 255.0f;
		m_data.metallic[index] = float( point::length( rgb_float( pass.GetSpecular() ) ) / point::length( Point3r{ 1, 1, 1 } ) );
		m_data.opacity[index] = pass.GetOpacity();
		m_data.emissive[index] = pass.GetEmissive();
		m_data.gamma[index] = pass.NeedsGammaCorrection() ? 2.2f : 1.0f;
		m_data.exposure[index] = 1.0f;
		m_data.alphaRef[index] = pass.GetAlphaValue();
		m_data.refractionRatio[index] = pass.GetRefractionRatio();
		m_data.hasRefraction[index] = CheckFlag( pass.GetTextureFlags(), TextureChannel::eRefraction ) ? 1 : 0;
		m_data.hasReflection[index] = CheckFlag( pass.GetTextureFlags(), TextureChannel::eReflection ) ? 1 : 0;
	}

	void MetallicRoughnessPassBuffer::Visit( MetallicRoughnessPbrPass const & pass )
	{
		REQUIRE( pass.GetId() > 0 );
		auto index = pass.GetId() - 1;
		m_data.albedo[index].r = pass.GetAlbedo().red();
		m_data.albedo[index].g = pass.GetAlbedo().green();
		m_data.albedo[index].b = pass.GetAlbedo().blue();
		m_data.roughness[index] = pass.GetRoughness();
		m_data.metallic[index] = pass.GetMetallic();
		m_data.opacity[index] = pass.GetOpacity();
		m_data.emissive[index] = pass.GetEmissive();
		m_data.gamma[index] = pass.NeedsGammaCorrection() ? 2.2f : 1.0f;
		m_data.exposure[index] = 1.0f;
		m_data.alphaRef[index] = pass.GetAlphaValue();
		m_data.refractionRatio[index] = pass.GetRefractionRatio();
		m_data.hasRefraction[index] = CheckFlag( pass.GetTextureFlags(), TextureChannel::eRefraction ) ? 1 : 0;
		m_data.hasReflection[index] = CheckFlag( pass.GetTextureFlags(), TextureChannel::eReflection ) ? 1 : 0;
	}
}
