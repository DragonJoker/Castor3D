/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SsaoPass_H___
#define ___C3D_SsaoPass_H___

#include "SsaoModule.hpp"

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/Passes/PassesModule.hpp"
#include "Castor3D/Render/Ssao/SsaoModule.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/SsaoConfigUbo.hpp"

#include <RenderGraph/FrameGraphPrerequisites.hpp>

#define C3D_DebugSSAO 0

namespace castor3d
{
	class SsaoPass
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	graph			The render graph.
		 *\param[in]	device			The GPU device.
		 *\param[in]	progress		The progress bar.
		 *\param[in]	previousPasses	The previous frame passes.
		 *\param[in]	size			The render area dimensions.
		 *\param[in]	ssaoConfig		The SSAO configuration.
		 *\param[in]	depthObj		The depth buffer.
		 *\param[in]	normal			The normals buffer.
		 *\param[in]	cameraUbo		The camera configuration UBO.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	graph			Le render graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	progress		La barre de progression
		 *\param[in]	previousPasses	Les frame passes précédentes.
		 *\param[in]	size			Les dimensions de la zone de rendu.
		 *\param[in]	ssaoConfig		La configuration du SSAO.
		 *\param[in]	depthObj		Le depth buffer.
		 *\param[in]	normal			Le buffer de normales.
		 *\param[in]	cameraUbo		L'UBO de configuration de la caméra.
		 */
		C3D_API SsaoPass( crg::FramePassGroup & graph
			, RenderDevice const & device
			, ProgressBar * progress
			, crg::FramePassArray const & previousPasses
			, castor::Size const & size
			, SsaoConfig & ssaoConfig
			, Texture const & depthObj
			, Texture const & normal
			, CameraUbo const & cameraUbo );
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
		C3D_API Texture const & getResult()const;
		C3D_API Texture const & getBentNormals()const;

		SsaoConfig const & getConfig()const
		{
			return m_ssaoConfig;
		}

		crg::FramePass const & getLastPass()const
		{
			return *m_lastPass;
		}

		static uint32_t countInitialisationSteps()
		{
			uint32_t result = 0u;
			result += 12;// m_linearisePass;
			result += 2;// m_rawAoPass;
			result += 2;// m_horizontalBlur;
			result += 2;// m_verticalBlur;
			return result;
		}
		/**@}*/

	private:
		RenderDevice const & m_device;
		SsaoConfig & m_ssaoConfig;
		crg::FramePassGroup & m_group;
		VkExtent2D m_size;
		uint32_t m_passIndex{};
		LineariseDepthPassUPtr m_linearisePass;
		SsaoConfigUbo m_ssaoConfigUbo;
		SsaoRawAOPassUPtr m_rawAoPass;
		SsaoBlurPassUPtr m_horizontalBlur;
		SsaoBlurPassUPtr m_verticalBlur;
		crg::FramePass const * m_lastPass{};
	};
}

#endif
