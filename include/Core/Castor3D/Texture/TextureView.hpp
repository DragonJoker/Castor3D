/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureView_H___
#define ___C3D_TextureView_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/ImageViewCreateInfo.hpp>

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

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
		 *\param[in]	engine	The engine.
		 *\~french
		 *\return		Le tampon de la texture.
		 *\param[in]	engine	Le moteur.
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
		C3D_API virtual castor::PxBufferBaseSPtr getBuffer( uint32_t level = 0u )const = 0;
		/**
		 *\~english
		 *\return		The texture buffers.
		 *\~french
		 *\return		Les tampons de la texture.
		 */
		C3D_API virtual castor::PxBufferPtrArray const & getBuffers()const = 0;
		/**
		 *\~english
		 *\return		The texture buffers.
		 *\~french
		 *\return		Les tampons de la texture.
		 */
		C3D_API virtual castor::PxBufferPtrArray & getBuffers() = 0;
		/**
		 *\~english
		 *\brief		sets the texture buffer.
		 *\param[in]	buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	buffer	Le tampon de la texture.
		 */
		C3D_API virtual void setBuffer( castor::PxBufferBaseSPtr buffer
			, uint32_t level = 0u ) = 0;
		/**
		 *\~english
		 *\return		The static source status.
		 *\~french
		 *\return		Le statut de source statique.
		 */
		C3D_API virtual bool isStatic()const = 0;

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
		inline VkExtent3D getDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The source's pixel format.
		 *\~french
		 *\return		Le format des pixels de la source.
		 */
		inline VkFormat getPixelFormat()const
		{
			return m_format;
		}
		/**
		 *\~english
		 *\return		The source's mipmap levels count.
		 *\~french
		 *\return		Le nombre de niveaux de mipmaps de la source.
		 */
		inline uint32_t getLevelCount()const
		{
			return uint32_t( getBuffers().size() );
		}

	protected:
		/**
		 *\~english
		 *\brief			Readjusts dimensions if the selected rendering API doesn't support NPOT textures.
		 *\param[in,out]	size	The size.
		 *\return			\p true if the dimensions have changed.
		 *\~french
		 *\brief			Réajuste les dimensions données si l'API de rendu sélectionnée ne supporte pas les textures NPOT.
		 *\param[in,out]	size	La taille.
		 *\return			\p true si les dimensions ont changé.
		 */
		bool doAdjustDimensions( VkExtent3D & size );

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The source's pixel format.
		//!\~french		Le format des pixels de la source.
		VkFormat m_format;
		//!\~english	The source's dimensions.
		//!\~french		Les dimensions de la source.
		VkExtent3D m_size;
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
		friend class TextureLayout;

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
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief		Writes a TextureView into a text file
			 *\param[in]	file	The file
			 *\param[in]	obj		The TextureView
			 *\~french
			 *\brief		Ecrit une TextureView dans un fichier texte
			 *\param[in]	file	Le fichier
			 *\param[in]	obj		La TextureView
			 */
			C3D_API bool operator()( TextureView const & obj
				, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	layout	The parent layout.
		 *\param[in]	info	The creation info.
		 *\param[in]	index	The image index in its layout.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	layout	Le layout parent.
		 *\param[in]	info	Les informations de création.
		 *\param[in]	index	L'index de l'image dans son layout.
		 */
		C3D_API TextureView( TextureLayout & layout
			, ashes::ImageViewCreateInfo info
			, uint32_t index );
		/**
		 *\~english
		 *\brief		Initialises the view.
		 *\return		\p true if OK.
		 *\~french
		 *\brief		Initialise la vue.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the view.
		 *\~french
		 *\brief		Nettoie la vue.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Defines the texture buffer from an image file.
		 *\param[in]	folder		The folder containing the image.
		 *\param[in]	relative	The image file path, relative to folder.
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	folder		Le dossier contenant l'image.
		 *\param[in]	relative	Le chemin d'accès à l'image, relatif à folder.
		 */
		C3D_API void initialiseSource( castor::Path const & folder
			, castor::Path const & relative );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\param[in]	buffer	The buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 *\param[in]	buffer	Le tampon.
		 */
		C3D_API void initialiseSource( castor::PxBufferBaseSPtr buffer );
		/**
		 *\~english
		 *\brief		Initialises the texture buffer.
		 *\~french
		 *\brief		Initialise le tampon de la texture.
		 */
		C3D_API void initialiseSource();
		/**
		 *\~english
		 *\brief		Sets the texture buffer.
		 *\param[in]	buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	buffer	Le tampon de la texture.
		 */
		C3D_API void setBuffer( castor::PxBufferBaseSPtr buffer );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		C3D_API castor::String toString()const;

		inline bool hasSource()const
		{
			return m_source != nullptr;
		}

		inline castor::PxBufferBaseSPtr getBuffer()const
		{
			return m_source->getBuffer();
		}

		inline castor::PxBufferPtrArray const & getBuffers()const
		{
			return m_source->getBuffers();
		}

		inline castor::PxBufferPtrArray & getBuffers()
		{
			return m_source->getBuffers();
		}

		inline bool isStaticSource()const
		{
			return m_source->isStatic();
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}

		inline uint32_t getBaseMipLevel()const
		{
			return m_info->subresourceRange.baseMipLevel;
		}

		inline uint32_t getLevelCount()const
		{
			return m_source->getLevelCount();
		}

		inline bool needsMipmapsGeneration()const
		{
			return m_needsMipmapsGeneration;
		}

		inline ashes::ImageView const & getView()const
		{
			return m_view;
		}
		/**@}*/

	private:
		C3D_API void doUpdate( ashes::ImageViewCreateInfo info );

	private:
		uint32_t m_index;
		ashes::ImageViewCreateInfo m_info;
		std::unique_ptr< TextureSource > m_source;
		ashes::ImageView m_view;
		bool m_needsMipmapsGeneration{ true };
	};
}

#endif
