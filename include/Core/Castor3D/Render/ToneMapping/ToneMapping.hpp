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
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	graph			The render graph.
		 *\param[in]	source			L'image source.
		 *\param[in]	target			L'image cible.
		 *\param[in]	previousPass	The previous frame pass.
		 *\param[in]	hdrConfigUbo	The configuration data.
		 *\param[in]	parameters		The implementation specific parameters.
		 *\param[in]	progress		The progress bar.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	graph			Le render graph.
		 *\param[in]	source			The source image.
		 *\param[in]	target			The target image.
		 *\param[in]	previousPass	La frame pass précédente.
		 *\param[in]	hdrConfigUbo	Les données de confiuration.
		 *\param[in]	parameters		Les paramètres spécifiques à l'implémentation.
		 *\param[in]	progress		La barre de progression.
		 */
		C3D_API ToneMapping( Engine & engine
			, RenderDevice const & device
			, castor::Size const & size
			, crg::FrameGraph & graph
			, crg::ImageViewId const & source
			, crg::ImageViewId const & target
			, crg::FramePass const & previousPass
			, HdrConfigUbo & hdrConfigUbo
			, Parameters const & parameters
			, ProgressBar * progress );
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
		 *\param[in]	name		The tone mapping name.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name		Le nom du mappage de tons.
		 */
		C3D_API void initialise( castor::String const & name );
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	name		The tone mapping name.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name		Le nom du mappage de tons.
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
			, crg::FramePass const & previousPass
			, ProgressBar * progress );
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

