/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelRenderer_H___
#define ___C3D_VoxelRenderer_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

namespace castor3d
{
	class VoxelRenderer
	{
	public:
		C3D_API VoxelRenderer( RenderDevice const & device
			, UniformBufferOffsetT< VoxelizerUboConfiguration > const & voxelizerUbo
			, MatrixUbo & matrixUbo
			, TextureUnit const & voxels
			, ashes::ImageView target
			, uint32_t voxelGridSize );

		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );

	private:
		RenderSystem & m_renderSystem;
		RenderDevice const & m_device;
		TextureUnit m_depthBuffer;
		ashes::BufferPtr< castor::Point4f > m_vertexBuffer;
		ashes::BufferPtr< uint32_t > m_indexBuffer;

		struct VoxelToScreen : castor::Named
		{
			VoxelToScreen( RenderDevice const & device
				, UniformBufferOffsetT< VoxelizerUboConfiguration > const & voxelizerUbo
				, MatrixUbo & matrixUbo
				, TextureUnit const & voxels
				, ashes::BufferBase const & vertexBuffer
				, ashes::BufferBase const & indexBuffer
				, ashes::ImageView color
				, ashes::ImageView depth
				, uint32_t voxelGridSize );

			ashes::DescriptorSetLayoutPtr descriptorSetLayout;
			ashes::PipelineLayoutPtr pipelineLayout;
			ashes::RenderPassPtr renderPass;
			ShaderModule vertexShader;
			ShaderModule geometryShader;
			ShaderModule pixelShader;
			ashes::GraphicsPipelinePtr pipeline;
			ashes::FrameBufferPtr frameBuffer;
			ashes::DescriptorSetPoolPtr descriptorSetPool;
			ashes::DescriptorSetPtr descriptorSet;
			CommandsSemaphore commands;
		};

		VoxelToScreen m_voxelToScreen;
	};
}

#endif
