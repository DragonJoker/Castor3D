/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ClustersConfig_H___
#define ___C3D_ClustersConfig_H___

#include "Castor3D/Render/Clustered/ClusteredModule.hpp"

namespace castor3d
{
	struct ClustersConfig
	{
		ClustersConfig();

		//!\~english	The activation status.
		//!\~french		Le statut d'activation.
		bool enabled{ true };
		//!\~english	Tells if lights are put in the BVH.
		//!\~french		Dit si les sources lumineuses sont mises dans le BVH.
		bool useLightsBVH{ true };
		//!\~english	Tells if the lights are sorted.
		//!\~french		Dit si les sources lumineuses sont triées.
		bool sortLights{ true };
		//!\~english	Tells if the depth buffer is used to reduce affected clusters.
		//!\~french		Dit si le buffer de profondeur est utlisé pour réduire le nombre de clusters affectés.
		bool parseDepthBuffer{ true };
	};
}

#endif
