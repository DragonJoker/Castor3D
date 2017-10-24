/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ComputeParticleSystem_H___
#define ___C3D_ComputeParticleSystem_H___

#include "ParticleSystemImpl.hpp"

#include "Mesh/Buffer/BufferDeclaration.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Texture/TextureUnit.hpp"

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
		 *\param[in]	p_parent	The parent particle system.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_parent	Le système de particules parent.
		 */
		C3D_API explicit ComputeParticleSystem( ParticleSystem & p_parent );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~ComputeParticleSystem();
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::Initialise
		 */
		C3D_API bool initialise()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::Cleanup
		 */
		C3D_API void cleanup()override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::Update
		 */
		C3D_API uint32_t update( castor::Milliseconds const & p_time
			, castor::Milliseconds const & p_total )override;
		/**
		 *\copydoc		castor3d::ParticleSystemImpl::addParticleVariable
		 */
		C3D_API void addParticleVariable( castor::String const & p_name, ElementType p_type, castor::String const & p_defaultValue )override;
		/**
		 *\~english
		 *\brief		Defines the program used to update the particles.
		 *\param[in]	p_program	The program.
		 *\~french
		 *\brief		Définit le programme utilisé pour mettre à jour les particules.
		 *\param[in]	p_program	Le programme.
		 */
		C3D_API void setUpdateProgram( ShaderProgramSPtr p_program );
		/**
		 *\~english
		 *\return		\p false if the update program has not been set.
		 *\~french
		 *\return		\p false si le programme de mise à jour n'a pas été défini.
		 */
		inline bool hasUpdateProgram()const
		{
			return m_updateProgram != nullptr;
		}
		/**
		 *\~english
		 *\return		\p false if the update program has not been set.
		 *\~french
		 *\return		\p false si le programme de mise à jour n'a pas été défini.
		 */
		inline ShaderProgram const & getUpdateProgram()const
		{
			REQUIRE( m_updateProgram );
			return *m_updateProgram;
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
		//!\~english	The particle's elements description.
		//!\~french		La description des éléments d'une particule.
		BufferDeclaration m_inputs;
		//!\~english	The program used to update the transform buffer.
		//!\~french		Le programme utilisé pour mettre à jour le tampon de transformation.
		ShaderProgramSPtr m_updateProgram;
		//!\~english	The frame variable buffer holding particle system related variables.
		//!\~french		Le tampon de variables contenant les variables relatives au système de particules.
		UniformBuffer m_ubo;
		//!\~english	The UBO binding to the update program.
		//!\~french		Le binding de l'UBO avec le programme.
		UniformBufferBindingRPtr m_binding;
		//!\~english	The frame variable holding time since last update.
		//!\~french		La variable de frame contenant le temps écoulé depuis la dernière mise à jour.
		Uniform1fSPtr m_deltaTime;
		//!\~english	The frame variable holding total elapsed time.
		//!\~french		La variable de frame contenant le temps total écoulé.
		Uniform1fSPtr m_time;
		//!\~english	The frame variable holding the maximum particles count.
		//!\~french		La variable de frame contenant le nombre maximal de particules.
		Uniform1uiSPtr m_maxParticleCount;
		//!\~english	The frame variable holding the current particles count.
		//!\~french		La variable de frame contenant le nombre actuel de particules.
		Uniform1uiSPtr m_currentParticleCount;
		//!\~english	The frame variable holding the particles emitter position.
		//!\~french		La variable de frame contenant la position de l'émetteur de particules.
		Uniform3fSPtr m_emitterPosition;
		//!\~english	The frame variable holding the launches lifetime.
		//!\~french		La variable de frame contenant la durée de vie des lanceurs.
		Uniform1fSPtr m_launcherLifetime;
		//!\~english	The frame variable holding the shells lifetime.
		//!\~french		La variable de frame contenant la durée de vie des particules.
		Uniform1fSPtr m_shellLifetime;
		//!\~english	The frame variable holding the secondary shells lifetime.
		//!\~french		La variable de frame contenant la durée de vie des particules secondaires.
		Uniform1fSPtr m_secondaryShellLifetime;
		//!\~english	The storage buffers holding the particles.
		//!\~french		Les tampons de stockage contenant les particules.
		std::array< ShaderStorageBufferSPtr, 2 > m_particlesStorages;
		//!\~english	The storage buffer containing random directions.
		//!\~french		Le tampon de stockage contenant des directions aléatoires.
		ShaderStorageBufferSPtr m_randomStorage;
		//!\~english	The current particles count.
		//!\~french		Le nombre actuel de particules.
		uint32_t m_particlesCount{ 0u };
		//!\~english	The computing workgroup size.
		//!\~french		La taille d'un groupe de travail.
		uint32_t m_worgGroupSize{ 128u };
		//!\~english	The compute pipeline.
		//!\~french		Le pipeline de calcul.
		ComputePipelineUPtr m_computePipeline;
		//!\~english	The buffer containing the number of particles generated in one pass.
		//!\~french		Le tampon contenant le nombre de particules générées en une passe.
		AtomicCounterBufferSPtr m_generatedCountBuffer;
		//!\~english	The index of the input particle storage.
		//!\~french		L'indice du stockage de particules d'entrée.
		uint32_t m_in{ 0 };
		//!\~english	The index of the output particle storage.
		//!\~french		L'indice du stockage de particules de sortie.
		uint32_t m_out{ 1 };
	};
}

#endif
