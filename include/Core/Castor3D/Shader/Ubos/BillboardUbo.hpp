/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BillboardUbo_H___
#define ___C3D_BillboardUbo_H___

#include "UbosModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct BillboardData
			: public sdw::StructInstance
		{
			C3D_API BillboardData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, BillboardData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec3 getCameraRight( ProgramFlags programFlags
				, MatrixData const & matrixData )const;
			C3D_API sdw::Vec3 getCameraUp( ProgramFlags programFlags
				, MatrixData const & matrixData )const;
			C3D_API sdw::Float getWidth( ProgramFlags programFlags
				, SceneData const & sceneData )const;
			C3D_API sdw::Float getHeight( ProgramFlags programFlags
				, SceneData const & sceneData )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Vec2 m_dimensions;
		};
	}

	class BillboardUbo
	{
	public:
		using Configuration = BillboardUboConfiguration;

	public:
		C3D_API static castor::String const BufferBillboard;
		C3D_API static castor::String const BillboardData;
	};
}

#define UBO_BILLBOARD( writer, binding, set )\
	sdw::Ubo billboard{ writer\
		, castor3d::BillboardUbo::BufferBillboard\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_billboardData = billboard.declStructMember< castor3d::shader::BillboardData >( castor3d::BillboardUbo::BillboardData );\
	billboard.end()

#endif
