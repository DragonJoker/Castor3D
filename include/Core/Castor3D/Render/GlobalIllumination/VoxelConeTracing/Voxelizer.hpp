/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Voxelizer_H___
#define ___C3D_Voxelizer_H___

#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Render/GlobalIllumination/VoxelConeTracing/VoxelizeModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Shader/Ubos/CameraUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"

#include <RenderGraph/FrameGraph.hpp>
#include <RenderGraph/RunnableGraph.hpp>

namespace c3d
{
	class Voxelizer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	resources		The render graph resources handler.
		 *\param[in]	device			The GPU device.
		 *\param[in]	progress		The optional progress bar.
		 *\param[in]	prefix			The pass name prefix.
		 *\param[in]	scene			The rendered scene.
		 *\param[in]	camera			The camera viewing the scene.
		 *\param[in]	voxelizerUbo	The voxelizer configuration UBO.
		 *\param[in]	voxelConfig		The voxelizer configuration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	resources		Le gestionnaire de ressources du render graph.
		 *\param[in]	device			Le device GPU.
		 *\param[in]	progress		La barre de progression optionnelle.
		 *\param[in]	prefix			Le préfixe du nom de la passe.
		 *\param[in]	scene			La scène rendue.
		 *\param[in]	camera			La caméra regardant la scène.
		 *\param[in]	voxelizerUbo	L'UBO de configuration du voxelizer.
		 *\param[in]	voxelConfig		La configuration du voxelizer.
		 */
		C3D_API Voxelizer( crg::ResourcesCache & resources
			, RenderDevice const & device
			, ProgressBar * progress
			, String const & prefix
			, Scene & scene
			, Camera & camera
			, VoxelizerUbo & voxelizerUbo
			, VctConfig const & voxelConfig );
		C3D_API ~Voxelizer()noexcept;
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
		 *\copydoc		RenderTechniquePass::accept
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
		C3D_API SemaphoreWaitArray render( SemaphoreWaitArray const & toWait
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
		static uint32_t countInitialisationSteps()noexcept;

		Texture const & getFirstBounce()const noexcept
		{
			return m_firstBounce;
		}

		Texture const & getSecondaryBounce()const noexcept
		{
			return m_secondaryBounce;
		}

		crg::FrameGraph const & getGraph()const noexcept
		{
			return m_graph;
		}
		/**@}*/

	private:
		void doCreateClearStaticsPass( ProgressBar * progress );
		void doCreateVoxelizePass( ProgressBar * progress
			, BufferT< Voxel > & outVoxels
			, SceneCuller & culler
			, bool isStatic );
		void doCreateMergeStaticsPass( ProgressBar * progress );
		void doCreateVoxelToTexture( ProgressBar * progress );
		void doCreateVoxelMipGen( String const & name
			, Texture & view
			, crg::RunnablePass::IsEnabledCallback isEnabled
			, ProgressBar * progress );
		void doCreateVoxelSecondaryBounce( ProgressBar * progress );
		bool doEnableClearStatic()const;
		bool doEnableCopyStatic()const;
		bool doEnableVoxelToTexture()const;
		bool doEnableSecondaryBounce()const;
		bool doEnableFirstBounceMipGen()const;
		bool doEnableSecondaryBounceMipGen()const;

	private:
		Engine & m_engine;
		RenderDevice const & m_device;
		VctConfig const & m_voxelConfig;
		Scene & m_scene;
		Camera & m_camera;
		SceneCullerUPtr m_staticsCuller;
		SceneCullerUPtr m_dynamicsCuller;
		crg::FrameGraph m_graph;
		CameraUbo m_cameraUbo;
		RawUniquePtr< RenderUbo > m_renderUbo;
		Texture m_firstBounce;
		Texture m_secondaryBounce;
		BufferUPtrT< Voxel > m_staticsVoxels;
		BufferUPtrT< Voxel > m_dynamicsVoxels;
		VoxelizerUbo & m_voxelizerUbo;
		Point4f m_grid;
		VoxelizePass * m_staticsVoxelizePass{};
		VoxelizePass * m_dynamicsVoxelizePass{};
		VoxelBufferToTexture * m_voxelToTexture{};
		VoxelSecondaryBounce * m_voxelSecondaryBounce{};
		crg::RunnableGraphPtr m_runnable;
	};
}

#endif
