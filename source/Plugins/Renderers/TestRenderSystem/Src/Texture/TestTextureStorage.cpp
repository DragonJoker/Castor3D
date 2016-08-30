#include "Texture/TestTextureStorage.hpp"

#include <Texture/TextureImage.hpp>

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestTextureStorage::TestTextureStorage( TestRenderSystem & p_renderSystem, TextureStorageType p_type, TextureImage & p_image, AccessType p_cpuAccess, AccessType p_gpuAccess )
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

	uint8_t * TestTextureStorage::Lock( AccessType p_lock )
	{
		ENSURE( CheckFlag( m_cpuAccess, p_lock ) );
		return nullptr;
	}

	void TestTextureStorage::Unlock( bool p_modified )
	{
	}

	void TestTextureStorage::Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::PixelFormat p_format )
	{
	}
}
