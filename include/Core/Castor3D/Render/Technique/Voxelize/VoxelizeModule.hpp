/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelizeModule_H___
#define ___C3D_VoxelizeModule_H___

#include "Castor3D/Render/Technique/TechniqueModule.hpp"
#include "Castor3D/Buffer/BufferModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Voxelize */
	//@{

	/*
	*\~english
	*\brief
	*	Copies the voxels buffer into a 3D texture.
	*\~french
	*\brief
	*	Copie le buffer de voxels dans une texture 3D.
	*/
	class VoxelBufferToTexture;
	/*
	*\~english
	*\brief
	*	Handles the voxelization of a scene.
	*\~french
	*\brief
	*	Gère la voxellisation d'une scène.
	*/
	class VoxelizePass;
	/**
	*\~english
	*\brief
	*	Holds all data needed to voxelize a scene.
	*\~french
	*\brief
	*	Contient toutes les données pour voxelliser une scène.
	*/
	class Voxelizer;
	/**
	*\~english
	*\brief
	*	Renders voxels on screen.
	*\~french
	*\brief
	*	Dessine les voxels à l'écran.
	*/
	class VoxelRenderer;
	/**
	*\~english
	*\brief
	*	Voxel configuration.
	*\~french
	*\brief
	*	Configuration des voxels.
	*/
	class VoxelSceneData;

	CU_DeclareCUSmartPtr( castor3d, Voxelizer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, VoxelRenderer, C3D_API );

	struct Voxel
	{
		uint32_t colorMask;
		uint32_t normalMask;
	};

	//@}
	//@}
	//@}
}

#endif
