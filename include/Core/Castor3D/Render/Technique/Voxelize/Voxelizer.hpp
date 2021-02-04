/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Voxelizer_H___
#define ___C3D_Voxelizer_H___

#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelBufferToTexture.hpp"
#include "Castor3D/Render/Technique/Voxelize/VoxelizePass.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"

namespace castor3d
{
	class Voxelizer
	{
	public:
		/**
		 *\~english
		 *\param[in]	engine			The engine.
		 *\param[in]	size			The render dimensions.
		 *\param[in]	scene			The rendered scene.
		 *\~french
		 *\param[in]	engine			Le moteur.
		 *\param[in]	size			Les dimensions du rendu.
		 *\param[in]	scene			La scène rendue.
		 */
		C3D_API Voxelizer( Engine & engine
			, RenderDevice const & device
			, Scene & scene
			, Camera & camera
			, ashes::ImageView colourView
			, MatrixUbo & matrixUbo
			, VoxelizerUbo & voxelizerUbo
			, VoxelSceneData const & voxelConfig );
		C3D_API ~Voxelizer();
		/**
		 *\~english
		 *\brief		Updates pass.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Met à jour la passe.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Updates pass.
		 *\param[out]	info	Receives the render informations.
		 *\~french
		 *\brief		Met à jour la passe.
		 *\param[out]	info	Reçoit les informations de rendu.
		 */
		C3D_API void update( GpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Renders nodes.
		 *\param[in]	toWait	The semaphore from the previous render pass.
		 *\~french
		 *\brief		Dessine les noeuds.
		 *\param[in]	toWait	Le sémaphore de la passe de rendu précédente.
		 */
		C3D_API ashes::Semaphore const & render( RenderDevice const & device
			, ashes::Semaphore const & toWait );
		/**
		 *\copydoc		castor3d::RenderTechniquePass::accept
		 */
		C3D_API void accept( RenderTechniqueVisitor & visitor );

		C3D_API void listIntermediates( RenderTechniqueVisitor & visitor );

		TextureUnit const & getResult()const
		{
			return m_result;
		}

	private:
		Engine & m_engine;
		VoxelSceneData const & m_voxelConfig;
		DummyCuller m_culler;
		MatrixUbo m_matrixUbo;
		TextureUnit m_result;
		ashes::BufferPtr< Voxel > m_voxels;
		VoxelizerUbo & m_voxelizerUbo;
		VoxelizePass m_voxelizePass;
		VoxelBufferToTexture m_voxelToTexture;
		castor::Point4f m_grid;
	};
}

#endif
