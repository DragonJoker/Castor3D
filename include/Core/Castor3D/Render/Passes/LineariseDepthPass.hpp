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

#include <ShaderAST/Shader.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>
#include <ashespp/Descriptor/DescriptorSet.hpp>
#include <ashespp/Descriptor/DescriptorSetLayout.hpp>
#include <ashespp/Descriptor/DescriptorSetPool.hpp>
#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/Sampler.hpp>
#include <ashespp/Pipeline/PipelineLayout.hpp>
#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>
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
		 *\param[in]	engine		The engine.
		 *\param[in]	prefix		The pass name's prefix.
		 *\param[in]	size		The render area dimensions.
		 *\param[in]	depthBuffer	The non linearised depth buffer.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	prefix		Le préfixe du nom de la passe.
		 *\param[in]	size		Les dimensions de la zone de rendu.
		 *\param[in]	depthBuffer	Le tampon de profondeur non linéarisé.
		 */
		C3D_API LineariseDepthPass( crg::FrameGraph & graph
			, crg::FramePass const & previousPass
			, RenderDevice const & device
			, castor::String const & prefix
			, SsaoConfig const & ssaoConfig
			, VkExtent2D const & size
			, crg::ImageViewId const & depthBuffer );
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
		Texture const & getResult()const
		{
			return m_result;
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}
		/**@}*/

	private:
		crg::FramePass const & doInitialiseLinearisePass();
		void doInitialiseMinifyPass();

	public:
		static constexpr uint32_t MaxMipLevel = 5u;

	private:
		RenderDevice const & m_device;
		crg::FrameGraph & m_graph;
		Engine & m_engine;
		SsaoConfig const & m_ssaoConfig;
		crg::ImageViewId m_srcDepthBuffer;
		castor::String m_prefix;
		VkExtent2D m_size;
		Texture m_result;
		UniformBufferOffsetT< castor::Point3f > m_clipInfo;
		castor::ChangeTracked< castor::Point3f > m_clipInfoValue;
		crg::FramePass const * m_lastPass{};
		/**
		*name
		*	Linearisation.
		*/
		/**@{*/
		ShaderModule m_lineariseVertexShader;
		ShaderModule m_linearisePixelShader;
		ashes::PipelineShaderStageCreateInfoArray m_lineariseStages;
		ashes::ImageView m_linearisedView;
		crg::FramePass const & m_linearisePass;
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
