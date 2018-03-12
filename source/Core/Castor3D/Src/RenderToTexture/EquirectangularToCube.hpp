/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EquirectangularToCube_H___
#define ___C3D_EquirectangularToCube_H___
#pragma once

#include "Castor3DPrerequisites.hpp"

#include <Buffer/UniformBuffer.hpp>
#include <Buffer/VertexBuffer.hpp>
#include <Command/CommandBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Image/Sampler.hpp>
#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>

#include <array>

namespace castor3d
{
	class EquirectangularToCube
	{
	public:
		EquirectangularToCube( renderer::Texture const & equiRectangular
			, RenderSystem & renderSystem
			, renderer::Texture const & texture );

		void render();
		void render( renderer::CommandBuffer & commandBuffer );

	private:
		struct FacePipeline
		{
			renderer::TextureViewPtr view;
			renderer::RenderPassPtr renderPass;
			renderer::FrameBufferPtr frameBuffer;
			renderer::PipelinePtr pipeline;
			renderer::DescriptorSetPtr descriptorSet;
		};

		renderer::Device const & m_device;
		renderer::Texture const & m_target;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::TexturePtr m_texture;
		renderer::TextureViewPtr m_view;
		renderer::SamplerPtr m_sampler;
		renderer::UniformBufferPtr< castor::Matrix4x4f > m_matrixUbo;
		renderer::VertexBufferPtr< castor::Point4f > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		std::array< FacePipeline, 6u > m_faces;
	};
}

#endif
