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
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render dimensions.
		 *\param[in]	scene			The rendered scene.
		 *\~french
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions du rendu.
		 *\param[in]	scene			La scène rendue.
		 */
		C3D_API VoxelBufferToTexture( RenderDevice const & device
			, VoxelizerUbo const & voxelizerUbo
			, ashes::Buffer< Voxel > const & voxels
			, TextureUnit const & result
			, uint32_t voxelGridSize );
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ShaderModule m_computeShader;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;
		CommandsSemaphore m_commands;
	};
}

#endif
