/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureView_H___
#define ___C3D_TextureView_H___

#include "Castor3DPrerequisites.hpp"

#include <Graphics/PixelBufferBase.hpp>

namespace castor3d
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
		C3D_API explicit TextureSource( Engine & engine )
			: m_engine{ engine }
		{
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		C3D_API virtual ~TextureSource()
		{
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer()const = 0;
		/**
		 *\~english
		 *\brief		sets the texture buffer.
		 *\param[in]	p_buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon de la texture.
		 */
		C3D_API virtual void setBuffer( castor::PxBufferBaseSPtr p_buffer ) = 0;
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		C3D_API virtual bool isStatic()const = 0;
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
		C3D_API virtual bool resize( castor::Size const & p_size, uint32_t p_depth ) = 0;

		/**
		 *\~english
		 *\return		The source as string.
		 *\~french
		 *\return		La source en chaîne de caractères.
		 */
		C3D_API virtual castor::String toString()const = 0;
		/**
		 *\~english
		 *\return		The source's dimensions.
		 *\~french
		 *\return		Les dimensions de la source.
		 */
		inline castor::Size getDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The source's pixel format.
		 *\~french
		 *\return		Le format des pixels de la source.
		 */
		inline castor::PixelFormat getPixelFormat()const
		{
			return m_format;
		}

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
		bool doAdjustDimensions( castor::Size & p_size, uint32_t & p_depth );

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The source's pixel format.
		//!\~french		Le format des pixels de la source.
		castor::PixelFormat m_format;
		//!\~english	The source's dimensions.
		//!\~french		Les dimensions de la source.
		castor::Size m_size;
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
	class TextureView
		: public castor::OwnedBy< TextureLayout >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		24/05/2016
		\~english
		\brief		TextureView loader
		\~french
		\brief		Loader de TextureView
		*/
		class TextWriter
			: public castor::TextWriter< TextureView >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a TextureView into a text file
			 *\param[in]	p_file	The file
			 *\param[in]	p_obj	The TextureView
			 *\~french
			 *\brief		Ecrit une TextureView dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_obj	La TextureView
			 */
			C3D_API bool operator()( TextureView const & p_obj, castor::TextFile & p_file )override;
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
		C3D_API TextureView( TextureLayout & p_layout, uint32_t p_index );
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
		C3D_API void initialiseSource( castor::Path const & p_folder, castor::Path const & p_relative );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\param[in]	p_buffer	The buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon.
		 */
		C3D_API void initialiseSource( castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 */
		C3D_API void initialiseSource();
		/**
		 *\~english
		 *\brief		Resizes the texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API void resize( castor::Size const & p_size );
		/**
		 *\~english
		 *\brief		Resizes the 3D texture buffer
		 *\param[in]	p_size	The new size
		 *\~french
		 *\brief		Redimensionne le buffer de la texture 3D
		 *\param[in]	p_size	La nouvelle taille
		 */
		C3D_API void resize( castor::Point3ui const & p_size );
		/**
		 *\~english
		 *\brief		sets the texture buffer.
		 *\param[in]	p_buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	p_buffer	Le tampon de la texture.
		 */
		C3D_API void setBuffer( castor::PxBufferBaseSPtr p_buffer );
		/**
		 *\~english
		 *\return		\p true if the texture's source has already been defined.
		 *\~french
		 *\return		\p true si la source de la texture a déjà été définie.
		 */
		inline bool hasSource()const
		{
			return m_source != nullptr;
		}
		/**
		 *\~english
		 *\return		The texture buffer.
		 *\~french
		 *\return		Le tampon de la texture.
		 */
		inline castor::PxBufferBaseSPtr getBuffer()const
		{
			return m_source->getBuffer();
		}
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		inline bool isStaticSource()const
		{
			return m_source->isStatic();
		}
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		inline castor::String toString()const
		{
			return m_source->toString();
		}
		/**
		 *\~english
		 *\return		The image index in its layout.
		 *\~french
		 *\return		L'index de l'image dans son layout.
		 */
		inline uint32_t getIndex()const
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
