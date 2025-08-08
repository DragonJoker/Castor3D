/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PbrBloom_DownsamplePass___
#define ___C3D_PbrBloom_DownsamplePass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace PbrBloom
{
	class DownsamplePass
	{
	public:
		DownsamplePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & sceneView
			, c3d::Vector< c3d::Texture > & resultImg
			, uint32_t passesCount
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor );

	private:
		crg::FramePassGroup & m_graph;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Vector< crg::RenderQuad * > m_quads;
	};
}

#endif
