/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoBlurPass_H___
#define ___C3D_SsaoBlurPass_H___

#include "SsaoModule.hpp"
#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Design/Named.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

namespace castor3d
{
	class SsaoBlurPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine			The engine.
		 *\param[in]	device			The GPU device.
		 *\param[in]	prefix			The pass name's prefix.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	config			The SSAO configuration.
		 *\param[in]	ssaoConfigUbo	The SSAO configuration UBO.
		 *\param[in]	gpInfoUbo		The GBuffer configuration UBO.
		 *\param[in]	axis			The axis to which the blur applies ({0, 1} for vertical, {1, 0} for horizontal).
		 *\param[in]	input			The texture to blur.
		 *\param[in]	bentInput		The bent normals texture to blur.
		 *\param[in]	normals			The normals buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine			Le moteur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	config			La configuration du SSAO.
		 *\param[in]	ssaoConfigUbo	L'UBO de configuration du SSAO.
		 *\param[in]	gpInfoUbo		L'UBO de configuration du GBuffer.
		 *\param[in]	axis			L'axe dans lequel le flou s'applique ({0, 1} pour vertical, {1, 0} pour horizontal).
		 *\param[in]	input			La texture à flouter.
		 *\param[in]	bentInput		La texture de bent normals à flouter
		 *\param[in]	normals			Le tampon de normales.
		 */
		SsaoBlurPass( crg::FrameGraph & graph
			, RenderDevice const & device
			, crg::FramePass const & previousPass
			, castor::String const & prefix
			, VkExtent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, castor::Point2i const & axis
			, Texture const & input
			, Texture const & bentInput
			, Texture const & normals );
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Accepts a visitor.
		 *\param[in]	horizontal	Tells if the blur is horizontal.
		 *\param[in]	config		The SSAO config.
		 *\param[in]	visitor		The visitor.
		 *\~french
		 *\brief		Accepte un visiteur.
		 *\param[in]	horizontal	Dit si le flou est horizontal.
		 *\param[in]	config		La configuration du SSAO.
		 *\param[in]	visitor		Le visiteur.
		 */
		C3D_API void accept( bool horizontal
			, SsaoConfig & config
			, PipelineVisitorBase & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Texture const & getResult()const
		{
			return m_result;
		}

		Texture const & getBentResult()const
		{
			return m_bentResult;
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}
		/**@}*/

	public:
		static VkFormat constexpr ResultFormat = VK_FORMAT_R8G8B8A8_UNORM;

	private:
		struct RenderQuad
			: crg::RenderQuad
		{
			RenderQuad( crg::FramePass const & pass
				, crg::GraphContext & context
				, crg::RunnableGraph & graph
				, crg::rq::Config config
				, SsaoConfig const & ssaoConfig );

		protected:
			uint32_t doGetPassIndex()const;

		private:
			SsaoConfig const & ssaoConfig;
		};

		struct Program
		{
			Program( RenderDevice const & device
				, bool useNormalsBuffer );

			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

	private:
		struct Configuration
		{
			castor::Point2i axis;
			castor::Point2i dummy;
			castor::Point4f gaussian[2];
		};

		RenderDevice const & m_device;
		crg::FrameGraph & m_graph;
		SsaoConfigUbo & m_ssaoConfigUbo;
		GpInfoUbo const & m_gpInfoUbo;
		Texture const & m_input;
		Texture const & m_bentInput;
		Texture const & m_normals;
		SsaoConfig const & m_config;
		VkExtent2D m_size;
		Texture m_result;
		Texture m_bentResult;
		UniformBufferOffsetT< Configuration > m_configurationUbo;
		std::array< Program, 2u > m_programs;
		crg::FramePass const * m_lastPass{};

	};
}

#endif
