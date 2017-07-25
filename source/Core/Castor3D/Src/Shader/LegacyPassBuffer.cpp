#include "LegacyPassBuffer.hpp"

#include "Material/LegacyPass.hpp"

using namespace Castor;

namespace Castor3D
{
	namespace
	{
		LegacyPassBuffer::PassesData DoBindData( CpuBuffer< uint8_t > & buffer
			, uint32_t count )
		{
			auto data = buffer.GetData();
			auto diffuse = make_array_view( reinterpret_cast< PassBuffer::RgbColour * >( data )
				, reinterpret_cast< PassBuffer::RgbColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbColour ) * count;
			auto specular = make_array_view( reinterpret_cast< PassBuffer::RgbColour * >( data )
				, reinterpret_cast< PassBuffer::RgbColour * >( data ) + count );
			data += sizeof( PassBuffer::RgbColour ) * count;
			auto shininess = make_array_view( reinterpret_cast< float * >( data )
				, reinterpret_cast< float * >( data ) + count );
			data += sizeof( float ) * count;
			auto ambient = make_array_view( reinterpret_cast< float * >( data )
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
				diffuse,
				specular,
				shininess,
				ambient,
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

	LegacyPassBuffer::LegacyPassBuffer( Engine & engine
		, uint32_t count )
		: PassBuffer{ engine, count, DataSize }
		, m_data{ DoBindData( m_buffer, count ) }
	{
	}

	LegacyPassBuffer::~LegacyPassBuffer()
	{
	}

	void LegacyPassBuffer::Visit( LegacyPass const & pass )
	{
		REQUIRE( pass.GetId() > 0 );
		auto index = pass.GetId() - 1;
		m_data.diffuse[index].r = pass.GetDiffuse().red();
		m_data.diffuse[index].g = pass.GetDiffuse().green();
		m_data.diffuse[index].b = pass.GetDiffuse().blue();
		m_data.specular[index].r = pass.GetSpecular().red();
		m_data.specular[index].g = pass.GetSpecular().green();
		m_data.specular[index].b = pass.GetSpecular().blue();
		m_data.shininess[index] = pass.GetShininess();
		m_data.ambient[index] = pass.GetAmbient();
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
