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
		C3D_API EnvironmentMap( Engine & engine
			, SceneNode & node );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~EnvironmentMap();
		/**
		 *\~english
		 *\brief		Initialises the frame buffer.
		 *\~french
		 *\brief		Initialise le frame buffer.
		 */
		C3D_API bool initialise( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer.
		 *\~french
		 *\brief		Nettoie le frame buffer.
		 */
		C3D_API void cleanup( RenderDevice const & device );
		/**
		 *\~english
		 *\brief		Updates the passes.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour les passes.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates the GPU data.
		 *\~french
		 *\brief		Met à jour les données GPU.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders the environment map.
		 *\~french
		 *\brief		Dessine la texture d'environnement.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		 *\~english
		 *\brief		Dumps the environment map on screen.
		 *\param[in]	size	The dump dimensions.
		 *\param[in]	index	The environment map index (to compute its position).
		 *\~french
		 *\brief		Dumpe la texture d'environnement sur l'écran.
		 *\param[in]	size	Les dimensions d'affichage.
		 *\param[in]	index	L'indice de la texture d'environnement (pour calculer sa position).
		 */
		C3D_API void debugDisplay( castor::Size const & size, uint32_t index );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API VkExtent3D const & getSize()const;

		TextureUnit & getTexture()
		{
			return *m_environmentMap;
		}

		TextureUnit const & getTexture()const
		{
			return *m_environmentMap;
		}

		ashes::ImageView & getDepthView()
		{
			return m_depthBufferView;
		}

		ashes::ImageView const & getDepthView()const
		{
			return m_depthBufferView;
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
		TextureUnitSPtr m_environmentMap;
		ashes::ImagePtr m_depthBuffer;
		ashes::ImageView m_depthBufferView;
		ashes::RenderPassPtr m_renderPass;
		ashes::DescriptorSetPoolPtr m_backgroundUboDescriptorPool;
		ashes::DescriptorSetPoolPtr m_backgroundTexDescriptorPool;
		SceneNode const & m_node;
		OnSceneNodeChangedConnection m_onNodeChanged;
		CubeMatrices m_matrices;
		castor::Size m_size;
		uint32_t m_index{ 0u };
		RenderPassTimerSPtr m_timer;
		EnvironmentMapPasses m_passes;
		bool m_first{ true };
		uint32_t m_render{ 0u };
		std::shared_ptr< IblTextures > m_ibl;
		CommandsSemaphore m_generateMipmaps;
	};
}

#endif
