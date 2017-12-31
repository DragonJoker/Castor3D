/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RENDER_BUFFER_H___
#define ___C3D_RENDER_BUFFER_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/Size.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		12/11/2012
	\~english
	\brief		Render buffer base class
	\remark		A render buffer is a buffer that receives specific PixelComponents from a frame buffer
	\~french
	\brief		Classe de base d'un tampon de rendu
	\remark		Un tampon de rendu est un tampon qui reçoit une composante spécifique d'un tampon d'image
	*/
	class RenderBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type			The buffer PixelComponents
		 *\param[in]	p_format	The buffer pixel format
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type			La composante du tampon
		 *\param[in]	p_format	Le format des pixels du tampon
		 */
		C3D_API RenderBuffer( BufferComponent p_type, castor::PixelFormat p_format );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~RenderBuffer();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API virtual void destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_size	The buffer dimensions
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_size	Les dimensions du tampon
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool initialise( castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void cleanup() = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 */
		C3D_API virtual void bind() = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API virtual void unbind() = 0;
		/**
		 *\~english
		 *\brief		Resizing function
		 *\param[in]	p_size	The buffer dimensions
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de redimensionnement
		 *\param[in]	p_size	Les dimensions du tampon
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool resize( castor::Size const & p_size ) = 0;
		/**
		 *\~english
		 *\brief		Retrieves the buffer pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels du tampon
		 *\return		Le format des pixels
		 */
		inline castor::PixelFormat getPixelFormat()const
		{
			return m_pixelFormat;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer dimensions
		 *\return		The dimensions
		 *\~french
		 *\brief		Récupère les dimensions du tampon
		 *\return		Les dimensions
		 */
		inline castor::Size const & getDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur du tampon
		 *\return		La largeur
		 */
		inline uint32_t getWidth()const
		{
			return m_size.getWidth();
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur du tampon
		 *\return		La hauteur
		 */
		inline uint32_t getHeight()const
		{
			return m_size.getHeight();
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer PixelComponents
		 *\return		The PixelComponents
		 *\~french
		 *\brief		Récupère la composante du tampon
		 *\return		La composante
		 */
		inline BufferComponent getComponent()const
		{
			return m_eComponent;
		}
		/**
		 *\~english
		 *\brief		Defines the buffer samples count
		 *\param[in]	p_iSamples	The samples count
		 *\~french
		 *\brief		Définit le nombre de samples du tampon
		 *\param[in]	p_iSamples	Le nombre de samples
		 */
		inline void setSamplesCount( int p_iSamples )
		{
			m_samplesCount = p_iSamples;
		}
		/**
		 *\~english
		 *\brief		Retrieves the buffer samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples du tampon
		 *\return		Le nombre de samples
		 */
		inline int getSamplesCount()
		{
			return m_samplesCount;
		}

	protected:
		//!\~english The buffer dimensions	\~french Les dimensions du buffer
		castor::Size m_size;

	private:
		//!\~english The PixelComponents to which this buffer is bound	\~french La composante à laquelle ce tampon est associé
		BufferComponent m_eComponent;
		//!\~english The buffer pixel format	\~french Le format des pixels du tampon
		castor::PixelFormat m_pixelFormat;
		//!\~english The samples count, if multisampling is enabled	\~french Le nombre d'échantillons, en cas de multisampling
		int m_samplesCount;
	};
}

#endif
