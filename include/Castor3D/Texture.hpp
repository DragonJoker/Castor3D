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
#ifndef ___C3D_Texture___
#define ___C3D_Texture___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

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
	class C3D_API TextureBase
	{
	protected:
		DECLARE_VECTOR( Castor::PxBufferBaseSPtr, PxBuffer );
		//!\~english	Initialisation status	\~french	Statut d'initialisation
		bool m_bInitialised;
		//!\~english	Texture type	\~french	Type de texture
		eTEXTURE_TYPE m_eType;
		//!\~english	Texture dimension count	\~french	Compte des dimensions de la texture
		eTEXTURE_DIMENSION m_eDimension;
		//!\~english	Texture mapping modes	\~french	Modes de mappage de la texture
		eTEXTURE_MAP_MODE m_eMapMode;
		//!\~english	Texture index	\~french	Index de la texture
		uint32_t m_uiIndex;
		//!\~english	3D Texture depth	\~french	Profondeur de la texture 3D
		uint32_t m_uiDepth;
		//!\~english	Texture pixels, at least at initialisation	\~french	Pixels de la texture, au moins au moment de l'initialisation
		Castor::PxBufferBaseSPtr m_pPixelBuffer;
		//!\~english	Texture sampler	\~french	Sampler de la texture
		SamplerWPtr m_pSampler;
		//!\~english	The pixel buffer format	\~french	Le format du buffer de pixels
		Castor::ePIXEL_FORMAT m_ePixelFormat;
		//!\~english	The pixel buffer dimensions	\~french	Les dimensions du buffer de pixels
		Castor::Size m_size;
		//!\~english	The render system	\~french	Le render system
		RenderSystem * m_pRenderSystem;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_eType			The texture type
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_eType			Le type de texture
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		TextureBase( eTEXTURE_TYPE p_eType, RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextureBase();
		/**
		 *\~english
		 *\brief		Creation function
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction de création
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Create() = 0;
		/**
		 *\~english
		 *\brief		Destruction function
		 *\~french
		 *\brief		Fonction de destruction
		 */
		virtual void Destroy() = 0;
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_uiIndex		The texture index
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_uiIndex		L'index de la texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise( uint32_t p_uiIndex )=0;
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup() = 0;
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
		virtual uint8_t * Lock( uint32_t p_uiLock ) = 0;
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU
		 *\param[in]	p_bModified	Tells if the buffer has been modified, so modifications are uploaded to GPU
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU
		 *\param[in]	p_bModified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU
		 */
		virtual void Unlock( bool p_bModified ) = 0;
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_uiIndex	The texture index
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_uiIndex	L'index de texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind() = 0;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_uiIndex	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_uiIndex	L'index de texture
		 */
		virtual void Unbind() = 0;
		/**
		 *\~english
		 *\brief		Generate texture mipmaps
		 *\~french
		 *\brief		Génère les mipmaps de la texture
		 */
		virtual void GenerateMipmaps() = 0;
		/**
		 *\~english
		 *\brief		Retrieves the initialisation status
		 *\return		The initialisation status
		 *\~french
		 *\brief		Récupère le statut d'initialisation
		 *\return		Le statut d'initialisation
		 */
		 inline bool IsInitialised()const { return m_bInitialised; }
		/**
		 *\~english
		 *\brief		Retrieves the texture type
		 *\return		The texture type
		 *\~french
		 *\brief		Récupère le type de texture
		 *\return		Le type de texture
		 */
		 inline eTEXTURE_TYPE GetType()const { return m_eType; }
		/**
		 *\~english
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		 inline eTEXTURE_DIMENSION GetDimension()const { return m_eDimension; }
		/**
		 *\~english
		 *\brief		Defines the texture dimension
		 *\param[in]	p_eDimension	The texture dimension
		 *\~french
		 *\brief		Définit la dimension de la texture
		 *\param[in]	p_eDimension	La dimension de la texture
		 */
		 inline void SetDimension( eTEXTURE_DIMENSION p_eDimension ) { m_eDimension = p_eDimension; }
		/**
		 *\~english
		 *\brief		Retrieves the texture mapping mode
		 *\return		The texture mapping mode
		 *\~french
		 *\brief		Récupère le mode mappage de la texture
		 *\return		Le mode mappage de la texture
		 */
		 inline eTEXTURE_MAP_MODE GetMappingMode()const { return m_eMapMode; }
		/**
		 *\~english
		 *\brief		Defines the texture mapping mode
		 *\param[in]	p_eMapMode	The texture mapping mode
		 *\~french
		 *\brief		Définit le mode mappage de la texture
		 *\param[in]	p_eMapMode	Le mode mappage de la texture
		 */
		 inline void SetMappingMode( eTEXTURE_MAP_MODE p_eMapMode ) { m_eMapMode = p_eMapMode; }
		/**
		 *\~english
		 *\brief		Retrieves the texture image dimensions
		 *\return		The image dimensions
		 *\~french
		 *\brief		Récupère les dimensions de l'image de la texture
		 *\return		Les dimensions de l'image
		 */
		inline Castor::Size GetDimensions()const { return Castor::Size( GetWidth(), GetHeight() ); }
		/**
		 *\~english
		 *\brief		Retrieves the texture depth
		 *\return		The depth, 1 if not 3D
		 *\~french
		 *\brief		Récupère la profondeur de la texture
		 *\return		La profondeur, 1 si pas 3D
		 */
		inline uint32_t GetDepth()const { return m_uiDepth; }
		/**
		 *\~english
		 *\brief		Retrieves the texture pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de la texture
		 *\return		Le format des pixels
		 */
		 inline Castor::ePIXEL_FORMAT GetPixelFormat()const { return (m_pPixelBuffer ? m_pPixelBuffer->format() : m_ePixelFormat ); }
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		uint32_t GetWidth()const { return (m_pPixelBuffer ? m_pPixelBuffer->dimensions().width() : m_size.width() ); }
		/**
		 *\~english
		 *\brief		Retrieves the texture image height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur de l'image de la texture
		 *\return		La hauteur
		 */
		uint32_t GetHeight()const { return (m_pPixelBuffer ? m_pPixelBuffer->dimensions().height() : m_size.height() ) / m_uiDepth; }
		/**
		 *\~english
		 *\brief		Defines the texture buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture
		 *\param[in]	p_pBuffer	Le buffer
		 */
		void SetImage( Castor::PxBufferBaseSPtr p_pBuffer );
		/**
		 *\~english
		 *\brief		Retrieves the texture buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le buffer de la texture
		 *\return		Le buffer
		 */
		inline Castor::PxBufferBaseSPtr GetBuffer() { return m_pPixelBuffer; }
		/**
		 *\~english
		 *\brief		Retrieves the texture index
		 *\return		The index
		 *\~french
		 *\brief		Récupère l'index de la texture
		 *\return		L'index
		 */
		inline uint32_t const & GetIndex()const { return m_uiIndex; }
		/**
		 *\~english
		 *\brief		Defines the texture sampler
		 *\param[in]	p_pSampler	The sampler
		 *\~french
		 *\brief		Définit le sampler de la texture
		 *\param[in]	p_pSampler	Le sampler
		 */
		inline void SetSampler( SamplerSPtr p_pSampler ) { m_pSampler = p_pSampler; }
		/**
		 *\~english
		 *\brief		Retrieves the texture sampler
		 *\return		The sampler
		 *\~french
		 *\brief		Récupère le sampler de la texture
		 *\return		Le buffer
		 */
		inline SamplerSPtr GetSampler()const { return m_pSampler.lock(); }

	protected:
		/**
		 *\~english
		 *\brief		API specific initialisation function
		 *\return		\p if OK
		 *\~french
		 *\brief		Initialisation spécifique selon l'API
		 *\return		\p si tout s'est bien passé
		 */
		virtual bool DoInitialise()=0;
		/**
		 *\~english
		 *\brief		API specific binding function
		 *\return		\p if OK
		 *\~french
		 *\brief		Activation spécifique selon l'API
		 *\return		\p si tout s'est bien passé
		 */
		virtual bool DoBind()=0;
		/**
		 *\~english
		 *\brief		API specific unbinding function
		 *\~french
		 *\brief		Désactivation spécifique selon l'API
		 */
		virtual void DoUnbind()=0;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		14/02/2010
	\~english
	\brief		Static texture class
	\remark		A static texture deletes it's buffer from ram when initialised
	\~french
	\brief		Class de texture statique
	\remark		Une texture statique supprime son buffer de la ram quand elle est initialisée
	*/
	class C3D_API StaticTexture : public TextureBase
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		StaticTexture( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~StaticTexture();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_uiIndex		The texture index
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_uiIndex		L'index de la texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_uiIndex	The texture index
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_uiIndex	L'index de texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind();
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_uiIndex	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_uiIndex	L'index de texture
		 */
		virtual void Unbind();
	};
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
	class C3D_API DynamicTexture : public TextureBase
	{
	private:
		//!\~english	Tells that the texture needs to be initialised as a render target	\~french	Dit qu e la texture doit être initialiséée en tant que render target
		bool m_bRenderTarget;
		//!\~english	The samples count, if it is a multisample texture					\~french	Le nombre de samples dans le cas où c'est une texture multisample
		int m_iSamplesCount;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_pRenderSystem	Le render system
		 */
		DynamicTexture( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~DynamicTexture();
		/**
		 *\~english
		 *\brief		Initialisation function
		 *\param[in]	p_uiIndex		The texture index
		 *\return		\p true if OK
		 *\~french
		 *\brief		Fonction d'initialisation
		 *\param[in]	p_uiIndex		L'index de la texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Initialise( uint32_t p_uiIndex );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active
		 *\param[in]	p_uiIndex	The texture index
		 *\return		\p true if successful
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé
		 *\param[in]	p_uiIndex	L'index de texture
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Bind();
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_uiIndex	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_uiIndex	L'index de texture
		 */
		virtual void Unbind();
		/**
		 *\~english
		 *\brief		Defines the texture buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture
		 *\param[in]	p_pBuffer	The buffer
		 */
		void SetImage( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\~english
		 *\brief		Defines the 3D texture buffer
		 *\param[in]	p_pBuffer	The buffer
		 *\~french
		 *\brief		Définit le buffer de la texture 3D
		 *\param[in]	p_pBuffer	The buffer
		 */
		void SetImage( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_ePixelFormat );
		/**
		 *\~english
		 *\brief		Resizes the texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture
		 *\param[in]	p_size	La nouvelle taille
		 */
		virtual void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Resizes the 3D texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture 3D
		 *\param[in]	p_size	La nouvelle taille
		 */
		virtual void Resize( Castor::Point3ui const & p_size );
		/**
		 *\~english
		 *\brief		Retrieves the render target status
		 *\return		The status
		 *\~french
		 *\brief		Récupère le statut de cible de rendu
		 *\return		Le statut
		 */
		inline bool	IsRenderTarget()const { return m_bRenderTarget; }
		/**
		 *\~english
		 *\brief		Defines the render target status
		 *\param[in]	val	The status
		 *\~french
		 *\brief		Définit le statut de cible de rendu
		 *\param[in]	val	Le statut
		 */
		inline void SetRenderTarget( bool val ) { m_bRenderTarget = val; }
		/**
		 *\~english
		 *\brief		Retrieves the samples count
		 *\return		The samples count
		 *\~french
		 *\brief		Récupère le nombre de samples
		 *\return		Le nombre de samples
		 */
		inline int	GetSamplesCount()const { return m_iSamplesCount; }
		/**
		 *\~english
		 *\brief		Defines the samples count
		 *\param[in]	val	The samples count
		 *\~french
		 *\brief		Définit le nombre de samples
		 *\param[in]	val	Le nombre de samples
		 */
		inline void SetSamplesCount( int p_iCount ) { m_iSamplesCount = p_iCount; }
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
		virtual void Fill( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat ) = 0;

		using TextureBase::SetImage;
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
	template< typename CharType > std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_streamOut, TextureBaseSPtr const & p_texture )
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
	template< typename CharType > std::basic_istream< CharType > & operator >>( std::basic_istream< CharType > & p_streamIn, TextureBaseSPtr & p_texture )
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
	template< typename CharType > std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_streamOut, TextureBaseRPtr const & p_texture )
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
	template< typename CharType > std::basic_istream< CharType > & operator >>( std::basic_istream< CharType > & p_streamIn, TextureBaseRPtr & p_texture )
	{
		uint32_t l_uiIndex = 0;
		p_streamIn >> l_uiIndex;
		p_texture->Initialise( l_uiIndex );
		return p_streamIn;
	}
}

#pragma warning( pop )

#endif
