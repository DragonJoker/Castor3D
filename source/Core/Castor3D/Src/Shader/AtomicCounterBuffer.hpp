/*
See LICENSE file in root folder
*/
#ifndef ___C3D_AtomicCounterBuffer_H___
#define ___C3D_AtomicCounterBuffer_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		24/11/2016
	\~english
	\brief		Atomic counter buffer interface.

	\~french
	\brief		Interface de tampon de compteur atomique.
	*/
	class AtomicCounterBuffer
		: public castor::OwnedBy< ShaderProgram >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	name	The buffer name.
		 *\param[in]	program	The parent program.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name	Le nom du tampon.
		 *\param[in]	program	Le programme parent.
		 */
		C3D_API AtomicCounterBuffer( castor::String const & name
			, ShaderProgram & program );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~AtomicCounterBuffer();
		/**
		 *\~english
		 *\brief		Initialises the GPU buffer.
		 *\param[in]	size	The buffer size.
		 *\param[in]	index	The buffer's binding point.
		 *\return		\p false if any problem occured.
		 *\~french
		 *\brief		Initialise le tampon GPU.
		 *\param[in]	size	Taille du tampon.
		 *\param[in]	index	Le point d'attache du tampon.
		 *\return		\p false if any problem occured.
		 */
		C3D_API bool initialise( uint32_t size
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Cleans all the variables up and the GPU buffer associated.
		 *\~french
		 *\brief		Nettoie toutes les variables et le tampon GPU associé.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Locks the buffer, id est maps it into memory so we can modify it.
		 *\remarks		Maps from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_uiSize-1)*sizeof( T )].
		 *\param[in]	offset	The start offset in the buffer.
		 *\param[in]	count	The mapped elements count.
		 *\param[in]	flags	The lock flags.
		 *\return		The mapped buffer address.
		 *\~french
		 *\brief		Locke le tampon, càd le mappe en mémoire ram afin d'y autoriser des modifications.
		 *\remarks		Mappe de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_uiSize-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Le nombre d'éléments à mapper.
		 *\param[in]	flags	Les flags de lock.
		 *\return		L'adresse du tampon mappé.
		 */
		C3D_API uint32_t * lock( uint32_t offset
			, uint32_t count
			, AccessTypes const & flags );
		/**
		 *\~english
		 *\brief		Unlocks the buffer, id est unmaps it from memory so no modification can be made after that.
		 *\remarks		All modifications made in the mapped buffer are put into GPU memory.
		 *\~french
		 *\brief		Un locke le tampon, càd l'unmappe de la mémoire ram afin de ne plus autoriser de modifications dessus.
		 *\remarks		Toutes les modifications qui avaient été effectuées sur le tampon mappé sont rapatriées dans la mémoire GPU.
		 */
		C3D_API void unlock();
		/**
		 *\~english
		 *\brief		Transfers data to the GPU buffer from RAM.
		 *\remarks		Transfers data from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count	Elements count.
		 *\param[in]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données au tampon GPU à partir de la RAM.
		 *\remarks		Transfère les données de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Nombre d'éléments.
		 *\param[in]	buffer	Les données.
		 */
		C3D_API void upload( uint32_t offset
			, uint32_t count
			, uint32_t const * buffer );
		/**
		 *\~english
		 *\brief		Transfers data from the GPU buffer to RAM.
		 *\remarks		Transfers data from buffer[p_offset*sizeof( T )] to buffer[(p_offset+p_count-1)*sizeof( T )].
		 *\param[in]	offset	The start offset.
		 *\param[in]	count	Elements count.
		 *\param[in]	buffer	The data.
		 *\~french
		 *\brief		Transfère des données du tampon GPU vers la RAM.
		 *\remarks		Transfère les données de tampon[p_offset*sizeof( T )] à tampon[(p_offset+p_count-1) * sizeof( T )].
		 *\param[in]	offset	L'offset de départ.
		 *\param[in]	count	Nombre d'éléments.
		 *\param[in]	buffer	Les données.
		 */
		C3D_API void download( uint32_t offset
			, uint32_t count
			, uint32_t * buffer );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 */
		C3D_API void bind();
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API void unbind();
		/**
		 *\~english
		 *\brief		Copies data from given buffer to this one.
		 *\param[in]	src			The source buffer.
		 *\param[in]	srcOffset	The offset in the source buffer.
		 *\param[in]	size		The number of elements to copy.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Copie les données du tampon donné dans celui-ci.
		 *\param[in]	src			Le tampon source.
		 *\param[in]	srcOffset	Le décalage dans le tampon source.
		 *\param[in]	size		Le nombre d'éléments à copier.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API void copy( GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t size );
		/**
		 *\~english
		 *\return		The GPU buffer.
		 *\~french
		 *\return		Le tampon GPU.
		 */
		inline GpuBuffer const & getGpuBuffer()const
		{
			REQUIRE( m_gpuBuffer );
			return *m_gpuBuffer;
		}

	protected:
		//!\~english	The GPU buffer.
		//!\~french		Le tampon GPU.
		GpuBufferSPtr m_gpuBuffer;
		//!\~english	The offset in the GPU buffer.
		//!\~french		Le décalage dans le tampon GPU.
		uint32_t m_offset{ 0u };
	};
}

#endif
