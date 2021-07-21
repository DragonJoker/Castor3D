/*
See LICENSE file in root folder
*/
#ifndef ___C3DDE_DrawEdgesUbo_H___
#define ___C3DDE_DrawEdgesUbo_H___

#include <Castor3D/Castor3DModule.hpp>
#include <Castor3D/Buffer/UniformBufferOffset.hpp>

namespace draw_edges
{
	struct DrawEdgesUboConfiguration
	{
		float normalDepthWidth;
		float objectWidth;
	};

	class DrawEdgesUbo
	{
	private:
		using Configuration = DrawEdgesUboConfiguration;

	public:
		explicit DrawEdgesUbo( castor3d::RenderDevice const & device );
		~DrawEdgesUbo();
		void cpuUpdate( float normalDepthWidth
			, float objectWidth );

		void createPassBinding( crg::FramePass & pass
			, uint32_t binding )const
		{
			m_ubo.createPassBinding( pass, "FxaaCfg", binding );
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
		static const castor::String Name;
		static const castor::String NormalDepthWidth;
		static const castor::String ObjectWidth;

	private:
		castor3d::RenderDevice const & m_device;
		castor3d::UniformBufferOffsetT< Configuration > m_ubo;
	};
}

#define UBO_DRAW_EDGES( writer, binding, set )\
	sdw::Ubo drawEdges{ writer, draw_edges::DrawEdgesUbo::Name, binding, set };\
	auto c3d_normalDepthWidth = drawEdges.declMember< sdw::Float >( draw_edges::DrawEdgesUbo::NormalDepthWidth );\
	auto c3d_objectWidth = drawEdges.declMember< sdw::Float >( draw_edges::DrawEdgesUbo::ObjectWidth );\
	drawEdges.end()

#endif
