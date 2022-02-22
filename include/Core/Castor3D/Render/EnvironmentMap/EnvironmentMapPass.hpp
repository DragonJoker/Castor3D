/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMapPass_H___
#define ___C3D_EnvironmentMapPass_H___

#include "EnvironmentMapModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Render/Technique/TechniqueModule.hpp"

#include "Castor3D/Render/Culling/SceneCuller.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class EnvironmentMapPass
		: public castor::OwnedBy< EnvironmentMap >
		, public castor::Named
	{
	public:
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
		C3D_API EnvironmentMapPass( crg::FrameGraph & graph
			, RenderDevice const & device
			, EnvironmentMap & environmentMap
			, SceneNodeSPtr faceNode
			, uint32_t index
			, CubeMapFace face
			, SceneBackground & background );
		C3D_API ~EnvironmentMapPass();
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
		C3D_API void attachTo( SceneNode & node );

		crg::FramePass const & getLastPass()const
		{
			return *m_transparentPassDesc;
		}

	private:
		crg::FramePass & doCreateOpaquePass( crg::FramePass const * previousPass );
		crg::FramePass & doCreateTransparentPass( crg::FramePass const * previousPass );
		void doCreateGenMipmapsPass( crg::FramePass const * previousPass );

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		SceneBackground & m_background;
		SceneNodeSPtr m_node;
		uint32_t m_index;
		CubeMapFace m_face;
		CameraSPtr m_camera;
		SceneNode const * m_currentNode{};
		SceneCullerUPtr m_culler;
		MatrixUbo m_matrixUbo;
		HdrConfigUbo m_hdrConfigUbo;
		SceneUbo m_sceneUbo;
		crg::ImageViewId m_colourView;
		BackgroundRendererUPtr m_backgroundRenderer;
		crg::FramePass * m_opaquePassDesc{};
		RenderTechniquePass * m_opaquePass{};
		crg::FramePass * m_transparentPassDesc{};
		RenderTechniquePass * m_transparentPass{};
	};
}

#endif
