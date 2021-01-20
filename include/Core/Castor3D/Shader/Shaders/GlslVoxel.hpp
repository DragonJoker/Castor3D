/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslVoxel_H___
#define ___C3D_GlslVoxel_H___

#include "SdwModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>

namespace castor3d
{
	namespace shader
	{
		struct Voxel
			: public sdw::StructInstance
		{
			C3D_API Voxel( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr );
			C3D_API Voxel & operator=( Voxel const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			// Raw values
			sdw::UInt colorMask;
			sdw::UInt normalMask;

			static uint32_t constexpr DataSize = 8u;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;
		};

		Writer_Parameter( Voxel );
	}
}

#endif
