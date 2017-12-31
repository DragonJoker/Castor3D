/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Line_H___
#define ___C3D_Line_H___

#include "LineIndices.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\date		12/11/2017
	\~english
	\brief		Line implementation.
	\~french
	\brief		Implémentation d'un ligne.
	*/
	class Line
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	a, b	The two vertices indices
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	a, b	Les indices des 2 sommets.
		 */
		C3D_API Line( uint32_t a, uint32_t b );
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
			REQUIRE( index < 2 );
			return m_line.m_index[index];
		}

	private:
		//!\~english	The face vertex indices.
		//!\~french		Les indices des sommets de la face.
		LineIndices m_line;
	};
	//!\~english	An array of Lines.
	//!\~french		Un tableau de Lines.
	using LineArray = std::vector< Line >;
}

#endif
