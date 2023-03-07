/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderInfo_H___
#define ___C3D_RenderInfo_H___

#include "RenderModule.hpp"

namespace castor3d
{
	struct RenderInfo
	{
		//!\~english	The total vertex count.
		//!\~french		Le nombre total de sommets.
		uint32_t totalVertexCount{ 0u };
		//!\~english	The total face count.
		//!\~french		Le nombre total de faces.
		uint32_t totalFaceCount{ 0u };
		//!\~english	The visble objects vertex count.
		//!\~french		Le nombre de sommets visibles.
		uint32_t visibleVertexCount{ 0u };
		//!\~english	The visible objects face count.
		//!\~french		Le nombre de faces visibles.
		uint32_t visibleFaceCount{ 0u };
		//!\~english	The total object count.
		//!\~french		Le nombre total d'objets.
		uint32_t totalObjectsCount{ 0u };
		//!\~english	The visible objects count.
		//!\~french		Le nombre d'objets visibles.
		uint32_t visibleObjectsCount{ 0u };
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		uint32_t particlesCount{ 0u };
		//!\~english	The total lights count.
		//!\~french		Le nombre total de lumières.
		uint32_t totalLightsCount{ 0u };
		//!\~english	The visible lights count.
		//!\~french		Le nombre de lumières visibles.
		uint32_t visibleLightsCount{ 0u };
		//!\~english	The visible ovzrlays count.
		//!\~french		Le nombre d'incrustations visibles.
		uint32_t visibleOverlayCount{ 0u };
		//!\~english	The draw calls count.
		//!\~french		Le nombre d'appels aux fonctions de dessin.
		uint32_t drawCalls{ 0u };
	};
}

#endif
