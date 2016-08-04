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
#ifndef ___C3D_RENDER_TECHNIQUE_H___
#define ___C3D_RENDER_TECHNIQUE_H___

#include "HDR/ToneMapping.hpp"
#include "Render/RenderQueue.hpp"
#include "Texture/TextureUnit.hpp"

#include <Graphics/Rectangle.hpp>

#if !defined( NDEBUG )
#	define DEBUG_BUFFERS 0
#else
#	define DEBUG_BUFFERS 0
#endif

#if defined( _MSC_VER )
#	pragma warning( push )
#	pragma warning( disable:4503 )
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
		, public Castor::Named
	{
	public:
		using DistanceSortedNodeMap = std::multimap< double, std::reference_wrapper< ObjectRenderNodeBase > >;

	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		19/12/2012
		\~english
		\brief		Internal struct holding a complete frame buffer.
		\~french
		\brief		Structure interne contenant un tampon d'image complet.
		*/
		struct stFRAME_BUFFER
		{
		public:
			explicit stFRAME_BUFFER( RenderTechnique & p_technique );
			bool Initialise( Castor::Size p_size );
			void Cleanup();

			//!\~english	The texture receiving the color render.
			//!\~french		La texture recevant le rendu couleur.
			TextureLayoutSPtr m_colourTexture;
			//!\~english	The buffer receiving the depth render.
			//!\~french		Le tampon recevant le rendu profondeur.
			DepthStencilRenderBufferSPtr m_depthBuffer;
			//!\~english	The frame buffer.
			//!\~french		Le tampon d'image.
			FrameBufferSPtr m_frameBuffer;
			//!\~english	The attach between texture and main frame buffer.
			//!\~french		L'attache entre la texture et le tampon principal.
			TextureAttachmentSPtr m_colourAttach;
			//!\~english	The attach between depth buffer and main frame buffer.
			//!\~french		L'attache entre le tampon profondeur et le tampon principal.
			RenderBufferAttachmentSPtr m_depthAttach;

		private:
			RenderTechnique & m_technique;
		};

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_renderTarget	The render target for this technique.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_params		The technique parameters.
		 *\param[in]	p_multisampling	The multisampling status
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_renderTarget	La render target pour cette technique.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_params		Les paramètres de la technique.
		 *\param[in]	p_multisampling	Le statut de multiéchantillonnage.
		 */
		C3D_API RenderTechnique( Castor::String const & p_name, RenderTarget & p_renderTarget, RenderSystem & p_renderSystem, Parameters const & p_params, bool p_multisampling = false );

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
		 *\param[in]	p_scene		The scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\~french
		 *\brief		Ajoute une scène dessinée via cette technique.
		 *\param[in]	p_scene		La scène.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 */
		C3D_API void AddScene( Scene & p_scene, Camera & p_camera );
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_scene		The scene to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered.
		 *\param[out]	p_visible	The visible objects count.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_scene		La scène à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame.
		 *\param[out]	p_visible	Le nombre d'objets visibles.
		 */
		C3D_API void Render( Scene & p_scene, Camera & p_camera, uint32_t p_frameTime, uint32_t & p_visible );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_flags	A combination of TextureChannel
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux flags donnés
		 *\param[in]	p_flags	Une combinaison de TextureChannel
		 */
		C3D_API Castor::String GetPixelShaderSource( uint32_t p_flags )const;
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	p_file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	p_file	Le fichier.
		 */
		C3D_API bool WriteInto( Castor::TextFile & p_file );
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
		inline TextureLayout const & GetResult()const
		{
			return *m_frameBuffer.m_colourTexture;
		}
		/**
		 *\~english
		 *\return		The multsampling status.
		 *\~french
		 *\return		Le statut d'échantillonnage.
		 */
		inline bool IsMultisampling()const
		{
			return m_multisampling;
		}

	protected:
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Pipeline & p_pipeline, StaticGeometryRenderNodesByProgramMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Pipeline & p_pipeline, AnimatedGeometryRenderNodesByProgramMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderInstancedSubmeshesNonInstanced( Scene & p_scene, Pipeline & p_pipeline, SubmeshStaticRenderNodesByProgramMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Pipeline & p_pipeline, SubmeshStaticRenderNodesByProgramMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Renders objects sorted by distance to camera.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine d'objets triés par distance à la caméra.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderByDistance( Scene & p_scene, Pipeline & p_pipeline, DistanceSortedNodeMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderBillboards( Scene & p_scene, Pipeline & p_pipeline, BillboardRenderNodesByProgramMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Render function.
		 *\param[in]	p_size		The render target dimensions.
		 *\param[in]	p_nodes		The scene render nodes.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[in]	p_size		Les dimensions de la cible de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu de la scène.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame.
		 */
		C3D_API void DoRender( Castor::Size const & p_size, SceneRenderNodes & p_nodes, Camera & p_camera, uint32_t p_frameTime );
		/**
		 *\~english
		 *\brief		Renders opaque render nodes.
		 *\param[in]	p_nodes		The scene render nodes.
		 *\param[in]	p_pipeline	The rendering pipeline.
		 *\~french
		 *\brief		Dessine les noeuds de rendu opaques.
		 *\param[in]	p_nodes		Les noeuds de rendu de la scène.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API void DoRenderOpaqueNodes( SceneRenderNodes & p_nodes, Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Renders transparent render nodes.
		 *\param[in]	p_nodes		The scene render nodes.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The rendering pipeline.
		 *\~french
		 *\brief		Dessine les noeuds de rendu transparents.
		 *\param[in]	p_nodes		Les noeuds de rendu de la scène.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API void DoRenderTransparentNodes( SceneRenderNodes & p_nodes, Pipeline & p_pipeline, Camera const & p_camera );

	private:
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags
		 *\param[in]	p_flags	A combination of TextureChannel
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés
		 *\param[in]	p_flags	Une combinaison de TextureChannel
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
		 *\param[in]	p_scene		The scene to render
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction de début de rendu.
		 *\remarks		A la sortie de cette méthode, le tampon d'image qui recevra les dessins doit être activé.
		 *\param[in]	p_scene		La scène à dessiner
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoBeginRender( Scene & p_scene ) = 0;
		/**
		 *\~english
		 *\brief		Render function
		 *\param[in]	p_nodes		The nodes to render.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_frameTime	The time elapsed since last frame was rendered.
		 *\~french
		 *\brief		Fonction de rendu
		 *\param[in]	p_nodes		Les noeuds à dessiner.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_frameTime	Le temps écoulé depuis le rendu de la dernière frame.
		 */
		C3D_API virtual void DoRender( SceneRenderNodes & p_nodes, Camera & p_camera, uint32_t p_frameTime ) = 0;
		/**
		 *\~english
		 *\brief		Render end function
		 *\remarks		At the end of this method, no frame buffer must be bound.
		 *\param[in]	p_scene		The scene to render
		 *\~french
		 *\brief		Fonction de fin de rendu
		 *\remarks		A la sortie de cette méthode, aucun tampon d'image ne doit être activé.
		 *\param[in]	p_scene		La scène à dessiner
		 */
		C3D_API virtual void DoEndRender( Scene & p_scene ) = 0;
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	p_file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	p_file	Le fichier.
		 */
		C3D_API virtual bool DoWriteInto( Castor::TextFile & p_file ) = 0;

	private:
		template< typename MapType >
		void DoRenderNonInstanced( Scene & p_scene
								   , Pipeline & p_pipeline
								   , MapType & p_nodes
								   , bool p_register )
		{
			for ( auto l_itPrograms : p_nodes )
			{
				for ( auto & l_renderNode : l_itPrograms.second )
				{
					l_renderNode.Render( p_scene, p_pipeline );

					if ( p_register )
					{
						m_renderedObjects.push_back( l_renderNode );
					}
				}
			}
		}

	protected:
		//!\~english	The technique intialisation status.
		//!\~french		Le statut d'initialisation de la technique.
		bool m_initialised;
		//!\~english	The parent render target.
		//!\~french		La render target parente.
		RenderTarget & m_renderTarget;
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		//!\~english	The render area dimension.
		//!\~french		Les dimensions de l'aire de rendu.
		Castor::Size m_size;
		//!\~english	The render queue.
		//!\~french		La file de rendu.
		RenderQueue m_renderQueue;
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< SceneRPtr, std::vector< CameraRPtr > > m_scenes;
		//!\~english	The HDR frame buffer.
		//!\~french		Le tampon d'image HDR.
		stFRAME_BUFFER m_frameBuffer;
		//!\~english	The rasteriser state to cull front faces.
		//!\~french		L'état de rastérisation pour masquer les faces avant.
		RasteriserStateWPtr m_wpFrontRasteriserState;
		//!\~english	The rasteriser state to cull back faces.
		//!\~french		L'état de rastérisation pour masquer les faces arrière.
		RasteriserStateWPtr m_wpBackRasteriserState;
		//!\~english	Tells if the technique uses multisampling.
		//!\~french		Dit si la technique utilise le multiéchantillonnage.
		bool m_multisampling{ false };
		//!\~english	The objects rendered in the last frame.
		//!\~french		Les objets dessinés lors de la dernière frame.
		std::vector< std::reference_wrapper< ObjectRenderNodeBase const > > m_renderedObjects;
	};
}

#if defined( _MSC_VER )
#	pragma warning( pop )
#endif

#endif
