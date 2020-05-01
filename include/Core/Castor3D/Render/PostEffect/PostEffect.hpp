/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PostEffect_H___
#define ___C3D_PostEffect_H___

#include "PostEffectModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Render/RenderPassTimer.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/Passes/CommandsSemaphore.hpp"

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	class PostEffect
		: public castor::OwnedBy< RenderSystem >
		, public castor::Named
	{
	public:
		enum class Kind
		{
			eHDR,
			eSRGB,
			eOverlay, // TODO: Unsupported yet.
		};

	protected:
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
			, uint32_t passesCount = 1u
			, Kind kind = Kind::eHDR );

	public:
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
		 *\param[in]	tabs	The current indentation.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 *\param[in]	tabs	L'indentation actuelle.
		 */
		C3D_API bool writeInto( castor::TextFile & file
			, castor::String const & tabs );
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	texture	The target texture.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	texture	La texture cible.
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
		 *\param[in]	elapsedTime	The time elapsed since last frame.
		 *\~french
		 *\brief		Met à jour les données en ayant besoin.
		 *\param[in]	elapsedTime	Le temps écoulé depuis la dernière frame.
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
			return m_kind == Kind::eSRGB;
		}

		inline TextureLayout const & getResult()const
		{
			CU_Require( m_result );
			return *m_result;
		}

		inline castor::String const & getFullName()const
		{
			return m_fullName;
		}
		/**@}*/

	protected:
		C3D_API void doCopyResultToTarget( ashes::ImageView const & result
			, ashes::CommandBuffer & commandBuffer );

	private:
		/**
		 *\~english
		 *\brief		Initialisation function.
		 *\param[in]	timer	The render pass timer.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Fonction d'initialisation.
		 *\param[in]	timer	Le timer de la passe de rendu.
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
		 *\param[in]	tabs	The current indentation.
		 *\~french
		 *\brief		Ecrit l'effet dans un fichier texte.
		 *\param[in]	file	Le fichier.
		 *\param[in]	tabs	L'indentation actuelle.
		 */
		C3D_API virtual bool doWriteInto( castor::TextFile & file
			, castor::String const & tabs ) = 0;

	protected:
		castor::String m_fullName;
		RenderTarget & m_renderTarget;
		uint32_t m_passesCount{ 1u };
		uint32_t m_currentPass{ 0u };
		std::unique_ptr< RenderPassTimer > m_timer;
		Kind m_kind{ Kind::eHDR };
		TextureLayout const * m_target{ nullptr };
		CommandsSemaphoreArray m_commands;
		TextureLayout const * m_result{ nullptr };
	};
}

#endif
