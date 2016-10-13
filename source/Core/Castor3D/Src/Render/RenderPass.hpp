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

#include <unordered_map>

namespace Castor3D
{
	inline bool operator<( PipelineFlags const & p_lhs, PipelineFlags const & p_rhs )
	{
		return p_lhs.m_colourBlendMode < p_rhs.m_colourBlendMode
			|| ( p_lhs.m_colourBlendMode == p_rhs.m_colourBlendMode
				 && ( p_lhs.m_alphaBlendMode < p_rhs.m_alphaBlendMode
					  || ( p_lhs.m_alphaBlendMode == p_rhs.m_alphaBlendMode
						   && ( p_lhs.m_textureFlags < p_rhs.m_textureFlags
								|| ( p_lhs.m_textureFlags == p_rhs.m_textureFlags
									 && ( p_lhs.m_programFlags < p_rhs.m_programFlags
										  || ( p_lhs.m_programFlags == p_rhs.m_programFlags
											   && p_lhs.m_sceneFlags < p_rhs.m_sceneFlags ) ) ) ) ) ) );
	}
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
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API Castor::String GetPixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const;
		/**
		 *\~english
		 *\brief		Prepares the pipeline matching the given flags.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Prépare le pipeline qui correspond aux indicateurs donnés.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API void PreparePipeline( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t & p_programFlags, uint8_t p_sceneFlags, bool p_twoSided );
		/**
		 *\~english
		 *\brief		Retrieves the opaque pipeline matching the given flags, for front face culling.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le pipeline opaque qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API Pipeline & GetOpaquePipelineFront( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags );
		/**
		 *\~english
		 *\brief		Retrieves the opaque pipeline matching the given flags, for back face culling.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le pipeline opaque qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API Pipeline & GetOpaquePipelineBack( BlendMode p_colourBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags );
		/**
		 *\~english
		 *\brief		Retrieves the transparent pipeline matching the given flags, for front faces culling.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le pipeline transparent qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API Pipeline & GetTransparentPipelineFront( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags );
		/**
		 *\~english
		 *\brief		Retrieves the transparent pipeline matching the given flags, for back face culling.
		 *\param[in]	p_colourBlendMode	The colour blend mode.
		 *\param[in]	p_colourBlendMode	The alpha blend mode.
		 *\param[in]	p_textureFlags		A combination of TextureChannel.
		 *\param[in]	p_programFlags		A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le pipeline transparent qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	p_colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	p_colourBlendMode	Le mode de mélange alpha.
		 *\param[in]	p_textureFlags		Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags		Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API Pipeline & GetTransparentPipelineBack( BlendMode p_colourBlendMode, BlendMode p_alphaBlendMode, uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags );
		/**
		 *\~english
		 *\brief		Creates an animated render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\param[in]	p_submesh	The submesh.
		 *\param[in]	p_primitive	The geometry.
		 *\param[in]	p_skeleton	The animated skeleton.
		 *\param[in]	p_mesh		The animated mesh.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu animé.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\param[in]	p_submesh	Le sous-maillage.
		 *\param[in]	p_primitive	La géométrie.
		 *\param[in]	p_skeleton	Le squelette animé.
		 *\param[in]	p_mesh		Le maillage animé.
		 *\return		Le noeud de rendu.
		 */
		C3D_API virtual AnimatedGeometryRenderNode CreateAnimatedNode( Pass & p_pass
																	   , Pipeline & p_pipeline
																	   , Submesh & p_submesh
																	   , Geometry & p_primitive
																	   , AnimatedSkeletonSPtr p_skeleton
																	   , AnimatedMeshSPtr p_mesh );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\param[in]	p_submesh	The submesh.
		 *\param[in]	p_primitive	The geometry.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\param[in]	p_submesh	Le sous-maillage.
		 *\param[in]	p_primitive	La géométrie.
		 *\return		Le noeud de rendu.
		 */
		C3D_API virtual StaticGeometryRenderNode CreateStaticNode( Pass & p_pass
																   , Pipeline & p_pipeline
																   , Submesh & p_submesh
																   , Geometry & p_primitive );
		/**
		*\~english
		*\brief		Creates a static render node.
		*\param[in]	p_pass		The pass.
		*\param[in]	p_pipeline	The pipeline.
		*\param[in]	p_billboard	The billboard.
		*\return		The render node.
		*\~french
		*\brief		Crée un noeud de rendu statique.
		*\param[in]	p_pass		La passe.
		*\param[in]	p_pipeline	Le pipeline.
		*\param[in]	p_billboard	Le billboard.
		*\return		Le noeud de rendu.
		*/
		C3D_API virtual BillboardRenderNode CreateBillboardNode( Pass & p_pass
																 , Pipeline & p_pipeline
																 , BillboardList & p_billboard );
		/**
		 *\~english
		 *\brief		Updates the opaque pipeline.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour lee pipeline opaque.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API void UpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const;
		/**
		 *\~english
		 *\brief		Updates the transparent pipeline.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour le pipeline transparent.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API void UpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const;
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
		 *\brief		Creates a render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API PassRenderNode DoCreatePassRenderNode( Pass & p_pass, Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Creates a scene render node.
		 *\param[in]	p_pass		The pass.
		 *\param[in]	p_pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu de scène.
		 *\param[in]	p_pass		La passe.
		 *\param[in]	p_pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API SceneRenderNode DoCreateSceneRenderNode( Scene & p_scene, Pipeline & p_pipeline );
		/**
		 *\~english
		 *\brief		Retrieves the shader program matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\param[in]	p_invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le programme shader correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 *\param[in]	p_invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API ShaderProgramSPtr DoGetProgram( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags, bool p_invertNormals )const;

	private:
		/**
		 *\~english
		 *\brief			Modifies the given program flags to make them match the render pass requirements.
		 *\param[in,out]	p_programFlags	A combination of ProgramFlag.
		 *\~french
		 *\brief			Modifie les indicateurs de programme donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	p_programFlags	Une combinaison de ProgramFlag.
		 */
		C3D_API virtual void DoCompleteProgramFlags( uint16_t & p_programFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 */
		C3D_API virtual Castor::String DoGetOpaquePixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	p_textureFlags	A combination of TextureChannel.
		 *\param[in]	p_programFlags	A combination of ProgramFlag.
		 *\param[in]	p_sceneFlags	Scene related flags.
		 *\~french
		 *\brief		Récupère le source du pixel shader correspondant aux flags donnés.
		 *\param[in]	p_textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	p_programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	p_sceneFlags	Les indicateurs relatifs à la scène.
		 */
		C3D_API virtual Castor::String DoGetTransparentPixelShaderSource( uint16_t p_textureFlags, uint16_t p_programFlags, uint8_t p_sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Updates the opaque pipeline.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour lee pipeline opaque.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void DoUpdateOpaquePipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const = 0;
		/**
		 *\~english
		 *\brief		Updates the transparent pipeline.
		 *\param[in]	p_camera	The camera through which the scene is viewed.
		 *\param[in]	p_pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour le pipeline transparent.
		 *\param[in]	p_camera	La caméra à travers laquelle la scène est vue.
		 *\param[in]	p_pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void DoUpdateTransparentPipeline( Camera const & p_camera, Pipeline & p_pipeline, DepthMapArray & p_depthMaps )const = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline for opaque objets render, culling front faces.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets opaques, en supprimant les faces avant.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareOpaqueFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline for opaque objets render, culling back faces.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets opaques, en supprimant les faces arrière.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareOpaqueBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline for transparent objets render, culling front faces.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets transparents, en supprimant les faces avant.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareTransparentFrontPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline for transparent objets render, culling back faces.
		 *\return		The ready to use pipeline.
		 *\~french
		 *\brief		Prépare le pipeline de rendu des objets transparents, en supprimant les faces arrière.
		 *\return		Le pipeline prêt à l'utilisation.
		 */
		C3D_API virtual Pipeline & DoPrepareTransparentBackPipeline( ShaderProgram & p_program, PipelineFlags const & p_flags ) = 0;

	protected:
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		//!\~english	The render queue.
		//!\~french		La file de rendu.
		RenderQueue m_renderQueue;
		//!\~english	The pipelines used to render opaque nodes' back faces.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les faces arrière des noeuds opaques.
		std::map< PipelineFlags, PipelineUPtr > m_frontOpaquePipelines;
		//!\~english	The pipelines used to render opaque nodes' front faces.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les faces avant noeuds opaques.
		std::map< PipelineFlags, PipelineUPtr > m_backOpaquePipelines;
		//!\~english	The pipelines used to render transparent nodes' back faces.
		//!\~french		Les pipeline de rendu utilisé pour dessiner les faces arrière des noeuds transparents.
		std::map< PipelineFlags, PipelineUPtr > m_frontTransparentPipelines;
		//!\~english	The pipelines used to render transparent nodes' front faces.
		//!\~french		Les pipelines de rendu utilisé pour dessiner les faces avant des noeuds transparents.
		std::map< PipelineFlags, PipelineUPtr > m_backTransparentPipelines;
		//!\~english	The objects rendered in the last frame.
		//!\~french		Les objets dessinés lors de la dernière frame.
		std::vector< std::reference_wrapper< ObjectRenderNodeBase const > > m_renderedObjects;
		//!\~english	Tells if the technique uses multisampling.
		//!\~french		Dit si la technique utilise le multiéchantillonnage.
		bool m_multisampling{ false };
	};
}

#endif
