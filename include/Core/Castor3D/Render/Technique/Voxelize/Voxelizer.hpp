/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Voxelizer_H___
#define ___C3D_Voxelizer_H___

#include "VoxelizeModule.hpp"

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

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
		C3D_API Voxelizer( RenderDevice const & device
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
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	device	The GPU device.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

		C3D_API void listIntermediates( RenderTechniqueVisitor & visitor );

		TextureUnit const & getFirstBounce()const
		{
			return m_firstBounce;
		}
		
		TextureUnit const & getSecondaryBounce()const
		{
			return m_secondaryBounce;
		}

	private:
		Engine & m_engine;
		VoxelSceneData const & m_voxelConfig;
		DummyCuller m_culler;
		MatrixUbo m_matrixUbo;
		TextureUnit m_firstBounce;
		TextureUnit m_secondaryBounce;
		ashes::BufferPtr< Voxel > m_voxels;
		VoxelizerUbo & m_voxelizerUbo;
		VoxelizePassUPtr m_voxelizePass;
		VoxelBufferToTextureUPtr m_voxelToTexture;
		VoxelSecondaryBounceUPtr m_voxelSecondaryBounce;
		castor::Point4f m_grid;
	};
}

#endif
