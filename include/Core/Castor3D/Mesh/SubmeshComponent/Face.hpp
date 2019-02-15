/*
See LICENSE file in root folder
*/
#ifndef ___C3D_FACE_H___
#define ___C3D_FACE_H___

#include "Castor3D/Mesh/SubmeshComponent/FaceIndices.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		19/10/2011
	\~english
	\brief		Face implementation
	\remark		A face is constituted from 3 vertices
	\~french
	\brief		Implémentation d'un face
	\remark		Une face est constituée de 3 vertices
	*/
	class Face
	{
	public:
		/**
		 *\~english
		 *\brief		Specified constructor
		 *\param[in]	a, b, c	The three vertices indices
		 *\~french
		 *\brief		Constructeur spécifié
		 *\param[in]	a, b, c	Les indices des 3 vertices
		 */
		C3D_API Face( uint32_t a, uint32_t b, uint32_t c );
		/**
		 *\~english
		 *\brief		Retrieves the vertex index
		 *\param[in]	index	The index of the concerned vertex
		 *\return		The vertex index
		 *\~french
		 *\brief		Récupère l'indice du vertex voulu
		 *\param[in]	index	L'index du vertex concerné
		 *\return		L'indice
		 */
		inline uint32_t operator[]( uint32_t index )const
		{
			CU_Require( index < 3 );
			return m_face.m_index[index];
		}

	private:
		//!\~english	The face vertex indices.
		//!\~french		Les indices des sommets de la face.
		FaceIndices m_face;
	};
}

#endif
