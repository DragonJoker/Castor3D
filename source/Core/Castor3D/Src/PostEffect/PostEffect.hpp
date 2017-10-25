/*
See LICENSE file in root folder
*/
#ifndef ___C3D_POST_EFFECT_H___
#define ___C3D_POST_EFFECT_H___

#include "Texture/TextureUnit.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		20/11/2012
	\~english
	\brief		Post render effect base class.
	\remark		A post render effect is an effect applied after 3D rendering and before 2D rendering.
				<br />Post render effects are applied in a cumulative way.
	\~french
	\brief		Classe de base d'effet post rendu.
	\remark		Un effet post rendu est un effet appliqué après le rendu 3D et avant le rendu 2D.
				<br />Les effets post rendu sont appliqués de manière cumulative.
	*/
	class PostEffect
		: public castor::OwnedBy< RenderSystem >
		, public castor::Named
	{
	protected:
		/*!
		\author		Sylvain DOREMUS
		\version	0.9.0
		\date		03/07/2016
		\~english
		\brief		Post render effect surface structure.
		\remark		Holds basic informations for a possible post effect surface: framebuffer and colour texture.
		\~french
		\brief		Surface pour effet post rendu.
		\remark		Contient les informations basiques de surface d'un effet: framebuffer, texture de couleur.
		*/
		struct PostEffectSurface
		{
			/**
			 *\~english
			 *\brief		Constructor.
			 *\param[in]	engine	The engine.
			 *\~french
			 *\brief		Constructeur.
			 *\param[in]	engine	Le moteur.
			 */
			C3D_API explicit PostEffectSurface( castor3d::Engine & engine );
			/**
			 *\~english
			 *\brief		Initialises the surface.
			 *\param[in]	renderTarget	The render target to which is attached this effect.
			 *\param[in]	size			The surface size.
			 *\param[in]	index			The surface index.
			 *\param[in]	sampler			The surface sampler.
			 *\param[in]	format			The surface pixel format.
			 *\~french
			 *\brief		Initialise la surface.
			 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
			 *\param[in]	size			Les dimensions de la surface.
			 *\param[in]	index			L'index de la surface.
			 *\param[in]	sampler			L'échantillonneur de la surface.
			 *\param[in]	format			Le format des pixels de la surface.
			 */
			C3D_API bool initialise( castor3d::RenderTarget & renderTarget
				, castor::Size const & size
				, uint32_t index
				, castor3d::SamplerSPtr sampler
				, castor::PixelFormat format = castor::PixelFormat::eRGBA32F );
			/**
			 *\~english
			 *\brief		Cleans up the surface.
			 *\~french
			 *\brief		Nettoie la surface.
			 */
			C3D_API void cleanup();

			//!\~english	The surface framebuffer.
			//!\~french		Le framebuffer de la surface.
			castor3d::FrameBufferSPtr m_fbo;
			//!\~english	The surface colour texture.
			//!\~french		La texture couleur de la surface.
			castor3d::TextureUnit m_colourTexture;
			//!\~english	The attach between framebuffer and texture.
			//!\~french		L'attache entre la texture et le framebuffer.
			castor3d::TextureAttachmentSPtr m_colourAttach;
			//!\~english	The surface dimensions.
			//!\~french		Les dimensions de la surface.
			castor::Size m_size;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name			The effect name.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	parameters		The optional parameters.
		 *\param[in]	postToneMapping	Tells if the effect applies after tone mapping.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name			Le nom de l'effet.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	parameters		Les paramètres optionnels.
		 *\param[in]	postToneMapping	Dit si l'effet s'applique après le mappage de tons.
		 */
		C3D_API PostEffect( castor::String const & name
			, RenderTarget & renderTarget
			, RenderSystem & renderSystem
			, Parameters const & parameters
			, bool postToneMapping = false );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~PostEffect();
		/**
		 *\~english
		 *\brief		Writes the effect into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API bool writeInto( castor::TextFile & file );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool initialise() = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief			Render function, applies the effect to the given framebuffer.
		 *\param[in,out]	framebuffer	The framebuffer.
		 *\return			\p true if ok.
		 *\~french
		 *\brief			Fonction de rendu, applique l'effet au tampon d'image donné.
		 *\param[in,out]	framebuffer	Le tampon d'image.
		 *\return			\p true si tout s'est bien passé.
		 */
		C3D_API virtual bool apply( FrameBuffer & framebuffer ) = 0;
		/**
		 *\~english
		 *\return		\p true if the effect applies after tone mapping.
		 *\~french
		 *\brief		\p true si l'effet s'applique après le mappage de tons.
		 */
		inline bool isAfterToneMapping()const
		{
			return m_postToneMapping;
		}

	private:
		/**
		 *\~english
		 *\brief		Writes the effect into a text file.
		 *\param[in]	file	The file.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 */
		C3D_API virtual bool doWriteInto( castor::TextFile & file ) = 0;

	protected:
		//!\~english	The render target to which this effect is attached.
		//!\~french		La cible de rendu à laquelle est attachée cet effet.
		RenderTarget & m_renderTarget;
		//!\~english	Tells if the effect applies after tone mapping.
		//!\~french		Dit si l'effet s'applique après le mappage de tons.
		bool m_postToneMapping{ false };
	};
}

#endif
