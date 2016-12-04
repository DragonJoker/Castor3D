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
		constexpr auto Width = 8u;
		constexpr auto Height = 8u;
		constexpr auto Format = PixelFormat::eR8G8B8;
		constexpr auto Bpp = pixel_definitions< Format >::Size;

		void DoTestStorage( GlTextureTest & p_case
			, Engine & p_engine
			, TextureType p_type
			, FlagCombination< AccessType > const & p_cpuAccess
			, FlagCombination< AccessType > const & p_gpuAccess )
		{
			auto & l_renderSystem = static_cast< GlRenderSystem & >( *p_engine.GetRenderSystem() );
			l_renderSystem.GetMainContext()->SetCurrent();
			auto l_texture = l_renderSystem.CreateTexture( p_type, p_cpuAccess, p_gpuAccess );
			l_texture->SetSource( PxBufferBase::create( Size{ Width, Height }, Format ) );
			l_texture->Initialise();
			std::array< uint8_t, Width * Height * Bpp > l_src;
			std::iota( l_src.begin(), l_src.end(), 0 );
			p_case.Upload( *l_texture, ArrayView< uint8_t >{ l_src.data(), l_src.size() } );

			if ( l_renderSystem.GetOpenGl().HasExtension( ARB_shader_image_load_store ) )
			{
				l_renderSystem.GetOpenGl().MemoryBarrier( GlBarrierBit::eTextureUpdate );
			}

			l_renderSystem.GetMainContext()->SwapBuffers();

			std::vector< uint8_t > l_dst;
			l_dst.resize( l_src.size() );
			std::memset( l_texture->GetImage().GetBuffer()->ptr(), 0, l_texture->GetImage().GetBuffer()->size() );
			p_case.Download( *l_texture, l_dst );

			if ( l_renderSystem.GetOpenGl().HasExtension( ARB_shader_image_load_store ) )
			{
				l_renderSystem.GetOpenGl().MemoryBarrier( GlBarrierBit::eTextureUpdate );
			}

			l_renderSystem.GetMainContext()->SwapBuffers();
			l_renderSystem.GetMainContext()->EndCurrent();
			p_case.Compare( l_src, l_dst );

			l_texture->Cleanup();
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
		if ( m_engine.GetRenderSystem()->GetGpuInformations().HasFeature( GpuFeature::eImmutableTextureStorage ) )
		{
			DoRegisterTest( "GlTextureTest::ImmutableStorage", std::bind( &GlTextureTest::ImmutableStorage, this ) );
		}

		DoRegisterTest( "GlTextureTest::DirectStorage", std::bind( &GlTextureTest::DirectStorage, this ) );
		DoRegisterTest( "GlTextureTest::PboStorage", std::bind( &GlTextureTest::PboStorage, this ) );
		DoRegisterTest( "GlTextureTest::GpuOnlyStorage", std::bind( &GlTextureTest::GpuOnlyStorage, this ) );

		if ( m_engine.GetRenderSystem()->GetGpuInformations().HasFeature( GpuFeature::eTextureBuffers ) )
		{
			DoRegisterTest( "GlTextureTest::TboStorage", std::bind( &GlTextureTest::TboStorage, this ) );
		}
	}

	void GlTextureTest::ImmutableStorage()
	{
	}

	void GlTextureTest::DirectStorage()
	{
		DoTestStorage( *this, m_engine, TextureType::eTwoDimensions, AccessType::eRead | AccessType::eWrite, AccessType::eRead );
	}

	void GlTextureTest::PboStorage()
	{
		DoTestStorage( *this, m_engine, TextureType::eTwoDimensions, AccessType::eRead | AccessType::eWrite, AccessType::eRead | AccessType::eWrite );
	}

	void GlTextureTest::GpuOnlyStorage()
	{
	}

	void GlTextureTest::TboStorage()
	{
		DoTestStorage( *this, m_engine, TextureType::eBuffer, AccessType::eRead | AccessType::eWrite, AccessType::eRead | AccessType::eWrite );
	}

	void GlTextureTest::Upload( TextureLayout & p_layout, ArrayView< uint8_t > const & p_view )
	{
		auto l_buffer = p_layout.Lock( AccessType::eWrite );
		CT_REQUIRE( l_buffer );
		std::memcpy( l_buffer, &p_view[0], p_view.size() );
		p_layout.Unlock( true );
	}

	void GlTextureTest::Download( TextureLayout & p_layout, std::vector< uint8_t > & p_dst )
	{
		auto l_buffer = p_layout.Lock( AccessType::eRead );
		CT_REQUIRE( l_buffer );
		std::memcpy( p_dst.data(), l_buffer, p_dst.size() );
		p_layout.Unlock( true );
	}

	void GlTextureTest::Compare( std::array< uint8_t, 8 * 8 * 3 > const & p_src, std::vector< uint8_t > const & p_dst )
	{
		for ( size_t i{ 0u }; i < p_src.size(); ++i )
		{
			CT_EQUAL( p_src[i], p_dst[i] );
		}
	}
}
