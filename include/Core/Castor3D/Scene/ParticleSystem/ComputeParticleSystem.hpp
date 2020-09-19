/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeParticleSystem_H___
#define ___C3D_ComputeParticleSystem_H___

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystemImpl.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleDeclaration.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ashespp/Buffer/Buffer.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/ComputePipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/Pipeline/PipelineVertexInputStateCreateInfo.hpp>
#include <ashespp/Sync/Fence.hpp>

namespace castor3d
{
	class ComputeParticleSystem
		: public ParticleSystemImpl
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	parent	Le système de particules parent.
		 */
		C3D_API explicit ComputeParticleSystem( ParticleSystem & parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ComputeParticleSystem();
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::initialise
		 */
		C3D_API bool initialise()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::cleanup
		 */
		C3D_API void cleanup()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::update
		 */
		C3D_API void update( CpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::update
		 */
		C3D_API uint32_t update( GpuUpdater & updater )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & name
			, ParticleFormat type
			, castor::String const & defaultValue )override;
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles.
		 *\param[in]	program	The shader program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules.
		 *\param[in]	program	Le programme shader.
		 */
		C3D_API void setUpdateProgram( ShaderProgramSPtr program );
		/**
		 *\~english
		 *\brief		Defines the workgroup sizes, as defined inside the compute shader.
		 *\param[in]	sizes	The sizes.
		 *\~french
		 *\brief		Définit les dimensions des groupes de travail, tels que définis dans le compute shader.
		 *\param[in]	sizes	Les dimensions.
		 */
		inline void setGroupSizes( castor::Point3i sizes )
		{
			m_worgGroupSizes = sizes;
		}
		/**
		 *\~english
		 *\return		\p false if the update program has not been set.
		 *\~french
		 *\return		\p false si le programme de mise à jour n'a pas été défini.
		 */
		inline bool hasUpdateProgram()const
		{
			return m_program != nullptr;
		}
		/**
		 *\~english
		 *\return		The update program.
		 *\~french
		 *\return		Le programme de mise à jour.
		 */
		inline ShaderProgram const & getUpdateProgram()const
		{
			return *m_program;
		}

	private:
		bool doInitialiseParticleStorage();
		bool doCreateRandomStorage();
		bool doInitialisePipeline();
		void doPrepareCommandBuffers();

	protected:
		struct Configuration
		{
			float deltaTime;
			float time;
			uint32_t maxParticleCount;
			uint32_t currentParticleCount;
			castor::Point3f emitterPosition;
		};

	protected:
		ParticleDeclaration m_inputs;
		ShaderProgramSPtr m_program;
		UniformBufferOffsetT< Configuration > m_ubo;
		std::array< ashes::BufferPtr< uint8_t >, 2 > m_particlesStorages;
		ashes::BufferPtr< uint32_t > m_generatedCountBuffer;
		ashes::BufferPtr< castor::Point4f > m_randomStorage;
		ashes::DescriptorSetLayoutPtr m_descriptorLayout;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::ComputePipelinePtr m_pipeline;
		ashes::DescriptorSetPoolPtr m_descriptorPool;
		std::array< ashes::DescriptorSetPtr, 2u > m_descriptorSets;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::FencePtr m_fence;
		uint32_t m_particlesCount{ 0u };
		castor::Point3i m_worgGroupSizes{ 128, 1, 1 };
		uint32_t m_in{ 0 };
		uint32_t m_out{ 1 };
	};
}

#endif
