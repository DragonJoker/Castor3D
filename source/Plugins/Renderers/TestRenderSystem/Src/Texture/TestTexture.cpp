#include "Texture/TestTexture.hpp"

#include "Render/TestRenderSystem.hpp"

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess )
		: TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess }
	{
	}

	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Size const & p_size )
		: TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
	{
	}

	TestTexture::TestTexture(
		TestRenderSystem & p_renderSystem,
		TextureType p_type,
		AccessTypes const & p_cpuAccess,
		AccessTypes const & p_gpuAccess,
		PixelFormat p_format,
		Point3ui const & p_size )
		: TextureLayout{ p_renderSystem, p_type, p_cpuAccess, p_gpuAccess, p_format, p_size }
	{
	}

	TestTexture::~TestTexture()
	{
	}

	void TestTexture::GenerateMipmaps()const
	{
	}

	bool TestTexture::DoInitialise()
	{
		return true;
	}

	void TestTexture::DoCleanup()
	{
	}

	void TestTexture::DoBind( uint32_t p_index )const
	{
	}

	void TestTexture::DoUnbind( uint32_t p_index )const
	{
	}
}
