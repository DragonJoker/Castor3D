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
			, OpaquePass & opaque
			, FrameBufferAttachment & depthAttach
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
		bool render( Scene const & scene
			, Camera const & camera
			, GeometryPassResult const & gp
			, RenderInfo & info );
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
		/**
		 *\~english
		 *\brief			Renders the light passes for a light sources type on currently bound framebuffer.
		 *\param[in]		scene	The scene.
		 *\param[in]		camera	The viewing camera.
		 *\param[in]		type	The light sources type.
		 *\param[in]		gp		The geometry pass result.
		 *\param[in,out]	first	Tells if this is the first light pass (\p true) or not (\p false).
		 *\~french
		 *\brief			Dessine les passes d'éclairage d'un type de source sur le tampon d'image donné.
		 *\param[in]		scene	La scène.
		 *\param[in]		camera	La caméra.
		 *\param[in]		type	Le type de sources lumineuse.
		 *\param[in]		gp		Le résultat de la geometry pass.
		 *\param[in,out]	first	Dit si cette passe d'éclairage est la première (\p true) ou pas (\p false).
		 */
		void doRenderLights( Scene const & scene
			, Camera const & camera
			, LightType type
			, GeometryPassResult const & gp
			, bool & first );

	private:
		castor::Size const m_size;
		//!\~english	The lighting pass diffuse result.
		//!\~french		L'éclairage diffus de la passe d'éclairage.
		TextureUnit m_diffuse;
		//!\~english	The lighting pass specular result.
		//!\~french		L'éclairage spéculaire de la passe d'éclairage.
		TextureUnit m_specular;
		//!\~english	The target FBO.
		//!\~french		Le FBO cible.
		FrameBufferSPtr m_frameBuffer;
		//!\~english	The attachment between diffuse result and the frame buffer.
		//!\~french		L'attache entre le résultat diffus et le tampon d'images.
		TextureAttachmentSPtr m_diffuseAttach;
		//!\~english	The attachment between specular result and the frame buffer.
		//!\~french		L'attache entre le résultat spéculaire et le tampon d'images.
		TextureAttachmentSPtr m_specularAttach;
		//!\~english	The light passes.
		//!\~french		Les passes de lumière.
		LightPasses m_lightPass;
		//!\~english	The light passes, with shadows.
		//!\~french		Les passes de lumière, avec ombres.
		LightPasses m_lightPassShadow;
		//!\~english	The render pass timer.
		//!\~french		Le timer de la passe de rendu.
		RenderPassTimerSPtr m_timer;
	};
}

#endif
