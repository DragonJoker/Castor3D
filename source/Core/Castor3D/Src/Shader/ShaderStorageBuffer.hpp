/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderStorageBuffer_H___
#define ___C3D_ShaderStorageBuffer_H___

#include "Castor3DPrerequisites.hpp"

#include "Mesh/Buffer/CpuBuffer.hpp"

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		22/11/2016
	\~english
	\brief		Shader storage buffer interface

	\~french
	\brief		Interface de tampon de stockage shader.
	*/
	class ShaderStorageBuffer
		: public CpuBuffer< uint8_t >
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
		C3D_API explicit ShaderStorageBuffer( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~ShaderStorageBuffer();
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
		 *\brief		sets the buffer's binding point.
		 *\param[in]	index	The binding point.
		 *\~french
		 *\brief		Définit le point d'attache du tampon.
		 *\param[in]	index	Le point d'attache.
		 */
		C3D_API void bindTo( uint32_t index )const;
	};
}

#endif
