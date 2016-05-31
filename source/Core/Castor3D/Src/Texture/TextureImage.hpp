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
#ifndef ___C3D_TEXTURE_IMAGE_H___
#define ___C3D_TEXTURE_IMAGE_H___

#include "TextureStorage.hpp"

#include <PixelBufferBase.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\date		19/05/2016
	\version	0.9.0
	\~english
	\brief		Texture image source.
	\~french
	\brief		Classe de source d'image de texture.
	*/
	class TextureSource
	{
	public:
		/**
		 *\~english
		 *\return		The image dimensions.
		 *\~french
		 *\return		Les dimensions de l'image.
		 */
		inline Castor::Size GetDimensions()const
		{
			return m_buffer->dimensions();
		}
		/**
		 *\~english
		 *\return		The pixel format.
		 *\~french
		 *\return		Le format des pixels.
		 */
		inline Castor::ePIXEL_FORMAT GetPixelFormat()const
		{
			return m_buffer->format();
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		inline Castor::PxBufferBaseSPtr GetBuffer()const
		{
			return m_buffer;
		}
		/**
		 *\~english
		 *\brief		Sets the texture buffer.
		 *\param[in]	p_buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon de la texture.
		 */
		inline void SetBuffer( Castor::PxBufferBaseSPtr p_buffer )
		{
			m_buffer = p_buffer;
		}
		/**
		 *\~english
		 *\return		The texture depth, 1 if not 3D.
		 *\~french
		 *\return		La profondeur de la texture, 1 si pas 3D.
		 */
		C3D_API virtual uint32_t GetDepth()const = 0;

		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		C3D_API virtual bool IsStatic()const = 0;
		/**
		 *\~english
		 *\brief		Resizes the source.
		 *\param[in]	p_size	The new size.
		 *\param[in]	p_depth	The new depth.
		 *\return		\p true if the source has been resized.
		 *\~french
		 *\brief		Redimensionne la source.
		 *\param[in]	p_size	La nouvelle taille.
		 *\param[in]	p_depth	La nouvelle profondeur.
		 *\return		\p true si la source a été redimensionnée.
		 */
		C3D_API virtual bool Resize( Castor::Size const & p_size, uint32_t p_depth ) = 0;

		/**
		 *\~english
		 *\return		The source as string.
		 *\~french
		 *\return		La source en chaîne de caractères.
		 */
		C3D_API virtual Castor::String ToString()const = 0;

	protected:
		/**
		 *\~english
		 *\brief			Readjusts dimensions if the selected rendering API doesn't support NPOT textures.
		 *\param[in,out]	p_size	The size.
		 *\param[in,out]	p_depth	The depth.
		 *\return			\p true if the dimensions have changed.
		 *\~french
		 *\brief			Réajuste les dimensions données si l'API de rendu sélectionnée ne supporte pas les textures NPOT.
		 *\param[in,out]	p_size	La taille.
		 *\param[in,out]	p_depth	La profondeur.
		 *\return			\p true si les dimensions ont changé.
		 */
		bool DoAdjustDimensions( Castor::Size & p_size, uint32_t & p_depth );

	protected:
		//!\~english	The texture buffer.
		//!\~french		Le tampon de la texture.
		Castor::PxBufferBaseSPtr m_buffer;
	};
	/*!
	\author		Sylvain DOREMUS
	\date		19/05/2016
	\version	0.9.0
	\~english
	\brief		Texture image.
	\remarks	Holds the GPU texture storage.
	\~french
	\brief		Classe d'image de texture.
	\remarks	Contient le stockage de la texture au niveau GPU.
	*/
	class TextureImage
		: public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		24/05/2016
		\~english
		\brief		TextureImage loader
		\~french
		\brief		Loader de TextureImage
		*/
		class TextWriter
			: public Castor::TextWriter< TextureImage >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a TextureImage into a text file
			 *\param[in]	p_file	The file
			 *\param[in]	p_obj	The TextureImage
			 *\~french
			 *\brief		Ecrit une TextureImage dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_obj	La TextureImage
			 */
			C3D_API bool operator()( TextureImage const & p_obj, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\param[in]	p_index		The image index in its layout.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_engine	Le moteur.
		 *\param[in]	p_index		L'index de l'image dans son layout.
		 */
		C3D_API TextureImage( Engine & p_engine, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Defines the texture buffer from an image file.
		 *\param[in]	p_folder	The folder containing the image.
		 *\param[in]	p_relative	The image file path, relative to p_folder.
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	p_folder	Le dossier contenant l'image.
		 *\param[in]	p_relative	Le chemin d'accès à l'image, relatif à p_folder.
		 */
		C3D_API void SetSource( Castor::Path const & p_folder, Castor::Path const & p_relative );
		/**
		 *\~english
		 *\brief		Defines the texture buffer.
		 *\param[in]	p_buffer	The buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon.
		 */
		C3D_API void SetSource( Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Defines the texture buffer and its dimensions, for a 3D texture or a texture array.
		 *\param[in]	p_dimensions	The texture dimensions.
		 *\param[in]	p_buffer		The buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture, ainsi que ses dimensions, dans le cas de texture 3D ou tableau de textures.
		 *\param[in]	p_dimensions	Les dimensions de la texture.
		 *\param[in]	p_buffer		Le tampon.
		 */
		C3D_API void SetSource( Castor::Point3ui const & p_dimensions, Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Defines the texture buffer.
		 *\param[in]	p_size		The buffer dimensions.
		 *\param[in]	p_format	The buffer format.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_size		La taille du tampon.
		 *\param[in]	p_format	Le format du tampon.
		 */
		C3D_API void SetSource( Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Defines the 3D texture buffer.
		 *\param[in]	p_size		The buffer dimensions.
		 *\param[in]	p_format	The buffer format.
		 *\~french
		 *\brief		Définit le tampon de la texture 3D.
		 *\param[in]	p_size		La taille du tampon.
		 *\param[in]	p_format	Le format du tampon.
		 */
		C3D_API void SetSource( Castor::Point3ui const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\~english
		 *\brief		Resizes the texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API void Resize( Castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Resizes the 3D texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture 3D
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API void Resize( Castor::Point3ui const & p_size );
		/**
		 *\~english
		 *\brief		Activation function, to tell the GPU it is active.
		 *\param[in]	p_index	The texture index.
		 *\return		\p true if successful.
		 *\~french
		 *\brief		Fonction d'activation, pour dire au GPU qu'il est activé.
		 *\param[in]	p_index	L'index de texture.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Bind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Deactivation function, to tell the GPU it is inactive
		 *\param[in]	p_index	The texture index
		 *\~french
		 *\brief		Fonction de désactivation, pour dire au GPU qu'il est désactivé
		 *\param[in]	p_index	L'index de texture
		 */
		C3D_API void Unbind( uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Initialises the GPU storage.
		 *\param[in]	p_type		The texture type.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of eACCESS_TYPE).
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le stockage GPU.
		 *\param[in]	p_type		Le type de texture.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool Initialise( TextureType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess );
		/**
		 *\~english
		 *\brief		Cleanup function
		 *\~french
		 *\brief		Fonction de nettoyage
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Locks image buffer from GPU, allowing modifications into it.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_lock	Defines the lock mode (r, w, rw), combination of eACCESS_TYPE.
		 *\return		The image buffer.
		 *\~french
		 *\brief		Locke le buffer de l'image à partir du GPU, permettant des modification dessus.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_lock	Définit le mode de lock (lecture, écriture, les 2), combinaison de eACCESS_TYPE.
		 *\return		Le buffer de l'image.
		 */
		C3D_API uint8_t * Lock( uint32_t p_lock );
		/**
		 *\~english
		 *\brief		Unlocks image buffer from GPU.
		 *\remarks		The parent texture must be bound.
		 *\param[in]	p_modified	Tells if the buffer has been modified, so modifications are uploaded to GPU.
		 *\~french
		 *\brief		Délocke le buffer de l'image à partir du GPU.
		 *\remarks		La texture parente doit être activée.
		 *\param[in]	p_modified	Dit si le buffer a été modifié, afin que les modifications soient mises sur le GPU.
		 */
		C3D_API void Unlock( bool p_modified );
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		inline uint32_t GetWidth()const
		{
			return m_source->GetDimensions().width();
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture image height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur de l'image de la texture
		 *\return		La hauteur
		 */
		inline uint32_t GetHeight()const
		{
			return m_source->GetDimensions().height() / GetDepth();
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
			return Castor::Size{ GetWidth(), GetHeight() };
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
			return m_source->GetPixelFormat();
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
			return m_source->GetDepth();
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
			return m_source->GetBuffer();
		}
		/**
		 *\~english
		 *\brief		Sets the texture buffer.
		 *\param[in]	p_buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon de la texture.
		 */
		inline void SetBuffer( Castor::PxBufferBaseSPtr p_buffer )
		{
			m_source->SetBuffer( p_buffer );
		}
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		inline bool IsStaticSource()const
		{
			return m_source->IsStatic();
		}
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		inline Castor::String ToString()const
		{
			return m_source->ToString();
		}
		/**
		 *\~english
		 *\return		The image index in its layout.
		 *\~french
		 *\return		L'index de l'image dans son layout.
		 */
		inline uint32_t GetIndex()const
		{
			return m_index;
		}

	private:
		/**
		 *\~english
		 *\brief		Reinitialises the storage.
		 *\remarks		If the storage was not created, does nothing.
		 *\~french
		 *\brief		Réinitialise le stockage.
		 *\remarks		Ne fait rien si le stockage n'avait pas été créé.
		 */
		bool DoResetStorage();
		/**
		 *\~english
		 *\brief		Initialises the GPU storage.
		 *\param[in]	p_type		The storage type.
		 *\param[in]	p_cpuAccess	The required CPU access (combination of eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	The required GPU access (combination of eACCESS_TYPE).
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise le stockage GPU.
		 *\param[in]	p_type		Le type de stockage.
		 *\param[in]	p_cpuAccess	Les accès requis pour le CPU (combinaison de eACCESS_TYPE).
		 *\param[in]	p_gpuAccess	Les accès requis pour le GPU (combinaison de eACCESS_TYPE).
		 *\return		\p true si tout s'est bien passé.
		 */
		bool DoCreateStorage( TextureStorageType p_type, uint8_t p_cpuAccess, uint8_t p_gpuAccess );

	protected:
		//!\~english	The texture source.
		//!\~french		La source de la texture.
		std::unique_ptr< TextureSource > m_source;
		//!\~english	The texture GPU storage.
		//!\~french		Le stockage GPU de la texture.
		TextureStorageUPtr m_storage;
		//!\~english	The image index in its layout.
		//!\~french		L'index de l'image dans son layout.
		uint32_t m_index;
	};
}

#endif
