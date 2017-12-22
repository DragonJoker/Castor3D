#include "Texture/TestTexture.hpp"

#include "Render/TestRenderSystem.hpp"

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & cpuAccess,
		AccessTypes const & gpuAccess )
		: TextureLayout{ p_renderSystem, p_type, cpuAccess, gpuAccess }
	{
	}

	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & cpuAccess,
		AccessTypes const & gpuAccess,
		uint32_t mipmapCount )
		: TextureLayout{ p_renderSystem, p_type, cpuAccess, gpuAccess, mipmapCount }
	{
	}

	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & cpuAccess,
		AccessTypes const & gpuAccess,
		PixelFormat p_format,
		Size const & p_size )
		: TextureLayout{ p_renderSystem, p_type, cpuAccess, gpuAccess, p_format, p_size }
	{
	}

	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & cpuAccess,
		AccessTypes const & gpuAccess,
		PixelFormat p_format,
		Point3ui const & p_size )
		: TextureLayout{ p_renderSystem, p_type, cpuAccess, gpuAccess, p_format, p_size }
	{
	}

	TestTexture::~TestTexture()
	{
	}

	void TestTexture::generateMipmaps()const
	{
	}

	bool TestTexture::doInitialise()
	{
		return true;
	}

	void TestTexture::doCleanup()
	{
	}

	void TestTexture::doBind( uint32_t p_index )const
	{
	}

	void TestTexture::doUnbind( uint32_t p_index )const
	{
	}
}
