#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Animation/AnimatedSkeleton.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include <ShaderWriter/Source.hpp>

namespace castor3d
{
	castor::String const SkinningUbo::BufferSkinning = cuT( "Skinning" );
	castor::String const SkinningUbo::Bones = cuT( "c3d_mtxBones" );

	shader::SkinningData SkinningUbo::declare( sdw::ShaderWriter & writer
		, uint32_t uboBinding
		, uint32_t sboBinding
		, uint32_t set
		, ProgramFlags const & flags )
	{
		shader::SkinningData result;

		if ( checkFlag( flags, ProgramFlag::eSkinning ) )
		{
			if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
			{
				result.ssbo = std::make_unique< sdw::ArraySsboT< sdw::Mat4 > >( writer
					, SkinningUbo::BufferSkinning
					, writer.getTypesCache().getMat4x4F()
					, ast::type::MemoryLayout::eStd140
					, sboBinding
					, set
					, true );
			}
			else
			{
				result.ubo = std::make_unique< sdw::Ubo >( writer
					, SkinningUbo::BufferSkinning
					, uboBinding
					, set );
				result.ubo->declMember< sdw::Mat4 >( SkinningUbo::Bones
					, 400
					, checkFlag( flags, ProgramFlag::eSkinning ) );
				result.ubo->end();
			}
		}

		return result;
	}
}
