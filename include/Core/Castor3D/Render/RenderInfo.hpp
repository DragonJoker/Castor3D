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
		uint32_t totalVertexCount{};
		//!\~english	The total face count.
		//!\~french		Le nombre total de faces.
		uint32_t totalFaceCount{};
		//!\~english	The visble objects vertex count.
		//!\~french		Le nombre de sommets visibles.
		uint32_t visibleVertexCount{};
		//!\~english	The visible objects face count.
		//!\~french		Le nombre de faces visibles.
		uint32_t visibleFaceCount{};
		//!\~english	The total object count.
		//!\~french		Le nombre total d'objets.
		uint32_t totalObjectsCount{};
		//!\~english	The visible objects count.
		//!\~french		Le nombre d'objets visibles.
		uint32_t visibleObjectsCount{};
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		uint32_t particlesCount{};
		//!\~english	The total lights count.
		//!\~french		Le nombre total de lumières.
		uint32_t totalLightsCount{};
		//!\~english	The visible lights count.
		//!\~french		Le nombre de lumières visibles.
		uint32_t visibleLightsCount{};
		//!\~english	The visible overlays count.
		//!\~french		Le nombre d'incrustations visibles.
		uint32_t visibleOverlaysCount{};
		//!\~english	The visible overlays quads count.
		//!\~french		Le nombre de quads d'incrustations visibles.
		uint32_t visibleOverlayQuadsCount{};
		//!\~english	The draw calls count.
		//!\~french		Le nombre d'appels aux fonctions de dessin.
		uint32_t drawCalls{};
		//!\~english	The binary size of uploads.
		//!\~french		La taille binaire des uploads.
		uint32_t uploadSize{};
		//!\~english	The upload staging buffers count.
		//!\~french		Le nombre de staging buffers pour l'upload.
		uint32_t stagingBuffersCount{};
	};
}

#endif
