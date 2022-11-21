/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Voxelizer_H___
#define ___C3D_Voxelizer_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

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
		 *\param[in]	handler			The render graph resources handler.
		 *\param[in]	device			The GPU device.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	prefix			The pass name prefix.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	camera			The camera viewing the scene.
		 *\param[in]	voxelizerUbo	The voxelizer configuration UBO.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	handler			Le gestionnaire de ressources du render graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	camera			La caméra regardant la scène.
		 *\param[in]	voxelizerUbo	L'UBO de configuration du voxelizer.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 */
		C3D_API Voxelizer( crg::ResourceHandler & handler
			, RenderDevice const & device
			, ProgressBar * progress
			, castor::String const & prefix
			, Scene & scene
			, Camera & camera
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
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );
		/**
		 *\~english
		 *\brief		Renders the voxels.
		 *\param[in]	toWait	The semaphores to wait.
		 *\param[in]	queue	The queue receiving the render commands.
		 *\return		The semaphores signaled by this render.
		 *\~french
		 *\brief		Dessine les voxels.
		 *\param[in]	toWait	Les sémaphores à attendre.
		 *\param[in]	queue	The queue recevant les commandes de dessin.
		 *\return		Les sémaphores signalés par ce dessin.
		 */
		C3D_API crg::SemaphoreWaitArray render( crg::SemaphoreWaitArray const & toWait
			, ashes::Queue const & queue );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		Texture const & getFirstBounce()const
		{
			return m_firstBounce;
		}

		Texture const & getSecondaryBounce()const
		{
			return m_secondaryBounce;
		}

		crg::FrameGraph const & getGraph()const
		{
			return m_graph;
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
		/**@}*/

	private:
		crg::FramePass & doCreateVoxelizePass( ProgressBar * progress );
		crg::FramePass & doCreateVoxelToTexture( crg::FramePass const & previousPass
			, ProgressBar * progress );
		crg::FramePass & doCreateVoxelMipGen( crg::FramePass const & previousPass
			, std::string const & name
			, crg::ImageViewId const & view
			, crg::RunnablePass::IsEnabledCallback isEnabled
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
		SceneUbo m_sceneUbo;
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
