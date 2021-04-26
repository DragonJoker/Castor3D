/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TexturesUbo_H___
#define ___C3D_TexturesUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct TextureData
			: public sdw::StructInstance
		{
			C3D_API TextureData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API TextureData & operator=( TextureData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			sdw::Array< sdw::UVec4 > config;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}

	class TexturesUbo
	{
	public:
		using Configuration = TexturesUboConfiguration;

	public:
		C3D_API static castor::String const BufferTextures;
		C3D_API static castor::String const TextureData;
	};
}

#define UBO_TEXTURES( writer, binding, set, enabled )\
	sdw::Ubo texturesUbo{ writer\
		, castor3d::TexturesUbo::BufferTextures\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, enabled };\
	auto c3d_textureData = texturesUbo.declStructMember< castor3d::shader::TextureData >( castor3d::TexturesUbo::TextureData );\
	texturesUbo.end()

#endif
