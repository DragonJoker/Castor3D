/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderQueue_H___
#define ___C3D_RenderQueue_H___

#include "Render/RenderNode/RenderNode.hpp"
#include "Scene/Camera.hpp"
#include "Scene/Scene.hpp"

#include <Design/OwnedBy.hpp>

#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( push )
#	pragma warning( disable:4503 )
#endif

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		21/02/2016
	\~english
	\brief		The render nodes for a specific scene.
	\~french
	\brief		Les noeuds de rendu pour une scène spécifique.
	*/
	template< typename NodeType, typename MapType >
	struct RenderNodesT
	{
		//!\~english	The geometries, sorted by shader program.
		//!\~french		Les géométries, triées par programme shader.
		MapType m_frontCulled;
		//!\~english	The geometries, sorted by shader program.
		//!\~french		Les géométries, triées par programme shader.
		MapType m_backCulled;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		21/02/2016
	\~english
	\brief		The render nodes for a specific scene.
	\~french
	\brief		Les noeuds de rendu pour une scène spécifique.
	*/
	struct SceneRenderNodes
	{
		SceneRenderNodes & operator=( SceneRenderNodes const & p_rhs )
		{
			m_staticNodes = p_rhs.m_staticNodes;
			m_instantiatedStaticNodes = p_rhs.m_instantiatedStaticNodes;
			m_skinnedNodes = p_rhs.m_skinnedNodes;
			m_instantiatedSkinnedNodes = p_rhs.m_instantiatedSkinnedNodes;
			m_morphingNodes = p_rhs.m_morphingNodes;
			m_billboardNodes = p_rhs.m_billboardNodes;
			return *this;
		}

		using StaticNodesMap = RenderNodesT< StaticRenderNode, StaticRenderNodesByPipelineMap >;
		using SkinnedNodesMap = RenderNodesT< SkinningRenderNode, SkinningRenderNodesByPipelineMap >;
		using InstantiatedStaticNodesMap = RenderNodesT< StaticRenderNode, SubmeshStaticRenderNodesByPipelineMap >;
		using InstantiatedSkinnedNodesMap = RenderNodesT< SkinningRenderNode, SubmeshSkinningRenderNodesByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< MorphingRenderNode, MorphingRenderNodesByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap >;

		//!\~english	The scene.	\~french La scène.
		Scene & m_scene;
		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		StaticNodesMap m_staticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		SkinnedNodesMap m_skinnedNodes;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		InstantiatedStaticNodesMap m_instantiatedStaticNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		InstantiatedSkinnedNodesMap m_instantiatedSkinnedNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		MorphingNodesMap m_morphingNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		BillboardNodesMap m_billboardNodes;

		SceneRenderNodes( Scene & p_scene
			, StaticNodesMap const & p_staticGeometries = StaticNodesMap()
			, SkinnedNodesMap const & p_skinnedGeometries = SkinnedNodesMap()
			, InstantiatedStaticNodesMap const & p_instantiatedStaticGeometries = InstantiatedStaticNodesMap()
			, InstantiatedSkinnedNodesMap const & p_instantiatedSkinnedGeometries = InstantiatedSkinnedNodesMap()
			, MorphingNodesMap const & p_morphingGeometries = MorphingNodesMap()
			, BillboardNodesMap const & p_billboards = BillboardNodesMap() )
			: m_scene{ p_scene }
			, m_staticNodes( p_staticGeometries )
			, m_skinnedNodes( p_skinnedGeometries )
			, m_instantiatedStaticNodes( p_instantiatedStaticGeometries )
			, m_instantiatedSkinnedNodes( p_instantiatedSkinnedGeometries )
			, m_morphingNodes( p_morphingGeometries )
			, m_billboardNodes( p_billboards )
		{
		}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render technique base class
	\remarks	A render technique is the description of a way to render a render target
	\~french
	\brief		Classe de base d'une technique de rendu
	\remarks	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class RenderQueue
		: public castor::OwnedBy< RenderPass >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderPass	The parent render pass.
		 *\param[in]	p_opaque		Tells if this render queue is for opaque nodes.
		 *\param[in]	p_ignored		The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderPass	La passe de rendu parente.
		 *\param[in]	p_opaque		Dit si cette file de rendu est pour les noeuds opaques.
		 *\param[in]	p_ignored		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderQueue( RenderPass & p_renderPass
			, bool p_opaque
			, SceneNode const * p_ignored );
		/**
		 *\~english
		 *\brief		Plugs the render queue to given scene and camera.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_camera	The camera.
		 *\~french
		 *\brief		Branche la file de rendu à la scène et à la caméra données.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_camera	La caméra.
		 */
		C3D_API void initialise( Scene & p_scene, Camera & p_camera );
		/**
		 *\~english
		 *\brief		Plugs the render queue to given scene.
		 *\param[in]	p_scene		The scene.
		 *\~french
		 *\brief		Branche la file de rendu à la scène donnée.
		 *\param[in]	p_scene		La scène.
		 */
		C3D_API void initialise( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Updates the render nodes.
		 *\~french
		 *\brief		Met à jour les noeuds de rendu.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Retrieves a scene's nodes from a camera viewpoint.
		 *\return		The render nodes.
		 *\~french
		 *\brief		Récupère les noeuds d'une scène, du point de vue d'une caméra.
		 *\return		Les noeuds de rendu
		 */
		C3D_API SceneRenderNodes & getRenderNodes()const;

	private:
		/**
		 *\~english
		 *\brief		Prepares given nodes for render.
		 *\param[in]	p_camera		The camera.
		 *\param[in]	p_inputNodes	The scene nodes.
		 *\param[out]	p_outputNodes	Receives the renderable nodes.
		 *\~french
		 *\brief		Prépare les noeuds donnés pour le rendu.
		 *\param[in]	p_camera		La caméra.
		 *\param[in]	p_inputNodes	Les noeuds de la scène.
		 *\param[out]	p_outputNodes	Reçoit les noeuds à dessiner.
		 */
		void doPrepareRenderNodes();
		/**
		 *\~english
		 *\brief			Sorts scene render nodes.
		 *\param[in,out]	p_nodes	The nodes.
		 *\~french
		 *\brief			Trie les noeuds de rendu de scène.
		 *\param[in,out]	p_nodes	Les noeuds.
		 */
		void doSortRenderNodes();
		/**
		 *\~english
		 *\brief		Notification that the scene has changed.
		 *\param[in]	p_scene	The changed scene.
		 *\~french
		 *\brief		Notification que la scène a changé.
		 *\param[in]	p_scene	La scène changée.
		 */
		void onSceneChanged( Scene const & p_scene );
		/**
		 *\~english
		 *\brief		Notification that the camera has changed.
		 *\param[in]	p_camera	The changed camera.
		 *\~french
		 *\brief		Notification que la caméra a changé.
		 *\param[in]	p_camera	La caméra changée.
		 */
		void onCameraChanged( Camera const & p_camera );

	protected:
		//!\~english	Tells if this queue is for opaque nodes.
		//!\~french		Dit si cette file est pour les noeuds opaques.
		bool m_opaque;
		//!\~english	The geometries attached to this node will be ignored in the render.
		//!\~french		Les géométries attachées à ce noeud seront ignorées lors du rendu.
		SceneNode const * m_ignored{ nullptr };
		//!\~english	The render nodes.
		//!\~french		Les noeuds de rendu.
		std::unique_ptr< SceneRenderNodes > m_renderNodes;
		//!\~english	The prepared render nodes.
		//!\~french		Les noeuds de rendu préparés.
		std::unique_ptr< SceneRenderNodes > m_preparedRenderNodes;
		//!\~english	Tells if the camera has changed.
		//!\~french		Dit si la caméra a changé.
		bool m_changed{ true };
		//!\~english	Tells if the scene has changed.
		//!\~french		Dit si la scène a changé.
		bool m_isSceneChanged{ true };
		//!\~english	The connection to the scene change notification.
		//!\~french		Les conenction à la notification de scène changée.
		Scene::OnChanged::connection m_sceneChanged;
		//!\~english	The connection to the camera change notification.
		//!\~french		Les conenction à la notification de caméra changée.
		Camera::OnChanged::connection m_cameraChanged;
		//!\~english	The optional camera.
		//!\~french		La camera optionnelle.
		Camera const * m_camera{ nullptr };
	};
}

#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( pop )
#endif

#endif
