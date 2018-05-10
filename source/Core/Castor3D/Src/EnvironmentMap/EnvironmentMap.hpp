/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EnvironmentMap_H___
#define ___C3D_EnvironmentMap_H___

#include "PBR/IblTextures.hpp"
#include "Scene/SceneNode.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Reflection mapping implementation.
	\~french
	\brief		Implémentation du reflection mapping.
	*/
	class EnvironmentMap
		: public castor::OwnedBy< Engine >
	{
	public:
		using CubeMatrices = std::array< castor::Matrix4x4r, size_t( CubeMapFace::eCount ) >;
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
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer.
		 *\~french
		 *\brief		Nettoie le frame buffer.
		 */
		C3D_API void cleanup();
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
		C3D_API void update( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Renders the environment map.
		 *\~french
		 *\brief		Dessine la texture d'environnement.
		 */
		C3D_API renderer::Semaphore const & render( renderer::Semaphore const & toWait );
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
		inline TextureUnit & getTexture()
		{
			return m_environmentMap;
		}

		inline TextureUnit const & getTexture()const
		{
			return m_environmentMap;
		}

		inline renderer::TextureView & getDepthView()
		{
			return *m_depthBufferView;
		}

		inline renderer::TextureView const & getDepthView()const
		{
			return *m_depthBufferView;
		}

		inline renderer::Extent3D const & getSize()const
		{
			return m_environmentMap.getTexture()->getDimensions();
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**@}*/

	private:
		static uint32_t m_count;
		TextureUnit m_environmentMap;
		renderer::TexturePtr m_depthBuffer;
		renderer::TextureViewPtr m_depthBufferView;
		renderer::RenderPassPtr m_renderPass;
		renderer::DescriptorSetPoolPtr m_backgroundDescriptorPool;
		SceneNode const & m_node;
		OnSceneNodeChangedConnection m_onNodeChanged;
		CubeMatrices m_matrices;
		castor::Size m_size;
		uint32_t m_index{ 0u };
		EnvironmentMapPasses m_passes;
		uint32_t m_render = 0u;
	};
}

#endif
