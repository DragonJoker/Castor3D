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
		C3D_API bool initialise( renderer::RenderPass const & renderPass );
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
		C3D_API virtual bool prepareFrame( renderer::CommandBuffer & commandBuffer
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
		C3D_API virtual bool prepareFrame( renderer::CommandBuffer & commandBuffer
			, castor::Size const & size
			, renderer::RenderPass const & renderPass );
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

		inline RenderPassTimer const & getTimer()const
		{
			REQUIRE( m_timer );
			return *m_timer;
		}
		/**@}*/

	private:
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
		*\return
		*	Initialises the descriptor set.
		*\~french
		*\brief
		*	Initialise l'ensemble de descripteurs.
		*/
		virtual void doInitialiseDescriptorSet();
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

	private:
		bool doInitialiseVertexBuffer();
		bool doInitialisePipeline( renderer::ShaderStageStateArray program
			, renderer::RenderPass const & renderPass );

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

		Scene & m_scene;
		BackgroundType m_type;
		bool m_hdr{ true };
		MatrixUbo m_matrixUbo;
		ModelMatrixUbo m_modelMatrixUbo;
		castor::Matrix4x4r m_mtxModel;
		HdrConfigUbo m_configUbo;
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
