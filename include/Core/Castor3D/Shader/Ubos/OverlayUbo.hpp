/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	struct OverlayData
		: public sdw::StructInstanceHelperT < "C3D_OverlayData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec4Field< "uv" >
			, sdw::Vec2Field< "position" >
			, sdw::Vec2Field< "size" >
			, sdw::UIntField< "materialId" >
			, sdw::UIntField< "pad1" >
			, sdw::UIntField< "pad2" >
			, sdw::UIntField< "pad3" > >
	{
		OverlayData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API sdw::Vec2 modelToView( sdw::Vec2 const & pos )const;

		auto materialId()const { return getMember< "materialId" >(); }
		auto size()const { return getMember< "size" >(); }
		auto uv()const { return getMember< "uv" >(); }

	private:
		auto position()const { return getMember< "position" >(); }
	};
}

#define C3D_Overlays( writer, binding, set )\
	sdw::StorageBuffer c3d_overlaysDataBuffer{ writer\
		, "C3D_OverlaysDataBuffer"\
		, "c3d_overlaysDataBuffer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_overlaysData = c3d_overlaysDataBuffer.declMemberArray< castor3d::shader::OverlayData >( "d" );\
	c3d_overlaysDataBuffer.end()

#endif
