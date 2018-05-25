/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneBackground_H___
#define ___C3D_SceneBackground_H___

#include "Render/Viewport.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "PBR/IblTextures.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Buffer/VertexBuffer.hpp>
#include <Descriptor/DescriptorSet.hpp>
#include <Descriptor/DescriptorSetLayout.hpp>
#include <Descriptor/DescriptorSetPool.hpp>
#include <Pipeline/Pipeline.hpp>
#include <Pipeline/PipelineLayout.hpp>
#include <Sync/Semaphore.hpp>

namespace castor3d
{
	class SceneBackground;
	using BackgroundChangedFunc = std::function< void( SceneBackground const & ) >;
	using OnBackgroundChanged = castor::Signal< BackgroundChangedFunc >;
	using OnBackgroundChangedConnection = OnBackgroundChanged::connection;
	/**
	*\author
	*	Sylvain DOREMUS
	*\version
	*	0.9.0
	*\date
	*	21/05/2016
	*\~english
	*\brief
	*	Scene background base class.
	*\~french
	*\brief
	*	Classe de base du fond d'une scène.
	*/
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
		*\~french
		*\brief
		*	Constructeur.
		*\param engine
		*	Le moteur.
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
		C3D_API bool initialise( renderer::RenderPass const & renderPass
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
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\param[in] frameBuffer
		*	The frame buffer into which the background is drawn.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*\param[in] frameBuffer
		*	Le tampon d'images dans laquelle le fond est dessiné.
		*/
		C3D_API bool prepareFrame( renderer::CommandBuffer & commandBuffer
			, castor::Size const & size
			, renderer::RenderPass const & renderPass
			, renderer::FrameBuffer const & frameBuffer );
		/**
		*\~english
		*\brief
		*	Records the commands used to draw the background.
		*\param[out] commandBuffer
		*	Receives the commands.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*/
		C3D_API bool prepareFrame( renderer::CommandBuffer & commandBuffer
			, castor::Size const & size
			, renderer::RenderPass const & renderPass );
		/**
		*\~english
		*\brief
		*	Records the commands used to draw the background.
		*\param[out] commandBuffer
		*	Receives the commands.
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*/
		C3D_API bool prepareFrame( renderer::CommandBuffer & commandBuffer
			, castor::Size const & size
			, renderer::RenderPass const & renderPass
			, renderer::DescriptorSet const & descriptorSet )const;
		/**
		*\~english
		*\return
		*	Initialises the descriptor set.
		*\~french
		*\brief
		*	Initialise l'ensemble de descripteurs.
		*/
		C3D_API virtual void initialiseDescriptorSet( MatrixUbo const & matrixUbo
			, ModelMatrixUbo const & modelMatrixUbo
			, HdrConfigUbo const & hdrConfigUbo
			, renderer::DescriptorSet & descriptorSet )const;
		/**
		 *\~english
		 *\brief		Starts the CPU timer, resets GPU time.
		 *\~french
		 *\brief		Démarre le timer CPU, réinitialise le temps GPU.
		 */
		C3D_API void start();
		/**
		 *\~english
		 *\brief		Notifies the given pass render.
		 *\~french
		 *\brief		Notifie le rendu de la passe donnée.
		 */
		C3D_API void notifyPassRender();
		/**
		 *\~english
		 *\brief		Stops the CPU timer.
		 *\~french
		 *\brief
		 *\brief		Arrête le timer CPU.
		 */
		C3D_API void stop();
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
		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_semaphore );
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
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline TextureLayout & getTexture()
		{
			REQUIRE( m_texture );
			return *m_texture;
		}

		inline renderer::Texture const & getImage()const
		{
			REQUIRE( m_texture );
			return m_texture->getTexture();
		}

		inline renderer::TextureView const & getView()const
		{
			REQUIRE( m_texture );
			return m_texture->getDefaultView();
		}

		inline bool hasIbl()const
		{
			return m_ibl != nullptr;
		}

		inline IblTextures const & getIbl()const
		{
			REQUIRE( m_ibl );
			return *m_ibl;
		}

		inline renderer::PipelineLayout const & getPipelineLayout()const
		{
			REQUIRE( m_pipelineLayout );
			return *m_pipelineLayout;
		}

		inline renderer::Pipeline const & getPipeline()const
		{
			REQUIRE( m_pipeline );
			return *m_pipeline;
		}

		inline renderer::DescriptorSetLayout const & getDescriptorLayout()const
		{
			REQUIRE( m_descriptorLayout );
			return *m_descriptorLayout;
		}

		inline renderer::VertexBuffer< Cube > const & getVertexBuffer()const
		{
			REQUIRE( m_vertexBuffer );
			return *m_vertexBuffer;
		}

		inline renderer::Buffer< uint16_t > const & getIndexBuffer()const
		{
			REQUIRE( m_indexBuffer );
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
		*\param[in] renderPass
		*	The render pass into which the background is drawn.
		*\~french
		*\brief
		*	Enregistre les commandes utilisées pour dessiner le fond.
		*\param[out] commandBuffer
		*	Reçoit les commandes.
		*\param[in] renderPass
		*	La passe de rendu dans laquelle le fond est dessiné.
		*/
		void doPrepareFrame( renderer::CommandBuffer & commandBuffer
			, castor::Size const & size
			, renderer::RenderPass const & renderPass
			, renderer::DescriptorSet const & descriptorSet )const;
		/**
		*\~english
		*\return
		*	The shader program used to render the background.
		*\~french
		*\brief
		*	Le programme shader utilisé pour dessiner le fond.
		*/
		virtual renderer::ShaderStageStateArray doInitialiseShader();
		/**
		*\~english
		*\return
		*	Initialises the descriptor layout.
		*\~french
		*\brief
		*	Initialise le layout de descripteurs.
		*/
		virtual void doInitialiseDescriptorLayout();
		/**
		*\~english
		*\brief
		*	Initialisation function.
		*\return
		*	\p true if ok.
		*\~french
		*\brief
		*	Fonction d'initialisation.
		*\return
		*	\p true if ok.
		*/
		C3D_API virtual bool doInitialise( renderer::RenderPass const & renderPass ) = 0;
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
		bool doInitialisePipeline( renderer::ShaderStageStateArray program
			, renderer::RenderPass const & renderPass
			, HdrConfigUbo const & hdrConfigUbo );

	protected:
		Scene & m_scene;
		BackgroundType m_type;
		std::atomic_bool m_initialised{ false };
		bool m_hdr{ true };
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Matrix4x4r m_mtxModel;
		renderer::SemaphorePtr m_semaphore;
		renderer::DescriptorSetLayoutPtr m_descriptorLayout;
		renderer::DescriptorSetPoolPtr m_descriptorPool;
		renderer::DescriptorSetPtr m_descriptorSet;
		renderer::PipelineLayoutPtr m_pipelineLayout;
		renderer::PipelinePtr m_pipeline;
		renderer::VertexBufferPtr< Cube > m_vertexBuffer;
		renderer::BufferPtr< uint16_t > m_indexBuffer;
		RenderPassTimerSPtr m_timer;
		TextureLayoutSPtr m_texture;
		SamplerWPtr m_sampler;
		std::unique_ptr< IblTextures > m_ibl;
	};
}

#endif
