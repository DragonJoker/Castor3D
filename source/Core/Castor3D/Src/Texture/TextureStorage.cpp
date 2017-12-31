#include "TextureStorage.hpp"

#include "Engine.hpp"

using namespace castor;

namespace castor3d
{
	TextureStorage::TextureStorage(
		TextureStorageType p_type,
		TextureLayout & p_layout,
		AccessTypes const & cpuAccess,
		AccessTypes const & gpuAccess )
		: OwnedBy< TextureLayout >{ p_layout }
		, m_cpuAccess{ cpuAccess }
		, m_gpuAccess{ gpuAccess }
		, m_type{ p_type }
	{
	}

	TextureStorage::~TextureStorage()
	{
	}
}
