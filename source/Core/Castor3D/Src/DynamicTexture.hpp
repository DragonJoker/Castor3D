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
#ifndef ___C3D_DYNAMIC_TEXTURE_H___
#define ___C3D_DYNAMIC_TEXTURE_H___

#include "Texture.hpp"

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
		: public TextureBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_renderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_renderSystem	Le render system
		 */
		C3D_API DynamicTexture( RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~DynamicTexture();
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual bool Initialise( uint32_t p_index, uint8_t p_cpuAccess = 0xFF, uint8_t p_gpuAccess = 0xFF );
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual bool BindAt( uint32_t p_index );
		/**
		 *\copydoc		Castor3D::TextureBase::Initialise
		 */
		C3D_API virtual void UnbindFrom( uint32_t p_index );
		/**
		 *\~english
		 *\brief		Defines the texture buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture
		 *\param[in]	p_pBuffer	The buffer
		 */
		C3D_API void SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\~english
		 *\brief		Defines the 3D texture buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture 3D
		 *\param[in]	p_pBuffer	The buffer
		 */
		C3D_API void SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\~english
		 *\brief		Resizes the texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API virtual void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Resizes the 3D texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture 3D
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API virtual void Resize( Castor::Point3ui const & p_size );
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
			return m_iSamplesCount;
		}
		/**
		 *\~english
		 *\brief		Defines the samples count
		 *\param[in]	val	The samples count
		 *\~french
		 *\brief		Définit le nombre de samples
		 *\param[in]	val	Le nombre de samples
		 */
		inline void SetSamplesCount( int p_iCount )
		{
			m_iSamplesCount = p_iCount;
		}
		/**
		 *\~english
		 *\brief		Sets the unit index
		 *\param[in]	p_index	The new value
		 *\~french
		 *\brief		Définit l'index de l'unité
		 *\param[in]	p_index	La nouvelle valeur
		 */
		inline void SetIndex( uint32_t p_index )
		{
			m_uiIndex = p_index;
		}
		/**
		 *\~english
		 *\brief		Sends the given image buffer to the driver
		 *\remark		Doesn't update the internal buffer, so either never use it or always use it
		 *\param[in]	p_pBuffer	The image buffer
		 *\param[in]	p_size		The image resolution
		 *\param[in]	p_eFormat	The image pixel format
		 *\~french
		 *\brief		Envoie le buffer d'image au driver
		 *\remark		Ne met pas à jour le buffer interne, donc ne l'utilisez jamais ou utilisez la tout le temps
		 *\param[in]	p_pBuffer	Le buffer d'image
		 *\param[in]	p_size		Les dimensions de l'image
		 *\param[in]	p_eFormat	Le format des pixels de l'image
		 */
		C3D_API virtual void Fill( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat ) = 0;

		using TextureBase::SetImage;

	private:
		//!\~english The texture render target	\~french La cible de rendu
		RenderTargetWPtr m_renderTarget;
		//!\~english The samples count, if it is a multisample texture	\~french Le nombre de samples dans le cas où c'est une texture multisample
		int m_iSamplesCount;
	};
}

#endif
