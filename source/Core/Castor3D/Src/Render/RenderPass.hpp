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
#ifndef ___C3D_RENDER_PASS_H___
#define ___C3D_RENDER_PASS_H___

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

#include "Render/RenderQueue.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Render pass base class.
	\~french
	\brief		Classe de base d'une passe de rendu.
	*/
	class RenderPass
		: public Castor::OwnedBy< Engine >
		, public Castor::Named
	{
	public:
		using DistanceSortedNodeMap = std::multimap< double, std::reference_wrapper< ObjectRenderNodeBase > >;

	protected:
		struct PipelineFlags
		{
			uint16_t m_textureFlags;
			uint8_t m_programFlags;
			BlendMode m_colourBlendMode;
			BlendMode m_alphaBlendMode;
		};
		struct PipelineFlagsHasher
		{
			size_t operator()( PipelineFlags const & p_flags )const
			{
				return size_t
				{
					( ( size_t( p_flags.m_textureFlags ) << 0 ) & 0x0000FFFF )
					| ( ( size_t( p_flags.m_programFlags ) << 16 ) & 0x00FF0000 )
					| ( ( size_t( p_flags.m_colourBlendMode ) << 24 ) & 0x0F000000 )
					| ( ( size_t( p_flags.m_alphaBlendMode ) << 28 ) & 0xF0000000 )
				};
			}
		};
		struct PipelineFlagsComparer
		{
			size_t operator()( PipelineFlags const & p_lhs, PipelineFlags const & p_rhs )const
			{
				return p_lhs.m_textureFlags == p_rhs.m_textureFlags
					&& p_lhs.m_programFlags == p_rhs.m_programFlags
					&& p_lhs.m_colourBlendMode == p_rhs.m_colourBlendMode
					&& p_lhs.m_alphaBlendMode == p_rhs.m_alphaBlendMode;
			}
		};
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_name			The pass name.
		 *\param[in]	p_renderTarget	The engine.
		 *\param[in]	p_multisampling	The multisampling status
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_name			Le nom de la passe.
		 *\param[in]	p_renderTarget	Le moteur.
		 *\param[in]	p_multisampling	Le statut de multiéchantillonnage.
		 */
		C3D_API RenderPass( Castor::String const & p_name, Engine & p_engine, bool p_multisampling = false );

	public:
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~RenderPass();
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
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API Castor::String GetPixelShaderSource( uint16_t p_textureFlags, uint8_t p_programFlags )const;
		/**
		 *\~english
		 *\brief		Prepares the pipeline matching the given flags.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\~french
		 *\brief		Prépare le pipeline qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 */
		C3D_API void PreparePipeline( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode );
		/**
		 *\~english
		 *\brief		Retrieves the opaque pipeline matching the given flags.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\~french
		 *\brief		Récupère le pipeline opaque qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 */
		C3D_API Pipeline & GetOpaquePipeline( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode );
		/**
		 *\~english
		 *\brief		Retrieves the transparent pipeline matching the given flags, for front faces culling.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\~french
		 *\brief		Récupère le pipeline transparent qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 */
		C3D_API Pipeline & GetTransparentPipelineFront( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode );
		/**
		 *\~english
		 *\brief		Retrieves the transparent pipeline matching the given flags, for back face culling.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\~french
		 *\brief		Récupère le pipeline transparent qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 */
		C3D_API Pipeline & GetTransparentPipelineBack( uint16_t p_textureFlags, uint8_t p_programFlags, BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode );
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
		C3D_API void DoRenderStaticSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, StaticGeometryRenderNodesByPipelineMap & p_nodes, bool p_register = true );
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
		C3D_API void DoRenderAnimatedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, AnimatedGeometryRenderNodesByPipelineMap & p_nodes, bool p_register = true );
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
		C3D_API void DoRenderInstancedSubmeshesNonInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register = true );
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
		C3D_API void DoRenderInstancedSubmeshesInstanced( Scene & p_scene, Camera const & p_camera, SubmeshStaticRenderNodesByPipelineMap & p_nodes, bool p_register = true );
		/**
		 *\~english
		 *\brief		Renders objects sorted by distance to camera.
		 *\param[in]	p_scene		The rendered scene.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\param[in]	p_nodes		The render nodes.
		 *\~french
		 *\brief		Dessine d'objets triés par distance à la caméra.
		 *\param[in]	p_scene		La scène rendue.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 *\param[in]	p_nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRenderByDistance( Scene & p_scene, Camera const & p_camera, DistanceSortedNodeMap & p_nodes, bool p_register = true );
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
		C3D_API void DoRenderBillboards( Scene & p_scene, Camera const & p_camera, BillboardRenderNodesByPipelineMap & p_nodes, bool p_register = true );

	private:
		/**
		 *\~english
		 *\brief		Prepares the pipeline for opaque objets render.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets opaques.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareOpaquePipeline( ShaderProgram & p_program, PipelineFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Prepares the pipeline for transparent objets render, culling front faces.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets transparents, en supprimant les faces avant.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Prepares the pipeline for transparent objets render, culling back faces.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets transparents, en supprimant les faces arrière.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags );
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API virtual Castor::String DoGetOpaquePixelShaderSource( uint32_t p_textureFlags, uint32_t p_programFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API virtual Castor::String DoGetTransparentPixelShaderSource( uint32_t p_textureFlags, uint32_t p_programFlags )const = 0;

	protected:
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		//!\~english	The render queue.
		//!\~french		La file de rendu.
		RenderQueue m_renderQueue;
		//!\~english	The scenes, and cameras used to render them.
		//!\~french		Les scènes, et les caméras utilisées pour les dessiner.
		std::map< SceneRPtr, std::vector< CameraRPtr > > m_scenes;
		//!\~english	The pipelines used to render opaque nodes.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les noeuds opaques.
		std::unordered_map< PipelineFlags, PipelineUPtr, PipelineFlagsHasher, PipelineFlagsComparer > m_opaquePipelines;
		//!\~english	The pipelines used to render transparent nodes' back faces.
		//!\~french		Les pipeline de rendu utilisé pour dessiner les faces arrière des noeuds transparents.
		std::unordered_map< PipelineFlags, PipelineUPtr, PipelineFlagsHasher, PipelineFlagsComparer > m_frontTransparentPipelines;
		//!\~english	The pipelines used to render transparent nodes' front faces.
		//!\~french		Les pipelines de rendu utilisé pour dessiner les faces avant des noeuds transparents.
		std::unordered_map< PipelineFlags, PipelineUPtr, PipelineFlagsHasher, PipelineFlagsComparer > m_backTransparentPipelines;
		//!\~english	The objects rendered in the last frame.
		//!\~french		Les objets dessinés lors de la dernière frame.
		std::vector< std::reference_wrapper< ObjectRenderNodeBase const > > m_renderedObjects;
		//!\~english	Tells if the technique uses multisampling.
		//!\~french		Dit si la technique utilise le multiéchantillonnage.
		bool m_multisampling{ false };
	};
}

#endif
