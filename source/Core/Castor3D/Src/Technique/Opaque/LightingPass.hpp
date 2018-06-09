/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightingPass_H___
#define ___C3D_LightingPass_H___

#include "ReflectionPass.hpp"

namespace castor3d
{
	class OpaquePass;
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		Handles the light passes.
	\~french
	\brief		Gère les passes d'éclairage.
	*/
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
		 *\param[in]	opaque		The opaque pass.
		 *\param[in]	depthAttach	The depth buffer attach.
		 *\param[in]	sceneUbo	The scene UBO.
		 *\param[in]	gpInfoUbo	The geometry pass UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	scene		La scène.
		 *\param[in]	opaque		La passe opaque.
		 *\param[in]	depthAttach	L'attache du tampon de profondeur.
		 *\param[in]	sceneUbo	L'UBO de scène.
		 *\param[in]	gpInfoUbo	L'UBO de la geometry pass.
		 */
		LightingPass( Engine & engine
			, castor::Size const & size
			, Scene const & scene
			, GeometryPassResult const & gpResult
			, OpaquePass & opaque
			, renderer::TextureView const & depthView
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
		 *\brief		Renders the light passes on currently bound framebuffer.
		 *\param[in]	scene	The scene.
		 *\param[in]	camera	The viewing camera.
		 *\param[in]	gp		The geometry pass result.
		 *\param[out]	info	The render infos.
		 *\~french
		 *\brief		Dessine les passes d'éclairage sur le tampon d'image donné.
		 *\param[in]	scene	La scène.
		 *\param[in]	camera	La caméra.
		 *\param[in]	gp		Le résultat de la geometry pass.
		 *\param[out]	info	Les informations de rendu.
		 */
		renderer::Semaphore const & render( Scene const & scene
			, Camera const & camera
			, GeometryPassResult const & gp
			, renderer::Semaphore const & toWait
			, RenderInfo & info );
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
		renderer::Semaphore const & doRenderLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, GeometryPassResult const & gp
			, renderer::Semaphore const & toWait
			, uint32_t & index
			, RenderInfo & info );

	private:
		castor::Size const m_size;
		TextureUnit m_diffuse;
		TextureUnit m_specular;
		LightPasses m_lightPass;
		LightPasses m_lightPassShadow;
		RenderPassTimerSPtr m_timer;
		renderer::FencePtr m_fence;
	};
}

#endif
