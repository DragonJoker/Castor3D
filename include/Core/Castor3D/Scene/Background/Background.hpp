/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneBackground_H___
#define ___C3D_SceneBackground_H___

#include "BackgroundModule.hpp"

#include "Castor3D/PBR/IblTextures.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/Viewport.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Pipeline/GraphicsPipeline.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class SceneBackground
		: public castor::OwnedBy< Engine >
	{
	protected:
		struct Cube
		{
			struct Quad
			{
				struct Vertex
				{
					castor::Point3f position;
				};

				Vertex vertex[4];
			};

			Quad faces[6];
		};

	public:
		/**
		*\~english
		*\brief
		*	Constructor.
		*\param engine
		*	The engine.
		*\param scene
		*	The parent scene.
		*\param type
		*	The background type.
		*\~french
		*\brief
		*	Constructeur.
		*\param engine
		*	Le moteur.
		*\param scene
		*	La scène parente.
		*\param type
		*	Le type de fond.
		*/
		C3D_API explicit SceneBackground( Engine & engine
			, Scene & scene
			, BackgroundType type );
		/**
		*\~english
		*\brief
		*	Destructor.
		*\~french
		*\brief
		*	Destructeur.
		*/
		C3D_API virtual ~SceneBackground();
		/**
		*\~english
		*\brief
		*	Initialisation function.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\param[in] hdrConfigUbo
		*	The HDR configuration UBO.
		*\return
		*	\p true if ok.
		*\~french
		*\brief
		*	Fonction d'initialisation.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*\param[in] hdrConfigUbo
		*	L'UBO de configuration HDR.
		*\return
		*	\p true if ok.
		*/
		C3D_API bool initialise( ashes::RenderPass const & renderPass
			, HdrConfigUbo const & hdrConfigUbo );
		/**
		*\~english
		*\brief
		*	Cleanup function.
		*\~french
		*\brief
		*	Fonction de nettoyage.
		*/
		C3D_API void cleanup();
		/**
		*\~english
		*\brief
		*	Updates the background.
		*\param[in] camera
		*	The scene's camera.
		*\~french
		*\brief
		*	Met à jour le fond.
		*\param[in] camera
		*	La caméra de la scène.
		*/
		C3D_API void update( Camera const & camera );
		/**
		*\~english
		*\brief
		*	Records the commands used to draw the background.
		*\param[out] commandBuffer
		*	Receives the commands.
		*\param[in] size
		*	The rendering size.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\param[in] frameBuffer
		*	The frame buffer into which the background is drawn.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*\param[in] frameBuffer
		*	Le tampon d'images dans laquelle le fond est dessiné.
		*/
		C3D_API bool prepareFrame( ashes::CommandBuffer & commandBuffer
			, castor::Size const & size
			, ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer );
		/**
		*\~english
		*\brief
		*	Records the commands used to draw the background.
		*\param[out] commandBuffer
		*	Receives the commands.
		*\param[in] size
		*	The rendering size.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*/
		C3D_API bool prepareFrame( ashes::CommandBuffer & commandBuffer
			, castor::Size const & size
			, ashes::RenderPass const & renderPass );
		/**
		*\~english
		*\brief
		*	Records the commands used to draw the background.
		*\param[out] commandBuffer
		*	Receives the commands.
		*\param[in] size
		*	The rendering size.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\param[in] descriptorSet
		*	The descriptors set.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*\param[in] descriptorSet
		*	L'ensemble de descripteurs.
		*/
		C3D_API bool prepareFrame( ashes::CommandBuffer & commandBuffer
			, castor::Size const & size
			, ashes::RenderPass const & renderPass
			, ashes::DescriptorSet const & uboDescriptorSet
			, ashes::DescriptorSet const & texDescriptorSet )const;
		/**
		*\~english
		*\return
		*	Initialises the descriptor set.
		*\param[in] matrixUbo
		*	The scene matrices UBO.
		*\param[in] modelMatrixUbo
		*	The model matrices UBO.
		*\param[in] hdrConfigUbo
		*	The HDR configuration UBO.
		*\param[out] descriptorSet
		*	Receives the descriptors.
		*\~french
		*\brief
		*	Initialise l'ensemble de descripteurs.
		*\param[in] matrixUbo
		*	L'UBO des matrices de la scène.
		*\param[in] modelMatrixUbo
		*	L'UBO des matrices de modèle.
		*\param[in] hdrConfigUbo
		*	L'UBO de configuration HDR.
		*\param[out] descriptorSet
		*	Reçoit les descripteurs.
		*/
		C3D_API virtual void initialiseDescriptorSets( MatrixUbo const & matrixUbo
			, ModelMatrixUbo const & modelMatrixUbo
			, HdrConfigUbo const & hdrConfigUbo
			, ashes::DescriptorSet & uboDescriptorSet
			, ashes::DescriptorSet & texDescriptorSet )const;
		/**
		 *\~english
		 *\brief		Starts the CPU timer, resets GPU time.
		 *\~french
		 *\brief		Démarre le timer CPU, réinitialise le temps GPU.
		 */
		C3D_API RenderPassTimerBlock start();
		/**
		 *\~english
		 *\brief		Notifies the given pass render.
		 *\~french
		 *\brief		Notifie le rendu de la passe donnée.
		 */
		C3D_API void notifyPassRender();
		/**
		*\~english
		*\return
		*	Notifies a change to apply on GPU for the background.
		*\~french
		*\brief
		*	Notifie un changement affectant le GPU pour le fond.
		*/
		C3D_API void notifyChanged();
		/**
		*\~english
		*\brief
		*	Visitor acceptance function.
		*\param	visitor
		*	The visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de visiteur.
		*\param visitor
		*	Le visiteur.
		*/
		C3D_API virtual void accept( BackgroundVisitor & visitor )const = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_semaphore );
			return *m_semaphore;
		}

		inline Scene const & getScene()const
		{
			return m_scene;
		}

		inline Scene & getScene()
		{
			return m_scene;
		}

		inline BackgroundType getType()const
		{
			return m_type;
		}

		inline TextureLayout const & getTexture()const
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline TextureLayout & getTexture()
		{
			CU_Require( m_texture );
			return *m_texture;
		}

		inline ashes::Image const & getImage()const
		{
			CU_Require( m_texture );
			return m_texture->getTexture();
		}

		inline ashes::ImageView const & getView()const
		{
			CU_Require( m_texture );
			return m_texture->getDefaultView();
		}

		inline bool hasIbl()const
		{
			return m_ibl != nullptr;
		}

		inline IblTextures const & getIbl()const
		{
			CU_Require( m_ibl );
			return *m_ibl;
		}

		inline ashes::PipelineLayout const & getPipelineLayout()const
		{
			CU_Require( m_pipelineLayout );
			return *m_pipelineLayout;
		}

		inline ashes::GraphicsPipeline const & getPipeline()const
		{
			CU_Require( m_pipeline );
			return *m_pipeline;
		}

		inline ashes::DescriptorSetLayout const & getUboDescriptorLayout()const
		{
			CU_Require( m_uboDescriptorLayout );
			return *m_uboDescriptorLayout;
		}
		
		inline ashes::DescriptorSetLayout const & getTexDescriptorLayout()const
		{
			CU_Require( m_texDescriptorLayout );
			return *m_texDescriptorLayout;
		}

		inline ashes::VertexBuffer< Cube > const & getVertexBuffer()const
		{
			CU_Require( m_vertexBuffer );
			return *m_vertexBuffer;
		}

		inline ashes::Buffer< uint16_t > const & getIndexBuffer()const
		{
			CU_Require( m_indexBuffer );
			return *m_indexBuffer;
		}
		/**@}*/

	private:
		/**
		*\~english
		*\brief
		*	Records the commands used to draw the background.
		*\param[out] commandBuffer
		*	Receives the commands.
		*\param[in] size
		*	The rendering size.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\param[in] descriptorSet
		*	The descriptors set.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] size
		*	Les dimensions de rendu.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*\param[in] descriptorSet
		*	L'ensemble de descripteurs.
		*/
		void doPrepareFrame( ashes::CommandBuffer & commandBuffer
			, castor::Size const & size
			, ashes::RenderPass const & renderPass
			, ashes::DescriptorSet const & uboDescriptorSet
			, ashes::DescriptorSet const & texDescriptorSet )const;
		/**
		*\~english
		*\return
		*	The shader program used to render the background.
		*\~french
		*\brief
		*	Le programme shader utilisé pour dessiner le fond.
		*/
		virtual ashes::PipelineShaderStageCreateInfoArray doInitialiseShader();
		/**
		*\~english
		*\return
		*	Initialises the descriptor layout.
		*\~french
		*\brief
		*	Initialise le layout de descripteurs.
		*/
		virtual void doInitialiseDescriptorLayouts();
		/**
		*\~english
		*\brief
		*	Initialisation function.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\return
		*	\p true if ok.
		*\~french
		*\brief
		*	Fonction d'initialisation.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*\return
		*	\p true if ok.
		*/
		C3D_API virtual bool doInitialise( ashes::RenderPass const & renderPass ) = 0;
		/**
		*\~english
		*\brief
		*	Cleanup function.
		*\~french
		*\brief
		*	Fonction de nettoyage.
		*/
		C3D_API virtual void doCleanup() = 0;
		/**
		*\~english
		*\brief
		*	Updates the background.
		*\param[in] camera
		*	The scene's camera.
		*\~french
		*\brief
		*	Met à jour le fond.
		*\param[in] camera
		*	La caméra de la scène.
		*/
		C3D_API virtual void doUpdate( Camera const & camera ) = 0;

	public:
		OnBackgroundChanged onChanged;

	private:
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( ashes::PipelineShaderStageCreateInfoArray program
			, ashes::RenderPass const & renderPass
			, HdrConfigUbo const & hdrConfigUbo );

	protected:
		Scene & m_scene;
		BackgroundType m_type;
		std::atomic_bool m_initialised{ false };
		bool m_hdr{ true };
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Matrix4x4f m_mtxModel;
		ashes::SemaphorePtr m_semaphore;
		ashes::DescriptorSetLayoutPtr m_uboDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_uboDescriptorPool;
		ashes::DescriptorSetPtr m_uboDescriptorSet;
		ashes::DescriptorSetLayoutPtr m_texDescriptorLayout;
		ashes::DescriptorSetPoolPtr m_texDescriptorPool;
		ashes::DescriptorSetPtr m_texDescriptorSet;
		ashes::PipelineLayoutPtr m_pipelineLayout;
		ashes::GraphicsPipelinePtr m_pipeline;
		ashes::VertexBufferPtr< Cube > m_vertexBuffer;
		ashes::BufferPtr< uint16_t > m_indexBuffer;
		RenderPassTimerSPtr m_timer;
		TextureLayoutSPtr m_texture;
		SamplerWPtr m_sampler;
		std::unique_ptr< IblTextures > m_ibl;

		C3D_API static uint32_t constexpr UboSetIdx = 0u;
		C3D_API static uint32_t constexpr MtxUboIdx = 0u;
		C3D_API static uint32_t constexpr MdlMtxUboIdx = 1u;
		C3D_API static uint32_t constexpr HdrCfgUboIdx = 2u;
		C3D_API static uint32_t constexpr TexSetIdx = 1u;
		C3D_API static uint32_t constexpr SkyBoxImgIdx = 0u;
	};
}

#endif
