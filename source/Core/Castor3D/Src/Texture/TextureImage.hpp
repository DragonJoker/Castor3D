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
#ifndef ___C3D_TEXTURE_IMAGE_H___
#define ___C3D_TEXTURE_IMAGE_H___

#include "TextureStorage.hpp"

#include <Graphics/PixelBufferBase.hpp>

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
		: public Castor::OwnedBy< TextureLayout >
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
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
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
		 *\param[in]	p_layout	The parent layout.
		 *\param[in]	p_index		The image index in its layout.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	p_layout	Le layout parent.
		 *\param[in]	p_index		L'index de l'image dans son layout.
		 */
		C3D_API TextureImage( TextureLayout & p_layout, uint32_t p_index );
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
		C3D_API void InitialiseSource( Castor::Path const & p_folder, Castor::Path const & p_relative );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\param[in]	p_buffer	The buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon.
		 */
		C3D_API void InitialiseSource( Castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 */
		C3D_API void InitialiseSource();
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
		 *\return		\p true if the texture's source has already been defined.
		 *\~french
		 *\return		\p true si la source de la texture a déjà été définie.
		 */
		inline bool HasSource()const
		{
			return m_source != nullptr;
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
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

	protected:
		//!\~english	The texture source.
		//!\~french		La source de la texture.
		std::unique_ptr< TextureSource > m_source;
		//!\~english	The image index in its layout.
		//!\~french		L'index de l'image dans son layout.
		uint32_t m_index;
	};
}

#endif
