/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TONE_MAPPING_H___
#define ___C3D_TONE_MAPPING_H___

#include "Castor3D/Miscellaneous/Parameter.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Render/ToneMapping/ToneMappingVisitor.hpp"
#include "Castor3D/Shader/Ubos/ColourGradingUbo.hpp"
#include "Castor3D/Shader/Ubos/RenderUbo.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/FramePass.hpp>

#include <ashespp/Sync/Semaphore.hpp>

namespace sdw
{
	class TraditionalGraphicsWriter;
}

namespace c3d
{
	class ToneMappingImpl
		: public OwnedBy< ToneMapping const >
	{
	public:
		ToneMappingImpl( ToneMapping const & parent )
			: OwnedBy{ parent }
		{
		}

		C3D_API VkPipelineLayout getPipelineLayout()const;
		virtual ~ToneMappingImpl()noexcept = default;
		virtual void getFragmentProgram( ast::ShaderBuilder & builder ) = 0;
		virtual void update()
		{
		}
		virtual void accept( ToneMappingVisitor & visitor )
		{
		}
		virtual Vector< VkDescriptorSetLayout > getDescriptorLayouts()const
		{
			return {};
		}
		virtual Vector< VkPushConstantRange > getPushConstantRanges()const
		{
			return {};
		}
		virtual void recordInto( crg::RecordContext const &, VkCommandBuffer, uint32_t )const
		{
		}
	};

	class ToneMapping
		: public OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor.
		 *\param[in]	engine				The engine.
		 *\param[in]	graph				The render graph.
		 *\param[in]	source				L'image source.
		 *\param[in]	target				L'image cible.
		 *\param[in]	renderUbo			The render configuration data.
		 *\param[in]	colourGradingUbo	The colour grading configuration data.
		 *\param[in]	parameters			The tone mapping parameters.
		 *\param[in]	progress			The progress bar.
		 *\~french
		 *\brief		Constructeur spécifié.
		 *\param[in]	engine				Le moteur.
		 *\param[in]	graph				Le render graph.
		 *\param[in]	source				The source image.
		 *\param[in]	target				The target image.
		 *\param[in]	renderUbo			Les données de configuration du rendu.
		 *\param[in]	colourGradingUbo	Les données de configuration de colour grading.
		 *\param[in]	parameters			Les paramètres de tone mapping.
		 *\param[in]	progress			La barre de progression.
		 */
		C3D_API ToneMapping( Engine & engine
			, crg::FramePassGroup & graph
			, Texture const & source
			, Texture & target
			, RenderUbo const & renderUbo
			, ColourGradingUbo & colourGradingUbo
			, Parameters parameters
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
		C3D_API void initialise( String const & name
			, Texture const & source );
		/**
		 *\~english
		 *\param[in, out]	updater	The update data.
		 *\param[in]		source	The current source image.
		 *\~french
		 *\param[in, out]	updater	Les données d'update.
		 *\param[in]		source	L'image source actuelle.
		 */
		C3D_API void update( CpuUpdater & updater
			, Texture const & source );
		/**
		 *\~english
		 *\brief		Initialises tone mapping shader and pipeline.
		 *\param[in]	name	The tone mapping name.
		 *\~french
		 *\brief		Initialise le shader et le pipeline de mappage de tons.
		 *\param[in]	name	Le nom du mappage de tons.
		 */
		C3D_API void updatePipeline( String const & name );
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
		C3D_API String const & getFullName()const;
		C3D_API VkPipelineLayout getPipelineLayout()const;

		String const & getName()const noexcept
		{
			return m_name;
		}
		/**@}*/

	private:
		void doCreate( String const & name );
		void doUpdatePassIndex( Texture const & source );

	protected:
		String m_name{ cuT( "linear" ) };
		RenderUbo const & m_renderUbo;
		ColourGradingUbo & m_colourGradingUbo;
		ProgramModule m_shader{ cuT( "ToneMapping" ) };
		Texture const & m_source;
		ToneMappingImplUPtr m_impl;
		ashes::PipelineShaderStageCreateInfoArray m_program;
		crg::RenderQuad * m_quad{};
		uint32_t m_passIndex{};
		Parameters m_parameters;
	};
}

#endif

