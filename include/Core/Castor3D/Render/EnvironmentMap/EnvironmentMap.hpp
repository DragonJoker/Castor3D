/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMap_H___
#define ___C3D_EnvironmentMap_H___

#include "EnvironmentMapModule.hpp"
#include "Castor3D/Event/Frame/FrameEventModule.hpp"
#include "Castor3D/Material/Texture/TextureModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/PBR/PbrModule.hpp"
#include "Castor3D/Scene/SceneModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"

#include <CastorUtils/Graphics/Size.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

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
			, QueueData const & queueData
			, Scene const & scene );
		/**
		 *\~english
		 *\brief		Clears the passes.
		 *\~french
		 *\brief		Vide les passes.
		 */
		C3D_API void cleanup();
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
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		/**
		 *\~english
		 *\return		Creates a reflection map for given node.
		 *\param[in]	node	The scene node from which the reflection map is generated.
		 *\~french
		 *\return		Crée une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène depuis lequel la reflection map est générée.
		 */
		C3D_API void addNode( SceneNode & node );
		/**
		 *\~english
		 *\return		Creates a reflection map for given node.
		 *\param[in]	node	The scene node from which the reflection map is generated.
		 *\~french
		 *\return		Crée une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène depuis lequel la reflection map est générée.
		 */
		C3D_API void removeNode( SceneNode & node );
		/**
		 *\~english
		 *\return		Tells if there is a reflection map for given node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\return		Dit s'il y a une reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API bool hasNode( SceneNode & node )const;
		/**
		 *\~english
		 *\remarks		Call hasEnvironmentMap before calling this function (since this one returns a reference to an existing EnvironmentMap).
		 *\return		Retrieves the reflection map for given node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\remarks		Appelez hasEnvironmentMap avant d'appeler cette fonction (celle-ci retournant une référence sur une EnvironmentMap existante)
		 *\return		Récupère la reflection map pour le noeud donné.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API uint32_t getIndex( SceneNode const & node )const;
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

		Texture const & getColourId()const
		{
			return m_environmentMap;
		}

		VkImageView getColourView( uint32_t index )const
		{
			return m_environmentMapViews[index];
		}

		crg::ImageViewId getColourViewId( uint32_t index
			, CubeMapFace face )const
		{
			return m_environmentMap.subViewsId[index * 6u + uint32_t( face )];
		}

		crg::ImageViewId getDepthViewId( uint32_t index
			, CubeMapFace face )const
		{
			return m_depthBuffer.subViewsId[index * 6u + uint32_t( face )];
		}
		/**@}*/

	private:
		void doAddPass();

	public:
		C3D_API static uint32_t const Count;
		C3D_API static uint32_t const Size;

	private:
		RenderDevice const & m_device;
		Scene const & m_scene;
		std::vector< std::unique_ptr< crg::FrameGraph > > m_graphs;
		Texture m_environmentMap;
		Texture m_depthBuffer;
		VkExtent3D m_extent;
		std::set< SceneNode * > m_reflectionNodes;
		std::set< SceneNode * > m_savedReflectionNodes;
		std::map< SceneNode const *, uint32_t > m_sortedNodes;
		std::vector< EnvironmentMapPasses > m_passes;
		bool m_first{ true };
		uint32_t m_render{ 0u };
		uint32_t m_count{ 0u };
		OnSceneNodeChangedConnection m_onNodeChanged;
		std::vector< crg::RunnableGraphPtr > m_runnables;
		ashes::ImagePtr m_image;
		std::vector< ashes::ImageView > m_environmentMapViews;
		OnBackgroundChangedConnection m_onSetBackground;
	};
}

#endif
