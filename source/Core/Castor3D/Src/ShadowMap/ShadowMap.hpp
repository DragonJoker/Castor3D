/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMap_H___
#define ___C3D_ShadowMap_H___

#include "Texture/TextureUnit.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		30/08/2016
	\~english
	\brief		Shadow mapping implementation.
	\~french
	\brief		Implémentation du mappage d'ombres.
	*/
	class ShadowMap
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	shadowMap	The shadow map.
		 *\param[in]	linearMap	The linear depth map.
		 *\param[in]	pass		The pass used to render map.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	shadowMap	La texture d'ombres.
		 *\param[in]	linearMap	La texture de profondeur linéaire.
		 *\param[in]	pass		La passe utilisée pour rendre cette texture.
		 */
		C3D_API ShadowMap( Engine & engine
			, TextureUnit && shadowMap
			, TextureUnit && linearMap
			, ShadowMapPassSPtr pass );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShadowMap();
		/**
		 *\~english
		 *\brief		Initialises the frame buffer and light type specific data.
		 *\~french
		 *\brief		Initialise le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the frame buffer and light type specific data.
		 *\~french
		 *\brief		Nettoie le frame buffer et les données spécifiques au type de source lumineuse.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the passes, selecting the lights that will project shadows.
		 *\remarks		Gather the render queues, for further update.
		 *\param[in]	camera	The viewer camera.
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\param[out]	light	The light source.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Met à jour les passes, en sélectionnant les lumières qui projetteront une ombre.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[in]	camera	La caméra de l'observateur.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 *\param[out]	light	La source lumineuse.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API virtual void update( Camera const & camera
			, RenderQueueArray & queues
			, Light & light
			, uint32_t index ) = 0;
		/**
		 *\~english
		 *\brief		Renders the given light's shadow map.
		 *\~french
		 *\brief		Dessine la shadow map de la lumière donnée.
		 */
		C3D_API virtual void render() = 0;
		/**
		 *\~english
		 *\brief		Dumps the shadow map on screen.
		 *\param[in]	size	The dump dimensions.
		 *\param[in]	index	The shadow map index (to compute its position).
		 *\~french
		 *\brief		Dumpe la texture d'ombres sur l'écran.
		 *\param[in]	size	Les dimensions d'affichage.
		 *\param[in]	index	L'indice de la texture d'ombres (pour calculer sa position).
		 */
		C3D_API virtual void debugDisplay( castor::Size const & size, uint32_t index ) = 0;
		/**
		 *\copydoc		castor3d::RenderPass::updateFlags
		 */
		C3D_API void updateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getVertexShaderSource
		 */
		C3D_API glsl::Shader getVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\copydoc		castor3d::RenderPass::getGeometryShaderSource
		 */
		C3D_API glsl::Shader getGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getPixelShaderSource
		 */
		C3D_API glsl::Shader getPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const;
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La texture d'ombres.
		 */
		inline TextureUnit & getTexture()
		{
			return m_shadowMap;
		}
		/**
		 *\~english
		 *\return		The shadow map.
		 *\~english
		 *\return		La texture d'ombres.
		 */
		inline TextureUnit const & getTexture()const
		{
			return m_shadowMap;
		}
		/**
		 *\~english
		 *\return		The linear depth map.
		 *\~english
		 *\return		La texture de profondeur linéaire.
		 */
		inline TextureUnit & getDepth()
		{
			return m_linearMap;
		}
		/**
		 *\~english
		 *\return		The linear depth map.
		 *\~english
		 *\return		La texture de profondeur linéaire.
		 */
		inline TextureUnit const & getDepth()const
		{
			return m_linearMap;
		}

	private:
		/**
		 *\~english
		 *\brief		Initialises the light type specific data.
		 *\param[in]	p_size	The wanted frame buffer dimensions.
		 *\~french
		 *\brief		Initialise les données spécifiques au type de source lumineuse.
		 *\param[in]	p_size	Les dimensions voulues pour le frame buffer.
		 */
		C3D_API virtual void doInitialise() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the light type specific data.
		 *\~french
		 *\brief		Nettoie les données spécifiques au type de source lumineuse.
		 */
		C3D_API virtual void doCleanup() = 0;
		/**
		 *\copydoc		castor3d::RenderPass::updateFlags
		 */
		C3D_API virtual void doUpdateFlags( PassFlags & passFlags
			, TextureChannels & textureFlags
			, ProgramFlags & programFlags
			, SceneFlags & sceneFlags )const = 0;
		/**
		 *\copydoc		castor3d::RenderPass::getVertexShaderSource
		 */
		C3D_API virtual glsl::Shader doGetVertexShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, bool invertNormals )const;
		/**
		 *\copydoc		castor3d::RenderPass::getGeometryShaderSource
		 */
		C3D_API virtual glsl::Shader doGetGeometryShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getPixelShaderSource
		 */
		C3D_API virtual glsl::Shader doGetPixelShaderSource( PassFlags const & passFlags
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags
			, SceneFlags const & sceneFlags
			, renderer::CompareOp alphaFunc )const = 0;

	protected:
		/**
		 *\~english
		 *\brief		Discards materials with alpha blend or alpha func, if needed.
		 *\param[in]	writer			The GLSL writer.
		 *\param[in]	textureFlags	The texture flags.
		 *\param		alphaFunc		The alpha function.
		 *\param[in]	material		The material index.
		 *\param[in]	materials		The materials.
		 *\~french
		 *\brief		Défausse les matériaux avec du mélange alpha ou un fonction alpha, si besoin est.
		 *\param[in]	writer			Le writer GLSL.
		 *\param[in]	textureFlags	Les indicateurs de texture.
		 *\param		alphaFunc		La fonction d'opacité.
		 *\param[in]	material		L'indice du matériau.
		 *\param[in]	materials		Les matériaux.
		 */
		void doDiscardAlpha( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, renderer::CompareOp alphaFunc
			, glsl::Int const & material
			, shader::Materials const & materials )const;

	protected:
		renderer::CommandBufferPtr m_commandBuffer;
		std::set< std::reference_wrapper< GeometryBuffers > > m_geometryBuffers;
		ShadowMapPassSPtr m_pass;
		TextureUnit m_shadowMap;
		TextureUnit m_linearMap;
	};
}

#endif
