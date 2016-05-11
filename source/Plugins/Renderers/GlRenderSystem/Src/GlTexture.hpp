/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___GL_TEXTURE_H___
#define ___GL_TEXTURE_H___

#include "GlObject.hpp"

#include <StaticTexture.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture, and texture storage.
	*/
	class GlTexture
		: private Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >
	{
		using ObjectType = Object< std::function< bool( int, uint32_t * ) >, std::function< bool( int, uint32_t const * ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 *\param[in]	p_static		Tells if this texture is a static one.
		 */
		GlTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem, bool p_static );
		/**
		 *\brief		Destructor.
		 */
		~GlTexture();
		/**
		 *\brief		Initialises the texture and IO buffers.
		 *\param[in]	p_buffer	The texture image buffer.
		 *\param[in]	p_dimension	The texture dimension.
		 *\param[in]	p_depth		The texture depth, in case p_dimension is eTEXTURE_TYPE_3D or eTEXTURE_TYPE_2DARRAY.
		 *\return		true on success.
		 */
		bool Initialise( Castor::PxBufferBaseSPtr p_buffer, Castor3D::eTEXTURE_TYPE p_dimension, uint32_t p_depth, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\brief		Cleans the texture and IO buffers up.
		 */
		void Cleanup();
		/**
		 *\brief		Initialises the texture initial storage data.
		 *\param[in]	p_buffer	The texture pixel buffer.
		 *\param[in]	p_size		The pixel buffer dimensions.
		 *\param[in]	p_format	The pixel buffer format.
		 */
		void Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\brief		Binds this texture.
		 *\param[in]	p_index	The texture unit index.
		 *\return		true on success.
		 */
		bool Bind( uint32_t p_index )const;
		/**
		 *\brief		Unbinds this texture.
		 *\param[in]	p_index	The texture unit index.
		 */
		void Unbind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of eACCESS_TYPE
		 *\return		The image buffer
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de eACCESS_TYPE
		 *\return		Le buffer de l'image
		 */
		uint8_t * Lock( uint32_t p_lock );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU
		 */
		void Unlock( bool p_modified );
		/**
		*\brief		Forces mipmaps generation.
		*/
		void GenerateMipmaps()const;
		/**
		*\return	The OpenGL texture dimension.
		*/
		eGL_TEXDIM GetGlDimension()const
		{
			return m_glDimension;
		}

	private:
		bool DoCreateStorage( uint8_t p_cpuAccess, uint8_t p_gpuAccess );

	public:
		using ObjectType::Create;
		using ObjectType::Destroy;
		using ObjectType::IsValid;
		using ObjectType::GetGlName;
		using ObjectType::GetOpenGl;

	private:
		//! The RenderSystem.
		GlRenderSystem * m_glRenderSystem;
		//! The texture sstorage, to handle IO operations,
		GlTextureStorageUPtr m_storage;
		//! The OpenGL texture dimension.
		eGL_TEXDIM m_glDimension;
		//! Tells if this texture is a static one.
		bool m_static;
	};
}

#endif
