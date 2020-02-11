/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureView_H___
#define ___C3D_TextureView_H___

#include "TextureModule.hpp"

#include <CastorUtils/Graphics/PixelBufferBase.hpp>

#include <ashespp/Image/ImageView.hpp>
#include <ashespp/Image/ImageViewCreateInfo.hpp>

namespace castor3d
{
	class TextureView
		: public castor::OwnedBy< TextureLayout >
	{
		friend class TextureLayout;

	public:
		/**
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
		 *\return		\p true if inverted.
		 *\~french
		 *\brief		Initialise la vue.
		 *\return		\p true si la vue est inversée.
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
		C3D_API castor::PxBufferBaseSPtr getBuffer()const;
		C3D_API castor::PxBufferPtrArray const & getBuffers()const;
		C3D_API castor::PxBufferPtrArray & getBuffers();
		C3D_API bool isStaticSource()const;
		C3D_API uint32_t getLevelCount()const;

		inline bool hasSource()const
		{
			return m_source != nullptr;
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}

		inline uint32_t getBaseMipLevel()const
		{
			return m_info->subresourceRange.baseMipLevel;
		}

		inline bool needsMipmapsGeneration()const
		{
			return m_needsMipmapsGeneration;
		}

		inline bool needsYInversion()const
		{
			return m_needsYInversion;
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
		bool m_needsYInversion{ false };
	};
}

#endif
