/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct OverlayData
			: public sdw::StructInstance
		{
			C3D_API OverlayData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled );
			SDW_DeclStructInstance( C3D_API, OverlayData );

			C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );
			C3D_API static std::unique_ptr< sdw::Struct > declare( sdw::ShaderWriter & writer );

			C3D_API sdw::Vec2 getOverlaySize()const;
			C3D_API sdw::Vec2 modelToView( sdw::Vec2 const & pos )const;
			C3D_API sdw::UInt getMaterialIndex()const;

		private:
			using sdw::StructInstance::getMember;
			using sdw::StructInstance::getMemberArray;

		private:
			sdw::Vec4 m_positionRatio;
			sdw::IVec4 m_renderSizeIndex;
		};
	}
}

#define C3D_Overlay( writer, binding, set )\
	sdw::UniformBuffer overlay{ writer\
		, "C3D_Overlay"\
		, "c3d_overlay"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd140\
		, true };\
	auto c3d_overlayData = overlay.declMember< castor3d::shader::OverlayData >( "d" );\
	overlay.end()

#endif
