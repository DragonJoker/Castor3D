/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslCullData_H___
#define ___C3D_GlslCullData_H___

#include "SdwModule.hpp"

#include <ShaderWriter/BaseTypes/UInt.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	struct CullData
		: public sdw::StructInstance
	{
		C3D_API CullData( sdw::ShaderWriter & writer
			, sdw::expr::ExprPtr expr
			, bool enabled = true );

		SDW_DeclStructInstance( C3D_API, CullData );

		C3D_API static sdw::type::BaseStructPtr makeType( sdw::type::TypesCache & cache );

	public:
		sdw::Vec4 sphere;
		sdw::Vec4 cone;

	private:
		mutable sdw::Function< sdw::Vec4
			, sdw::InInt > m_unpackCone;
	};

	Writer_Parameter( CullData );
}

#endif
