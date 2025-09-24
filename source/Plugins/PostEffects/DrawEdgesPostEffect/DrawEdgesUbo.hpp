/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_DrawEdgesUbo_H___
#define ___C3DDE_DrawEdgesUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
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
	{
	private:
		using Configuration = DrawEdgesUboConfiguration;
		DrawEdgesUbo( DrawEdgesUbo const & ) = delete;
		DrawEdgesUbo & operator=( DrawEdgesUbo const & ) = delete;
		DrawEdgesUbo( DrawEdgesUbo && )noexcept = delete;
		DrawEdgesUbo & operator=( DrawEdgesUbo && )noexcept = delete;

	public:
		explicit DrawEdgesUbo( c3d::RenderDevice const & device );
		~DrawEdgesUbo();
		void cpuUpdate( int normalDepthWidth
			, int objectWidth );

		template< typename BindingT >
		void createPassBinding( crg::FramePass & pass
			, BindingT binding )const
		{
			m_ubo.createPassBinding( pass, binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		c3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		c3d::UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

	public:
		static const c3d::MbString Buffer;
		static const c3d::MbString Data;

	private:
		c3d::RenderDevice const & m_device;
		c3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_DrawEdges( writer, binding, set )\
	sdw::UniformBuffer drawEdges{ writer, draw_edges::DrawEdgesUbo::Buffer, uint32_t( binding ), uint32_t( set ) };\
	auto c3d_drawEdgesData = drawEdges.declMember< draw_edges::DrawEdgesData >( draw_edges::DrawEdgesUbo::Data );\
	drawEdges.end()

#endif
