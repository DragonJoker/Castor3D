/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightStreaks_KawasePass_HPP___
#define ___C3D_LightStreaks_KawasePass_HPP___

#include "KawaseUbo.hpp"

#include <Castor3D/Render/Passes/GaussianBlur.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

namespace light_streaks
{
	class KawasePass
	{
	public:
		KawasePass( crg::FramePassGroup & graph
			, c3d::RenderDevice const & device
			, c3d::Texture & hiViews
			, c3d::Texture & kawaseViews
			, KawaseUbo const & kawaseUbo
			, c3d::Extent2D dimensions
			, bool const * enabled );
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

	public:
		static constexpr uint32_t MaxCoefficients{ 64u };

	private:
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
