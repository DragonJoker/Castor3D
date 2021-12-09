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
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

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

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

			sdw::Float morph( sdw::Float & lhs, sdw::Float const & rhs )const;
			sdw::Vec2 morph( sdw::Vec2 & lhs, sdw::Vec2 const & rhs )const;
			sdw::Vec3 morph( sdw::Vec3 & lhs, sdw::Vec3 const & rhs )const;
			sdw::Vec4 morph( sdw::Vec4 & lhs, sdw::Vec4 const & rhs )const;
			sdw::Vec4 morph( sdw::Vec4 & lhs, sdw::Vec3 const & rhs )const;

		private:
			sdw::Float m_time;
		};
	}

	class MorphingUbo
	{
	public:
		using Configuration = MorphingUboConfiguration;

	public:
		C3D_API static castor::String const BufferMorphing;
		C3D_API static castor::String const MorphingData;
	};
}

#define UBO_MORPHING( writer, binding, set, flags )\
	sdw::Ubo morphing{ writer\
		, castor3d::MorphingUbo::BufferMorphing\
		, binding\
		, set\
		, ast::type::MemoryLayout::eStd140\
		, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) };\
	auto c3d_morphingData = morphing.declStructMember< castor3d::shader::MorphingData >( castor3d::MorphingUbo::MorphingData, checkFlag( flags, castor3d::ProgramFlag::eMorphing ) );\
	morphing.end()

#endif
