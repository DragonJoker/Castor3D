/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TexturesUbo_H___
#define ___C3D_TexturesUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"

#include <ShaderWriter/Optional/OptionalUbo.hpp>

namespace castor3d
{
	class TexturesUbo
	{
	public:
		using Configuration = TexturesUboConfiguration;

	public:
		C3D_API static uint32_t const BindingPoint;
		//!\~english	Name of the HDR configuration frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant la configuration du HDR.
		C3D_API static castor::String const BufferTextures;
		//!\~english	Name of the texture configurations indices frame variable.
		//!\~french		Nom de la frame variable contenant les indices de configurations de textures.
		C3D_API static castor::String const TexturesConfig;
	};
}

#define UBO_TEXTURES( writer, binding, set, enabled )\
	Optional< Ubo > textures{ writer\
		, enabled\
		, castor3d::TexturesUbo::BufferTextures\
		, binding\
		, set };\
	auto c3d_textureConfig = textures.declMember< UVec4 >( castor3d::TexturesUbo::TexturesConfig, 3u );\
	textures.end()

#endif
