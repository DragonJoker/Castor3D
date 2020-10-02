/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Castor3D/Render/ToneMapping/ToneMappingVisitor.hpp"

#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class ToneMapping
		: public castor::OwnedBy< Engine >
		, private RenderQuad
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	name		The tone mapping name.
		 *\param[in]	fullName	The tone mapping full (fancy) name.
		 *\param[in]	engine		The engine.
		 *\param[in]	config		The configuration data.
		 *\param[in]	parameters	The implementation specific parameters.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	name		Le nom du mappage de tons.
		 *\param[in]	fullName	Le nom complet (et joli) du mappage de tons.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	config		Les données de confiuration.
		 *\param[in]	parameters	Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API ToneMapping( castor::String const & name
			, castor::String const & fullName
			, Engine & engine
			, RenderDevice const & device
			, HdrConfigUbo & hdrConfigUbo
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
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	size		The render size.
		 *\param[in]	source		The source texture.
		 *\param[in]	renderPass	The render pass to use.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	size		Les dimensions du rendu.
		 *\param[in]	source		La texture source.
		 *\param[in]	renderPass	La passe de rendu à utiliser.
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
		 *\~english
		 *\brief		Met à jour le tone mapping.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates the tone mapping.
		 *\~english
		 *\brief		Met à jour le tone mapping.
		 */
		C3D_API void update( GpuUpdater & updater );
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
		using RenderQuad::registerPass;
		using RenderQuad::getName;

	private:
		/**
		 *\~english
		 *\brief			Creates tone mapping shader, and the shader variables.
		 *\return			The pixel shader source.
		 *\~french
		 *\brief			Crée le shader de mappage de tons, ainsi que les variables shader.
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
		C3D_API virtual void doCpuUpdate()
		{
		}
		/**
		 *\~english
		 *\brief		Updates the tone mapping shader variables.
		 *\~english
		 *\brief		Met à jour les variables shader du mappage de tons.
		 */
		C3D_API virtual void doGpuUpdate()
		{
		}

	protected:
		castor::String m_fullName;
		HdrConfigUbo & m_hdrConfigUbo;
		ashes::SemaphorePtr m_signalFinished;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
	};
}

#endif

