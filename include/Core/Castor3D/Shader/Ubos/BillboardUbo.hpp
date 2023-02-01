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
			: public sdw::StructInstanceHelperT< "C3D_BillboardData"
				, sdw::type::MemoryLayout::eStd430
				, sdw::Vec2Field< "dimensions" >
				, sdw::UIntField< "isSpherical" >
				, sdw::UIntField< "isFixedSize" > >
		{
			BillboardData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec3 getCameraRight( CameraData const & cameraData )const;
			C3D_API sdw::Vec3 getCameraUp( CameraData const & cameraData )const;
			C3D_API sdw::Float getWidth( CameraData const & cameraData )const;
			C3D_API sdw::Float getHeight( CameraData const & cameraData )const;

			auto dimensions()const { return getMember< "dimensions" >(); }
			auto isSpherical()const { return getMember< "isSpherical" >(); }
			auto isFixedSize()const { return getMember< "isFixedSize" >(); }
		};
	}
}

#define C3D_Billboard( writer, binding, set )\
	sdw::StorageBuffer billboard{ writer\
		, "C3D_Billboard"\
		, "c3d_billboard"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_billboardData = billboard.declMemberArray< castor3d::shader::BillboardData >( "d" );\
	billboard.end()

#define C3D_BillboardOpt( writer, binding, set, enable )\
	sdw::StorageBuffer billboard{ writer\
		, "C3D_Billboard"\
		, "c3d_billboard"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, enable };\
	auto c3d_billboardData = billboard.declMemberArray< castor3d::shader::BillboardData >( "d", enable );\
	billboard.end()

#endif
