/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Castor3D/Render/ToneMapping/ToneMappingVisitor.hpp"

#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/FramePass.hpp>

#include <ashespp/Sync/Semaphore.hpp>

namespace castor3d
{
	class ToneMapping
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	device			The GPU device.
		 *\param[in]	hdrConfigUbo	The configuration data.
		 *\param[in]	parameters		The implementation specific parameters.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	hdrConfigUbo	Les données de confiuration.
		 *\param[in]	parameters		Les paramètres spécifiques à l'implémentation.
		 */
		C3D_API ToneMapping( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, crg::FrameGraph & graph
			, crg::ImageViewId const & source
			, crg::ImageViewId const & target
			, crg::FramePass const & previousPass
			, HdrConfigUbo & hdrConfigUbo
			, Parameters const & parameters );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~ToneMapping();
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	name		The tone mapping name.
		 *\param[in]	fullName	The tone mapping full (fancy) name.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name		Le nom du mappage de tons.
		 *\param[in]	fullName	Le nom complet (et joli) du mappage de tons.
		 */
		C3D_API void initialise( castor::String const & name );
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	name		The tone mapping name.
		 *\param[in]	fullName	The tone mapping full (fancy) name.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name		Le nom du mappage de tons.
		 *\param[in]	fullName	Le nom complet (et joli) du mappage de tons.
		 */
		C3D_API void updatePipeline( castor::String const & name );
		/**
		 *\~english
		 *\brief			Visitor acceptance function.
		 *\param[in, out]	visitor	The visitor.
		 *\~french
		 *\brief			Fonction d'acceptation de visiteur.
		 *\param[in, out]	visitor	Le visiteur.
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
		C3D_API castor::String const & getFullName()const;

		crg::FramePass const & getPass()const
		{
			return *m_pass;
		}

		castor::String const & getName()const
		{
			return m_name;
		}
		/**@}*/

	private:
		crg::FramePass & doCreatePass( castor::Size const & size
			, crg::FrameGraph & graph
			, crg::ImageViewId const & source
			, crg::ImageViewId const & target
			, crg::FramePass const & previousPass );
		void doCreate( castor::String const & name );

	protected:
		castor::String m_name;
		HdrConfigUbo & m_hdrConfigUbo;
		castor3d::ShaderModule m_vertexShader;
		castor3d::ShaderModule m_pixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		crg::FramePass * m_pass{};
		crg::RenderQuad * m_quad{};
	};
}

#endif

