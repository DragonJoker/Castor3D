/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Picking_H___
#define ___C3D_Picking_H___

#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Render/Culling/CullingModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/OwnedBy.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class Picking
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	resources	The graph resources handler.
		 *\param[in]	device		The GPU device.
		 *\param[in]	queueData	The queue receiving the GPU commands.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	cameraUbo	The scene matrices UBO.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	resources	Le gestionnaire de ressources du graphe.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	queueData	La queue recevant les commandes GPU.
		 *\param[in]	size		Les dimensions de la zone de rendu
		 *\param[in]	cameraUbo	L'UBO de matrices de la scène.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		C3D_API Picking( crg::ResourcesCache const & resources
			, RenderDevice const & device
			, QueueData const & queueData
			, castor::Size const & size
			, CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Picking()noexcept;
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	position	The position in the pass.
		 *\return		PickingPass::PickNodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	position	La position dans la passe.
		 *\return		PickingPass::PickNodeType si rien n'a été pické.
		 */
		C3D_API PickNodeType pick( castor::Position position );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Geometry const * getPickedGeometry()const noexcept
		{
			return m_geometry;
		}

		BillboardBase const * getPickedBillboard()const noexcept
		{
			return m_billboard;
		}

		Submesh const * getPickedSubmesh()const noexcept
		{
			return m_submesh;
		}

		uint32_t getPickedFace()const noexcept
		{
			return m_face;
		}

		PickNodeType getPickedNodeType()const noexcept
		{
			return m_pickNodeType;
		}

		crg::ImageViewId const & getImageView()const noexcept
		{
			return m_colourImageView;
		}

		crg::SemaphoreWaitArray const & getSemaphoreWait()const noexcept
		{
			return m_toWait;
		}

		bool isPicking()const noexcept
		{
			return m_picking;
		}
		/**@}*/

	private:
		crg::FramePass & doCreatePickingPass( CameraUbo const & cameraUbo
			, SceneUbo const & sceneUbo
			, SceneCuller & culler );
		castor::Point4ui doFboPick( castor::Position const & position );
		PickNodeType doPick( castor::Point4ui const & pixel
			, Scene const & scene );

	private:
		C3D_API static uint32_t const UboBindingPoint;

	private:
		RenderDevice const & m_device;
		uint32_t m_bandSize;
		castor::Size m_realSize;
		crg::FrameGraph m_graph;
		crg::ImageId m_colourImage;
		crg::ImageViewId m_colourImageView;
		crg::ImageId m_depthImage;
		crg::ImageViewId m_depthImageView;
		crg::FramePass * m_pickingPassDesc;
		PickingPass * m_pickingPass;
		crg::RunnableGraphPtr m_runnable;
		ashes::ImagePtr m_colourTexture;
		ashes::ImageView m_colourView;
		VkBufferImageCopy m_copyRegion;
		castor::Vector< VkBufferImageCopy > m_pickDisplayRegions;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::BufferPtr< castor::Point4ui > m_pickBuffer;
		castor::ArrayView< castor::Point4ui > m_pickData;
		Geometry const * m_geometry{};
		Submesh const * m_submesh{};
		BillboardBase const * m_billboard{};
		uint32_t m_face{ 0u };
		castor::Vector< castor::Point4ui > m_buffer;
		ashes::FencePtr m_transferFence;
		PickNodeType m_pickNodeType{ PickNodeType::eNone };
		std::atomic_bool m_picking{ false };
		crg::SemaphoreWaitArray m_toWait;
		bool m_first{ true };
	};
}

#endif
