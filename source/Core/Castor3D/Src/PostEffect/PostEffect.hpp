/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PostEffect_H___
#define ___C3D_PostEffect_H___

#include "Miscellaneous/PipelineVisitor.hpp"

#include "Render/RenderPassTimer.hpp"
#include "Render/RenderTarget.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	struct CommandsSemaphore
	{
		CommandsSemaphore( ashes::CommandBufferPtr && commandBuffer
			, ashes::SemaphorePtr && semaphore )
			: commandBuffer{ std::move( commandBuffer ) }
			, semaphore{ std::move( semaphore ) }
		{
		}

		ashes::CommandBufferPtr commandBuffer;
		ashes::SemaphorePtr semaphore;
	};
	using CommandsSemaphoreArray = std::vector< CommandsSemaphore >;
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
		 *\param[in]	fullName		The effect full (fancy) name.
		 *\param[in]	renderTarget	The render target to which is attached this effect.
		 *\param[in]	renderSystem	The render system.
		 *\param[in]	parameters		The optional parameters.
		 *\param[in]	postToneMapping	Tells if the effect applies after tone mapping.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name			Le nom de l'effet.
		 *\param[in]	fullName		Le nom complet (et joli) de l'effet.
		 *\param[in]	renderTarget	La cible de rendu sur laquelle cet effet s'applique.
		 *\param[in]	renderSystem	Le render system.
		 *\param[in]	parameters		Les paramètres optionnels.
		 *\param[in]	postToneMapping	Dit si l'effet s'applique après le mappage de tons.
		 */
		C3D_API PostEffect( castor::String const & name
			, castor::String const & fullName
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
		C3D_API bool writeInto( castor::TextFile & file, castor::String const & tabs );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\return		\p true if ok.
		 */
		C3D_API bool initialise( TextureLayout const & texture );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Starts rendering the effect.
		 *\~french
		 *\brief		Démarre le rendu de l'effet.
		 */
		C3D_API RenderPassTimerBlock start();
		/**
		 *\~english
		 *\brief		Notifies a pass render.
		 *\~french
		 *\brief		Notifie le rendu d'une passe.
		 */
		C3D_API void notifyPassRender();
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
		 *\brief		Visitor acceptance function.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 */
		C3D_API virtual void accept( PipelineVisitorBase & visitor ) = 0;
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline CommandsSemaphoreArray const & getCommands()const
		{
			return m_commands;
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

		inline castor::String const & getFullName()const
		{
			return m_fullName;
		}
		/**@}*/

	protected:
		C3D_API void doCopyResultToTarget( ashes::TextureView const & result
			, ashes::CommandBuffer & commandBuffer );

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
		C3D_API virtual bool doWriteInto( castor::TextFile & file, castor::String const & tabs ) = 0;

	protected:
		castor::String m_fullName;
		RenderTarget & m_renderTarget;
		uint32_t m_passesCount{ 1u };
		uint32_t m_currentPass{ 0u };
		std::unique_ptr< RenderPassTimer > m_timer;
		bool m_postToneMapping{ false };
		TextureLayout const * m_target{ nullptr };
		CommandsSemaphoreArray m_commands;
		TextureLayout const * m_result{ nullptr };
	};
}

#endif
