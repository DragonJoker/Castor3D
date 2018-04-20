/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PostEffect_H___
#define ___C3D_PostEffect_H___

#include "Render/RenderTarget.hpp"
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
		C3D_API bool initialise( TextureLayout const & texture
			, RenderPassTimer const & timer );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updated needed data.
		 *\param[in]	elapsedTime	The time elapsed since last frame, in seconds.
		 *\~french
		 *\brief		Met à jour les données en ayant besoin.
		 *\param[in]	elapsedTime	Le temps écoulé depuis la dernière frame, en secondes.
		 */
		C3D_API virtual void update( castor::Nanoseconds const & elapsedTime );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline renderer::CommandBuffer const & getCommands()const
		{
			REQUIRE( m_commandBuffer );
			return *m_commandBuffer;
		}

		inline renderer::Semaphore const & getSemaphore()const
		{
			REQUIRE( m_signalFinished );
			return *m_signalFinished;
		}

		inline bool isAfterToneMapping()const
		{
			return m_postToneMapping;
		}

		inline TextureLayout const & getResult()const
		{
			REQUIRE( m_result );
			return *m_result;
		}
		/**@}*/

	protected:
		C3D_API void doCopyResultToTarget( renderer::TextureView const & result
			, renderer::CommandBuffer & commandBuffer );

	private:
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API virtual bool doInitialise( RenderPassTimer const & timer ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API virtual void doCleanup() = 0;
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
		RenderTarget & m_renderTarget;
		bool m_postToneMapping{ false };
		TextureLayout const * m_target{ nullptr };
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::SemaphorePtr m_signalFinished;
		TextureLayout const * m_result{ nullptr };
	};
}

#endif
