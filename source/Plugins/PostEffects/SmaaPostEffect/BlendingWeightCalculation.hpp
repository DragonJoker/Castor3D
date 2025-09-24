/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_BlendingWeightCalculation_H___
#define ___C3DSMAA_BlendingWeightCalculation_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class SmaaUbo;

	class BlendingWeightCalculation
	{
	public:
		BlendingWeightCalculation( crg::FramePassGroup & graph
			, c3d::RenderTarget & renderTarget
			, c3d::RenderDevice const & device
			, SmaaUbo const & ubo
			, c3d::Texture const & edgeDetectionView
			, c3d::Texture const & stencilView
			, bool const * enabled );
		~BlendingWeightCalculation();
		void accept( c3d::ConfigurationVisitorBase & visitor )const;

		c3d::Texture const & getResult()const
		{
			return m_result;
		}

	private:
		c3d::RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		crg::ResourcesCache & m_resources;
		c3d::Extent3D m_extent;
		crg::ImageViewId m_areaView;
		crg::ImageViewId m_searchView;
		c3d::Texture m_result;
		c3d::ProgramModule m_shader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
	};
}

#endif
