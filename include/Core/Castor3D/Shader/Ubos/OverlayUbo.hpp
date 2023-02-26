/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d
{
	namespace shader
	{
		struct OverlayData
			: public sdw::StructInstanceHelperT < "C3D_OverlayData"
				, sdw::type::MemoryLayout::eStd140
				, sdw::Vec2Field< "position" >
				, sdw::Vec2Field< "ratio" >
				, sdw::UVec2Field< "refRenderSize" >
				, sdw::UIntField< "materialId" >
				, sdw::UIntField< "pad" > >
		{
			OverlayData( sdw::ShaderWriter & writer
				, ast::expr::ExprPtr expr
				, bool enabled )
				: StructInstanceHelperT{ writer, std::move( expr ), enabled }
			{
			}

			C3D_API sdw::Vec2 getOverlaySize()const;
			C3D_API sdw::Vec2 modelToView( sdw::Vec2 const & pos )const;
			C3D_API sdw::UInt getMaterialIndex()const;

		private:
			auto position()const { return getMember< "position" >(); }
			auto ratio()const { return getMember< "ratio" >(); }
			auto refRenderSize()const { return getMember< "refRenderSize" >(); }
			auto materialId()const { return getMember< "materialId" >(); }
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
