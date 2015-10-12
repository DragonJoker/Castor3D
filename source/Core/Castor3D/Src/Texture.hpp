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
#ifndef ___C3D_TEXTURE_H___
#define ___C3D_TEXTURE_H___

#include "Castor3DPrerequisites.hpp"

#include <PixelBufferBase.hpp>
#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Texture base class
	\~french
	\brief		Class de base d'une texture
	*/
	class TextureBase
		: public Castor::OwnedBy< RenderSystem >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_type			The texture type
		 *\param[in]	p_renderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_type			Le type de texture
		 *\param[in]	p_renderSystem	Le render system
		 */
		C3D_API TextureBase( eTEXTURE_BASE_TYPE p_type, RenderSystem & p_renderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~TextureBase();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API bool Bind();
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 */
		C3D_API void Unbind();
		/**
		*\~english
		*\brief		Defines the texture buffer
		*\param[in]	p_pBuffer	The buffer
		*\~french
		*\brief		Définit le buffer de la texture
		*\param[in]	p_pBuffer	Le buffer
		*/
		C3D_API void SetImage( Castor::PxBufferBaseSPtr p_pBuffer );
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		C3D_API virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_index		The texture index
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_index		L'index de la texture
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool Initialise( uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API virtual void Cleanup() = 0;
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it
		 *\param[in]	p_eLock	Defines the lock mode (r, w, rw), combination of eLOCK_FLAG
		 *\return		The image buffer
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus
		 *\param[in]	p_eLock	Définit le mode de lock (lecture, écriture, les 2), combinaison de eLOCK_FLAG
		 *\return		Le buffer de l'image
		 */
		C3D_API virtual uint8_t * Lock( uint32_t p_uiLock ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU
		 *\param[in]	p_bModified	Tells if the buffer has been modified, so modifications are uploaded to GPU
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU
		 *\param[in]	p_bModified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU
		 */
		C3D_API virtual void Unlock( bool p_bModified ) = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_index	The texture index
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_index	L'index de texture
		 *\return		\p true si tout s'est bien passé
		 */
		C3D_API virtual bool BindAt( uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API virtual void UnbindFrom( uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 */
		C3D_API virtual void GenerateMipmaps() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the initialisation status
		 *\return		The initialisation status
		 *\~french
		 *\brief		Récupère le statut d'initialisation
		 *\return		Le statut d'initialisation
		 */
		inline bool IsInitialised()const
		{
			return m_bInitialised;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture type
		 *\return		The texture type
		 *\~french
		 *\brief		Récupère le type de texture
		 *\return		Le type de texture
		 */
		inline eTEXTURE_BASE_TYPE GetBaseType()const
		{
			return m_baseType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		inline eTEXTURE_TYPE GetType()const
		{
			return m_type;
		}
		/**
		 *\~english
		 *\brief		Defines the texture dimension
		 *\param[in]	p_eDimension	The texture dimension
		 *\~french
		 *\brief		Définit la dimension de la texture
		 *\param[in]	p_eDimension	La dimension de la texture
		 */
		inline void SetType( eTEXTURE_TYPE p_eDimension )
		{
			m_type = p_eDimension;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture mapping mode
		 *\return		The texture mapping mode
		 *\~french
		 *\brief		Récupère le mode mappage de la texture
		 *\return		Le mode mappage de la texture
		 */
		inline eTEXTURE_MAP_MODE GetMappingMode()const
		{
			return m_eMapMode;
		}
		/**
		 *\~english
		 *\brief		Defines the texture mapping mode
		 *\param[in]	p_eMapMode	The texture mapping mode
		 *\~french
		 *\brief		Définit le mode mappage de la texture
		 *\param[in]	p_eMapMode	Le mode mappage de la texture
		 */
		inline void SetMappingMode( eTEXTURE_MAP_MODE p_eMapMode )
		{
			m_eMapMode = p_eMapMode;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image dimensions
		 *\return		The image dimensions
		 *\~french
		 *\brief		Récupère les dimensions de l'image de la texture
		 *\return		Les dimensions de l'image
		 */
		inline Castor::Size GetDimensions()const
		{
			return Castor::Size( GetWidth(), GetHeight() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture depth
		 *\return		The depth, 1 if not 3D
		 *\~french
		 *\brief		Récupère la profondeur de la texture
		 *\return		La profondeur, 1 si pas 3D
		 */
		inline uint32_t GetDepth()const
		{
			return m_uiDepth;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la texture
		 *\return		Le format des pixels
		 */
		inline Castor::ePIXEL_FORMAT GetPixelFormat()const
		{
			return ( m_pPixelBuffer ? m_pPixelBuffer->format() : m_ePixelFormat );
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		uint32_t GetWidth()const
		{
			return ( m_pPixelBuffer ? m_pPixelBuffer->dimensions().width() : m_size.width() );
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur de l'image de la texture
		 *\return		La hauteur
		 */
		uint32_t GetHeight()const
		{
			return ( m_pPixelBuffer ? m_pPixelBuffer->dimensions().height() : m_size.height() ) / m_uiDepth;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le buffer de la texture
		 *\return		Le buffer
		 */
		inline Castor::PxBufferBaseSPtr GetBuffer()const
		{
			return m_pPixelBuffer;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture index
		 *\return		The index
		 *\~french
		 *\brief		Récupère l'index de la texture
		 *\return		L'index
		 */
		inline uint32_t const & GetIndex()const
		{
			return m_uiIndex;
		}
		/**
		 *\~english
		 *\brief		Sets the texture index
		 *\param[in]	p_index	The index
		 *\~french
		 *\brief		Définit l'index de la texture
		 *\param[in]	p_index	L'index
		 */
		inline void SetIndex( uint32_t p_index )
		{
			m_uiIndex = p_index;
		}
		/**
		 *\~english
		 *\brief		Defines the texture sampler
		 *\param[in]	p_pSampler	The sampler
		 *\~french
		 *\brief		Définit le sampler de la texture
		 *\param[in]	p_pSampler	Le sampler
		 */
		inline void SetSampler( SamplerSPtr p_pSampler )
		{
			m_pSampler = p_pSampler;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture sampler
		 *\return		The sampler
		 *\~french
		 *\brief		Récupère le sampler de la texture
		 *\return		Le buffer
		 */
		inline SamplerSPtr GetSampler()const
		{
			return m_pSampler.lock();
		}

	protected:
		/**
		 *\~english
		 *\brief		API specific initialisation function
		 *\return		\p if OK
		 *\~french
		 *\brief		Initialisation spécifique selon l'API
		 *\return		\p si tout s'est bien passé
		 */
		C3D_API virtual bool DoInitialise() = 0;
		/**
		 *\~english
		 *\brief		API specific binding function
		 *\return		\p if OK
		 *\~french
		 *\brief		Activation spécifique selon l'API
		 *\return		\p si tout s'est bien passé
		 */
		C3D_API virtual bool DoBind( uint32_t p_index ) = 0;
		/**
		 *\~english
		 *\brief		API specific unbinding function
		 *\~french
		 *\brief		Désactivation spécifique selon l'API
		 */
		C3D_API virtual void DoUnbind( uint32_t p_index ) = 0;

	protected:
		DECLARE_VECTOR( Castor::PxBufferBaseSPtr, PxBuffer );
		//!\~english Initialisation status	\~french Statut d'initialisation
		bool m_bInitialised;
		//!\~english Texture base type.	\~french La type de base de la texture.
		eTEXTURE_BASE_TYPE m_baseType;
		//!\~english Texture type.	\~french Type de texture.
		eTEXTURE_TYPE m_type;
		//!\~english Texture mapping modes	\~french Modes de mappage de la texture
		eTEXTURE_MAP_MODE m_eMapMode;
		//!\~english Texture index	\~french Index de la texture
		uint32_t m_uiIndex;
		//!\~english 3D Texture depth	\~french Profondeur de la texture 3D
		uint32_t m_uiDepth;
		//!\~english Texture pixels, at least at initialisation	\~french Pixels de la texture, au moins au moment de l'initialisation
		Castor::PxBufferBaseSPtr m_pPixelBuffer;
		//!\~english Texture sampler	\~french Sampler de la texture
		SamplerWPtr m_pSampler;
		//!\~english The pixel buffer format	\~french Le format du buffer de pixels
		Castor::ePIXEL_FORMAT m_ePixelFormat;
		//!\~english The pixel buffer dimensions	\~french Les dimensions du buffer de pixels
		Castor::Size m_size;
	};
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamOut	The stream receiving texture's data
	 *\param[in]		p_texture	The input texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la texture
	 *\param[in]		p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::ostream & operator <<( std::ostream & p_streamOut, TextureBaseSPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamIn	The stream holding texture's data
	 *\param[in,out]	p_texture	The output texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamIn	Le flux qui contient les données de la texture
	 *\param[in,out]	p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::istream & operator >>( std::istream & p_streamIn, TextureBaseSPtr & p_texture )
	{
		uint32_t l_uiIndex = 0;
		p_streamIn >> l_uiIndex;
		p_texture->Initialise( l_uiIndex );
		return p_streamIn;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamOut	The stream receiving texture's data
	 *\param[in]		p_texture	The input texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la texture
	 *\param[in]		p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::ostream & operator <<( std::ostream & p_streamOut, TextureBaseRPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamIn	The stream holding texture's data
	 *\param[in,out]	p_texture	The output texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamIn	Le flux qui contient les données de la texture
	 *\param[in,out]	p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::istream & operator >>( std::istream & p_streamIn, TextureBaseRPtr & p_texture )
	{
		uint32_t l_uiIndex = 0;
		p_streamIn >> l_uiIndex;
		p_texture->Initialise( l_uiIndex );
		return p_streamIn;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamOut	The stream receiving texture's data
	 *\param[in]		p_texture	The input texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la texture
	 *\param[in]		p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::wostream & operator <<( std::wostream & p_streamOut, TextureBaseSPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamIn	The stream holding texture's data
	 *\param[in,out]	p_texture	The output texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamIn	Le flux qui contient les données de la texture
	 *\param[in,out]	p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::wistream & operator >>( std::wistream & p_streamIn, TextureBaseSPtr & p_texture )
	{
		uint32_t l_uiIndex = 0;
		p_streamIn >> l_uiIndex;
		p_texture->Initialise( l_uiIndex );
		return p_streamIn;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamOut	The stream receiving texture's data
	 *\param[in]		p_texture	The input texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la texture
	 *\param[in]		p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::wostream & operator <<( std::wostream & p_streamOut, TextureBaseRPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamIn	The stream holding texture's data
	 *\param[in,out]	p_texture	The output texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamIn	Le flux qui contient les données de la texture
	 *\param[in,out]	p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	inline std::wistream & operator >>( std::wistream & p_streamIn, TextureBaseRPtr & p_texture )
	{
		uint32_t l_uiIndex = 0;
		p_streamIn >> l_uiIndex;
		p_texture->Initialise( l_uiIndex );
		return p_streamIn;
	}
}

#endif
