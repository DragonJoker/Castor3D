/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoRawAOPass_H___
#define ___C3D_SsaoRawAOPass_H___

#include "SsaoModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/Ssao/SsaoConfig.hpp"
#include "Castor3D/Render/Opaque/Lighting/LightPass.hpp"
#include "Castor3D/Render/Passes/RenderQuad.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <ShaderAST/Shader.hpp>

#include <RenderGraph/RunnablePasses/RenderQuad.hpp>

namespace castor3d
{
	class SsaoRawAOPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph					The render graph.
		 *\param[in]	device					The GPU device.
		 *\param[in]	progress				The progress bar.
		 *\param[in]	previousPass			The previous frame pass.
		 *\param[in]	size					The render area dimensions.
		 *\param[in]	config					The SSAO configuration.
		 *\param[in]	ssaoConfigUbo			The SSAO configuration UBO.
		 *\param[in]	gpInfoUbo				The GBuffer configuration UBO.
		 *\param[in]	linearisedDepthBuffer	The linearised depth buffer.
		 *\param[in]	normals					The normals buffer.
		 *\param[in]	passIndex				The frame pass index.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph					Le render graph.
		 *\param[in]	device					Le device GPU.
		 *\param[in]	progress				La barre de progression
		 *\param[in]	previousPass			La frame pass précédente.
		 *\param[in]	size					Les dimensions de la zone de rendu.
		 *\param[in]	config					La configuration du SSAO.
		 *\param[in]	ssaoConfigUbo			L'UBO de configuration du SSAO.
		 *\param[in]	gpInfoUbo				L'UBO de configuration du GBuffer.
		 *\param[in]	linearisedDepthBuffer	Le tampon de profondeur linéarisé.
		 *\param[in]	normals					Le tampon de normales.
		 *\param[in]	passIndex				L'index de la passe de frame.
		 */
		C3D_API SsaoRawAOPass( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, crg::FramePass const & previousPass
			, VkExtent2D const & size
			, SsaoConfig const & config
			, SsaoConfigUbo & ssaoConfigUbo
			, GpInfoUbo const & gpInfoUbo
			, Texture const & linearisedDepthBuffer
			, Texture const & normals
			, uint32_t const & passIndex );
		C3D_API ~SsaoRawAOPass();
		/**
		 *\~english
		 *\brief		Accepts a visitor.
		 *\param[in]	config		The SSAO config.
		 *\param[in]	visitor		The visitor.
		 *\~french
		 *\brief		Accepte un visiteur.
		 *\param[in]	config		La configuration du SSAO.
		 *\param[in]	visitor		Le visiteur.
		 */
		C3D_API void accept( SsaoConfig & config
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
			return m_bentNormals;
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}
		/**@}*/

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
				, bool useNormalsBuffer
				, castor::String const & prefix );

			ShaderModule vertexShader;
			ShaderModule pixelShader;
			ashes::PipelineShaderStageCreateInfoArray stages;
		};

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		SsaoConfig const & m_ssaoConfig;
		SsaoConfigUbo & m_ssaoConfigUbo;
		GpInfoUbo const & m_gpInfoUbo;
		VkExtent2D m_size;
		Texture m_result;
		Texture m_bentNormals;
		std::array< Program, 2u > m_programs;
		crg::FramePass const * m_lastPass{};
	};
}

#endif
