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

#include "Render/RenderInfo.hpp"
#include "Render/RenderQueue.hpp"
#include "Shader/BillboardUbo.hpp"
#include "Shader/MatrixUbo.hpp"
#include "Shader/ModelMatrixUbo.hpp"
#include "Shader/ModelUbo.hpp"
#include "Shader/MorphingUbo.hpp"
#include "Shader/SceneUbo.hpp"
#include "Shader/SkinningUbo.hpp"

#include <unordered_map>

namespace Castor3D
{
	inline bool operator<( PipelineFlags const & lhs, PipelineFlags const & rhs )
	{
		return lhs.m_colourBlendMode < rhs.m_colourBlendMode
			   || ( lhs.m_colourBlendMode == rhs.m_colourBlendMode
					&& ( lhs.m_alphaBlendMode < rhs.m_alphaBlendMode
						 || ( lhs.m_alphaBlendMode == rhs.m_alphaBlendMode
							  && ( lhs.m_textureFlags < rhs.m_textureFlags
								   || ( lhs.m_textureFlags == rhs.m_textureFlags
										&& ( lhs.m_programFlags < rhs.m_programFlags
											 || ( lhs.m_programFlags == rhs.m_programFlags
												  && lhs.m_sceneFlags < rhs.m_sceneFlags ) ) ) ) ) ) );
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
		using DistanceSortedNodeMap = std::multimap< double, std::unique_ptr< DistanceRenderNodeBase > >;

	protected:
		/**
		 *\~english
		 *\brief		Constructor for opaque nodes.
		 *\param[in]	name	The pass name.
		 *\param[in]	engine	The engine.
		 *\param[in]	ignored	The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur pour les noeuds opaques.
		 *\param[in]	name	Le nom de la passe.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	ignored	Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderPass( Castor::String const & name
			, Engine & engine
			, SceneNode const * ignored );
		/**
		 *\~english
		 *\brief		Constructor for transparent nodes.
		 *\param[in]	name	The pass name.
		 *\param[in]	engine	The engine.
		 *\param[in]	oit		The order independant status.
		 *\param[in]	ignored	The geometries attached to this node will be ignored in the render.
		 *\~french
		 *\brief		Constructeur pour les noeuds transparents.
		 *\param[in]	name	Le nom de la passe.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	oit		Le statut de rendu indépendant de l'ordre des objets.
		 *\param[in]	ignored	Les géométries attachées à ce noeud seront ignorées lors du rendu.
		 */
		C3D_API RenderPass( Castor::String const & name
			, Engine & engine
			, bool oit
			, SceneNode const * ignored );

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
		 *\brief		Initialises the pass.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( Castor::Size const & size );
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Updates the render pass.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met à jour la passe de rendu.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void Update( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\param[in]	programFlags	Combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API GLSL::Shader GetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API GLSL::Shader GetPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API GLSL::Shader GetGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Prepares the pipeline matching the given flags.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	twoSided		Tells if the pass is two sided.
		 *\~french
		 *\brief		Prépare le pipeline qui correspond aux indicateurs donnés.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	twoSided		Dit si la passe est sur les deux faces.
		 */
		C3D_API void PreparePipeline( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, ComparisonFunc alphaFunc
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags
			, bool twoSided );
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for front face culling.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces avant supprimées.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * GetPipelineFront( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, ComparisonFunc alphaFunc
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Retrieves the pipeline matching the given flags, for back face culling.
		 *\param[in]	colourBlendMode	The colour blend mode.
		 *\param[in]	alphaBlendMode	The alpha blend mode.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\return		The ready to use pipeline, \p nullptr if none is available for wanted configuration.
		 *\~french
		 *\brief		Récupère le pipeline qui correspond aux indicateurs donnés, pour les faces arrière supprimées.
		 *\param[in]	colourBlendMode	Le mode de mélange de couleurs.
		 *\param[in]	alphaBlendMode	Le mode de mélange alpha.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\return		Le pipeline prêt à l'utilisation, \p nullptr si aucun pipeline n'est disponible pour la configuration voulue.
		 */
		C3D_API RenderPipeline * GetPipelineBack( BlendMode colourBlendMode
			, BlendMode alphaBlendMode
			, ComparisonFunc alphaFunc
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\~english
		 *\brief		Creates an animated render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	primitive	The geometry.
		 *\param[in]	skeleton	The animated skeleton.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu animé.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	primitive	La géométrie.
		 *\param[in]	skeleton	Le squelette animé.
		 *\return		Le noeud de rendu.
		 */
		C3D_API SkinningRenderNode CreateSkinningNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedSkeleton & skeleton );
		/**
		 *\~english
		 *\brief		Creates an animated render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	primitive	The geometry.
		 *\param[in]	mesh		The animated mesh.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu animé.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	primitive	La géométrie.
		 *\param[in]	mesh		Le maillage animé.
		 *\return		Le noeud de rendu.
		 */
		C3D_API MorphingRenderNode CreateMorphingNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive
			, AnimatedMesh & mesh );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	submesh		The submesh.
		 *\param[in]	primitive	The geometry.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	submesh		Le sous-maillage.
		 *\param[in]	primitive	La géométrie.
		 *\return		Le noeud de rendu.
		 */
		C3D_API StaticRenderNode CreateStaticNode( Pass & pass
			, RenderPipeline & pipeline
			, Submesh & submesh
			, Geometry & primitive );
		/**
		 *\~english
		 *\brief		Creates a static render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\param[in]	billboard	The billboard.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu statique.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\param[in]	billboard	Le billboard.
		 *\return		Le noeud de rendu.
		 */
		C3D_API BillboardRenderNode CreateBillboardNode( Pass & pass
			, RenderPipeline & pipeline
			, BillboardBase & billboard );
		/**
		 *\~english
		 *\brief		Updates the opaque pipeline.
		 *\param[in]	pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour lee pipeline opaque.
		 *\param[in]	pipeline	Le pipeline de rendu.
		 */
		C3D_API void UpdatePipeline( RenderPipeline & pipeline )const;
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	textureFlags	A combination of TextureChannel.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Une combinaison de SceneFlag.
		 */
		C3D_API void UpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const;
		/**
		 *\~english
		 *\return		The multsampling status.
		 *\~french
		 *\return		Le statut d'échantillonnage.
		 */
		inline bool IsOrderIndependent()const
		{
			return m_oit;
		}

	protected:
		/**
		 *\~english
		 *\brief		Creates a render node.
		 *\param[in]	pass		The pass.
		 *\param[in]	pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu.
		 *\param[in]	pass		La passe.
		 *\param[in]	pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API PassRenderNode DoCreatePassRenderNode( Pass & pass
			, RenderPipeline & pipeline );
		/**
		 *\~english
		 *\brief		Creates a scene render node.
		 *\param[in]	scene		The scene.
		 *\param[in]	pipeline	The pipeline.
		 *\return		The render node.
		 *\~french
		 *\brief		Crée un noeud de rendu de scène.
		 *\param[in]	scene		La scène.
		 *\param[in]	pipeline	Le pipeline.
		 *\return		Le noeud de rendu.
		 */
		C3D_API SceneRenderNode DoCreateSceneRenderNode( Scene & scene
			, RenderPipeline & pipeline );
		/**
		 *\~english
		 *\brief		Retrieves the shader program matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le programme shader correspondant aux flags donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API ShaderProgramSPtr DoGetProgram( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instanced nodes.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t DoCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
			, VertexBuffer & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instanced nodes.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t DoCopyNodesMatrices( StaticRenderNodeArray const & renderNodes
			, VertexBuffer & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instanced nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes		The instanced nodes.
		 *\param[in]	matrixBuffer	The matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes		Les noeuds instanciés.
		 *\param[in]	matrixBuffer	Le tampon de matrices.
		 */
		C3D_API uint32_t DoCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
			, VertexBuffer & matrixBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes		The instanced nodes.
		 *\param[in]		matrixBuffer	The matrix buffer.
		 *\param[in, out]	info			Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes		Les noeuds instanciés.
		 *\param[in]		matrixBuffer	Le tampon de matrices.
		 *\param[in,out]	info			Reçoit les informations de rendu.
		 */
		C3D_API uint32_t DoCopyNodesMatrices( SkinningRenderNodeArray const & renderNodes
			, VertexBuffer & matrixBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Copies the instanced skinned nodes model matrices into given matrix buffer.
		 *\param[in]	renderNodes	The instanced nodes.
		 *\param[in]	bonesBuffer	The bones matrix buffer.
		 *\~french
		 *\brief		Copie les matrices de noeuds skinnés instanciés dans le tampon de matrices donné.
		 *\param[in]	renderNodes	Les noeuds instanciés.
		 *\param[in]	bonesBuffer	Le tampon de matrices des os.
		 */
		C3D_API uint32_t DoCopyNodesBones( SkinningRenderNodeArray const & renderNodes
			, ShaderStorageBuffer & bonesBuffer )const;
		/**
		 *\~english
		 *\brief			Copies the instanced skinned nodes model matrices into given matrix buffer.
		 *\remarks			The nodes which are copied will be registered in the rendered nodes list.
		 *\param[in]		renderNodes	The instanced nodes.
		 *\param[in]		bonesBuffer	The bones matrix buffer.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Copie les matrices de noeuds skinnés instanciés dans le tampon de matrices donné.
		 *\remarks			Les noeuds pour lesquels les matrices sont copiées seront enregistrés dans la liste des noeuds dessinés.
		 *\param[in]		renderNodes	Les noeuds instanciés.
		 *\param[in]		bonesBuffer	Le tampon de matrices des os.
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API uint32_t DoCopyNodesBones( SkinningRenderNodeArray const & renderNodes
			, ShaderStorageBuffer & bonesBuffer
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param		camera	La caméra regardant la scène.
		 */
		C3D_API void DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param		camera		The viewing camera.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param		camera		La caméra regardant la scène.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief			Renders instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void DoRender( SubmeshStaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRender( StaticRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( StaticRenderNodesByPipelineMap & nodes
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param		camera	La caméra regardant la scène.
		 */
		C3D_API void DoRender( StaticRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param		camera		The viewing camera.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param		camera		La caméra regardant la scène.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( StaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void DoRender( StaticRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRender( SkinningRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( SkinningRenderNodesByPipelineMap & nodes
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param		camera	La caméra regardant la scène.
		 */
		C3D_API void DoRender( SkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param		camera		The viewing camera.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param		camera		La caméra regardant la scène.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( SkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void DoRender( SkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 */
		C3D_API void DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param		camera	La caméra regardant la scène.
		 */
		C3D_API void DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param		camera		The viewing camera.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param		camera		La caméra regardant la scène.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief			Renders instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void DoRender( SubmeshSkinningRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRender( MorphingRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( MorphingRenderNodesByPipelineMap & nodes
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes	The render nodes.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param		camera	La caméra regardant la scène.
		 */
		C3D_API void DoRender( MorphingRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders non instanced submeshes.
		 *\param[in]	nodes		The render nodes.
		 *\param		camera		The viewing camera.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des sous maillages non instanciés.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param		camera		La caméra regardant la scène.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( MorphingRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief			Renders non instanced submeshes.
		 *\param[in]		nodes		The render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des sous maillages non instanciés.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void DoRender( MorphingRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes		The render nodes.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes		Les noeuds de rendu.
		 */
		C3D_API void DoRender( BillboardRenderNodesByPipelineMap & nodes )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes		The render nodes.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( BillboardRenderNodesByPipelineMap & nodes
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes	The render nodes.
		 *\param		camera	The viewing camera.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes	Les noeuds de rendu.
		 *\param		camera	La caméra regardant la scène.
		 */
		C3D_API void DoRender( BillboardRenderNodesByPipelineMap & nodes
			, Camera const & camera )const;
		/**
		 *\~english
		 *\brief		Renders billboards.
		 *\param[in]	nodes		The render nodes.
		 *\param		camera		The viewing camera.
		 *\param[in]	depthMaps	The depth (shadows and other) maps.
		 *\~french
		 *\brief		Dessine des billboards.
		 *\param[in]	nodes		Les noeuds de rendu.
		 *\param		camera		La caméra regardant la scène.
		 *\param[in]	depthMaps	Les textures de profondeur (ombres et autres).
		 */
		C3D_API void DoRender( BillboardRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps )const;
		/**
		 *\~english
		 *\brief			Renders billboards.
		 *\param[in]		nodes		The render nodes.
		 *\param			camera		The viewing camera.
		 *\param[in]		depthMaps	The depth (shadows and other) maps.
		 *\param[in, out]	info		Receives the render informations.
		 *\~french
		 *\brief			Dessine des billboards.
		 *\param[in]		nodes		Les noeuds de rendu.
		 *\param			camera		La caméra regardant la scène.
		 *\param[in]		depthMaps	Les textures de profondeur (ombres et autres).
		 *\param[in,out]	info		Reçoit les informations de rendu.
		 */
		C3D_API void DoRender( BillboardRenderNodesByPipelineMap & nodes
			, Camera const & camera
			, DepthMapArray & depthMaps
			, RenderInfo & info )const;

	private:
		/**
		 *\~english
		 *\brief		Initialises the pass.
		 *\param		size	The pass needed dimensions.
		 *\return		\p true on ok.
		 *\~french
		 *\brief		Initialise la passe.
		 *\param		size	Les dimensions voulues pour la passe.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool DoInitialise( Castor::Size const & size ) = 0;
		/**
		 *\~english
		 *\brief		Cleans up the pass.
		 *\~french
		 *\brief		Nettoie la passe.
		 */
		C3D_API virtual void DoCleanup() = 0;
		/**
		 *\~english
		 *\brief		Updates the specific data.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Met les données spécifiques.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API virtual void DoUpdate( RenderQueueArray & queues ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the vertex shader source matching the given flags.
		 *\param[in]	textureFlags	Bitwise ORed TextureChannel.
		 *\param[in]	programFlags	Bitwise ORed ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	invertNormals	Tells if the normals must be inverted, in the program.
		 *\~french
		 *\brief		Récupère le source du vertex shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Combinaison de TextureChannel.
		 *\param[in]	programFlags	Combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	invertNormals	Dit si les normales doivent être inversées, dans le programme.
		 */
		C3D_API virtual GLSL::Shader DoGetVertexShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\~english
		 *\brief		Retrieves the geometry shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\~french
		 *\brief		Récupère le source du geometry shader correspondant aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 */
		C3D_API virtual GLSL::Shader DoGetGeometryShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API virtual GLSL::Shader DoGetLegacyPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API virtual GLSL::Shader DoGetPbrMRPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const = 0;
		/**
		 *\~english
		 *\brief		Retrieves the pixel shader source matching the given flags.
		 *\param[in]	textureFlags	A combination of TextureChannel.
		 *\param[in]	programFlags	A combination of ProgramFlag.
		 *\param[in]	sceneFlags		Scene related flags.
		 *\param[in]	alphaFunc		The alpha test function.
		 *\~french
		 *\brief		Récupère le source du pixel shader qui correspond aux indicateurs donnés.
		 *\param[in]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in]	sceneFlags		Les indicateurs relatifs à la scène.
		 *\param[in]	alphaFunc		La fonction de test alpha.
		 */
		C3D_API virtual GLSL::Shader DoGetPbrSGPixelShaderSource( TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, ComparisonFunc alphaFunc )const = 0;
		/**
		 *\~english
		 *\brief			Modifies the given flags to make them match the render pass requirements.
		 *\param[in,out]	textureFlags	A combination of TextureChannel.
		 *\param[in,out]	programFlags	A combination of ProgramFlag.
		 *\param[in,out]	sceneFlags		A combination of SceneFlag.
		 *\~french
		 *\brief			Modifie les indicateurs donnés pour le faire correspondre au pré-requis de la passe de rendus.
		 *\param[in,out]	textureFlags	Une combinaison de TextureChannel.
		 *\param[in,out]	programFlags	Une combinaison de ProgramFlag.
		 *\param[in,out]	sceneFlags		Une combinaison de SceneFlag.
		 */
		C3D_API virtual void DoUpdateFlags( TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const = 0;
		/**
		 *\~english
		 *\brief		Updates the pipeline.
		 *\param[in]	pipeline	The render pipeline.
		 *\~french
		 *\brief		Met à jour le pipeline.
		 *\param[in]	pipeline	Le pipeline de rendu.
		 */
		C3D_API virtual void DoUpdatePipeline( RenderPipeline & pipeline  )const = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling front faces.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces avant.
		 */
		C3D_API virtual void DoPrepareFrontPipeline( ShaderProgram & program
			, PipelineFlags const & flags ) = 0;
		/**
		 *\~english
		 *\brief		Prepares the pipeline, culling back faces.
		 *\~french
		 *\brief		Prépare le pipeline de rendu, en supprimant les faces arrière.
		 */
		C3D_API virtual void DoPrepareBackPipeline( ShaderProgram & program
			, PipelineFlags const & flags ) = 0;

	protected:
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		//!\~english	The render queue.
		//!\~french		La file de rendu.
		RenderQueue m_renderQueue;
		//!\~english	Tells if this pass is used for opaque render nodes.
		//!\~french		Dit si cette passe est utilisée pour les noeuds de rendu opaques.
		bool m_opaque{ false };
		//!\~english	The pipelines used to render nodes' back faces.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les faces arrière des noeuds.
		std::map< PipelineFlags, RenderPipelineUPtr > m_frontPipelines;
		//!\~english	The pipelines used to render nodes' front faces.
		//!\~french		Les pipelines de rendu utilisés pour dessiner les faces avant noeuds.
		std::map< PipelineFlags, RenderPipelineUPtr > m_backPipelines;
		//!\~english	The geometries buffers.
		//!\~french		Les tampons de géométries.
		std::set< GeometryBuffersSPtr > m_geometryBuffers;
		//!\~english	Tells if the technique uses order independant rendering.
		//!\~french		Dit si la technique utilise le rendu indépendant de l'ordre.
		bool m_oit{ false };
		//!\~english	The uniform buffer containing the scene data.
		//!\~french		Le tampon d'uniformes contenant les données de scène.
		SceneUbo m_sceneUbo;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		MatrixUbo m_matrixUbo;
		//!\~english	The uniform buffer containing model data.
		//!\~french		Le tampon d'uniformes contenant les données du modèle.
		ModelUbo m_modelUbo;
		//!\~english	The uniform buffer containing matrices data.
		//!\~french		Le tampon d'uniformes contenant les données de matrices.
		ModelMatrixUbo m_modelMatrixUbo;
		//!\~english	The uniform buffer containing billboard data.
		//!\~french		Le tampon d'uniformes contenant les données de billboard.
		BillboardUbo m_billboardUbo;
		//!\~english	The uniform buffer containing skinning animation data.
		//!\~french		Le tampon d'uniformes contenant les données d'animation de skinning.
		SkinningUbo m_skinningUbo;
		//!\~english	The uniform buffer containing morphing animation data.
		//!\~french		Le tampon d'uniformes contenant les données d'animation de morphing.
		MorphingUbo m_morphingUbo;
		//!\~english	The render pass timer.
		//!\~french		Le timer de la passe de rendu.
		RenderPassTimerSPtr m_timer;
	};
}

#endif
