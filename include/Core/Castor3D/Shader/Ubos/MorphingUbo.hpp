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
			C3D_API MorphingData & operator=( MorphingData const & rhs );

			C3D_API static ast::type::StructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Float morph( sdw::Float & lhs, sdw::Float const & rhs )const;
			C3D_API sdw::Vec2 morph( sdw::Vec2 & lhs, sdw::Vec2 const & rhs )const;
			C3D_API sdw::Vec3 morph( sdw::Vec3 & lhs, sdw::Vec3 const & rhs )const;
			C3D_API sdw::Vec4 morph( sdw::Vec4 & lhs, sdw::Vec4 const & rhs )const;
			C3D_API sdw::Vec4 morph( sdw::Vec4 & lhs, sdw::Vec3 const & rhs )const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

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
