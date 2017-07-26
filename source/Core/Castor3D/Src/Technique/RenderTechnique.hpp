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
#include "Technique/Opaque/DeferredRendering.hpp"
#include "Technique/Transparent/WeightedBlendRendering.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Rectangle.hpp>

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
			explicit TechniqueFbo( RenderTechnique & technique );
			bool Initialise( Castor::Size size );
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
		 *\param[in]	name			The technique name.
		 *\param[in]	renderTarget	The render target for this technique.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	parameters		The technique parameters.
		 *\param[in]	config			The SSAO configuration.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name			Le nom de la technique.
		 *\param[in]	renderTarget	La render target pour cette technique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	parameters		Les paramètres de la technique.
		 *\param[in]	config			La configuration du SSAO.
		 */
		C3D_API RenderTechnique( Castor::String const & name
			, RenderTarget & renderTarget
			, RenderSystem & renderSystem
			, Parameters const & parameters
			, SsaoConfig const & config );
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
		 *\param[in]	index		The base texture index.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	index		L'index de texture de base.
		 *\return		\p true if ok.
		 */
		C3D_API bool Initialise( uint32_t & index );
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
		 *\param[out]	queues	Receives the render queues needed for the rendering of the frame.
		 *\~french
		 *\brief		Fonction de mise à jour.
		 *\remarks		Récupère les files de rendu, pour mise à jour ultérieure.
		 *\param[out]	queues	Reçoit les files de rendu nécessaires pour le dessin de la frame.
		 */
		C3D_API void Update( RenderQueueArray & queues );
		/**
		 *\~english
		 *\brief		Render function
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Fonction de rendu.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void Render( RenderInfo & info );
		/**
		 *\~english
		 *\brief		Writes the technique into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit la technique dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool WriteInto( Castor::TextFile & file );
		/**
		 *\~english
		 *\brief		Adds a shadow producing light source.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse produisant des ombres.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void AddShadowProducer( Light & light );
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
			return false;
		}

	private:
		//!\~english	The technique intialisation status.
		//!\~french		Le statut d'initialisation de la technique.
		bool m_initialised;
		//!\~english	The parent render target.
		//!\~french		La render target parente.
		RenderTarget & m_renderTarget;
		//!\~english	The render system.
		//!\~french		Le render system.
		RenderSystem & m_renderSystem;
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
		//!\~english	The deferred rendering used for opaque meshes.
		//!\~french		Le rendu différé utilisé pour les maillages opaques.
		std::unique_ptr< DeferredRendering > m_deferredRendering;
		//!\~english	The weighted blend rendering used for transparent meshes.
		//!\~french		Le rendu weighted blend utilisé pour les maillages transparents.
		std::unique_ptr< WeightedBlendRendering > m_weightedBlendRendering;
		//!\~english	The particles timer.
		//!\~french		Le timer de particules.
		RenderPassTimerSPtr m_particleTimer;
		//!\~english	The post effect timer.
		//!\~french		Le timer d'effets post-rendu.
		RenderPassTimerSPtr m_postFxTimer;
	};
}

#endif
