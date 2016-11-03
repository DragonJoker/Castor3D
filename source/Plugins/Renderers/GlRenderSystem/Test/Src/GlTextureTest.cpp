#include "GlTextureTest.hpp"

#include <Render/GlRenderSystem.hpp>
#include <Texture/GlTextureStorage.hpp>
#include <Texture/GlDirectTextureStorage.hpp>
#include <Texture/GlGpuOnlyTextureStorage.hpp>
#include <Texture/GlImmutableTextureStorage.hpp>
#include <Texture/GlPboTextureStorage.hpp>
#include <Texture/GlTboTextureStorage.hpp>
#include <Texture/GlTexture.hpp>

#include <Design/ArrayView.hpp>
#include <Buffer/GlBuffer.hpp>

using namespace Castor;
using namespace Castor3D;
using namespace GlRender;

namespace Testing
{
	namespace
	{
		TextureStorageType GetStorageType( TextureType p_type )
		{
			TextureStorageType l_return = TextureStorageType::Count;

			switch ( p_type )
			{
			case TextureType::Buffer:
				l_return = TextureStorageType::Buffer;
				break;

			case TextureType::OneDimension:
				l_return = TextureStorageType::OneDimension;
				break;

			case TextureType::OneDimensionArray:
				l_return = TextureStorageType::OneDimensionArray;
				break;

			case TextureType::TwoDimensions:
				l_return = TextureStorageType::TwoDimensions;
				break;

			case TextureType::TwoDimensionsArray:
				l_return = TextureStorageType::TwoDimensionsArray;
				break;

			case TextureType::TwoDimensionsMS:
				l_return = TextureStorageType::TwoDimensionsMS;
				break;

			case TextureType::ThreeDimensions:
				l_return = TextureStorageType::ThreeDimensions;
				break;

			case TextureType::Cube:
				l_return = TextureStorageType::CubeMap;
				break;

			case TextureType::CubeArray:
				l_return = TextureStorageType::CubeMapArray;
				break;

			default:
				FAILURE( "The given texture type doesn't have any associated storage type" );
				CASTOR_EXCEPTION( cuT( "The given texture type doesn't have any associated storage type" ) );
				break;
			}

			return l_return;
		}

		void DoTestStorage( GlTextureTest & p_case
							, Engine & p_engine
							, TextureType p_type
							, AccessType p_cpuAccess
							, AccessType p_gpuAccess )
		{
			auto l_renderSystem = p_engine.GetRenderSystem();
			l_renderSystem->GetMainContext()->SetCurrent();
			auto l_texture = l_renderSystem->CreateTexture( p_type, p_cpuAccess, p_gpuAccess );
			l_texture->SetSource( PxBufferBase::create( Size{ 8, 8 }, PixelFormat::eR8G8B8 ) );
			l_texture->Initialise();
			std::array< uint8_t, 8 * 8 * 3 > l_src;
			std::iota( l_src.begin(), l_src.end(), 0 );
			p_case.Upload( *l_texture, ArrayView< uint8_t >{ l_src.data(), l_src.size() } );

			l_renderSystem->GetMainContext()->SwapBuffers();

			std::vector< uint8_t > l_dst;
			l_dst.resize( l_src.size() );
			p_case.Download( *l_texture, l_dst );

			l_renderSystem->GetMainContext()->EndCurrent();
			p_case.Compare( l_src, l_dst );
		}
	}

	GlTextureTest::GlTextureTest( Engine & p_engine )
		: GlTestCase{ "GlTextureTest", p_engine }
	{
	}

	GlTextureTest::~GlTextureTest()
	{
	}

	void GlTextureTest::DoRegisterTests()
	{
		DoRegisterTest( "GlTextureTest::ImmutableStorage", std::bind( &GlTextureTest::ImmutableStorage, this ) );
		DoRegisterTest( "GlTextureTest::DirectStorage", std::bind( &GlTextureTest::DirectStorage, this ) );
		DoRegisterTest( "GlTextureTest::PboStorage", std::bind( &GlTextureTest::PboStorage, this ) );
		DoRegisterTest( "GlTextureTest::GpuOnlyStorage", std::bind( &GlTextureTest::GpuOnlyStorage, this ) );
		DoRegisterTest( "GlTextureTest::TboStorage", std::bind( &GlTextureTest::TboStorage, this ) );
	}

	void GlTextureTest::ImmutableStorage()
	{
	}

	void GlTextureTest::DirectStorage()
	{
		DoTestStorage( *this, m_engine, TextureType::TwoDimensions, AccessType::ReadWrite, AccessType::Read );
	}

	void GlTextureTest::PboStorage()
	{
		DoTestStorage( *this, m_engine, TextureType::TwoDimensions, AccessType::ReadWrite, AccessType::ReadWrite );
	}

	void GlTextureTest::GpuOnlyStorage()
	{
	}

	void GlTextureTest::TboStorage()
	{
	}

	void GlTextureTest::Upload( TextureLayout & p_layout, ArrayView< uint8_t > const & p_view )
	{
		CT_REQUIRE( p_layout.Bind( 0u ) );
		auto l_buffer = p_layout.Lock( AccessType::Write );
		CT_REQUIRE( l_buffer );
		std::memcpy( l_buffer, &p_view[0], p_view.size() );
		p_layout.Unlock( true );
		p_layout.Unbind( 0u );
	}

	void GlTextureTest::Download( TextureLayout & p_layout, std::vector< uint8_t > & p_dst )
	{
		CT_REQUIRE( p_layout.Bind( 0u ) );
		auto l_buffer = p_layout.Lock( AccessType::Write );
		CT_REQUIRE( l_buffer );
		std::memcpy( p_dst.data(), l_buffer, p_dst.size() );
		p_layout.Unlock( true );
		p_layout.Unbind( 0u );
	}

	void GlTextureTest::Compare( std::array< uint8_t, 8 * 8 * 3 > const & p_src, std::vector< uint8_t > const & p_dst )
	{
		for ( auto i{ 0u }; i < p_src.size(); ++i )
		{
			CT_EQUAL( p_src[i], p_dst[i] );
		}
	}
}
