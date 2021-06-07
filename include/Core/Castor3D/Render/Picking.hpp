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

#include <CastorUtils/Design/OwnedBy.hpp>

#include <atomic>

namespace castor3d
{
	class Picking
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	matrixUbo	The scene matrices UBO.
		 *\param[in]	culler		The culler for this pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	matrixUbo	L'UBO de matrices de la scène.
		 *\param[in]	culler		Le culler pour cette passe.
		 */
		C3D_API Picking( crg::ResourceHandler & handler
			, RenderDevice const & device
			, castor::Size const & size
			, MatrixUbo & matrixUbo
			, SceneCuller & culler );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~Picking();
		/**
		 *\~english
		 *\brief		Adds a scene rendered through this technique.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Ajoute une scène dessinée via cette technique.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void addScene( Scene & scene
			, Camera & camera );
		/**
		 *\~english
		 *\brief		Picks a geometry at given mouse position.
		 *\param[in]	device		The GPU device.
		 *\param[in]	position	The position in the pass.
		 *\param[in]	camera		The viewing camera.
		 *\return		PickingPass::PickNodeType::eNone if nothing was picked.
		 *\~french
		 *\brief		Sélectionne la géométrie à la position de souris donnée.
		 *\param[in]	device		Le device GPU.
		 *\param[in]	position	La position dans la passe.
		 *\param[in]	camera		La caméra regardant la scène.
		 *\return		PickingPass::PickNodeType si rien n'a été pické.
		 */
		C3D_API PickNodeType pick( RenderDevice const & device
			, castor::Position position
			, Camera const & camera );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		GeometrySPtr getPickedGeometry()const
		{
			return m_geometry.lock();
		}

		BillboardBaseSPtr getPickedBillboard()const
		{
			return m_billboard.lock();
		}

		SubmeshSPtr getPickedSubmesh()const
		{
			return m_submesh.lock();
		}

		uint32_t getPickedFace()const
		{
			return m_face;
		}

		PickNodeType getPickedNodeType()const
		{
			return m_pickNodeType;
		}

		bool isPicking()const
		{
			return m_picking;
		}
		/**@}*/

	private:
		crg::FramePass doCreatePickingPass( MatrixUbo & matrixUbo
			, SceneCuller & culler );
		castor::Point4f doFboPick( castor::Position const & position
			, Camera const & camera );
		PickNodeType doPick( castor::Point4f const & pixel
			, QueueCulledRenderNodes const & nodes );

	public:
		static uint32_t constexpr PickingWidth = 32u;

	private:
		using CameraQueueMap = std::map< Camera const *, RenderQueue >;
		C3D_API static uint32_t const UboBindingPoint;

	private:
		RenderDevice const & m_device;
		castor::Size m_size;
		crg::FrameGraph m_graph;
		crg::ImageId m_colourImage;
		crg::ImageViewId m_colourImageView;
		crg::ImageId m_depthImage;
		crg::ImageViewId m_depthImageView;
		crg::FramePass m_pickingPassDesc;
		PickingPass * m_pickingPass;
		crg::RunnableGraphPtr m_runnable;
		ashes::ImagePtr m_colourTexture;
		ashes::ImageView m_colourView;
		VkBufferImageCopy m_copyRegion;
		VkBufferImageCopy m_transferDisplayRegion;
		std::vector< VkBufferImageCopy > m_pickDisplayRegions;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::BufferPtr< castor::Point4f > m_stagingBuffer;
		std::map< Scene const *, CameraQueueMap > m_scenes;
		GeometryWPtr m_geometry;
		BillboardBaseWPtr m_billboard;
		SubmeshWPtr m_submesh;
		uint32_t m_face{ 0u };
		std::vector< castor::Point4f > m_buffer;
		ashes::FencePtr m_transferFence;
		PickNodeType m_pickNodeType{ PickNodeType::eNone };
		std::atomic_bool m_picking{ false };
	};
}

#endif
