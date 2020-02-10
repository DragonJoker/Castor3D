/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShadowMap_H___
#define ___C3D_ShadowMap_H___

#include "Castor3D/Texture/TextureUnit.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>

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
		struct PassData
		{
			std::unique_ptr< MatrixUbo > matrixUbo;
			CameraSPtr camera;
			SceneCullerUPtr culler;
			ShadowMapPassSPtr pass;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	shadowMap	The shadow map.
		 *\param[in]	linearMap	The linear depth map.
		 *\param[in]	passes		The passes used to render map.
		 *\param[in]	count		The passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	shadowMap	La texture d'ombres.
		 *\param[in]	linearMap	La texture de profondeur linéaire.
		 *\param[in]	passes		Les passes utilisées pour rendre cette texture.
		 *\param[in]	count		Le nombre de passes.
		 */
		C3D_API ShadowMap( Engine & engine
			, TextureUnit && shadowMap
			, TextureUnit && linearMap
			, std::vector< PassData > && passes
			, uint32_t count );
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
		 *\brief		Renders the light's shadow map.
		 *\param[out]	toWait	The semaphore from previous render pass.
		 *\param[out]	index	The map index.
		 *\~french
		 *\brief		Dessine la shadow map de la lumière.
		 *\param[out]	toWait	Le sémaphore de la précédente passe de rendu.
		 *\param[out]	index	L'indice de la texture.
		 */
		C3D_API virtual ashes::Semaphore const & render( ashes::Semaphore const & toWait
			, uint32_t index/* = 0u*/ ) = 0;
		/**
		 *\~english
		 *\brief		Dumps the shadow map on screen.
		 *\param[in]	renderPass	The render pass to use.
		 *\param[in]	frameBuffer	The framebuffer receiving the render.
		 *\param[in]	size		The dump dimensions.
		 *\param[in]	index		The shadow map index (to compute its position).
		 *\~french
		 *\brief		Dumpe la texture d'ombres sur l'écran.
		 *\param[in]	renderPass	La passe de rendu à utiliser.
		 *\param[in]	frameBuffer	Le framebuffer recevant le rendu.
		 *\param[in]	size		Les dimensions d'affichage.
		 *\param[in]	index		L'indice de la texture d'ombres (pour calculer sa position).
		 */
		C3D_API virtual void debugDisplay( ashes::RenderPass const & renderPass
			, ashes::FrameBuffer const & frameBuffer
			, castor::Size const & size
			, uint32_t index ) = 0;
		/**
		 *\copydoc		castor3d::RenderPass::updateFlags
		 */
		C3D_API void updateFlags( PipelineFlags & flags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getVertexShaderSource
		 */
		C3D_API ShaderPtr getVertexShaderSource( PipelineFlags const & flags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getGeometryShaderSource
		 */
		C3D_API ShaderPtr getGeometryShaderSource( PipelineFlags const & flags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getPixelShaderSource
		 */
		C3D_API ShaderPtr getPixelShaderSource( PipelineFlags const & flags )const;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API ashes::Sampler const & getLinearSampler()const;
		C3D_API ashes::Sampler const & getVarianceSampler()const;
		C3D_API ashes::ImageView const & getLinearView()const;
		C3D_API ashes::ImageView const & getVarianceView()const;
		C3D_API virtual ashes::ImageView const & getLinearView( uint32_t index )const;
		C3D_API virtual ashes::ImageView const & getVarianceView( uint32_t index )const;
		inline TextureUnit & getTexture()
		{
			return m_shadowMap;
		}

		inline TextureUnit const & getTexture()const
		{
			return m_shadowMap;
		}

		inline TextureUnit & getLinearDepth()
		{
			return m_linearMap;
		}

		inline TextureUnit const & getLinearDepth()const
		{
			return m_linearMap;
		}

		inline uint32_t getCount()const
		{
			return m_count;
		}
		/**@}*/

	private:
		/**
		 *\~english
		 *\brief		Initialises the shadow map's depth raw format.
		 *\~french
		 *\brief		Initialise le format de profondeur brute de la shadow map.
		 */
		C3D_API virtual void doInitialiseDepthFormat() = 0;
		/**
		 *\~english
		 *\brief		Initialises the light type specific data.
		 *\~french
		 *\brief		Initialise les données spécifiques au type de source lumineuse.
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
		C3D_API virtual void doUpdateFlags( PipelineFlags & flags )const = 0;
		/**
		 *\copydoc		castor3d::RenderPass::getVertexShaderSource
		 */
		C3D_API virtual ShaderPtr doGetVertexShaderSource( PipelineFlags const & flags )const = 0;
		/**
		 *\copydoc		castor3d::RenderPass::getGeometryShaderSource
		 */
		C3D_API virtual ShaderPtr doGetGeometryShaderSource( PipelineFlags const & flags )const;
		/**
		 *\copydoc		castor3d::RenderPass::getPixelShaderSource
		 */
		C3D_API virtual ShaderPtr doGetPixelShaderSource( PipelineFlags const & flags )const = 0;

	protected:
		ashes::FencePtr m_fence;
		std::set< std::reference_wrapper< GeometryBuffers > > m_geometryBuffers;
		std::vector< PassData > m_passes;
		uint32_t m_count;
		ashes::SemaphorePtr m_finished;
		TextureUnit m_shadowMap;
		TextureUnit m_linearMap;
		bool m_initialised{ false };
	};
}

#endif
