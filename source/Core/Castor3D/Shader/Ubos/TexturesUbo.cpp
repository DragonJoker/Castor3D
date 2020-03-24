#include "Castor3D/Shader/Ubos/TexturesUbo.hpp"

using namespace castor;

namespace castor3d
{
	uint32_t const TexturesUbo::BindingPoint = 6u;
	String const TexturesUbo::BufferTextures = cuT( "Textures" );
	String const TexturesUbo::TexturesConfig = cuT( "c3d_textureConfig" );
}
