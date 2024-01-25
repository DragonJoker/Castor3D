/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OverlayUbo_H___
#define ___C3D_OverlayUbo_H___

#include "UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <ShaderWriter/CompositeTypes/Function.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec4.hpp>

namespace castor3d::shader
{
	struct OverlayData
		: public sdw::StructInstanceHelperT < "C3D_OverlayData"
			, sdw::type::MemoryLayout::eStd430
			, sdw::Vec4Field< "uv" >
			, sdw::Vec4Field< "parentRect" >
			, sdw::Vec4Field< "scissorRect" >
			, sdw::Vec2Field< "relativePosition" >
			, sdw::Vec2Field< "relativeSize" >
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
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		auto vertexOffset()const { return getMember< "vertexOffset" >(); }
		auto materialId()const { return getMember< "materialId" >(); }
		auto parentRect()const { return getMember< "parentRect" >(); }
		auto scissorRect()const { return getMember< "scissorRect" >(); }
		auto relativeSize()const { return getMember< "relativeSize" >(); }
		auto relativePosition()const { return getMember< "relativePosition" >(); }
		auto uv()const { return getMember< "uv" >(); }

		auto border()const { return getMember< "border" >(); }
		auto borderInnerUV()const { return getMember< "borderInnerUV" >(); }
		auto borderOuterUV()const { return getMember< "borderOuterUV" >(); }
		auto borderPosition()const { return getMember< "borderPosition" >(); }

		auto textWordOffset()const { return getMember< "textWordOffset" >(); }
		auto textLineOffset()const { return getMember< "textLineOffset" >(); }
		auto textTopOffset()const { return getMember< "textTopOffset" >(); }
		auto textTexturingMode()const { return getMember< "textTexturingMode" >(); }
		/**
		 *\~english
		 *\brief		Crops a minimum boundary and its UV.
		 *\~french
		 *\brief		Découpe une borne minimum et son UV.
		 */
		C3D_API void cropMinValue( sdw::Float const & ssRelPosition
			, sdw::Float const & ssAbsParentSize
			, sdw::Float const & ssAbsBoundSize
			, sdw::Vec2 const & ssCropRange
			, sdw::Vec2 const & uvRange
			, sdw::Float ssRelBound
			, sdw::Float uv );
		/**
		 *\~english
		 *\brief		Crops a maximum boundary and its UV.
		 *\~french
		 *\brief		Découpe une borne maximum et son UV.
		 */
		C3D_API void cropMaxValue( sdw::Float const & ssRelPosition
			, sdw::Float const & ssAbsParentSize
			, sdw::Float const & ssAbsBoundSize
			, sdw::Vec2 const & ssCropRange
			, sdw::Vec2 const & uvRange
			, sdw::Float ssRelBound
			, sdw::Float uv );
		/**
		 *\~english
		 *\brief		Crops a minimum boundary and its UVs.
		 *\~french
		 *\brief		Découpe une borne minimum et son UVs.
		 */
		C3D_API void cropMinMinValue( sdw::Float const & ssRelPosition
			, sdw::Float const & ssAbsParentSize
			, sdw::Float const & ssAbsBoundSize
			, sdw::Float const & ssAbsCharSize
			, sdw::Vec2 const & ssCropRange
			, sdw::Vec2 const & texUvRange
			, sdw::Vec2 const & fontUvRange
			, sdw::Float ssRelBound
			, sdw::Float texUv
			, sdw::Float fontUv );
		/**
		 *\~english
		 *\brief		Crops a maximum boundary and its UVs.
		 *\~french
		 *\brief		Découpe une borne maximum et son UVs.
		 */
		C3D_API void cropMinMaxValue( sdw::Float const & ssRelPosition
			, sdw::Float const & ssAbsParentSize
			, sdw::Float const & ssAbsBoundSize
			, sdw::Float const & ssAbsCharSize
			, sdw::Vec2 const & ssCropRange
			, sdw::Vec2 const & texUvRange
			, sdw::Vec2 const & fontUvRange
			, sdw::Float ssRelBound
			, sdw::Float texUv
			, sdw::Float fontUv );
		/**
		 *\~english
		 *\brief		Crops a minimum boundary and its UVs.
		 *\~french
		 *\brief		Découpe une borne minimum et son UVs.
		 */
		C3D_API void cropMaxMinValue( sdw::Float const & ssRelPosition
			, sdw::Float const & ssAbsParentSize
			, sdw::Float const & ssAbsBoundSize
			, sdw::Float const & ssAbsCharSize
			, sdw::Vec2 const & ssCropRange
			, sdw::Vec2 const & texUvRange
			, sdw::Vec2 const & fontUvRange
			, sdw::Float ssRelBound
			, sdw::Float texUv
			, sdw::Float fontUv );
		/**
		 *\~english
		 *\brief		Crops a maximum boundary and its UVs.
		 *\~french
		 *\brief		Découpe une borne maximum et son UVs.
		 */
		C3D_API void cropMaxMaxValue( sdw::Float const & ssRelPosition
			, sdw::Float const & ssAbsParentSize
			, sdw::Float const & ssAbsBoundSize
			, sdw::Float const & ssAbsCharSize
			, sdw::Vec2 const & ssCropRange
			, sdw::Vec2 const & texUvRange
			, sdw::Vec2 const & fontUvRange
			, sdw::Float ssRelBound
			, sdw::Float texUv
			, sdw::Float fontUv );

	private:
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InOutFloat
			, sdw::InOutFloat > m_cropMinValue;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InOutFloat
			, sdw::InOutFloat > m_cropMaxValue;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InOutFloat
			, sdw::InOutFloat
			, sdw::InOutFloat > m_cropMinMinValue;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InOutFloat
			, sdw::InOutFloat
			, sdw::InOutFloat > m_cropMinMaxValue;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InOutFloat
			, sdw::InOutFloat
			, sdw::InOutFloat > m_cropMaxMinValue;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InOutFloat
			, sdw::InOutFloat
			, sdw::InOutFloat > m_cropMaxMaxValue;
	};

	struct OverlaysIDs
		: public sdw::StructInstanceHelperT < "C3D_OverlaysIDs"
		, sdw::type::MemoryLayout::eStd430
		, sdw::UIntArrayField< "v", MaxWordsPerBuffer > >
	{
		OverlaysIDs( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, castor::move( expr ), enabled }
		{
		}

		auto operator[]( sdw::UInt const & index )const
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
