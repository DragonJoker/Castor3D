/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Voxelizer_H___
#define ___C3D_Voxelizer_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace castor3d
{
	class Voxelizer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	device			The GPU device.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	camera			The camera viewing the scene.
		 *\param[in]	matrixUbo		The scene matrices UBO.
		 *\param[in]	voxelizerUbo	The voxelizer configuration UBO.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	camera			La caméra regardant la scène.
		 *\param[in]	matrixUbo		L'UBO de matrices de la scène.
		 *\param[in]	voxelizerUbo	L'UBO de configuration du voxelizer.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 */
		C3D_API Voxelizer( crg::ResourceHandler & handler
			, RenderDevice const & device
			, ProgressBar * progress
			, Scene & scene
			, Camera & camera
			, MatrixUbo & matrixUbo
			, VoxelizerUbo & voxelizerUbo
			, VoxelSceneData const & voxelConfig );
		C3D_API ~Voxelizer();
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
		 *\~english
		 *\brief			Updates the render pass, GPU wise.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour la passe de rendu, au niveau GPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & semaphore
			, ashes::Queue const & queue );

		Texture const & getFirstBounce()const
		{
			return m_firstBounce;
		}

		Texture const & getSecondaryBounce()const
		{
			return m_secondaryBounce;
		}

		static uint32_t countInitialisationSteps()
		{
			uint32_t result = 0u;
			result += 2;// m_voxelizePass;
			result += 2;// m_voxelToTexture;
			result += 2;// m_voxelMipGen;
			result += 2;// m_voxelSecondaryBounce;
			result += 2;// m_voxelSecondaryMipGen;
			return result;
		}

	private:
		crg::FramePass & doCreateVoxelizePass( ProgressBar * progress );
		crg::FramePass & doCreateVoxelToTexture( crg::FramePass const & previousPass
			, ProgressBar * progress );
		crg::FramePass & doCreateVoxelMipGen( crg::FramePass const & previousPass
			, std::string const & name
			, crg::ImageViewId const & view
			, ProgressBar * progress );
		crg::FramePass & doCreateVoxelSecondaryBounce( crg::FramePass const & previousPass
			, ProgressBar * progress );

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		VoxelSceneData const & m_voxelConfig;
		crg::FrameGraph m_graph;
		DummyCuller m_culler;
		MatrixUbo m_matrixUbo;
		Texture m_firstBounce;
		Texture m_secondaryBounce;
		ashes::BufferPtr< Voxel > m_voxels;
		VoxelizerUbo & m_voxelizerUbo;
		castor::Point4f m_grid;
		crg::FramePass & m_voxelizePassDesc;
		VoxelizePass * m_voxelizePass{};
		crg::FramePass & m_voxelToTextureDesc;
		VoxelBufferToTexture * m_voxelToTexture{};
		crg::FramePass & m_voxelMipGen;
		crg::FramePass & m_voxelSecondaryBounceDesc;
		VoxelSecondaryBounce * m_voxelSecondaryBounce{};
		crg::FramePass & m_voxelSecondaryMipGen;
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
