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
#include <Utils/Mat4.hpp>

#include <array>

namespace castor3d
{
	class EquirectangularToCube
	{
	public:
		EquirectangularToCube( TextureLayout const & equiRectangular
			, RenderSystem & renderSystem
			, TextureLayout const & target
			, float gamma );

		void render();
		void render( renderer::CommandBuffer & commandBuffer );

	private:
		struct FacePipeline
		{
			renderer::TextureView const * view;
			renderer::FrameBufferPtr frameBuffer;
			renderer::PipelinePtr pipeline;
			renderer::DescriptorSetPtr descriptorSet;
		};

		renderer::Device const & m_device;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::TextureView const & m_view;
		renderer::SamplerPtr m_sampler;
		renderer::UniformBufferPtr< renderer::Mat4 > m_matrixUbo;
		renderer::VertexBufferPtr< renderer::Vec4 > m_vertexBuffer;
		renderer::VertexLayoutPtr m_vertexLayout;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::RenderPassPtr m_renderPass;
		std::array< FacePipeline, 6u > m_faces;
	};
}

#endif
