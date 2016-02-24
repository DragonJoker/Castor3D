/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_CPU_BUFFER_H___
#define ___C3D_CPU_BUFFER_H___

#include "GpuBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		3D Buffer management class
	\remark		Class which holds the buffers used in 3D (texture, VBO ...)
				<br />Not to be used to manage usual buffers of data, use Castor::Buffer for that
	\~french
	\brief		Classe de gestion de tampon 3D
	\remark		A ne pas utiliser pour gérer des buffers de données, utiliser Castor::Buffer pour cela
	*/
	template< typename T >
	class CpuBuffer
		: public Castor::OwnedBy< Engine >
	{
	protected:
		typedef typename std::shared_ptr< GpuBuffer< T > > GpuBufferSPtr;
		typedef typename std::weak_ptr< GpuBuffer< T > > GpuBufferWPtr;
		typedef typename std::weak_ptr< CpuBuffer< T > > CpuBufferWPtr;
		DECLARE_TPL_VECTOR( T, T );

	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_engine		The engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine		Le moteur
		 */
		inline CpuBuffer( Engine & p_engine )
			: Castor::OwnedBy< Engine >( p_engine )
			, m_bAssigned( false )
			, m_bToDelete( false )
			, m_pBuffer()
			, m_arrayData()
			, m_uiSavedSize( 0 )

		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~CpuBuffer()
		{
		}

	public:
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		inline void Destroy()
		{
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_pBuffer->Destroy();
				m_pBuffer.reset();
			}
		}
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		inline void Cleanup()
		{
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_pBuffer->Cleanup();
			}
		}
		/**
		 *\~english
		 *\brief		Initialisation function, initialises GPU buffer
		 *\param[in]	p_type		Buffer access type
		 *\param[in]	p_nature	Buffer access nature
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation, initialise le GPU tampon
		 *\param[in]	p_type		Type d'accès du tampon
		 *\param[in]	p_nature	Nature d'accès du tampon
		 *\return		\p true si tout s'est bien passé
		 */
		inline bool Initialise( eBUFFER_ACCESS_TYPE p_type, eBUFFER_ACCESS_NATURE p_nature )
		{
			bool l_return	= false;
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_return = l_pBuffer->Initialise( p_type, p_nature );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it
		 *\remark		Maps from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_uiSize-1)*sizeof( T )]
		 *\param[in]	p_offset	The start offset in the buffer
		 *\param[in]	p_count	The mapped elements count
		 *\param[in]	p_flags	The lock flags
		 *\return		The mapped buffer address
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications
		 *\remark		Mappe de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_uiSize-1) * sizeof( T )]
		 *\param[in]	p_offset	L'offset de départ
		 *\param[in]	p_count	Le nombre d'éléments à mapper
		 *\param[in]	p_flags	Les flags de lock
		 *\return		L'adresse du tampon mappé
		 */
		inline T * Lock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags )
		{
			T * l_return = nullptr;
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_return = l_pBuffer->Lock( p_offset, p_count, p_flags );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that
		 *\remark		All modifications made in the mapped buffer are put into GPU memory
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus
		 *\remark		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU
		 */
		inline void Unlock()
		{
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_pBuffer->Unlock();
			}
		}
		/**
		 *\~english
		 *\brief		Transmits data to the GPU buffer from RAM
		 *\param[in]	p_buffer	The data
		 *\param[in]	p_size		Data buffer size
		 *\param[in]	p_type		Transfer type
		 *\param[in]	p_nature	Transfer nature
		 *\return		\p true if successful
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la ram
		 *\param[in]	p_buffer	Les données
		 *\param[in]	p_size		Taille du tampon de données
		 *\param[in]	p_type		Type de transfert
		 *\param[in]	p_nature	Nature du transfert
		 *\return		\p true si tout s'est bien passé
		 */
		inline bool Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature )
		{
			bool l_return = false;
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_return = l_pBuffer->Fill( p_buffer, p_size, p_type, p_nature );
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		inline bool Bind()
		{
			bool l_return	= false;
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_return = l_pBuffer->Bind();
			}

			return l_return;
		}
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\return		\p true si tout s'est bien passé
		 */
		inline void Unbind()
		{
			GpuBufferSPtr l_pBuffer = GetGpuBuffer();

			if ( l_pBuffer )
			{
				l_pBuffer->Unbind();
			}
		}
		/**
		 *\~english
		 *\brief		Sets element value at given index
		 *\param[in]	p_index	The index
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Définit la valeur de l'élément à l'index donné
		 *\param[in]	p_index	L'index
		 *\param[in]	p_value	La valeur
		 */
		inline void SetElement( uint32_t p_index, T const & p_value )
		{
			REQUIRE( p_index < m_arrayData.size() );
			m_arrayData[p_index] = p_value;
		}
		/**
		 *\~english
		 *\brief		Adds a value at the end of the buffer
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Ajoute une valeur à la fin du tampon
		 *\param[in]	p_value	La valeur
		 */
		inline void AddElement( T const & p_value )
		{
			m_arrayData.push_back( p_value );
		}
		/**
		 *\~english
		 *\brief		Retrieves the filled buffer size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille remplie du tampon
		 *\return		La taille
		 */
		inline uint32_t GetSize()const
		{
			uint32_t l_uiReturn = m_uiSavedSize;

			if ( m_arrayData.size() )
			{
				l_uiReturn = uint32_t( m_arrayData.size() );
			}

			return l_uiReturn;
		}
		/**
		 *\~english
		 *\brief		Retrieves the allocated buffer size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille allouée du tampon
		 *\return		La taille
		 */
		inline uint32_t GetCapacity()const
		{
			// Safe cast since I limit a buffer size to uint32_t
			return uint32_t( m_arrayData.capacity() );
		}
		/**
		 *\~english
		 *\brief		Sets the allocated size of the buffer
		 *\param[in]	p_uiNewSize	The new size
		 *\~french
		 *\brief		Définit la taille allouée du tampon
		 *\param[in]	p_uiNewSize	La nouvelle taille
		 */
		inline void Resize( uint32_t p_uiNewSize )
		{
			m_arrayData.resize( p_uiNewSize, 0 );
		}
		/**
		 *\~english
		 *\brief		Increases the allocated size of the buffer
		 *\param[in]	p_uiIncrement	The size increment
		 *\~french
		 *\brief		Augmente la taille allouée du tampon
		 *\param[in]	p_uiIncrement	L'incrément de taille
		 */
		inline void Reserve( uint32_t p_uiIncrement )
		{
			if ( p_uiIncrement + GetCapacity() < p_uiIncrement )
			{
				throw std::range_error( "Can't reserve more space for this buffer" );
			}
			else
			{
				m_arrayData.reserve( m_arrayData.capacity() + p_uiIncrement );
			}
		}
		/**
		 *\~english
		 *\brief		Clears the buffer
		 *\~french
		 *\brief		Vide le tampon
		 */
		inline void Clear()
		{
			m_uiSavedSize = uint32_t( m_arrayData.size() );
			m_arrayData.clear();
		}
		/**
		 *\~english
		 *\brief		Retrieves the delete status of the buffer
		 *\return		The delete status
		 *\~french
		 *\brief		Récupère le statut de suppression du tampon
		 *\return		Le statut de suppression
		 */
		inline bool IsToDelete()const
		{
			return m_bToDelete;
		}
		/**
		 *\~english
		 *\brief		Retrieves the assignment status of the buffer
		 *\return		The assignment status
		 *\~french
		 *\brief		Récupère le statut d'affectation du tampon
		 *\return		Le statut d'affectation
		 */
		inline bool IsAssigned()const
		{
			return m_bAssigned;
		}
		/**
		 *\~english
		 *\brief		Retrieves the GPU buffer
		 *\return		The GPU buffer
		 *\~french
		 *\brief		Récupère le tampon GPU
		 *\return		Le tampon GPU
		 */
		inline GpuBufferSPtr GetGpuBuffer()const
		{
			return m_pBuffer;
		}
		/**
		 *\~english
		 *\brief		Sets the assignment status to \p true
		 *\~french
		 *\brief		Définit le status d'affectation à \p true
		 */
		inline void Assign()
		{
			m_bAssigned = true;
		}
		/**
		 *\~english
		 *\brief		Sets the assignment status to \p false
		 *\~french
		 *\brief		Définit le status d'affectation à \p false
		 */
		inline void Unassign()
		{
			m_bAssigned = false;
		}
		/**
		 *\~english
		 *\brief		Retrieves the data pointer
		 *\return		The data pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Le pointeur sur les données
		 */
		inline T const * data()const
		{
			return ( m_arrayData.size() ? &m_arrayData[0] : nullptr );
		}
		/**
		 *\~english
		 *\brief		Retrieves the data pointer
		 *\return		The data pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Le pointeur sur les données
		 */
		inline T * data()
		{
			return ( m_arrayData.size() ? &m_arrayData[0] : nullptr );
		}

	protected:
		//!\~english Tells the buffer is to be deleted at next render loop	\~french Dit que le tampon doit être détruit à la prochaîne boucle de rendu
		bool m_bToDelete;
		//!\~english Tells the buffer has it's GPU buffer assigned	\~french Dit que le tampon s'est vu affecter un tampon GPU
		bool m_bAssigned;
		//!\~english The GPU buffer	\~french Le tampon GPU
		GpuBufferSPtr m_pBuffer;
		//!\~english The buffer data	\~french Les données du tampon
		TArray m_arrayData;
		//!<\~english The saved buffer size (to still have a size after clear)	\~french La taille sauvegardée, afin de toujours l'avoir après un clear
		uint32_t m_uiSavedSize;
	};
}

#endif
