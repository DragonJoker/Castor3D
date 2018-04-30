/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeParticleSystem_H___
#define ___C3D_ComputeParticleSystem_H___

#include "ParticleSystemImpl.hpp"

#include "Scene/ParticleSystem/ParticleDeclaration.hpp"
#include "Texture/TextureUnit.hpp"

#include <Pipeline/ShaderStageState.hpp>
#include <Pipeline/VertexLayout.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		21/11/2016
	\~english
	\brief		Compute shader Particle system implementation.
	\~french
	\brief		Implémentation d'un système de particules utilisant un compute shader.
	*/
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
		C3D_API uint32_t update( RenderPassTimer & timer
			, castor::Milliseconds const & time
			, castor::Milliseconds const & total )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & name, ParticleFormat type, castor::String const & defaultValue )override;
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles.
		 *\param[in]	programFile	The program file path.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules.
		 *\param[in]	programFile	Le chemin d'accès au fichier du programme.
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
		renderer::UniformBufferPtr< Configuration > m_ubo;
		std::array< renderer::BufferPtr< uint8_t >, 2 > m_particlesStorages;
		renderer::BufferPtr< uint32_t > m_generatedCountBuffer;
		renderer::BufferPtr< castor::Point4f > m_randomStorage;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::ComputePipelinePtr m_pipeline;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		std::array< renderer::DescriptorSetPtr, 2u > m_descriptorSets;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::FencePtr m_fence;
		uint32_t m_particlesCount{ 0u };
		castor::Point3i m_worgGroupSizes{ 128, 1, 1 };
		uint32_t m_in{ 0 };
		uint32_t m_out{ 1 };
	};
}

#endif
