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
#ifndef ___C3D_RenderTechnique_H___
#define ___C3D_RenderTechnique_H___

#include "HDR/ToneMapping.hpp"
#include "Miscellaneous/SsaoConfig.hpp"
#include "Render/RenderInfo.hpp"
#include "Texture/TextureUnit.hpp"
#include "Technique/Deferred/CombinePass.hpp"
#include "Technique/Deferred/LightingPass.hpp"
#include "Technique/Deferred/ReflectionPass.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Rectangle.hpp>

#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( push )
#	pragma warning( disable:4503 )
#endif

namespace Castor3D
{
	class RenderTechniquePass;
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render technique base class
	\remarks	A render technique is the description of a way to render a render target
	\~french
	\brief		Classe de base d'une technique de rendu
	\remarks	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class RenderTechnique
		: public Castor::OwnedBy< Engine >
		, public Castor::Named
	{
		friend class RenderTechniquePass;

	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.7.0.0
		\date		19/12/2012
		\~english
		\brief		Internal struct holding a complete frame buffer.
		\~french
		\brief		Structure interne contenant un tampon d'image complet.
		*/
		struct TechniqueFbo
		{
		public:
			explicit TechniqueFbo( RenderTechnique & p_technique );
			bool Initialise( Castor::Size p_size );
			void Cleanup();

			//!\~english	The texture receiving the color render.
			//!\~french		La texture recevant le rendu couleur.
			TextureLayoutSPtr m_colourTexture;
			//!\~english	The buffer receiving the depth render.
			//!\~french		Le tampon recevant le rendu profondeur.
			TextureLayoutSPtr m_depthBuffer;
			//!\~english	The frame buffer.
			//!\~french		Le tampon d'image.
			FrameBufferSPtr m_frameBuffer;
			//!\~english	The attach between texture and main frame buffer.
			//!\~french		L'attache entre la texture et le tampon principal.
			TextureAttachmentSPtr m_colourAttach;
			//!\~english	The attach between depth buffer and main frame buffer.
			//!\~french		L'attache entre le tampon profondeur et le tampon principal.
			TextureAttachmentSPtr m_depthAttach;

		private:
			RenderTechnique & m_technique;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_name				The technique name.
		 *\param[in]	p_renderTarget		The render target for this technique.
		 *\param[in]	p_renderSystem		The render system.
		 *\param[in]	p_params			The technique parameters.
		 *\param[in]	p_config			The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_name				Le nom de la technique.
		 *\param[in]	p_renderTarget		La render target pour cette technique.
		 *\param[in]	p_renderSystem		Le render system.
		 *\param[in]	p_params			Les paramètres de la technique.
		 *\param[in]	p_config			La configuration du SSAO.
		 */
		C3D_API RenderTechnique( Castor::String const & p_name
			, RenderTarget & p_renderTarget
			, RenderSystem & p_renderSystem
			, Parameters const & p_params
			, SsaoConfig const & p_config );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderTechnique();
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	p_index		The base texture index.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	p_index		L'index de texture de base.
		 *\return		\p true if ok.
		 */
		C3D_API bool Initialise( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Update function.
		 *\remarks		Gather the render queues, for further update.
		 *\param[out]	p_queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	p_queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void Update( RenderQueueArray & p_queues );
		/**
		 *\~english
		 *\brief		Render function
		 *\param[out]	p_info	Receives the render informations.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	p_info	Reçoit les informations de rendu.
		 */
		C3D_API void Render( RenderInfo & p_info );
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	p_file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	p_file	Le fichier.
		 */
		C3D_API bool WriteInto( Castor::TextFile & p_file );
		/**
		 *\~english
		 *\return		The render area dimensions.
		 *\~french
		 *\return		Les dimensions de la zone de rendu.
		 */
		inline Castor::Size const & GetSize()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The colour texture holding the render's result.
		 *\~french
		 *\return		La texture de couleurs contenant le résultat du rendu.
		 */
		inline TextureLayout const & GetResult()const
		{
			REQUIRE( m_frameBuffer.m_colourTexture );
			return *m_frameBuffer.m_colourTexture;
		}
		/**
		 *\~english
		 *\return		The opaque nodes render pass.
		 *\~french
		 *\return		La passe de rendu des noeuds opaques.
		 */
		inline RenderTechniquePass const & GetOpaquePass()const
		{
			REQUIRE( m_opaquePass );
			return *m_opaquePass;
		}
		/**
		 *\~english
		 *\return		The transparent nodes render pass.
		 *\~french
		 *\return		La passe de rendu des noeuds transparents.
		 */
		inline RenderTechniquePass const & GetTransparentPass()const
		{
			REQUIRE( m_transparentPass );
			return *m_transparentPass;
		}
		/**
		 *\~english
		 *\return		\p true if the samples count is greater than 1.
		 *\~french
		 *\return		\p true si le nombre d'échantillons est plus grand que 1.
		 */
		inline bool IsMultisampling()const
		{
			return m_msaa.m_samplesCount > 1;
		}

	private:
		/**
		 *\~english
		 *\brief		Renders opaque nodes.
		 *\param[out]	p_info	Receives the render informations.
		 *\~french
		 *\brief		Dessine les noeuds opaques.
		 *\param[out]	p_info	Reçoit les informations de rendu.
		 */
		void DoRenderOpaque( RenderInfo & p_info );
		/**
		 *\~english
		 *\brief		Renders transparent nodes.
		 *\param[out]	p_info	Receives the render informations.
		 *\~french
		 *\brief		Dessine les noeuds transparents.
		 *\param[out]	p_info	Reçoit les informations de rendu.
		 */
		void DoRenderTransparent( RenderInfo & p_info );
		/**
		 *\~english
		 *\brief		Initialises deferred rendering related stuff.
		 *\return		\p false if anything went wrong (the technique is then not usable).
		 *\~french
		 *\brief		Initialise les données liées au deferred rendering.
		 *\return		\p false si un problème est survenu, la technique est alors inutilisable.
		 */
		bool DoInitialiseDeferred( uint32_t & p_index );
		/**
		 *\~english
		 *\brief		Initialises MSAA related stuff.
		 *\return		\p false if anything went wrong (the technique is then not usable).
		 *\~french
		 *\brief		Initialise les données liées au MSAA.
		 *\return		\p false si un problème est survenu, la technique est alors inutilisable.
		 */
		bool DoInitialiseMsaa();
		/**
		 *\~english
		 *\brief		Destroys deferred rendering related stuff.
		 *\~french
		 *\brief		Détruit les données liées au deferred rendering.
		 */
		void DoCleanupDeferred();
		/**
		 *\~english
		 *\brief		Destroys MSAA related stuff.
		 *\~french
		 *\brief		Détruit les données liées au MSAA.
		 */
		void DoCleanupMsaa();
		bool DoInitialiseGeometryPass( uint32_t & p_index );
		void DoCleanupGeometryPass();
		void DoRenderLights( LightType p_type
			, Castor::Matrix4x4r const & p_invViewProj
			, Castor::Matrix4x4r const & p_invView
			, Castor::Matrix4x4r const & p_invProj
			, bool & p_first );

	private:
		using GeometryBufferTextures = std::array< TextureUnitUPtr, size_t( DsTexture::eCount ) >;
		using GeometryBufferAttachs = std::array< TextureAttachmentSPtr, size_t( DsTexture::eCount ) >;

		//!\~english	The technique intialisation status.
		//!\~french		Le statut d'initialisation de la technique.
		bool m_initialised;
		//!\~english	The parent render target.
		//!\~french		La render target parente.
		RenderTarget & m_renderTarget;
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
		struct
		{
			//!\~english	The depth buffer.
			//!\~french		Le tampon de profondeur.
			RenderBufferSPtr m_lightPassDepthBuffer;
			//!\~english	The attachment between depth buffer and deferred shading frame buffer.
			//!\~french		L'attache entre le tampon de profondeur et le tampon deferred shading.
			RenderBufferAttachmentSPtr m_geometryPassDepthAttach;
			//!\~english	The multisampled frame buffer.
			//!\~french		Le tampon d'image multisamplé
			FrameBufferSPtr m_frameBuffer;
			//!\~english	The buffer receiving the multisampled color render.
			//!\~french		Le tampon recevant le rendu couleur multisamplé.
			ColourRenderBufferSPtr m_colorBuffer;
			//!\~english	The buffer receiving the multisampled depth render.
			//!\~french		Le tampon recevant le rendu profondeur multisamplé.
			DepthStencilRenderBufferSPtr m_depthBuffer;
			//!\~english	The attach between multisampled colour buffer and multisampled frame buffer.
			//!\~french		L'attache entre le tampon couleur multisamplé et le tampon multisamplé.
			RenderBufferAttachmentSPtr m_colorAttach;
			//!\~english	The attach between multisampled depth buffer and multisampled frame buffer.
			//!\~french		L'attache entre le tampon profondeur multisamplé et le tampon multisamplé.
			RenderBufferAttachmentSPtr m_depthAttach;
			//!\~english	The technique blit rectangle.
			//!\~french		Le rectangle de blit de la technique.
			Castor::Rectangle m_rect;
			//!\~english	The samples count.
			//!\~french		Le nombre de samples.
			uint8_t m_samplesCount{ 0 };
		} m_msaa;
		//!\~english	The render area dimension.
		//!\~french		Les dimensions de l'aire de rendu.
		Castor::Size m_size;
		//!\~english	The HDR frame buffer.
		//!\~french		Le tampon d'image HDR.
		TechniqueFbo m_frameBuffer;
		//!\~english	The pass used to render opaque nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds opaques.
		std::unique_ptr< RenderTechniquePass > m_opaquePass;
		//!\~english	The pass used to render transparent nodes.
		//!\~french		La passe utilisée pour dessiner les noeuds transparents.
		std::unique_ptr< RenderTechniquePass > m_transparentPass;
		//!\~english	The SSAO configuration.
		//!\~french		La configuration du SSAO.
		SsaoConfig m_ssaoConfig;
		//!\~english	The uniform buffer containing the scene data.
		//!\~french		Le tampon d'uniformes contenant les données de scène.
		SceneUbo m_sceneUbo;
		//!\~english	The fog pass.
		//!\~french		La passe de brouillard.
		std::unique_ptr< LightingPass > m_lightingPass;
		//!\~english	The combination pass.
		//!\~french		La passe de combinaison.
		std::unique_ptr< CombinePass > m_combinePass;
		//!\~english	The reflection pass.
		//!\~french		La passe de réflexion.
		std::unique_ptr< ReflectionPass > m_reflection;
		//!\~english	The various textures.
		//!\~french		Les diverses textures.
		GeometryBufferTextures m_geometryPassResult;
		//!\~english	The deferred shading frame buffer.
		//!\~french		Le tampon d'image pour le deferred shading.
		FrameBufferSPtr m_geometryPassFrameBuffer;
		//!\~english	The attachments between textures and deferred shading frame buffer.
		//!\~french		Les attaches entre les textures et le tampon deferred shading.
		GeometryBufferAttachs m_geometryPassTexAttachs;
		//!\~english	The selected frame buffer.
		//!\~french		Le tampon d'image sélectionné.
		FrameBufferRPtr m_currentFrameBuffer{ nullptr };
		//!\~english	The selected attach.
		//!\~french		L'attache sélectionnée.
		FrameBufferAttachmentRPtr m_currentDepthAttach{ nullptr };
	};
}

#if defined( CASTOR_COMPILER_MSVC )
#	pragma warning( pop )
#endif

#endif
