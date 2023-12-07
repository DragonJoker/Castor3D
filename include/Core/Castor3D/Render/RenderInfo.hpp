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
		//!\~english	The visible objects counts.
		//!\~french		Les comptes d'objets visibles.
		RenderCounts visible{};
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		uint32_t particlesCount{};
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
