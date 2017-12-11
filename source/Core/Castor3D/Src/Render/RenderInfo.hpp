/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderInfo_H___
#define ___C3D_RenderInfo_H___

#include "Castor3DPrerequisites.hpp"
#include <cstdint>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		22/06/2017
	\~english
	\brief		Holds render informations.
	\~french
	\brief		Contient les informations de rendu.
	*/
	struct RenderInfo
	{
		//!\~english	The total vertex count.
		//!\~french		Le nombre total de sommets.
		uint32_t m_totalVertexCount{ 0u };
		//!\~english	The total face count.
		//!\~french		Le nombre total de faces.
		uint32_t m_totalFaceCount{ 0u };
		//!\~english	The visble objects vertex count.
		//!\~french		Le nombre de sommets visibles.
		uint32_t m_visibleVertexCount{ 0u };
		//!\~english	The visible objects face count.
		//!\~french		Le nombre de faces visibles.
		uint32_t m_visibleFaceCount{ 0u };
		//!\~english	The total object count.
		//!\~french		Le nombre total d'objets.
		uint32_t m_totalObjectsCount{ 0u };
		//!\~english	The visible objects count.
		//!\~french		Le nombre d'objets visibles.
		uint32_t m_visibleObjectsCount{ 0u };
		//!\~english	The particles count.
		//!\~french		Le nombre de particules.
		uint32_t m_particlesCount{ 0u };
		//!\~english	The total lights count.
		//!\~french		Le nombre total de lumières.
		uint32_t m_totalLightsCount{ 0u };
		//!\~english	The visible lights count.
		//!\~french		Le nombre de lumières visibles.
		uint32_t m_visibleLightsCount{ 0u };
		//!\~english	The draw calls count.
		//!\~french		Le nombre d'appels aux fonctions de dessin.
		uint32_t m_drawCalls{ 0u };
	};
}

#endif
