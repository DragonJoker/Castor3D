/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_DrawEdgesUbo_H___
#define ___C3DDE_DrawEdgesUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Shader/Ubos/Ubo.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstanceHelper.hpp>
#include <ShaderWriter/VecTypes/Vec2.hpp>

namespace draw_edges
{
	struct DrawEdgesUboConfiguration
	{
		int normalDepthWidth{ 1 };
		int objectWidth{ 1 };
	};

	struct DrawEdgesData
		: public sdw::StructInstanceHelperT< "C3D_DrawEdgesData"
			, sdw::type::MemoryLayout::eStd140
			, sdw::IntField< "normalDepthWidth" >
			, sdw::IntField< "objectWidth" > >
	{
	public:
		SDW_DeclStructInstance( , DrawEdgesData );

		DrawEdgesData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled )
			: StructInstanceHelperT{ writer, c3d::move( expr ), enabled }
			, normalDepthWidth{ StructInstanceHelperT::getMember< "normalDepthWidth" >() }
			, objectWidth{ StructInstanceHelperT::getMember< "objectWidth" >() }
		{
		}

	public:
		sdw::Int normalDepthWidth;
		sdw::Int objectWidth;

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;
	};

	class DrawEdgesUbo
		: public c3d::UboT< DrawEdgesUboConfiguration >
	{
	private:
		using Configuration = DrawEdgesUboConfiguration;

	public:
		explicit DrawEdgesUbo( c3d::RenderDevice const & device );

		void cpuUpdate( int normalDepthWidth
			, int objectWidth );

	public:
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;
	};
}

#define C3D_DrawEdges( writer, binding, set )\
	sdw::UniformBuffer drawEdges{ writer, draw_edges::DrawEdgesUbo::Buffer, uint32_t( binding ), uint32_t( set ) };\
	auto c3d_drawEdgesData = drawEdges.declMember< draw_edges::DrawEdgesData >( draw_edges::DrawEdgesUbo::Data );\
	drawEdges.end()

#endif
