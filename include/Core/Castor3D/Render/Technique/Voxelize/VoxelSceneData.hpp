/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VoxelSceneData_H___
#define ___C3D_VoxelSceneData_H___

#include "VoxelizeModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace castor3d
{
	class VoxelSceneData
	{
	public:
		/**
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		Skybox loader.
		\~english
		\brief		Loader de Skybox.
		*/
		class TextWriter
			: public castor::TextWriter< VoxelSceneData >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a HdrConfig into a text file.
			 *\param[in]	obj		The HdrConfig to save.
			 *\param[in]	file	The file to write the HdrConfig in.
			 *\~french
			 *\brief		Ecrit une HdrConfig dans un fichier texte.
			 *\param[in]	obj		La HdrConfig.
			 *\param[in]	file	Le fichier.
			 */
			C3D_API bool operator()( VoxelSceneData const & obj
				, castor::TextFile & file )override;
		};

	public:
		bool enabled{ false };
		bool conservativeRasterization{ false };
		bool temporalSmoothing{ false };
		//!\~english	The maximum distance for cones.
		//!\~french		La distance maximale pour les cones.
		float maxDistance{ 20.0f };
		//!\~english	The number of radiance cones used in voxel cone tracing.
		//!\~french		Le nombre de cones de radiance utilisés dans le voxel cone tracing.
		castor::RangedValue< uint32_t > numCones{ 2u, castor::makeRange( 1u, 16u ) };
		//!\~english	The size of a ray step in voxel cone tracint.
		//!\~french		La taille d'un pas de rayon dans le voxel cone tracing.
		float rayStepSize{ 0.75f };
		float voxelSize{ 1.0f };
	};
}

#endif
