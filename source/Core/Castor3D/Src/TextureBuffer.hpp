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
#ifndef ___C3D_TEXTURE_BUFFER_H___
#define ___C3D_TEXTURE_BUFFER_H___

#include "CpuBuffer.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Texture buffer representation, holds texture data
	\remark		Holds the texture image
	\~french
	\brief		Repr�sentation d'un tampon de texture
	\remark		Contient l'image d'une texture
	*/
	class C3D_API TextureBuffer
		:	public CpuBuffer< uint8_t >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	The RenderSystem
		 */
		TextureBuffer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~TextureBuffer();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_format	The texture's pixel format
		 *\param[in]	p_uiSize	The buffer size
		 *\param[in]	p_pBytes	The buffer data
		 *\param[in]	p_pProgram	The shader program
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_format	Le format des pixels de la texture
		 *\param[in]	p_uiSize	La taille du tampon de texture
		 *\param[in]	p_pBytes	Les donn�es du tampon
		 *\param[in]	p_pProgram	Le programme shader
		 */
		virtual void Initialise( Castor::ePIXEL_FORMAT const & p_format, uint32_t p_uiSize, uint8_t const * p_pBytes, Castor3D::ShaderProgramBaseSPtr p_pProgram );
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_format	The texture's pixel format
		 *\param[in]	p_pBytes	The buffer data
		 *\param[in]	p_pProgram	The shader program
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_format	Le format des pixels de la texture
		 *\param[in]	p_pBytes	Les donn�es du tampon
		 *\param[in]	p_pProgram	Le programme shader
		 */
		template< uint32_t N >
		inline void Initialise( Castor::ePIXEL_FORMAT const & p_format, uint8_t const( & p_pBytes )[N], Castor3D::ShaderProgramBaseSPtr p_pProgram )
		{
			Initialise( p_format, N, p_pBytes, p_pProgram );
		}
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_format	The texture's pixel format
		 *\param[in]	p_pBytes	The buffer data
		 *\param[in]	p_pProgram	The shader program
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_format	Le format des pixels de la texture
		 *\param[in]	p_pBytes	Les donn�es du tampon
		 *\param[in]	p_pProgram	Le programme shader
		 */
		inline void Initialise( Castor::ePIXEL_FORMAT const & p_format, std::vector< uint8_t > const & p_pBytes, Castor3D::ShaderProgramBaseSPtr p_pProgram )
		{
			Initialise( p_format, uint32_t( p_pBytes.size() ), &p_pBytes[0], p_pProgram );
		}

	private:
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\~french
		 *\brief		Fonction d'initialisation
		 */
		virtual bool DoCreateBuffer();

	protected:
		//!\~english The texture pixel format	\~french Le format des pixels de la texture
		Castor::ePIXEL_FORMAT m_pixelFormat;
		//!\~english The texture size	\~french La taille du tampon de la texture
		uint32_t m_uiSize;
		//!\~english The texture data	\~french Les donn�es de la texture
		uint8_t const * m_pBytes;
	};
}

#endif
