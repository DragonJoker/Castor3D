/*
See LICENSE file in root folder
*/
#ifndef ___C3D_INDEX_BUFFER_H___
#define ___C3D_INDEX_BUFFER_H___

#include "CpuBuffer.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Index buffer representation.
	\remark		Holds the index of the submesh vertices.
	\~french
	\brief		Représentation d'un tampon d'indices.
	\remark		Contient les indices des vertex d'un submesh.
	*/
	class IndexBuffer
		: public CpuBuffer< uint32_t >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 */
		C3D_API explicit IndexBuffer( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~IndexBuffer();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	type	Buffer access type.
		 *\param[in]	nature	Buffer access nature.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	type	Type d'accès du tampon.
		 *\param[in]	nature	Nature d'accès du tampon.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( BufferAccessType type
			, BufferAccessNature nature );
		/**
		 *\~english
		 *\brief		Clean up the GPU buffer.
		 *\~french
		 *\brief		Nettoie le tampon GPU.
		 */
		C3D_API void cleanup();
	};
}

#endif
