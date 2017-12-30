/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERTEX_BONE_DATA_H___
#define ___C3D_VERTEX_BONE_DATA_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	static const uint32_t C3D_MAX_BONES_PER_VERTEX = 8;
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date 		26/06/2013
	\~english
	\brief		Holds bone data for one vertex
	\~french
	\brief		Contient les données de bones pour un vertice
	*/
	struct VertexBoneData
	{
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		C3D_API VertexBoneData();
		/**
		 *\~english
		 *\brief		adds bone informations to the vertex
		 *\param[in]	p_boneId	The bone ID
		 *\param[in]	p_weight	The bone weight
		 *\~french
		 *\brief		Ajoute des informations de bone au vertice
		 *\param[in]	p_boneId	L'ID du bone
		 *\param[in]	p_weight	Le poids du bone
		 */
		C3D_API void addBoneData( uint32_t p_boneId, real p_weight );

		//!\~english	The bones ID.
		//!\~french		L'ID des bones.
		std::array< uint32_t, C3D_MAX_BONES_PER_VERTEX > m_ids;
		//!\~english	The bones weights.
		//!\~french		Les poids des bones.
		std::array< real, C3D_MAX_BONES_PER_VERTEX > m_weights;
	};
}

#endif
