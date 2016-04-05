/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_RENDER_TECHNIQUE_H___
#define ___C3D_RENDER_TECHNIQUE_H___

#include "ToneMapping.hpp"
#include "RenderNode.hpp"
#include "TextureUnit.hpp"

#include <Rectangle.hpp>
#include <OwnedBy.hpp>

#if !defined( NDEBUG )
#	define DEBUG_BUFFERS 0
#else
#	define DEBUG_BUFFERS 0
#endif

namespace Castor3D
{
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
	class RenderTechnique
		: public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		21/02/2016
		\~english
		\brief		The render nodes for a specific scene.
		\~french
		\brief		Les noeuds de rendu pour une scène spécifique.
		*/
		template< typename MapT, typename NodeT >
		struct stRENDER_NODES
		{
			//!\~english The render nodes, sorted by shader program.	\~french Les noeuds de rendu, triés par programme shader.
			MapT m_renderNodes;
			//!\~english The geometries without alpha blending, sorted by shader program.	\~french Les géométries sans alpha blending, triées par programme shader.
			MapT m_opaqueRenderNodes;
			//!\~english The geometries with alpha blending, sorted by shader program.	\~french Les géométries avec de l'alpha blend, triées par programme shader.
			MapT m_transparentRenderNodes;
			//!\~english The geometries with alpha blending, sorted by distance to the camera.	\~french Les géométries avec de l'alpha blend, triées par distance à la caméra.
			std::multimap< double, NodeT > m_distanceSortedRenderNodes;
		};

	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.8.0
		\date		21/02/2016
		\~english
		\brief		The render nodes for a specific scene.
		\~french
		\brief		Les noeuds de rendu pour une scène spécifique.
		*/
		struct stSCENE_RENDER_NODES
		{
			//!\~english The scene.	\~french La scène.
			Scene & m_scene;
			//!\~english The render nodes, sorted by shader program.	\~french Les noeuds de rendu, triés par programme shader.
			stRENDER_NODES< SubmeshRenderNodesByProgramMap, GeometryRenderNode > m_geometries;
			//!\~english The billboards render nodes, sorted by shader program.	\~french Les noeuds de rendu de billboards, triés par programme shader.
			stRENDER_NODES< BillboardRenderNodesByProgramMap, BillboardRenderNode > m_billboards;
		};
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		19/12/2012
		\~english
		\brief		Internal struct holding a complete frame buffer
		\~french
		\brief		Structure interne contenant un tampon d'image complet
		*/
		struct stFRAME_BUFFER
		{
		public:
			stFRAME_BUFFER( RenderTechnique & p_technique );
			bool Initialise( Castor::Size p_size );
			void Cleanup();

			//!\~english The texture receiving the color render	\~french La texture recevant le rendu couleur
			DynamicTextureSPtr m_colourTexture;
			//!\~english The buffer receiving the depth render	\~french Le tampon recevant le rendu profondeur
			DepthStencilRenderBufferSPtr m_depthBuffer;
			//!\~english The frame buffer	\~french Le tampon d'image
			FrameBufferSPtr m_frameBuffer;
			//!\~english The attach between texture and main frame buffer	\~french L'attache entre la texture et le tampon principal
			TextureAttachmentSPtr m_colourAttach;
			//!\~english The attach between depth buffer and main frame buffer	\~french L'attache entre le tampon profondeur et le tampon principal
			RenderBufferAttachmentSPtr m_depthAttach;

