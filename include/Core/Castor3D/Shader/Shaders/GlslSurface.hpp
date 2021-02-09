/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslSurface_H___
#define ___C3D_GlslSurface_H___

#include "SdwModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec3.hpp>

namespace castor3d
{
	namespace shader
	{
		struct Surface
			: public sdw::StructInstance
		{
			C3D_API Surface( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			C3D_API Surface & operator=( Surface const & rhs );

			C3D_API void create( sdw::Vec2 clip
				, sdw::Vec3 view
				, sdw::Vec3 world
				, sdw::Vec3 normal );
			C3D_API void create( sdw::Vec3 world
				, sdw::Vec3 normal );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			sdw::Vec2 clipPosition;
			sdw::Vec3 viewPosition;
			sdw::Vec3 worldPosition;
			sdw::Vec3 worldNormal;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};
	}
}

#endif
