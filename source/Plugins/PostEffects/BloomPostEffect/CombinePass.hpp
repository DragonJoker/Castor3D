/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Bloom_CombinePass_HPP___
#define ___C3D_Bloom_CombinePass_HPP___

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ashespp/Image/ImageView.hpp>

namespace Bloom
{
	class CombinePass
	{
	public:
		CombinePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture const & sceneView
			, c3d::Texture const & blurView
			, c3d::Texture & result
			, c3d::Extent2D const & size
			, uint32_t blurPassesCount
			, bool const * enabled
			, uint32_t const * passIndex );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	public:
		static c3d::MbString const CombineMapPasses;
		static c3d::MbString const CombineMapScene;

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
