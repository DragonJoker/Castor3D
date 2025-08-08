/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_HiPass___
#define ___C3D_Bloom_HiPass___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#define Bloom_DebugHiPass 0

namespace Bloom
{
	class HiPass
	{
	public:
		HiPass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & sceneView
			, c3d::Extent2D size
			, uint32_t blurPassesCount
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor );

		c3d::Texture & getResult()
		{
			return m_result;
		}

	private:
		crg::FramePassGroup & m_graph;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		c3d::Texture m_result;
	};
}

#endif
