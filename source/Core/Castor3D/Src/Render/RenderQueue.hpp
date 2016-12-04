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
	template< typename NodeType, typename MapType >
	struct RenderNodesT
	{
		//!\~english	The geometries without alpha blending, sorted by shader program.
		//!\~french		Les géométries sans alpha blending, triées par programme shader.
		MapType m_opaqueRenderNodesFront;
		//!\~english	The geometries without alpha blending, sorted by shader program.
		//!\~french		Les géométries sans alpha blending, triées par programme shader.
		MapType m_opaqueRenderNodesBack;
		//!\~english	The geometries with alpha blending, sorted by shader program.
		//!\~french		Les géométries avec de l'alpha blend, triées par programme shader.
		MapType m_transparentRenderNodesFront;
		//!\~english	The geometries with alpha blending, sorted by shader program.
		//!\~french		Les géométries avec de l'alpha blend, triées par programme shader.
		MapType m_transparentRenderNodesBack;
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
			m_staticGeometries = p_rhs.m_staticGeometries;
			m_instancedGeometries = p_rhs.m_instancedGeometries;
			m_animatedGeometries = p_rhs.m_animatedGeometries;
			m_billboards = p_rhs.m_billboards;
			return *this;
		}

		//!\~english	The scene.	\~french La scène.
		Scene & m_scene;
		//!\~english	The static render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu statiques, triés par programme shader.
		RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > m_staticGeometries;
		//!\~english	The instanced render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu instanciés, triés par programme shader.
		RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap > m_instancedGeometries;
		//!\~english	The animated render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu animés, triés par programme shader.
		RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > m_animatedGeometries;
		//!\~english	The billboards render nodes, sorted by shader program.
		//!\~french		Les noeuds de rendu de billboards, triés par programme shader.
		RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > m_billboards;

		SceneRenderNodes( Scene & p_scene
						 , RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap > const & p_staticGeometries = RenderNodesT< StaticGeometryRenderNode, StaticGeometryRenderNodesByPipelineMap >()
						  , RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap > const & p_instancedGeometries = RenderNodesT< StaticGeometryRenderNode, SubmeshStaticRenderNodesByPipelineMap >()
						  , RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap > const & p_animatedGeometries = RenderNodesT< AnimatedGeometryRenderNode, AnimatedGeometryRenderNodesByPipelineMap >()
						  , RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap > const & p_billboards = RenderNodesT< BillboardRenderNode, BillboardRenderNodesByPipelineMap >() )
			: m_scene{ p_scene }
			, m_staticGeometries( p_staticGeometries )
			, m_instancedGeometries( p_instancedGeometries )
			, m_animatedGeometries( p_animatedGeometries )
			, m_billboards( p_billboards )
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
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderPass	La passe de rendu parente.
		 */
		C3D_API RenderQueue( RenderPass & p_renderPass );
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
		uint32_t m_sceneChanged{ false };
		//!\~english	The connection to the camera change notification.
		//!\~french		Les conenction à la notification de caméra changée.
		uint32_t m_cameraChanged{ false };
		//!\~english	The optional camera.
		//!\~french		La camera optionnelle.
		Camera const * m_camera{ nullptr };
	};
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif
