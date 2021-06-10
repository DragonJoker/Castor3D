/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMap_H___
#define ___C3D_EnvironmentMap_H___

#include "EnvironmentMapModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/PBR/PbrModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/Image.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

#include <RenderGraph/FrameGraph.hpp>

namespace castor3d
{
	class EnvironmentMap
		: public castor::OwnedBy< Engine >
	{
	public:
		using CubeMatrices = std::array< castor::Matrix4x4f, size_t( CubeMapFace::eCount ) >;
		using CubeCameras = std::array< CameraSPtr, size_t( CubeMapFace::eCount ) >;
		using EnvironmentMapPasses = std::array< std::unique_ptr< EnvironmentMapPass >, size_t( CubeMapFace::eCount ) >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API EnvironmentMap( crg::ResourceHandler & handler
			, RenderDevice const & device
			, SceneNode & node );
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
		 *\brief		Renders the environment map.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore to wait.
		 *\~french
		 *\brief		Dessine la texture d'environnement.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore à attendre.
		 */
		C3D_API crg::SemaphoreWait render( crg::SemaphoreWait const & toWait );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		VkExtent3D const & getSize()const
		{
			return m_extent;
		}

		Texture getColourId()const
		{
			return m_environmentMap;
		}

		VkImageView getColourView()const
		{
			return m_environmentMap.wholeView;
		}

		crg::ImageViewId getColourViewId( CubeMapFace face )const
		{
			return m_environmentMap.subViewsId[uint32_t( face )];
		}

		crg::ImageViewId getDepthViewId( CubeMapFace face )const
		{
			return m_depthBuffer.subViewsId[uint32_t( face )];
		}

		RenderPassTimer & getTimer()const
		{
			return *m_timer;
		}

		uint32_t getIndex()const
		{
			return m_index;
		}
		/**@}*/

	private:
		static uint32_t m_count;
		RenderDevice const & m_device;
		crg::FrameGraph m_graph;
		Texture m_environmentMap;
		Texture m_depthBuffer;
		SceneNode const & m_node;
		VkExtent3D m_extent;
		uint32_t m_index{ 0u };
		RenderPassTimerSPtr m_timer;
		EnvironmentMapPasses m_passes;
		bool m_first{ true };
		uint32_t m_render{ 0u };
		OnSceneNodeChangedConnection m_onNodeChanged;
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
