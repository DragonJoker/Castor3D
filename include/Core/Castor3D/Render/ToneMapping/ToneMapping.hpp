/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Castor3D/Render/ToneMapping/ToneMappingVisitor.hpp"

#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Shader/Ubos/ColourGradingUbo.hpp"
#include "Castor3D/Shader/Ubos/HdrConfigUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/FramePass.hpp>

#include <ashespp/Sync/Semaphore.hpp>

namespace sdw
{
	class TraditionalGraphicsWriter;
}

namespace castor3d
{
	class ToneMapping
		: public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	engine				The engine.
		 *\param[in]	graph				The render graph.
		 *\param[in]	source				L'image source.
		 *\param[in]	target				L'image cible.
		 *\param[in]	previousPass		The previous frame pass.
		 *\param[in]	hdrConfigUbo		The HDR configuration data.
		 *\param[in]	colourGradingUbo	The colour grading configuration data.
		 *\param[in]	progress			The progress bar.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	engine				Le moteur.
		 *\param[in]	graph				Le render graph.
		 *\param[in]	source				The source image.
		 *\param[in]	target				The target image.
		 *\param[in]	previousPass		La frame pass précédente.
		 *\param[in]	hdrConfigUbo		Les données de confiuration HDR.
		 *\param[in]	colourGradingUbo	Les données de confiuration de colour grading.
		 *\param[in]	progress			La barre de progression.
		 */
		C3D_API ToneMapping( Engine & engine
			, crg::FramePassGroup & graph
			, crg::ImageViewIdArray const & source
			, crg::ImageViewId const & target
			, crg::FramePass const & previousPass
			, HdrConfigUbo & hdrConfigUbo
			, ColourGradingUbo & colourGradingUbo
			, ProgressBar * progress );
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	name	The tone mapping name.
		 *\param[in]	source	The initial source image.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name	Le nom du mappage de tons.
		 *\param[in]	source	L'image source initiale.
		 */
		C3D_API void initialise( castor::String const & name
			, crg::ImageViewId const & source );
		/**
		 *\~english
		 *\param[in, out]	updater	The update data.
		 *\param[in]		source	The current source image.
		 *\~french
		 *\param[in, out]	updater	Les données d'update.
		 *\param[in]		source	L'image source actuelle.
		 */
		C3D_API void update( CpuUpdater & updater
			, crg::ImageViewId const & source );
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	name	The tone mapping name.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name	Le nom du mappage de tons.
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
		C3D_API void accept( ToneMappingVisitor & visitor );
		/**
		 *\~english
		 *\brief			Retrieves the vertex shader for tone mapping.
		 *\param[in, out]	builder	Receives the shader source.
		 *\~french
		 *\brief			Récupère le vertex shader pour le tone mapping.
		 *\param[in, out]	builder	Reçoit le source du shader.
		 */
		C3D_API static void getVertexProgram( ast::ShaderBuilder & builder );
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

		crg::FramePass const & getPass()const noexcept
		{
			return *m_pass;
		}

		castor::String const & getName()const noexcept
		{
			return m_name;
		}
		/**@}*/

	private:
		crg::FramePass & doCreatePass( crg::FramePassGroup & graph
			, crg::ImageViewIdArray const & source
			, crg::ImageViewId const & target
			, crg::FramePass const & previousPass
			, ProgressBar * progress );
		void doCreate( castor::String const & name );
		void doUpdatePassIndex( crg::ImageViewId const & source );

	protected:
		castor::String m_name{ cuT( "linear" ) };
		HdrConfigUbo & m_hdrConfigUbo;
		ColourGradingUbo & m_colourGradingUbo;
		castor3d::ProgramModule m_shader{ cuT( "ToneMapping" ) };
		crg::ImageViewId m_source;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		crg::FramePass * m_pass{};
		crg::RenderQuad * m_quad{};
		uint32_t m_passIndex{};
	};
}

#endif

