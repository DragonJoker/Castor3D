/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_DrawEdgesUbo_H___
#define ___C3DDE_DrawEdgesUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

#include <ShaderWriter/BaseTypes/Int.hpp>
#include <ShaderWriter/CompositeTypes/StructInstance.hpp>
#include <ShaderWriter/VecTypes/Vec2.hpp>

namespace draw_edges
{
	struct DrawEdgesUboConfiguration
	{
		int normalDepthWidth{ 1 };
		int objectWidth{ 1 };
	};

	struct DrawEdgesData
		: public sdw::StructInstance
	{
	public:
		DrawEdgesData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( , DrawEdgesData );

		static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

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

	public:
		explicit DrawEdgesUbo( castor3d::RenderDevice const & device );
		~DrawEdgesUbo();
		void cpuUpdate( int normalDepthWidth
			, int objectWidth );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, Buffer + "Cfg", binding );
		}

		void createSizedBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & layoutBinding )const
		{
			return m_ubo.createSizedBinding( descriptorSet, layoutBinding );
		}

		castor3d::UniformBufferOffsetT< Configuration > const & getUbo()const
		{
			return m_ubo;
		}

		castor3d::UniformBufferOffsetT< Configuration > & getUbo()
		{
			return m_ubo;
		}

	public:
		static const castor::String Buffer;
		static const castor::String Data;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define C3D_DrawEdges( writer, binding, set )\
	sdw::Ubo drawEdges{ writer, draw_edges::DrawEdgesUbo::Buffer, binding, set };\
	auto c3d_drawEdgesData = drawEdges.declMember< draw_edges::DrawEdgesData >( draw_edges::DrawEdgesUbo::Data );\
	drawEdges.end()

#endif
