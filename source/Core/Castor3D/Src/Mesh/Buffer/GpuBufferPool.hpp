/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuBufferPool_HPP___
#define ___C3D_GpuBufferPool_HPP___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	struct GpuBufferOffset
	{
		GpuBufferSPtr buffer;
		uint32_t offset;
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.10.0
	\date		22/08/2017
	\~english
	\brief		Buddy allocator implementation.
	\~french
	\brief		Implémentation d'un buddy allocator.
	*/
	class GpuBufferPool
		: public castor::OwnedBy< RenderSystem >
	{
	public:
		using BufferArray = std::vector< GpuBufferSPtr >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	renderSystem	The RenderSystem.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	renderSystem	Le RenderSystem.
		 */
		explicit GpuBufferPool( RenderSystem & renderSystem );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~GpuBufferPool();
		/**
		 *\~english
		 *\brief		Cleans up all GPU buffers.
		 *\~french
		 *\brief		Nettoie tous les tampons GPU.
		 */
		void cleanup();
		/**
		 *\~english
		 *\brief		Retrieves a GPU buffer with the given size.
		 *\param[in]	type			The buffer type.
		 *\param[in]	size			The wanted buffer size.
		 *\param[in]	accessType		Buffer access type.
		 *\param[in]	accessNature	Buffer access nature.
		 *\return		The created buffer, depending on current API.
		 *\~french
		 *\brief		Récupère un tampon GPU avec la taille donnée.
		 *\param[in]	type			Le type de tampon.
		 *\param[in]	size			La taille voulue pour le tampon.
		 *\param[in]	accessType		Type d'accès du tampon.
		 *\param[in]	accessNature	Nature d'accès du tampon.
		 *\return		Le tampon créé.
		 */
		GpuBufferOffset getGpuBuffer( BufferType type
			, uint32_t size
			, BufferAccessType accessType
			, BufferAccessNature accessNature );
		/**
		 *\~english
		 *\brief		Releases a GPU buffer.
		 *\param[in]	type			The buffer type.
		 *\param[in]	accessType		Buffer access type.
		 *\param[in]	accessNature	Buffer access nature.
		 *\param[in]	bufferOffset	The buffer offset to release.
		 *\~french
		 *\brief		Libère un tampon GPU.
		 *\param[in]	type			Le type de tampon.
		 *\param[in]	accessType		Type d'accès du tampon.
		 *\param[in]	accessNature	Nature d'accès du tampon.
		 *\param[in]	bufferOffset	Le tampon à libérer.
		 */
		void putGpuBuffer( BufferType type
			, BufferAccessType accessType
			, BufferAccessNature accessNature
			, GpuBufferOffset const & bufferOffset );

	private:
		BufferArray::iterator doFindBuffer( uint32_t size
			, BufferArray & array );

	private:
		std::map< uint32_t, BufferArray > m_buffers;
	};
}

#endif
