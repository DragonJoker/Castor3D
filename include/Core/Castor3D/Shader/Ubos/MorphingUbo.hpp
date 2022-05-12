/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MorphingUbo_H___
#define ___C3D_MorphingUbo_H___

#include "UbosModule.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/BaseTypes/Float.hpp>

namespace castor3d
{
	namespace shader
	{
		struct MorphingData
			: public sdw::StructInstance
		{
			C3D_API MorphingData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, MorphingData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 const & pos2
				, sdw::Vec3 & uvw
				, sdw::Vec3 const & uvw2 )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 const & pos2
				, sdw::Vec4 & nml
				, sdw::Vec3 const & nml2
				, sdw::Vec3 & uvw
				, sdw::Vec3 const & uvw2 )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 const & pos2
				, sdw::Vec4 & nml
				, sdw::Vec3 const & nml2
				, sdw::Vec4 & tan
				, sdw::Vec3 const & tan2
				, sdw::Vec3 & uvw
				, sdw::Vec3 const & uvw2 )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 const & pos2
				, sdw::Vec4 & uvuv
				, sdw::Vec4 const & uvuv2 )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 const & pos2
				, sdw::Vec4 & nml
				, sdw::Vec3 const & nml2
				, sdw::Vec4 & uvuv
				, sdw::Vec4 const & uvuv2 )const;
			C3D_API void morph( sdw::Vec4 & pos
				, sdw::Vec4 const & pos2
				, sdw::Vec4 & nml
				, sdw::Vec3 const & nml2
				, sdw::Vec4 & tan
				, sdw::Vec3 const & tan2
				, sdw::Vec4 & uvuv
				, sdw::Vec4 const & uvuv2 )const;

	public:
		C3D_API static castor::String const BufferName;
		C3D_API static castor::String const DataName;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

			sdw::Float morph( sdw::Float & lhs, sdw::Float const & rhs )const;
			sdw::Vec2 morph( sdw::Vec2 & lhs, sdw::Vec2 const & rhs )const;
			sdw::Vec3 morph( sdw::Vec3 & lhs, sdw::Vec3 const & rhs )const;
			sdw::Vec4 morph( sdw::Vec4 & lhs, sdw::Vec4 const & rhs )const;
			sdw::Vec4 morph( sdw::Vec4 & lhs, sdw::Vec3 const & rhs )const;

		private:
			sdw::Vec4 m_data;
			sdw::Float m_time;
		};
	}
}

#define C3D_Morphing( writer, binding, set, flags )\
	sdw::Ssbo morphing{ writer\
		, castor3d::shader::MorphingData::BufferName\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) };\
	auto c3d_morphingData = morphing.declMemberArray< castor3d::shader::MorphingData >( castor3d::shader::MorphingData::DataName\
		, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) );\
	morphing.end()

#endif
