/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "OpaqueModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class LightingPass
	{
	private:
		using LightPasses = std::array< std::unique_ptr< LightPass >, size_t( LightType::eCount ) >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	scene		The scene.
		 *\param[in]	gpResult	The geometry pass buffers.
		 *\param[in]	depthView	The depth buffer attach.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	scene		La scène.
		 *\param[in]	gpResult	Les tampons de la geometry pass.
		 *\param[in]	depthView	L'attache du tampon de profondeur.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 */
		LightingPass( Engine & engine
			, castor::Size const & size
			, Scene const & scene
			, GeometryPassResult const & gpResult
			, ashes::ImageView const & depthView
			, SceneUbo & sceneUbo
			, GpInfoUbo & gpInfoUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LightingPass();
		/**
		 *\~english
		 *\brief		Updates opaque pass.
		 *\param[out]	info	Receives the render informations.
		 *\param[in]	scene	The rendered scene.
		 *\param[in]	camera	The viewer camera.
		 *\param[in]	jitter	The jittering value.
		 *\~french
		 *\brief		Met à jour la passe opaque.
		 *\param[out]	info	Reçoit les informations de rendu.
		 *\param[in]	scene	La scène rendue.
		 *\param[in]	camera	La caméra par laquelle la scène est rendue.
		 *\param[in]	jitter	La valeur de jittering.
		 */
		void update( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, castor::Point2f const & jitter );
		/**
		 *\~english
		 *\brief		Renders the light passes on currently bound framebuffer.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The viewing camera.
		 *\param[in]	gp		The geometry pass result.
		 *\param[out]	toWait	The semaphore from previous render pass.
		 *\~french
		 *\brief		Dessine les passes d'éclairage sur le tampon d'image donné.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra.
		 *\param[in]	gp		Le résultat de la geometry pass.
		 *\param[out]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		ashes::Semaphore const & render( Scene const & scene
			, Camera const & camera
			, GeometryPassResult const & gp
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\~english
		 *\return		The light pass diffuse result.
		 *\~french
		 *\return		Le résultat diffus de la passe d'éclairage.
		 */
		inline TextureUnit const & getDiffuse()const
		{
			return m_diffuse;
		}
		/**
		 *\~english
		 *\return		The light pass specular result.
		 *\~french
		 *\return		Le résultat spéculaire de la passe d'éclairage.
		 */
		inline TextureUnit const & getSpecular()const
		{
			return m_specular;
		}

	private:
		void doUpdateLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, uint32_t & index
			, RenderInfo & info );
		ashes::Semaphore const & doRenderLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, GeometryPassResult const & gp
			, ashes::Semaphore const & toWait
			, uint32_t & index );

	private:
		Engine & m_engine;
		castor::Size const m_size;
		TextureUnit m_depth;
		TextureUnit m_diffuse;
		TextureUnit m_specular;
		LightPasses m_lightPass;
		LightPasses m_lightPassShadow;
		RenderPassTimerSPtr m_timer;
		ashes::FencePtr m_fence;
		ashes::ImageView const & m_srcDepth;
		ashes::CommandBufferPtr m_blitDepthCommandBuffer;
		ashes::SemaphorePtr m_blitDepthSemaphore;
	};
}

#endif
