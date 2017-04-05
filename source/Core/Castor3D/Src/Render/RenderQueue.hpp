/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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

namespace Castor3D
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
			m_instancedNodes = p_rhs.m_instancedNodes;
			m_skinningNodes = p_rhs.m_skinningNodes;
			m_morphingNodes = p_rhs.m_morphingNodes;
			m_billboardNodes = p_rhs.m_billboardNodes;
			return *this;
		}

		using StaticNodesMap = RenderNodesT< StaticRenderNode, StaticRenderNodesByPipelineMap >;
		using InstancedNodesMap = RenderNodesT< StaticRenderNode, SubmeshStaticRenderNodesByPipelineMap >;
		using SkinningNodesMap = RenderNodesT< SkinningRenderNode, SkinningRenderNodesByPipelineMap >;
		using MorphingNodesMap = RenderNodesT< MorphingRenderNode, MorphingRenderNodesByPipelineMap >;
		using BillboardNodesMap = RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap >;

		//!\~english	The scene.	\~french La scène.
		Scene & m_scene;
		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		StaticNodesMap m_staticNodes;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		InstancedNodesMap m_instancedNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		SkinningNodesMap m_skinningNodes;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		MorphingNodesMap m_morphingNodes;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		BillboardNodesMap m_billboardNodes;

		SceneRenderNodes( Scene & p_scene
			, StaticNodesMap const & p_staticGeometries = StaticNodesMap()
			, InstancedNodesMap const & p_instancedGeometries = InstancedNodesMap()
			, SkinningNodesMap const & p_skinningGeometries = SkinningNodesMap()
			, MorphingNodesMap const & p_morphingGeometries = MorphingNodesMap()
			, BillboardNodesMap const & p_billboards = BillboardNodesMap() )
			: m_scene{ p_scene }
			, m_staticNodes( p_staticGeometries )
			, m_instancedNodes( p_instancedGeometries )
			, m_skinningNodes( p_skinningGeometries )
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
		: public Castor::OwnedBy< RenderPass >
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
		C3D_API void Initialise( Scene & p_scene, Camera & p_camera );
		/**
		 *\~english
		 *\brief		Plugs the render queue to given scene.
		 *\param[in]	p_scene		The scene.
		 *\~french
		 *\brief		Branche la file de rendu à la scène donnée.
		 *\param[in]	p_scene		La scène.
		 */
		C3D_API void Initialise( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Updates the render nodes.
		 *\~french
		 *\brief		Met à jour les noeuds de rendu.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Retrieves a scene's nodes from a camera viewpoint.
		 *\return		The render nodes.
		 *\~french
		 *\brief		Récupère les noeuds d'une scène, du point de vue d'une caméra.
		 *\return		Les noeuds de rendu
		 */
		C3D_API SceneRenderNodes & GetRenderNodes();

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
		void DoPrepareRenderNodes();
		/**
		 *\~english
		 *\brief			Sorts scene render nodes.
		 *\param[in,out]	p_nodes	The nodes.
		 *\~french
		 *\brief			Trie les noeuds de rendu de scène.
		 *\param[in,out]	p_nodes	Les noeuds.
		 */
		void DoSortRenderNodes();
		/**
		 *\~english
		 *\brief		Notification that the scene has changed.
		 *\param[in]	p_scene	The changed scene.
		 *\~french
		 *\brief		Notification que la scène a changé.
		 *\param[in]	p_scene	La scène changée.
		 */
		void OnSceneChanged( Scene const & p_scene );
		/**
		 *\~english
		 *\brief		Notification that the camera has changed.
		 *\param[in]	p_camera	The changed camera.
		 *\~french
		 *\brief		Notification que la caméra a changé.
		 *\param[in]	p_camera	La caméra changée.
		 */
		void OnCameraChanged( Camera const & p_camera );

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
		//!\~english	Tells if the scene or the camera have changed.
		//!\~french		Dit si la caméra ou la scène a changé.
		bool m_changed{ true };
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
