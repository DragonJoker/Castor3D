/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMapPass_H___
#define ___C3D_EnvironmentMapPass_H___

#include "EnvironmentMapModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class EnvironmentMapPass
		: castor::OwnedBy< EnvironmentMap >
	{
	public:
		EnvironmentMapPass( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass & operator=( EnvironmentMapPass const & )=delete;
		EnvironmentMapPass( EnvironmentMapPass && )= default;
		EnvironmentMapPass & operator=( EnvironmentMapPass && )=delete;
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	reflectionMap	The parent reflection map.
		 *\param[in]	node			The node from which the camera is created.
		 *\param[in]	objectNode		The node to which the object is attached.
		 *\param[in]	face			The cube face this pass renders to.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	reflectionMap	Le reflection map parente.
		 *\param[in]	node			Le noeud depuis lequel on crée la caméra.
		 *\param[in]	objectNode		Le noeud auquel l'objet est attaché.
		 *\param[in]	face			La face de cube que cette passe dessine.
		 */
		C3D_API EnvironmentMapPass( EnvironmentMap & reflectionMap
			, SceneNodeSPtr node
			, SceneNode const & objectNode
			, CubeMapFace face );
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
		 *\param		device		The GPU device.
		 *\param		size		The pass needed dimensions.
		 *\param		face		The target face.
		 *\param		renderPass	The render pass to use.
		 *\param		background	The scene background.
		 *\param		uboPool		The UBO descriptor pool used to bind the scene background.
		 *\param		texPool		The textures descriptor pool used to bind the scene background.
		 *\param		timer		The render pass timer to use.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		device		Le device GPU.
		 *\param		size		Les dimensions voulues pour la passe.
		 *\param		face		La face cible.
		 *\param		renderPass	La passe de rendu à utiliser.
		 *\param		background	Le fond de la scène.
		 *\param		uboPool		Le pool de descripteurs d'UBO utilisé pour binder le fond de la scène.
		 *\param		texPool		Le pool de descripteurs de textures utilisé pour binder le fond de la scène.
		 *\param		timer		Le timer de passe de rendu à utiliser.
		 *\return		\p true si tout s'est bien passé.
		 */
		bool initialise( RenderDevice const & device
			, castor::Size const & size
			, uint32_t face
			, ashes::RenderPass const & renderPass
			, SceneBackground const & background
			, ashes::DescriptorSetPool const & uboPool
			, ashes::DescriptorSetPool const & texPool
			, RenderPassTimer & timer );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\param		device	The GPU device.
		 *\~french
		 *\brief		Nettoie la passe.
		 *\param		device	Le device GPU.
		 */
		void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Render function.
		 *\param		device	The GPU device.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param		device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );

	private:
		SceneNodeSPtr m_node;
		CubeMapFace m_face;
		CameraSPtr m_camera;
		SceneCullerUPtr m_culler;
		ashes::ImageView m_envView;
		ashes::FrameBufferPtr m_frameBuffer;
		ashes::RenderPass const * m_renderPass{ nullptr };
		ashes::CommandBufferPtr m_backgroundCommands;
		ashes::DescriptorSetPtr m_backgroundUboDescriptorSet;
		ashes::DescriptorSetPtr m_backgroundTexDescriptorSet;
		MatrixUbo m_matrixUbo;
		std::shared_ptr< ForwardRenderTechniquePass > m_opaquePass;
		std::shared_ptr< ForwardRenderTechniquePass > m_transparentPass;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::SemaphorePtr m_finished;
		castor::Matrix4x4f m_mtxView;
		castor::Matrix4x4f m_mtxModel;
		UniformBufferOffsetT< ModelUboConfiguration > m_modelUbo;
		HdrConfigUbo m_hdrConfigUbo;
	};
}

#endif
