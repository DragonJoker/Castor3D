#include "Texture/TestTextureStorage.hpp"

#include <Texture/TextureImage.hpp>

#include <Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestTextureStorage::TestTextureStorage( TestRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, uint8_t p_cpuAccess, uint8_t p_gpuAccess )
		: TextureStorage{ p_type, p_image, p_cpuAccess, p_gpuAccess }
	{
	}

	TestTextureStorage::~TestTextureStorage()
	{
	}

	bool TestTextureStorage::Bind( uint32_t p_index )const
	{
		return true;
	}

	void TestTextureStorage::Unbind( uint32_t p_index )const
	{
	}

	uint8_t * TestTextureStorage::Lock( uint32_t p_lock )
	{
		return nullptr;
	}

	void TestTextureStorage::Unlock( bool p_modified )
	{
	}

	void TestTextureStorage::DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format )
	{
	}

	void TestTextureStorage::DoUploadImage( uint32_t p_width, uint32_t p_height, ePIXEL_FORMAT p_format, uint8_t const * p_buffer )
	{
	}
}
