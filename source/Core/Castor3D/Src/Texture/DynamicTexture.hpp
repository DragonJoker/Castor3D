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
#ifndef ___C3D_DYNAMIC_TEXTURE_H___
#define ___C3D_DYNAMIC_TEXTURE_H___

#include "TextureLayout.hpp"

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Dynamic texture class
	\remark		A dynamic texture can update it's buffer
	\~french
	\brief		Class de texture dynamique
	\remark		Une texture statique peut mettre à jour son buffer
	*/
	class DynamicTexture
		: public TextureLayout
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_type			The texture type.
		 *\param[in]	p_cpuAccess		The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		The required GPU access (combination of eACCESS_TYPE).
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_renderSystem	Le render system.
		 *\param[in]	p_type			Le type de texture.
		 *\param[in]	p_cpuAccess		Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess		Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 */
		C3D_API DynamicTexture( RenderSystem & p_renderSystem, eTEXTURE_TYPE p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~DynamicTexture();
		/**
		 *\~english
		 *\brief		Retrieves the render target
		 *\return		The target
		 *\~french
		 *\brief		Récupère la cible de rendu
		 *\return		La cible
		 */
		inline RenderTargetSPtr	GetRenderTarget()const
		{
			return m_renderTarget.lock();
		}
		/**
		 *\~english
		 *\brief		Defines the render target
		 *\param[in]	p_target	The target
		 *\~french
		 *\brief		Définit la cible de rendu
		 *\param[in]	p_target	La cible
		 */
		inline void SetRenderTarget( RenderTargetSPtr p_target )
		{
			m_renderTarget = p_target;
		}
		/**
		 *\~english
		 *\brief		Retrieves the samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples
		 *\return		Le nombre de samples
		 */
		inline int	GetSamplesCount()const
		{
			return m_samplesCount;
		}
		/**
		 *\~english
		 *\brief		Defines the samples count
		 *\param[in]	p_count	The samples count
		 *\~french
		 *\brief		Définit le nombre de samples
		 *\param[in]	p_count	Le nombre de samples
		 */
		inline void SetSamplesCount( int p_count )
		{
			m_samplesCount = p_count;
		}
		/**
		 *\~english
		 *\brief		Sends the given image buffer to the driver
		 *\remarks		Doesn't update the internal buffer, so either never use it or always use it
		 *\param[in]	p_buffer	The image buffer
		 *\param[in]	p_size		The image resolution
		 *\param[in]	p_format	The image pixel format
		 *\~french
		 *\brief		Envoie le buffer d'image au driver
		 *\remarks		Ne met pas à jour le buffer interne, donc ne l'utilisez jamais ou utilisez la tout le temps
		 *\param[in]	p_buffer	Le buffer d'image
		 *\param[in]	p_size		Les dimensions de l'image
		 *\param[in]	p_format	Le format des pixels de l'image
		 */
		C3D_API virtual void Fill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format ) = 0;

	private:
		//!\~english The texture render target	\~french La cible de rendu
		RenderTargetWPtr m_renderTarget;
		//!\~english The samples count, if it is a multisample texture	\~french Le nombre de samples dans le cas où c'est une texture multisample
		int m_samplesCount;
	};
}

#endif
