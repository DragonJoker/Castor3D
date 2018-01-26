/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Render/RenderInfo.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Shader/UniformBuffer.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		11/03/2016
	\~english
	\brief		Tone mapping base class.
	\~french
	\brief		Classe de base de mappage de ton.
	*/
	class ToneMapping
		: public castor::OwnedBy< Engine >
		, public castor::Named
		, private RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	name		The tone mapping name.
		 *\param[in]	engine		The engine.
		 *\param[in]	parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	name		Le nom du mappage de tons.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API ToneMapping( castor::String const & name
			, Engine & engine
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~ToneMapping();
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader.
		 *\~french
		 *\brief		Initialise le shader de mappage de tons.
		 */
		C3D_API bool initialise( castor::Size const & size
			, TextureLayout const & source
			, renderer::RenderPass const & renderPass );
		/**
		 *\~english
		 *\brief		Cleanup function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Updates the tone mapping.
		 *\param[in]	config	The HDR configuration.
		 *\~english
		 *\brief		Met à jour le tone mapping.
		 *\param[in]	config	La configuration HDR.
		 */
		C3D_API void update( HdrConfig const & config );
		/**
		 *\~english
		 *\brief		Applies the tone mapping for given HDR texture.
		 *\param[in]	size	The target dimensions.
		 *\param[in]	texture	The HDR texture.
		 *\param[in]	info	Receives the render information.
		 *\~english
		 *\brief		Applique le mappage de tons pour la texture HDR donnée.
		 *\param[in]	size	Les dimensions de la cible.
		 *\param[in]	texture	La texture HDR.
		 *\param[in]	info	Reçoit les informations de rendu.
		 */
		C3D_API void apply( castor::Size const & size
			, TextureLayout const & texture
			, RenderInfo & info );
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
		/**@}*/

	private:
		/**
		 *\~english
		 *\brief			Creates tone mapping shader, and the shader variables.
		 *\param[in,out]	The shader program UBO to create variables.
		 *\return			The pixel shader source.
		 *\~french
		 *\brief			Crée le shader de mappage de tons, ainsi que les variables shader.
		 *\param[in,out]	Le tampon de variables shader, pour créer les variables.
		 *\return			Le source du pixel shader.
		 */
		C3D_API virtual glsl::Shader doCreate() = 0;
		/**
		 *\~english
		 *\brief		Cleans up the tone mapping shader variables.
		 *\~french
		 *\brief		Nettoie les variables shader de mappage de tons.
		 */
		C3D_API virtual void doDestroy() = 0;
		/**
		 *\~english
		 *\brief		Updates the tone mapping shader variables.
		 *\~english
		 *\brief		Met à jour les variables shader du mappage de tons.
		 */
		C3D_API virtual void doUpdate() = 0;

	private:
		void doFillDescriptorSet( renderer::DescriptorSetLayout & descriptorSetLayout
			, renderer::DescriptorSet & descriptorSet )override;

	protected:
		//!\~english	The configuration data UBO.
		//!\~french		L'UBO de données de configuration.
		HdrConfigUbo m_configUbo;
		//!\~english	The render pass timer.
		//!\~french		Le timer de passe de rendu.
		RenderPassTimerSPtr m_timer;
		//!\~english	The command buffer holding the tone mapping commands.
		//!\~french		Le tampon de commandes contenant les commandes de mappage de tons.
		renderer::CommandBufferPtr m_commandBuffer;
	};
}

#endif

