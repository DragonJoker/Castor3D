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
#ifndef ___C3D_RENDER_QUEUE_H___
#define ___C3D_RENDER_QUEUE_H___

#include "Render/RenderNode.hpp"

#include <Design/OwnedBy.hpp>

#if defined( _MSC_VER )
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
	template< typename NodeType, typename OpaqueMapType, typename TransparentMapType = OpaqueMapType >
	struct RenderNodesT
	{
		//!\~english	The geometries without alpha blending, sorted by shader program.
		//!\~french		Les géométries sans alpha blending, triées par programme shader.
		OpaqueMapType m_opaqueRenderNodesFront;
		//!\~english	The geometries without alpha blending, sorted by shader program.
		//!\~french		Les géométries sans alpha blending, triées par programme shader.
		OpaqueMapType m_opaqueRenderNodesBack;
		//!\~english	The geometries with alpha blending, sorted by shader program.
		//!\~french		Les géométries avec de l'alpha blend, triées par programme shader.
		TransparentMapType m_transparentRenderNodesFront;
		//!\~english	The geometries with alpha blending, sorted by shader program.
		//!\~french		Les géométries avec de l'alpha blend, triées par programme shader.
		TransparentMapType m_transparentRenderNodesBack;
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

		//!\~english	The scene.	\~french La scène.
		Scene & m_scene;
		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > m_staticGeometries;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap, SubmeshStaticRenderNodesByPipelineMap > m_instancedGeometries;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > m_animatedGeometries;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > m_billboards;
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
		 *\param[in]	p_technique	The parent render technique.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_technique	La technique de rendu parente.
		 */
		C3D_API RenderQueue( RenderPass & p_technique );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderQueue();
		/**
		 *\~english
		 *\brief		Prepares a scene's nodes from a camera viewpoint.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_camera	The camera.
		 *\~french
		 *\brief		Prépare les noeuds d'une scène, du point de vue d'une caméra.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_camera	La caméra.
		 */
		C3D_API void Prepare( Camera const & p_camera, Scene & p_scene );
		/**
		 *\~english
		 *\brief		Prepares a scene's nodes.
		 *\param[in]	p_scene		The scene.
		 *\return		\p true if the nodes have changed.
		 *\~french
		 *\brief		Prépare les noeuds d'une scène.
		 *\param[in]	p_scene		La scène.
		 *\return		\p true si les noeuds ont changé.
		 */
		C3D_API bool Prepare( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Adds a scene rendered through this technique.
		 *\param[in]	p_scene	The scene.
		 *\~french
		 *\brief		Ajoute une scène dessinée via cette technique.
		 *\param[in]	p_scene	La scène.
		 */
		C3D_API void AddScene( Scene & p_scene );
		/**
		 *\~english
		 *\brief		Retrieves a scene's nodes from a camera viewpoint.
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_camera	The camera.
		 *\return		The render nodes.
		 *\~french
		 *\brief		Récupère les noeuds d'une scène, du point de vue d'une caméra.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_camera	La caméra.
		 *\return		Les noeuds de rendu
		 */
		C3D_API SceneRenderNodes & GetRenderNodes( Camera const & p_camera, Scene & p_scene );
		/**
		 *\~english
		 *\brief		Retrieves a scene's nodes.
		 *\param[in]	p_scene		The scene.
		 *\return		The render nodes.
		 *\~french
		 *\brief		Récupère les noeuds d'une scène.
		 *\param[in]	p_scene		La scène.
		 *\return		Les noeuds de rendu
		 */
		C3D_API SceneRenderNodes & GetRenderNodes( Scene & p_scene );

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
		void DoPrepareRenderNodes( Camera const & p_camera, SceneRenderNodes const & p_inputNodes, SceneRenderNodes & p_outputNodes );
		/**
		 *\~english
		 *\brief			Sorts scene render nodes.
		 *\param[in,out]	p_nodes	The nodes.
		 *\~french
		 *\brief			Trie les noeuds de rendu de scène.
		 *\param[in,out]	p_nodes	Les noeuds.
		 */
		void DoSortRenderNodes( SceneRenderNodes & p_nodes );
		/**
		 *\~english
		 *\brief		Notification that a scene has changed.
		 *\param[in]	p_scene	The changed scene.
		 *\~french
		 *\brief		Notification qu'une scène a changé.
		 *\param[in]	p_scene	La scène changée.
		 */
		void OnSceneChanged( Scene const & p_scene );

	protected:
		using SceneRenderNodesMap = std::map< Scene const *, SceneRenderNodes >;
		using CameraSceneRenderNodesMap = std::map< Camera const *, SceneRenderNodesMap >;
		//!\~english	The scenes render nodes.
		//!\~french		Les noeuds de rendu des scènes.
		SceneRenderNodesMap m_scenesRenderNodes;
		//!\~english	The scenes prepared render nodes, sorted by technique, then camera.
		//!\~french		Les noeuds de rendu des scènes, triées par techniques puis par caméra.
		CameraSceneRenderNodesMap m_preparedRenderNodes;
		//!\~english	The scenes prepared render nodes, sorted by technique, then camera.
		//!\~french		Les noeuds de rendu des scènes, triées par techniques puis par caméra.
		CameraSceneRenderNodesMap m_nextPreparedRenderNodes;
		//!\~english	The camera positions at previous frame.
		//!\~french		La position des caméras à la frame précédente.
		std::map< Camera const *, std::pair<  Castor::Point3r, Castor::Quaternion > > m_cameraPositions;
		//!\~english	The newly added scenes.
		//!\~french		Les scènes récemment ajoutées.
		std::vector< SceneRPtr > m_newScenes;
		//!\~english	The changed scenes.
		//!\~french		Les scènes changées.
		std::set< Scene const * > m_changedScenes;
		//!\~english	The connections to the scenes change notification.
		//!\~french		Les conenctions aux notifications de scènes changées.
		std::map< Scene *, uint32_t > m_scenes;
	};
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif
