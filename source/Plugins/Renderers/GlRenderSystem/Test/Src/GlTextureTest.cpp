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

using namespace castor;
using namespace castor3d;
using namespace GlRender;

namespace Testing
{
	namespace
	{
		constexpr auto Width = 8u;
		constexpr auto Height = 8u;
		constexpr auto Format = PixelFormat::eR8G8B8;
		constexpr auto Bpp = PixelDefinitions< Format >::Size;

		void doTestStorage( GlTextureTest & p_case
			, Engine & engine
			, TextureType p_type
			, AccessTypes const & cpuAccess
			, AccessTypes const & gpuAccess )
		{
			auto & renderSystem = static_cast< GlRenderSystem & >( *engine.getRenderSystem() );
			renderSystem.getMainContext()->setCurrent();
			auto texture = renderSystem.createTexture( p_type, cpuAccess, gpuAccess );
			texture->setSource( PxBufferBase::create( Size{ Width, Height }, Format ) );
			texture->initialise();
			std::array< uint8_t, Width * Height * Bpp > src;
			std::iota( src.begin(), src.end(), 0 );
			p_case.upload( *texture, ArrayView< uint8_t >{ src.data(), src.size() } );

			if ( renderSystem.getOpenGl().hasExtension( ARB_shader_image_load_store ) )
			{
				renderSystem.getOpenGl().MemoryBarrier( GlBarrierBit::eTextureUpdate );
			}

			renderSystem.getMainContext()->swapBuffers();

			std::vector< uint8_t > dst;
			dst.resize( src.size() );
			std::memset( texture->getImage().getBuffer()->ptr(), 0, texture->getImage().getBuffer()->size() );
			p_case.download( *texture, dst );

			if ( renderSystem.getOpenGl().hasExtension( ARB_shader_image_load_store ) )
			{
				renderSystem.getOpenGl().MemoryBarrier( GlBarrierBit::eTextureUpdate );
			}

			renderSystem.getMainContext()->swapBuffers();
			renderSystem.getMainContext()->endCurrent();
			p_case.Compare( src, dst );

			texture->cleanup();
		}
	}

	GlTextureTest::GlTextureTest( Engine & engine )
		: GlTestCase{ "GlTextureTest", engine }
	{
	}

	GlTextureTest::~GlTextureTest()
	{
	}

	void GlTextureTest::doRegisterTests()
	{
		if ( m_engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eImmutableTextureStorage ) )
		{
			doRegisterTest( "GlTextureTest::ImmutableStorage", std::bind( &GlTextureTest::ImmutableStorage, this ) );
		}

		doRegisterTest( "GlTextureTest::DirectStorage", std::bind( &GlTextureTest::DirectStorage, this ) );
		doRegisterTest( "GlTextureTest::PboStorage", std::bind( &GlTextureTest::PboStorage, this ) );
		doRegisterTest( "GlTextureTest::GpuOnlyStorage", std::bind( &GlTextureTest::GpuOnlyStorage, this ) );

		if ( m_engine.getRenderSystem()->getGpuInformations().hasFeature( GpuFeature::eTextureBuffers ) )
		{
			doRegisterTest( "GlTextureTest::TboStorage", std::bind( &GlTextureTest::TboStorage, this ) );
		}
	}

	void GlTextureTest::ImmutableStorage()
	{
	}

	void GlTextureTest::DirectStorage()
	{
		doTestStorage( *this, m_engine, TextureType::eTwoDimensions, AccessType::eRead | AccessType::eWrite, AccessType::eRead );
	}

	void GlTextureTest::PboStorage()
	{
		doTestStorage( *this, m_engine, TextureType::eTwoDimensions, AccessType::eRead | AccessType::eWrite, AccessType::eRead | AccessType::eWrite );
	}

	void GlTextureTest::GpuOnlyStorage()
	{
	}

	void GlTextureTest::TboStorage()
	{
		doTestStorage( *this, m_engine, TextureType::eBuffer, AccessType::eRead | AccessType::eWrite, AccessType::eRead | AccessType::eWrite );
	}

	void GlTextureTest::upload( TextureLayout & p_layout, ArrayView< uint8_t > const & p_view )
	{
		auto buffer = p_layout.lock( AccessType::eWrite );
		CT_REQUIRE( buffer );
		std::memcpy( buffer, &p_view[0], p_view.size() );
		p_layout.unlock( true );
	}

	void GlTextureTest::download( TextureLayout & p_layout, std::vector< uint8_t > & p_dst )
	{
		auto buffer = p_layout.lock( AccessType::eRead );
		CT_REQUIRE( buffer );
		std::memcpy( p_dst.data(), buffer, p_dst.size() );
		p_layout.unlock( true );
	}

	void GlTextureTest::Compare( std::array< uint8_t, 8 * 8 * 3 > const & p_src, std::vector< uint8_t > const & p_dst )
	{
		for ( size_t i{ 0u }; i < p_src.size(); ++i )
		{
			CT_EQUAL( p_src[i], p_dst[i] );
		}
	}
}
