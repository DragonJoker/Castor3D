/*
See LICENSE file in root folder
*/
#ifndef ___C3D_VERTEX_BUFFER_H___
#define ___C3D_VERTEX_BUFFER_H___

#include "CpuBuffer.hpp"
#include "BufferElementDeclaration.hpp"
#include "BufferDeclaration.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Vertex buffer representation
	\remark		Holds the submesh vertices informations
	\~french
	\brief		Représentation d'un tampon de sommets
	\remark		Contient les informations des sommets d'un submesh
	*/
	class VertexBuffer
		: public CpuBuffer< uint8_t >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	declaration	The buffer declaration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	declaration	La déclaration du tampon.
		 */
		C3D_API VertexBuffer( Engine & engine
			, BufferDeclaration const & declaration );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~VertexBuffer();
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
		/**
		 *\~english
		 *\return		The buffer declaration.
		 *\~french
		 *\return		La déclaration du tampon.
		 */
		inline BufferDeclaration const & getDeclaration()const
		{
			return m_bufferDeclaration;
		}
		/**
		 *\~english
		 *\brief		Link all elements to this buffer.
		 *\param[in]	begin, end	The elements array iterators.
		 *\~french
		 *\brief		Lie tous les éméents à ce tampon.
		 *\param[in]	begin, end	Les itérateurs du tableau d'éléments.
		 */
		template< typename ItType >
		inline void linkCoords( ItType begin, ItType end )
		{
			uint32_t stride = m_bufferDeclaration.stride();
			uint8_t * buffer = CpuBuffer< uint8_t >::getData();

			for ( auto it = begin; it != end; ++it )
			{
				( *it )->linkCoords( buffer, stride );
				buffer += stride;
			}
		}

	protected:
		//!\~english	Buffer elements description.
		//!\~french		Description des élément du tampon.
		BufferDeclaration m_bufferDeclaration;
	};
}

#endif
