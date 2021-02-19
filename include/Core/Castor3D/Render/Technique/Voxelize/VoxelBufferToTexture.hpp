/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelBufferToTexture_H___
#define ___C3D_VoxelBufferToTexture_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

namespace castor3d
{
	class VoxelBufferToTexture
	{
	public:
		/**
		 *\~english
		 *\param[in]	device		The GPU device.
		 *\param[in]	vctConfig	The voxelizer configuration.
		 *\param[in]	voxels		The voxels buffer.
		 *\param[in]	result		The resulting texture.
		 *\~french
		 *\param[in]	device		Le device GPU.
		 *\param[in]	vctConfig	La configuration du voxelizer.
		 *\param[in]	voxels		Le tampon de voxels.
		 *\param[in]	result		La texture résultante.
		 */
		C3D_API VoxelBufferToTexture( RenderDevice const & device
			, VoxelSceneData const & vctConfig
			, ashes::Buffer< Voxel > const & voxels
			, TextureUnit const & result );
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 *\param[in]	device	Le device GPU.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		RenderDevice const & m_device;
		VoxelSceneData const & m_vctConfig;
		ashes::Buffer< Voxel > const & m_voxels;
		TextureUnit const & m_result;
		RenderPassTimerSPtr m_timer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		struct Pipeline
		{
			Pipeline( RenderDevice const & device
				, ashes::PipelineLayout const & pipelineLayout
				, ashes::DescriptorSet const & descriptorSet
				, ashes::Buffer< Voxel > const & voxels
				, TextureUnit const & result
				, RenderPassTimer & timer
				, uint32_t voxelGridSize
				, bool temporalSmoothing
				, bool secondaryBounce );

			ShaderModule computeShader;
			ashes::ComputePipelinePtr pipeline;
			CommandsSemaphore commands;
		};
		std::array< std::unique_ptr< Pipeline >, 4u > m_pipelines;

	private:
		Pipeline & getPipeline();
	};
}

#endif
