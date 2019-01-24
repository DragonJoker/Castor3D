/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "ToneMappingVisitor.hpp"

#include "Render/RenderInfo.hpp"
#include "RenderToTexture/RenderQuad.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Shader/Ubos/HdrConfigUbo.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

#include <ShaderWriter/Shader.hpp>

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
		 *\param[in]	fullName	The tone mapping full (fancy) name.
		 *\param[in]	engine		The engine.
		 *\param[in]	parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	name		Le nom du mappage de tons.
		 *\param[in]	fullName	Le nom complet (et joli) du mappage de tons.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API ToneMapping( castor::String const & name
			, castor::String const & fullName
			, Engine & engine
			, HdrConfig & config
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
			, ashes::RenderPass const & renderPass );
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
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Visitor acceptance function.
		 *\~french
		 *\brief		Fonction d'acceptation de visiteur.
		 */
		C3D_API virtual void accept( ToneMappingVisitor & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline ashes::Semaphore const & getSemaphore()const
		{
			CU_Require( m_signalFinished );
			return *m_signalFinished;
		}

		inline castor::String const & getFullName()const
		{
			return m_fullName;
		}
		/**@}*/

	public:
		using RenderQuad::registerFrame;

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
		C3D_API virtual ShaderPtr doCreate() = 0;
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
		C3D_API void doFillDescriptorSet( ashes::DescriptorSetLayout & descriptorSetLayout
			, ashes::DescriptorSet & descriptorSet )override;

	protected:
		castor::String m_fullName;
		HdrConfig & m_config;
		HdrConfigUbo m_hdrConfigUbo;
		ashes::SemaphorePtr m_signalFinished;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif

