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
		C3D_API uint32_t update( castor::Milliseconds const & time
			, castor::Milliseconds const & total )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & name, ParticleFormat type, castor::String const & defaultValue )override;
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles.
		 *\param[in]	program	The program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules.
		 *\param[in]	program	Le programme.
		 */
		C3D_API void setUpdateProgram( renderer::ShaderStageState const & program );
		/**
		 *\~english
		 *\return		\p false if the update program has not been set.
		 *\~french
		 *\return		\p false si le programme de mise à jour n'a pas été défini.
		 */
		inline bool hasUpdateProgram()const
		{
			return m_updateProgram.module != nullptr;
		}

	private:
		/**
		 *\~english
		 *\brief		Initialises the shader storage buffer holding the particles data.
		 *\return		\p false on failure.
		 *\~french
		 *\brief		Initialise le tampon de stockage shader contenant les données des particules.
		 *\return		\p false en cas d'échec.
		 */
		bool doInitialiseParticleStorage();
		/**
		 *\~english
		 *\brief		Creates the storage buffer containing random values.
		 *\return		\p false on failure.
		 *\~french
		 *\brief		Crée le tampon de stockage contenant des valeurs aléatoires.
		 *\return		\p false en cas d'échec.
		 */
		bool doCreateRandomStorage();
		/**
		 *\~english
		 *\brief		Creates the pipeline used to run the shader.
		 *\return		\p false on failure.
		 *\~french
		 *\brief		Crée le pipeline utilisé pour lancer le shader.
		 *\return		\p false en cas d'échec.
		 */
		bool doInitialisePipeline();

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
		renderer::ShaderStageState m_updateProgram;
		renderer::UniformBufferPtr< Configuration > m_ubo;
		std::array< renderer::BufferPtr< uint8_t >, 2 > m_particlesStorages;
		renderer::BufferPtr< castor::Point4f > m_randomStorage;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::ComputePipelinePtr m_pipeline;
		uint32_t m_particlesCount{ 0u };
		uint32_t m_worgGroupSize{ 128u };
		renderer::BufferPtr< uint32_t > m_generatedCountBuffer;
		uint32_t m_in{ 0 };
		uint32_t m_out{ 1 };
	};
}

#endif
