#include "Texture/TestTextureStorage.hpp"

#include <Texture/TextureImage.hpp>

#include <Log/Logger.hpp>

using namespace castor3d;
using namespace castor;

namespace TestRender
{
	TestTextureStorage::TestTextureStorage(
		TestRenderSystem & p_renderSystem,
		TextureStorageType p_type,
		TextureLayout & p_layout,
		AccessTypes const & cpuAccess,
		AccessTypes const & gpuAccess )
		: TextureStorage{ p_type, p_layout, cpuAccess, gpuAccess }
	{
	}

	TestTextureStorage::~TestTextureStorage()
	{
	}

	void TestTextureStorage::bind( uint32_t p_index )const
	{
	}

	void TestTextureStorage::unbind( uint32_t p_index )const
	{
	}

	uint8_t * TestTextureStorage::lock( AccessTypes const & p_lock )
	{
		ENSURE( checkFlag( m_cpuAccess, p_lock ) );
		return nullptr;
	}

	void TestTextureStorage::unlock( bool p_modified )
	{
	}

	uint8_t * TestTextureStorage::lock( AccessTypes const & p_lock, uint32_t p_index )
	{
		ENSURE( checkFlag( m_cpuAccess, p_lock ) );
		return nullptr;
	}

	void TestTextureStorage::unlock( bool p_modified, uint32_t p_index )
	{
	}
}
