/*
See LICENSE file in root folder
*/
#ifndef ___C3DSMAA_BlendingWeightCalculation_H___
#define ___C3DSMAA_BlendingWeightCalculation_H___

#include "SmaaPostEffect/SmaaConfig.hpp"

#include <Castor3D/Buffer/UniformBuffer.hpp>
#include <Castor3D/Render/PostEffect/PostEffect.hpp>

#include <ShaderAST/Shader.hpp>

namespace smaa
{
	class BlendingWeightCalculation
	{
	public:
		BlendingWeightCalculation( crg::FramePass const & previousPass
			, castor3d::RenderTarget & renderTarget
			, castor3d::RenderDevice const & device
			, crg::ImageViewId const & edgeDetectionView
			, crg::ImageViewId const & stencilView
			, SmaaConfig const & config );
		~BlendingWeightCalculation();
		void cpuUpdate( castor::Point4f const & subsampleIndices );
		void accept( castor3d::PipelineVisitorBase & visitor );

		crg::ImageViewId const & getResult()const
		{
			return m_result.wholeViewId;
		}

		crg::FramePass const & getPass()const
		{
			return m_pass;
		}

	private:
		castor3d::RenderDevice const & m_device;
		crg::FrameGraph & m_graph;
		castor3d::UniformBufferOffsetT< castor::Point4f > m_ubo;
		crg::ImageId m_areaImg;
		crg::ImageViewId m_areaView;
		crg::ImageId m_searchImg;
		crg::ImageViewId m_searchView;
		castor3d::Texture m_result;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_stages;
		crg::FramePass & m_pass;
	};
}

#endif
