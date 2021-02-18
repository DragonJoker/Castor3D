/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightPropagationPass_HPP___
#define ___C3D_LightPropagationPass_HPP___

#include "LightPropagationVolumesModule.hpp"

#include "Castor3D/Buffer/UniformBuffer.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightVolumePassResult.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueModule.hpp"
#include "Castor3D/Render/Passes/RenderGrid.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Named.hpp>
#include <CastorUtils/Graphics/Grid.hpp>

#include <ShaderAST/Shader.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineViewportStateCreateInfo.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>

namespace castor3d
{
	class LightPropagationPass
		: public RenderGrid
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device		The GPU device.
		 *\param[in]	prefix		The pass name's prefix.
		 *\param[in]	suffix		The pass name's suffix.
		 *\param[in]	occlusion	Enable occlusion or not.
		 *\param[in]	gridSize	The grid dimensions.
		 *\param[in]	blendMode	The blend mode.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	prefix		Le préfixe du nom de la passe.
		 *\param[in]	suffix		Le suffixe du nom de la passe.
		 *\param[in]	occlusion	Active l'occlusion ou pas.
		 *\param[in]	gridSize	Les dimensions de la grille.
		 *\param[in]	blendMode	Le mode de mélange.
		 */
		C3D_API LightPropagationPass( RenderDevice const & device
			, castor::String const & prefix
			, castor::String const & suffix
			, bool occlusion
			, uint32_t gridSize
			, BlendMode blendMode );
		C3D_API void registerPassIO( TextureUnit const * occlusion
			, LightVolumePassResult const & injection
			, LpvGridConfigUbo const & lpvConfigUbo
			, LightVolumePassResult const & accumulation
			, LightVolumePassResult const & propagate );
		/**
		*\~english
		*\brief
		*	Initialises the descriptor sets for all registered passes.
		*\~french
		*\brief
		*	Crée les descriptor sets pour toute les passes enregistrées.
		*/
		C3D_API void initialisePasses();
		/**
		 *\~english
		 *\brief		Renders the pass.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\param[in]	index	The pass index.
		 *\~french
		 *\brief		Dessine la passe.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 *\param[in]	index	L'indice de la passe.
		 */
		C3D_API ashes::Semaphore const & compute( ashes::Semaphore const & toWait
			, uint32_t index )const;
		C3D_API CommandsSemaphore getCommands( RenderPassTimer const & timer
			, uint32_t index )const;
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );

	private:
		using RenderGrid::registerPassInputs;

		void registerPassOutputs( ashes::ImageViewCRefArray const & outputs );

	private:
		Engine & m_engine;
		RenderPassTimerSPtr m_timer;

		ShaderModule m_vertexShader;
		ShaderModule m_geometryShader;
		ShaderModule m_pixelShader;
		ashes::RenderPassPtr m_renderPass;
		std::vector< ashes::ImageViewCRefArray > m_passesOutputs;
		ashes::FrameBufferPtrArray m_frameBuffers;
		std::vector< CommandsSemaphore > m_commands;
	};
}

#endif