		private:
			RenderTechnique & m_technique;
		};

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name
		 *\param[in]	p_renderTarget	The render target for this technique
		 *\param[in]	p_renderSystem	The render system
		 *\param[in]	p_params		The technique parameters
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique
		 *\param[in]	p_renderTarget	La render target pour cette technique
		 *\param[in]	p_renderSystem	Le render system
		 *\param[in]	p_params		Les paramètres de la technique
		 */
		C3D_API RenderTechnique( Castor::String const & p_name, RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Create();
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API void Destroy();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_index	The base texture index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_index	L'index de texture de base
		 *\return		\p true if ok
		 */
		C3D_API bool Initialise( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Updates the scenes render nodes, if needed.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Met les noeuds de scènes à jour, si nécessaire.
		 */
		C3D_API void Update();
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
		 *\brief		Render function
		 *\param[in]	p_scene		The scene to render
		 *\param[in]	p_camera	The camera through which the scene is viewed
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered
		 *\~french
		 *\brief		Fonction de rendu
		 *\param[in]	p_scene		La scène à rendre
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame
		 */
		C3D_API void Render( Scene & p_scene, Camera & p_camera, uint32_t p_frameTime );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_flags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux flags donnés
		 *\param[in]	p_flags	Une combinaison de eTEXTURE_CHANNEL
		 */
		C3D_API Castor::String GetPixelShaderSource( uint32_t p_flags )const;
		/**
		 *\~english
		 *\return		The technique name.
		 *\~french
		 *\return		Le nom de la technique.
		 */
		inline Castor::String const & GetName()const
		{
			return m_name;
		}
		/**
		 *\~english
		 *\return		The render area dimensions.
		 *\~french
		 *\return		Les dimensions de la zone de rendu.
		 */
		inline Castor::Size const & GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The colour texture holding the render's result.
		 *\~french
		 *\return		La texture de couleurs contenant le résultat du rendu.
		 */
		inline DynamicTexture const & GetResult()const
		{
			return *m_frameBuffer.m_colourTexture;
		}

	protected:
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_flags	A combination of eTEXTURE_CHANNEL
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_flags	Une combinaison de eTEXTURE_CHANNEL
		 */
		C3D_API virtual Castor::String DoGetPixelShaderSource( uint32_t p_flags )const;
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool DoCreate() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API virtual void DoDestroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_index	The base texture index
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_index	L'index de texture de base
		 *\return		\p true if ok
		 */
		C3D_API virtual bool DoInitialise( uint32_t & p_index ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Render begin function.
		 *\remarks		At the end of this method, the frame buffer that will receive draw calls must be bound.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction de début de rendu.
		 *\remarks		A la sortie de cette méthode, le tampon d'image qui recevra les dessins doit être activé.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoBeginRender() = 0;
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_nodes		The nodes to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered.
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de rendu
		 *\param[in]	p_nodes		Les noeuds à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual void DoRender( stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, uint32_t p_frameTime ) = 0;
		/**
		 *\~english
		 *\brief		Render end function
		 *\remarks		At the end of this method, the render target frame buffer must be bound.
		 *\~french
		 *\brief		Fonction de fin de rendu
		 *\remarks		A la sortie de cette méthode, le tampon d'image de la cible de rendu doit être activé.
		 */
		C3D_API virtual void DoEndRender() = 0;

	protected:
		/**
		 *\~english
		 *\brief			Sorts scene render nodes.
		 *\param[in,out]	p_nodes	The nodes.
		 *\~french
		 *\brief			Trie les noeuds de rendu de scène.
		 *\param[in,out]	p_nodes	Les noeuds.
		 */
		C3D_API void DoSortRenderNodes( stSCENE_RENDER_NODES & p_nodes );
		/**
		 *\~english
		 *\brief		Binds the given pass.
		 *\param[in]	p_scene				The rendered scene.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_node				The render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée.
		 *\param[in]	p_scene				La scène rendue.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_node				Le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void DoBindPass( Scene & p_scene, Pipeline & p_pipeline, GeometryRenderNode & p_node, uint64_t p_excludedMtxFlags );
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\param[in]	p_scene			The scene.
		 *\param[in]	p_renderNode	The render node.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 *\param[in]	p_scene			La scène.
		 *\param[in]	p_renderNode	Le noeud de rendu.
		 */
		C3D_API void DoUnbindPass( Scene & p_scene, GeometryRenderNode & p_renderNode );
		/**
		 *\~english
		 *\brief		Binds the given pass.
		 *\param[in]	p_scene				The rendered scene.
		 *\param[in]	p_pipeline			The render pipeline.
		 *\param[in]	p_node				The render node.
		 *\param[in]	p_excludedMtxFlags	Combination of MASK_MTXMODE, to be excluded from matrices used in program.
		 *\~french
		 *\brief		Active la passe donnée.
		 *\param[in]	p_scene				La scène rendue.
		 *\param[in]	p_pipeline			Le pipeline de rendu.
		 *\param[in]	p_node				Le noeud de rendu.
		 *\param[in]	p_excludedMtxFlags	Combinaison de MASK_MTXMODE, à exclure des matrices utilisées dans le programme.
		 */
		C3D_API void DoBindPass( Scene & p_scene, Pipeline & p_pipeline, BillboardRenderNode & p_node, uint64_t p_excludedMtxFlags );
		/**
		 *\~english
		 *\brief		Unbinds the render node's pass.
		 *\param[in]	p_scene			The scene.
		 *\param[in]	p_renderNode	The render node.
		 *\~french
		 *\brief		Désctive la passe du noeud de rendu.
		 *\param[in]	p_scene			La scène.
		 *\param[in]	p_renderNode	Le noeud de rendu.
		 */
		C3D_API void DoUnbindPass( Scene & p_scene, BillboardRenderNode & p_renderNode );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByProgramMap & p_nodes );
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, SubmeshRenderNodesByProgramMap & p_nodes );
		/**
		 *\~english
		 *\brief		Renders distance sorted submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages triés par distance.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, Pipeline & p_pipeline, GeometryRenderNodeByDistanceMMap & p_nodes );
		/**
		 *\~english
		 *\brief		Renders submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderSubmeshes( Scene & p_scene, Camera const & p_camera,  Pipeline & p_pipeline, SubmeshRenderNodesByProgramMap & p_nodes );
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderBillboards( Scene & p_scene, Camera const & p_camera,  Pipeline & p_pipeline, BillboardRenderNodeByDistanceMMap & p_nodes );
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderBillboards( Scene & p_scene, Camera const & p_camera,  Pipeline & p_pipeline, BillboardRenderNodesByProgramMap & p_nodes );
		/**
		 *\~english
		 *\brief		Sorts the given render nodes by distance to the camera.
		 *\param[in]	p_input		The unsorted render nodes.
		 *\param[in]	p_camera	The camera.
		 *\param[out]	p_output	The sorted render nodes.
		 *\~french
		 *\brief		Trie les noeuds de rendu donnés par distance à la caméra.
		 *\param[in]	p_input		Les noeuds de rendu non triés.
		 *\param[in]	p_camera	La caméra.
		 *\param[out]	p_output	Les noeuds de rendu triés.
		 */
		C3D_API void DoResortAlpha( SubmeshRenderNodesByProgramMap p_input, Camera const & p_camera, GeometryRenderNodeByDistanceMMap & p_output );
		/**
		 *\~english
		 *\brief		Sorts the given render nodes by distance to the camera.
		 *\param[in]	p_input		The unsorted render nodes.
		 *\param[in]	p_camera	The camera.
		 *\param[out]	p_output	The sorted render nodes.
		 *\~french
		 *\brief		Trie les noeuds de rendu donnés par distance à la caméra.
		 *\param[in]	p_input		Les noeuds de rendu non triés.
		 *\param[in]	p_camera	La caméra.
		 *\param[out]	p_output	Les noeuds de rendu triés.
		 */
		C3D_API void DoResortAlpha( BillboardRenderNodesByProgramMap p_input, Camera const & p_camera, BillboardRenderNodeByDistanceMMap & p_output );
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[in]	p_size		The render target dimensions.
		 *\param[in]	p_nodes		The scene render nodes.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered.
		 *\return		\p true if ok
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_size		Les dimensions de la cible de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu de la scène.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API void DoRender( Castor::Size const & p_size, stSCENE_RENDER_NODES & p_nodes, Camera & p_camera, uint32_t p_frameTime );

	protected:
		//!\~english The technique name	\~french Le nom de la technique
		Castor::String m_name;
		//!\~english The technique intialisation status.	\~french Le statut d'initialisation de la technique.
		bool m_initialised;
		//!\~english The parent render target	\~french La render target parente
		RenderTarget * m_renderTarget;
		//!\~english The render system	\~french Le render system
		RenderSystem * m_renderSystem;
		//!\~english The render area dimension.	\~french Les dimensions de l'aire de rendu.
		Castor::Size m_size;
		//!\~english The scenes rendered through this technique.	\~french Les scènes dessinées via cette technique.
		std::map < Castor::String, stSCENE_RENDER_NODES > m_scenesRenderNodes;
		//!\~english The HDR frame buffer.	\~french Le tampon d'image HDR.
		stFRAME_BUFFER m_frameBuffer;
		//!\~english The rasteriser state to cull front faces.	\~french L'état de rastérisation pour masquer les faces avant.
		RasteriserStateWPtr m_wpFrontRasteriserState;
		//!\~english The rasteriser state to cull back faces.	\~french L'état de rastérisation pour masquer les faces arrière.
		RasteriserStateWPtr m_wpBackRasteriserState;
	};
}

#endif
