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
#ifndef ___C3D_DeferredRendering_H___
#define ___C3D_DeferredRendering_H___

#include "Technique/Opaque/CombinePass.hpp"
#include "Technique/Opaque/LightingPass.hpp"
#include "Technique/Opaque/ReflectionPass.hpp"
#include "Technique/Opaque/SubsurfaceScatteringPass.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		08/06/2017
	\~english
	\brief		The deferred rendering class.
	\~french
	\brief		Classe de rendu différé.
	*/
	class DeferredRendering
	{
	public:
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 */
		DeferredRendering( Engine & engine
			, OpaquePass & opaquePass
			, FrameBuffer & frameBuffer
			, FrameBufferAttachment & depthAttach
			, castor::Size const & size
			, Scene const & scene
			, SsaoConfig const & config );
		/**
		 *\~english
		 *\brief		Destroys deferred rendering related stuff.
		 *\~french
		 *\brief		Détruit les données liées au deferred rendering.
		 */
		~DeferredRendering();
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		void render( RenderInfo & info
			, Scene const & scene
			, Camera const & camera
			, ShadowMapLightTypeArray & shadowMaps
			, castor::Point2r const & jitter
			, TextureUnit const & velocity );
		/**
		 *\~english
		 *\brief		Displays debug data on screen.
		 *\~french
		 *\brief		Dessine les données de débogage sur l'écran.
		 */
		void debugDisplay()const;
		/**
		 *\~english
		 *\brief		Blits the depth and stencil from the geometry pass into given framebuffer.
		 *\param[out]	p_fbo	The target framebuffer.
		 *\~french
		 *\brief		Blitte la profondeur et le stencil de la geometry pass dans le tampon d'image donné.
		 *\param[out]	p_fbo	Le tampon d'image cible.
		 */
		void blitDepthInto( FrameBuffer & fbo );

	private:
		using GeometryBufferTextures = std::array< TextureUnitUPtr, size_t( DsTexture::eCount ) >;
		using GeometryBufferAttachs = std::array< TextureAttachmentSPtr, size_t( DsTexture::eCount ) >;

		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The SSAO configuration.
		//!\~french		La configuration du SSAO.
		SsaoConfig const & m_ssaoConfig;
		//!\~english	The pass used to render opaque nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds opaques.
		OpaquePass & m_opaquePass;
		//!\~english	The target framebuffer.
		//!\~french		Le tampon d'image ciblé.
		FrameBuffer & m_frameBuffer;
		//!\~english	The target framebuffer's depth attachment.
		//!\~french		L'attache profondeur du tampon d'image ciblé.
		FrameBufferAttachment & m_depthAttach;
		//!\~english	The render area dimension.
		//!\~french		Les dimensions de l'aire de rendu.
		castor::Size m_size;
		//!\~english	The uniform buffer containing the scene data.
		//!\~french		Le tampon d'uniformes contenant les données de scène.
		SceneUbo m_sceneUbo;
		//!\~english	The geometry pass informations.
		//!\~french		Les informations de la passe de géométrie.
		GpInfoUbo m_gpInfoUbo;
		//!\~english	The fog pass.
		//!\~french		La passe de brouillard.
		std::unique_ptr< LightingPass > m_lightingPass;
		//!\~english	The combination pass.
		//!\~french		La passe de combinaison.
		std::unique_ptr< CombinePass > m_combinePass;
		//!\~english	The reflection pass.
		//!\~french		La passe de réflexion.
		std::unique_ptr< ReflectionPass > m_reflection;
		//!\~english	The reflection pass.
		//!\~french		La passe de réflexion.
		std::unique_ptr< SubsurfaceScatteringPass > m_subsurfaceScattering;
		//!\~english	The various textures for deferred shading.
		//!\~french		Les diverses textures pour le deferred shading.
		GeometryBufferTextures m_geometryPassResult;
		//!\~english	The deferred shading frame buffer.
		//!\~french		Le tampon d'image pour le deferred shading.
		FrameBufferSPtr m_geometryPassFrameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		GeometryBufferAttachs m_geometryPassTexAttachs;
	};
}

#endif
