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
		GpuBufferPool( RenderSystem & renderSystem );
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
