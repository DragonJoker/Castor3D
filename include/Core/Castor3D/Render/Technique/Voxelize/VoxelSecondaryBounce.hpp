/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelSecondaryBounce_H___
#define ___C3D_VoxelSecondaryBounce_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>

namespace castor3d
{
	class VoxelSecondaryBounce
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\param[in]	voxels			The voxels buffer.
		 *\param[in]	voxelUbo		The voxelizer configuration UBO.
		 *\param[in]	firstBounce		The first bounce result.
		 *\param[in]	result			The resulting texture.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 *\param[in]	voxels			Le tampon de voxels.
		 *\param[in]	voxelUbo		L'UBO de configuration du voxelizer.
		 *\param[in]	firstBounce		Le résultat du premier rebond.
		 *\param[in]	result			La texture résultante.
		 */
		C3D_API VoxelSecondaryBounce( RenderDevice const & device
			, VoxelSceneData const & voxelConfig
			, ashes::Buffer< Voxel > const & voxels
			, VoxelizerUbo const & voxelUbo
			, TextureUnit const & firstBounce
			, TextureUnit const & result );
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

	private:
		VoxelSceneData const & m_vctConfig;
		RenderPassTimerSPtr m_timer;
		ashes::DescriptorSetLayoutPtr m_descriptorSetLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::DescriptorSetPoolPtr m_descriptorSetPool;
		ashes::DescriptorSetPtr m_descriptorSet;

		struct SecondaryBounce
		{
			SecondaryBounce( RenderDevice const & device
				, ashes::PipelineLayout const & pipelineLayout
				, ashes::DescriptorSet const & descriptorSet
				, ashes::Buffer< Voxel > const & voxels
				, VoxelizerUbo const & voxelUbo
				, TextureUnit const & firstBounce
				, TextureUnit const & result
				, RenderPassTimer & timer
				, uint32_t voxelGridSize );

			ShaderModule computeShader;
			ashes::ComputePipelinePtr pipeline;
			CommandsSemaphore commands;
		};

		SecondaryBounce m_secondaryBounce;
	};
}

#endif
