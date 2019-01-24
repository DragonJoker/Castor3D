/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureView_H___
#define ___C3D_TextureView_H___

#include "Castor3DPrerequisites.hpp"

#include <Image/ImageViewCreateInfo.hpp>
#include <Image/ImageSubresourceRange.hpp>
#include <Image/TextureView.hpp>
#include <Miscellaneous/Extent3D.hpp>

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
		 *\param[in]	buffer	The texture buffer.
		 *\~french
		 *\brief		Définit le tampon de la texture.
		 *\param[in]	buffer	Le tampon de la texture.
		 */
		C3D_API virtual void setBuffer( castor::PxBufferBaseSPtr buffer ) = 0;
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
		inline ashes::Extent3D getDimensions()const
		{
			return m_size;
		}
		/**
		 *\~english
		 *\return		The source's pixel format.
		 *\~french
		 *\return		Le format des pixels de la source.
		 */
		inline ashes::Format getPixelFormat()const
		{
			return m_format;
		}

	protected:
		/**
		 *\~english
		 *\brief			Readjusts dimensions if the selected rendering API doesn't support NPOT textures.
		 *\param[in,out]	size	The size.
		 *\param[in,out]	depth	The depth.
		 *\return			\p true if the dimensions have changed.
		 *\~french
		 *\brief			Réajuste les dimensions données si l'API de rendu sélectionnée ne supporte pas les textures NPOT.
		 *\param[in,out]	size	La taille.
		 *\param[in,out]	depth	La profondeur.
		 *\return			\p true si les dimensions ont changé.
		 */
		bool doAdjustDimensions( ashes::Extent3D & size );

	protected:
		//!\~english	The engine.
		//!\~french		Le moteur.
		Engine & m_engine;
		//!\~english	The source's pixel format.
		//!\~french		Le format des pixels de la source.
		ashes::Format m_format;
		//!\~english	The source's dimensions.
		//!\~french		Les dimensions de la source.
		ashes::Extent3D m_size;
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
		 *\param[in]	type	The view texture type.
		 *\param[in]	index	The image index in its layout.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	layout	Le layout parent.
		 *\param[in]	type	Le type de texture de la vue.
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
		 *\param[in]	components	The components to keep in the loaded image.
		 *\~french
		 *\brief		Définit le tampon de la texture depuis un fichier image.
		 *\param[in]	folder		Le dossier contenant l'image.
		 *\param[in]	relative	Le chemin d'accès à l'image, relatif à folder.
		 *\param[in]	components	Les composantes à garder dans l'image chargée.
		 */
		C3D_API void initialiseSource( castor::Path const & folder
			, castor::Path const & relative
			, ImageComponents components );
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
		inline bool hasSource()const
		{
			return m_source != nullptr;
		}

		inline castor::PxBufferBaseSPtr getBuffer()const
		{
			return m_source->getBuffer();
		}

		inline bool isStaticSource()const
		{
			return m_source->isStatic();
		}

		inline castor::String toString()const
		{
			return m_source->toString();
		}

		inline uint32_t getIndex()const
		{
			return m_index;
		}

		inline uint32_t getBaseMipLevel()const
		{
			return m_info.subresourceRange.baseMipLevel;
		}

		inline ashes::TextureView const & getView()const
		{
			CU_Require( m_view );
			return *m_view;
		}
		/**@}*/

	private:
		C3D_API void doUpdate( ashes::ImageViewCreateInfo info );

	private:
		uint32_t m_index;
		ashes::ImageViewCreateInfo m_info;
		std::unique_ptr< TextureSource > m_source;
		ashes::TextureViewPtr m_view;
	};
}

#endif
