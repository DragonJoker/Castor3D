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
			, sdw::Vec4Field< "border" >
			, sdw::Vec4Field< "borderInnerUV" >
			, sdw::Vec4Field< "borderOuterUV" >
			, sdw::UIntField< "vertexOffset" >
			, sdw::UIntField< "materialId" >
			, sdw::UIntField< "borderPosition" >
			, sdw::UIntField< "textTexturingMode" >
			, sdw::UIntField< "textWordOffset" >
			, sdw::UIntField< "textLineOffset" >
			, sdw::FloatField< "textTopOffset" >
			, sdw::UIntField< "pad1" > >
	{
		OverlayData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		C3D_API sdw::Vec2 modelToView( sdw::Vec2 const & pos )const;
		
		auto vertexOffset()const { return getMember< "vertexOffset" >(); }
		auto materialId()const { return getMember< "materialId" >(); }
		auto size()const { return getMember< "size" >(); }
		auto uv()const { return getMember< "uv" >(); }

		auto border()const { return getMember< "border" >(); }
		auto borderInnerUV()const { return getMember< "borderInnerUV" >(); }
		auto borderOuterUV()const { return getMember< "borderOuterUV" >(); }
		auto borderPosition()const { return getMember< "borderPosition" >(); }

		auto textWordOffset()const { return getMember< "textWordOffset" >(); }
		auto textLineOffset()const { return getMember< "textLineOffset" >(); }
		auto textTopOffset()const { return getMember< "textTopOffset" >(); }
		auto textTexturingMode()const { return getMember< "textTexturingMode" >(); }

	private:
		auto position()const { return getMember< "position" >(); }
	};

	struct OverlaysIDs
		: public sdw::StructInstanceHelperT < "C3D_OverlaysIDs"
		, sdw::type::MemoryLayout::eStd430
		, sdw::UIntArrayField< "v", MaxWordsPerBuffer > >
	{
		OverlaysIDs( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, std::move( expr ), enabled }
		{
		}

		auto operator[]( sdw::UInt const & index )
		{
			return getMember< "v" >()[index];
		}
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

#define C3D_OverlaysIDs( writer, binding, set )\
	sdw::StorageBuffer c3d_overlaysIDsBuffer{ writer\
		, "C3D_OverlaysIDsBuffer"\
		, "c3d_overlaysIDsBuffer"\
		, uint32_t( binding )\
		, uint32_t( set )\
		, ast::type::MemoryLayout::eStd430\
		, true };\
	auto c3d_overlaysIDs = c3d_overlaysIDsBuffer.declMember< castor3d::shader::OverlaysIDs >( "d" );\
	c3d_overlaysIDsBuffer.end()

#endif
