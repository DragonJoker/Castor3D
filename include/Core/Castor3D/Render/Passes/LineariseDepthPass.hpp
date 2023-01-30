/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LineariseDepthPass_H___
#define ___C3D_LineariseDepthPass_H___

#include "PassesModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Miscellaneous/PipelineVisitor.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"

#include <CastorUtils/Design/ChangeTracked.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Pipeline/PipelineShaderStageCreateInfo.hpp>
#include <ashespp/Sync/Semaphore.hpp>

#define C3D_DebugLinearisePass 0

namespace castor3d
{
	class LineariseDepthPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The render graph.
		 *\param[in]	previousPasses	The previous frame passes.
		 *\param[in]	device			The GPU device.
		 *\param[in]	progress		The progress bar.
		 *\param[in]	prefix			The pass name's prefix.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	depthBuffer		The non linearised depth buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le render graph.
		 *\param[in]	previousPasses	Les frame passes précédentes.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	progress		La barre de progression.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	depthBuffer		Le tampon de profondeur non linéarisé.
		 */
		C3D_API LineariseDepthPass( crg::ResourcesCache & resources
			, crg::FramePassGroup & graph
			, crg::FramePassArray const & previousPasses
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & prefix
			, SsaoConfig const & ssaoConfig
			, VkExtent2D const & size
			, Texture const & depthObj );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LineariseDepthPass();
		/**
		 *\~english
		 *\brief			Updates the render pass, CPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( PipelineVisitorBase & visitor );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		crg::ImageViewIdArray const & getResult()const
		{
			return m_mipViews;
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}
		/**@}*/

	private:
		crg::FramePass const & doInitialiseExtractPass( ProgressBar * progress
			, crg::FramePassArray const & previousPasses
			, Texture const & depthObj );
		void doInitialiseMinifyPass( ProgressBar * progress );

	private:
		RenderDevice const & m_device;
		crg::FramePassGroup & m_graph;
		Engine & m_engine;
		SsaoConfig const & m_ssaoConfig;
		castor::String m_prefix;
		VkExtent2D m_size;
		Texture m_result;
		crg::ImageViewIdArray m_mipViews;
		crg::FramePass const * m_lastPass{};
		/**
		*name
		*	Linearisation.
		*/
		/**@{*/
		ShaderModule m_extractVertexShader;
		ShaderModule m_extractPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_extractStages;
		crg::FramePass const & m_extractPass;
		/**@}*/
		/**
		*name
		*	Minification.
		*/
		/**@{*/
		std::vector< UniformBufferOffsetT< castor::Point2i > > m_previousLevel;
		ShaderModule m_minifyVertexShader;
		ShaderModule m_minifyPixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_minifyStages;
		/**@}*/

	};
}

#endif
