/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMapPass_H___
#define ___C3D_EnvironmentMapPass_H___

#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Technique/ForwardRenderTechniquePass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Reflection mapping pass implementation.
	\~french
	\brief		Implémentation d'une passe de reflection mapping.
	*/
	class EnvironmentMapPass
		: castor::OwnedBy< EnvironmentMap >
	{
	public:
		EnvironmentMapPass( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass & operator=( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass( EnvironmentMapPass && )= default;
		EnvironmentMapPass & operator=( EnvironmentMapPass && )=default;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	reflectionMap	The parent reflection map.
		 *\param[in]	node			The node from which the camera is created.
		 *\param[in]	objectNode		The node to which the object is attached.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	reflectionMap	Le reflection map parente.
		 *\param[in]	node			Le noeud depuis lequel on crée la caméra.
		 *\param[in]	objectNode		Le noeud auquel l'objet est attaché.
		 */
		C3D_API EnvironmentMapPass( EnvironmentMap & reflectionMap
			, SceneNodeSPtr node
			, SceneNode const & objectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~EnvironmentMapPass();
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool initialise( castor::Size const & size
			, uint32_t face
			, renderer::RenderPass const & renderPass
			, SceneBackground const & background
			, renderer::DescriptorSetPool const & pool );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	node	The base node.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	node	Le noeud de base.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void update( SceneNode const & node, RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Render function.
		 *\~french
		 *\brief		Fonction de rendu.
		 */
		C3D_API renderer::Semaphore const & render( renderer::Semaphore const & toWait );

	private:
		SceneNodeSPtr m_node;
		CameraSPtr m_camera;
		SceneCullerUPtr m_culler;
		renderer::TextureViewPtr m_envView;
		renderer::FrameBufferPtr m_frameBuffer;
		renderer::RenderPass const * m_renderPass{ nullptr };
		renderer::CommandBufferPtr m_backgroundCommands;
		renderer::DescriptorSetPtr m_backgroundDescriptorSet;
		MatrixUbo m_matrixUbo;
		std::unique_ptr< ForwardRenderTechniquePass > m_opaquePass;
		std::unique_ptr< ForwardRenderTechniquePass > m_transparentPass;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_finished;
		renderer::FencePtr m_fence;
		castor::Matrix4x4f m_mtxView;
		castor::Matrix4x4f m_mtxModel;
		ModelMatrixUbo m_modelMatrixUbo;
		HdrConfigUbo m_hdrConfigUbo;
	};
}

#endif
