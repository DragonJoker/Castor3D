#include "Texture/TestTextureStorage.hpp"

#include <Texture/TextureImage.hpp>

#include <Log/Logger.hpp>

using namespace Castor3D;
using namespace Castor;

namespace TestRender
{
	TestTextureStorage::TestTextureStorage(
		TestRenderSystem & p_renderSystem,
		TextureStorageType p_type,
		TextureLayout & p_layout,
		FlagCombination< AccessType > const & p_cpuAccess,
		FlagCombination< AccessType > const & p_gpuAccess )
		: TextureStorage{ p_type, p_layout, p_cpuAccess, p_gpuAccess }
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

	uint8_t * TestTextureStorage::Lock( FlagCombination< AccessType > const & p_lock )
	{
		ENSURE( CheckFlag( m_cpuAccess, p_lock ) );
		return nullptr;
	}

	void TestTextureStorage::Unlock( bool p_modified )
	{
	}

	uint8_t * TestTextureStorage::Lock( FlagCombination< AccessType > const & p_lock, uint32_t p_index )
	{
		ENSURE( CheckFlag( m_cpuAccess, p_lock ) );
		return nullptr;
	}

	void TestTextureStorage::Unlock( bool p_modified, uint32_t p_index )
	{
	}
}
