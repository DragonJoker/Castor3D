/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_CPU_TRANSFORM_BUFFER_H___
#define ___C3D_CPU_TRANSFORM_BUFFER_H___

#include "CpuBuffer.hpp"
#include "TransformBufferElementDeclaration.hpp"
#include "TransformBufferDeclaration.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version 	0.9.0
	\date 		19/10/2016
	\~english
	\brief		Transform feedback buffer representation.
	\~french
	\brief		Représentation d'un tampon de Transform feedback.
	*/
	template< typename T >
	class CpuTransformBuffer
		: public Castor::OwnedBy< Engine >
	{
	protected:
		using CpuTransformBufferWPtr = typename std::weak_ptr< CpuTransformBuffer< T > >;

		DECLARE_TPL_VECTOR( T, T );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine		The engine.
		 *\param[in]	p_declaration	The buffer elements declaration.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine		Le moteur.
		 *\param[in]	p_declaration	La déclaration des éléments du tampon.
		 */
		inline CpuTransformBuffer( Engine & p_engine, TransformBufferDeclaration const & p_declaration );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		inline ~CpuTransformBuffer();
		/**
		 *\~english
		 *\brief		GPU side creation function.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Fonction de création sur le GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		inline bool Create();
		/**
		 *\~english
		 *\brief		GPU side destruction function.
		 *\~french
		 *\brief		Fonction de destruction sur le GPU.
		 */
		inline void Destroy();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		inline bool Initialise();
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		inline void Cleanup();
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it
		 *\remarks		Maps from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_uiSize-1)*sizeof( T )]
		 *\param[in]	p_offset	The start offset in the buffer
		 *\param[in]	p_count	The mapped elements count
		 *\param[in]	p_flags	The lock flags
		 *\return		The mapped buffer address
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications
		 *\remarks		Mappe de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_uiSize-1) * sizeof( T )]
		 *\param[in]	p_offset	L'offset de départ
		 *\param[in]	p_count	Le nombre d'éléments à mapper
		 *\param[in]	p_flags	Les flags de lock
		 *\return		L'adresse du tampon mappé
		 */
		inline T * Lock( uint32_t p_offset, uint32_t p_count, AccessType p_flags );
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU
		 */
		inline void Unlock();
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
		inline bool Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		inline bool Bind();
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\return		\p true si tout s'est bien passé
		 */
		inline void Unbind();
		/**
		*\~english
		*\brief		Index opertor.
		*\param[in]	p_index	The index.
		*\~french
		*\brief		Opérateur d'indexation.
		*\param[in]	p_index	L'index.
		*/
		inline T const & operator[]( uint32_t p_index )const;
		/**
		*\~english
		*\brief		Index opertor.
		*\param[in]	p_index	The index.
		*\~french
		*\brief		Opérateur d'indexation.
		*\param[in]	p_index	L'index.
		*/
		inline T & operator[]( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Adds a value at the end of the buffer
		 *\param[in]	p_value	The value
		 *\~french
		 *\brief		Ajoute une valeur à la fin du tampon
		 *\param[in]	p_value	La valeur
		 */
		inline void AddElement( T const & p_value );
		/**
		 *\~english
		 *\brief		Retrieves the filled buffer size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille remplie du tampon
		 *\return		La taille
		 */
		inline uint32_t GetSize()const;
		/**
		 *\~english
		 *\brief		Retrieves the allocated buffer size
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille allouée du tampon
		 *\return		La taille
		 */
		inline uint32_t GetCapacity()const;
		/**
		 *\~english
		 *\brief		Sets the allocated size of the buffer
		 *\param[in]	p_uiNewSize	The new size
		 *\~french
		 *\brief		Définit la taille allouée du tampon
		 *\param[in]	p_uiNewSize	La nouvelle taille
		 */
		inline void Resize( uint32_t p_uiNewSize );
		/**
		 *\~english
		 *\brief		Increases the allocated size of the buffer
		 *\param[in]	p_uiIncrement	The size increment
		 *\~french
		 *\brief		Augmente la taille allouée du tampon
		 *\param[in]	p_uiIncrement	L'incrément de taille
		 */
		inline void Grow( uint32_t p_uiIncrement );
		/**
		 *\~english
		 *\brief		Clears the buffer
		 *\~french
		 *\brief		Vide le tampon
		 */
		inline void Clear();
		/**
		 *\~english
		 *\brief		Retrieves the GPU buffer
		 *\return		The GPU buffer
		 *\~french
		 *\brief		Récupère le tampon GPU
		 *\return		Le tampon GPU
		 */
		inline GpuTransformBuffer const & GetGpuBuffer()const;
		/**
		 *\~english
		 *\brief		Retrieves the data pointer
		 *\return		The data pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Le pointeur sur les données
		 */
		inline T const * data()const;
		/**
		 *\~english
		 *\brief		Retrieves the data pointer
		 *\return		The data pointer
		 *\~french
		 *\brief		Récupère le pointeur sur les données
		 *\return		Le pointeur sur les données
		 */
		inline T * data();
		/**
		 *\~english
		 *\brief		Gets buffer declaration
		 *\return		The buffer declaration
		 *\~french
		 *\brief		Récupère la déclaration du tampon
		 *\return		La déclaration du tampon
		 */
		inline TransformBufferDeclaration const & GetDeclaration()const;

	protected:
		//!\~english	Buffer elements description.
		//!\~french		Description des élément du tampon.
		TransformBufferDeclaration m_bufferDeclaration;
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuTransformBufferUPtr m_gpuBuffer;
		//!\~english	The buffer data.
		//!\~french		Les données du tampon.
		TArray m_data;
		//!<\~english	The saved buffer size (to still have a size after clear).
		//!\~french		La taille sauvegardée, afin de toujours l'avoir après un clear.
		uint32_t m_savedSize;

	public:
		//!\~english	The byte size of one element in the buffer.
		//!\~french		La taille en octets d'un élément du tampon. 
		static uint32_t const ElementSize = sizeof( T );
	};
}

#include "CpuTransformBuffer.inl"

#endif
