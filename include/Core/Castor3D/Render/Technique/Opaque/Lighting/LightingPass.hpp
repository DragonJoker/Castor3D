/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "LightingModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/Technique/Opaque/OpaqueResolvePass.hpp"
#include "Castor3D/Render/Technique/Opaque/Lighting/LightPassResult.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/RsmConfig.hpp"

#include <CastorUtils/Design/DelayedInitialiser.hpp>

#include <ashespp/Command/CommandBuffer.hpp>
#include <ashespp/Sync/Fence.hpp>
#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class LightingPass
	{
	public:
		enum class Type
		{
			eNoShadow = 0u,
			eShadowNoGI,
			eShadowRsmGI,
			eShadowLpvGI,
			eShadowLpvGGI,
			eShadowLayeredLpvGI,
			eShadowLayeredLpvGGI,
			CU_ScopedEnumBounds( eNoShadow ),
		};
		static_assert( uint32_t( Type::eCount ) == uint32_t( GlobalIlluminationType::eCount ) + 1u );
		using DelayedLightPass = castor::DelayedInitialiserT< LightPass >;
		using TypeLightPasses = std::array< DelayedLightPass, size_t( LightType::eCount ) >;
		using LightPasses = std::array< TypeLightPasses, size_t( Type::eCount ) >;

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
			, RenderDevice const & device
			, castor::Size const & size
			, Scene & scene
			, OpaquePassResult const & gpResult
			, ShadowMapResult const & smDirectionalResult
			, ShadowMapResult const & smPointResult
			, ShadowMapResult const & smSpotResult
			, ashes::ImageView const & depthView
			, SceneUbo & sceneUbo
			, GpInfoUbo const & gpInfoUbo );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~LightingPass();
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
			, OpaquePassResult const & gp
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		void accept( PipelineVisitorBase & visitor );
		/**
		 *\~english
		 *\return		The light pass diffuse result.
		 *\~french
		 *\return		Le résultat diffus de la passe d'éclairage.
		 */
		inline LightPassResult const & getResult()const
		{
			return m_result;
		}

	private:
		ashes::Semaphore const & doRenderLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, OpaquePassResult const & gp
			, ashes::Semaphore const & toWait
			, uint32_t & index );
		LightPass * doGetLightPass( LightType lightType )const;
		LightPass * doGetShadowLightPass( LightType lightType
			, GlobalIlluminationType giType )const;

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		castor::Size const m_size;
		LightPassResult m_result;
		LightPasses m_lightPasses;
		RenderPassTimerSPtr m_timer;
		ashes::FencePtr m_fence;
		ashes::ImageView const & m_srcDepth;
		CommandsSemaphore m_blitDepth;
		CommandsSemaphore m_lpResultBarrier;
	};
}

#endif
