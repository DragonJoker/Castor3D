/*
See LICENSE file in root folder
*/
#ifndef ___CU_TEXTURE_UNIT_H___
#define ___CU_TEXTURE_UNIT_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace castor3d
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Texture unit representation.
	\remark		A texture unit is a texture with few options, like channel, blend modes, transformations...
	\~french
	\brief		Représentation d'une unité de texture.
	\remark		Une unité de texture se compose d'une texture avec des options telles que son canal, modes de mélange, transformations...
	*/
	class TextureUnit
		: public castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		TextureUnit loader.
		\~french
		\brief		Loader de TextureUnit.
		*/
		class TextWriter
			: public castor::TextWriter< TextureUnit >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs
				, MaterialType type );
			/**
			 *\~english
			 *\brief		Writes a TextureUnit into a text file.
			 *\param[in]	file	The file.
			 *\param[in]	unit	The TextureUnit.
			 *\~french
			 *\brief		Ecrit une TextureUnit dans un fichier texte.
			 *\param[in]	file	Le fichier.
			 *\param[in]	unit	La TextureUnit.
			 */
			C3D_API bool operator()( TextureUnit const & unit, castor::TextFile & file )override;

		private:
			MaterialType m_type;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param		engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param		engine	Le moteur.
		 */
		C3D_API explicit TextureUnit( Engine & engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~TextureUnit();
		/**
		 *\~english
		 *\brief		Initialises the texture, id est : fills the image buffer, creates the texture in the render system.
		 *\~french
		 *\brief		Initialise la texture, id est : remplit le buffer d'image, cree la texture au niveau du renderer.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleans up the texture.
		 *\~french
		 *\brief		Nettoie l'objet.
		 */
		C3D_API void cleanup();
		/**
		 *\~english
		 *\brief		Sets the texture.
		 *\param[in]	texture	The texture.
		 *\~french
		 *\brief		Definit la texture.
		 *\param[in]	texture	La texture.
		 */
		C3D_API void setTexture( TextureLayoutSPtr texture );
		/**
		 *\~english
		 *\brief		Applies the texture unit.
		 *\~french
		 *\brief		Applique la texture.
		 */
		C3D_API void bind()const;
		/**
		 *\~english
		 *\brief		Removes the texture unit from the stack, in order not to interfere with other ones.
		 *\~french
		 *\brief		Désactive la texture.
		 */
		C3D_API void unbind()const;
		/**
		 *\~english
		 *\return		The texture dimension.
		 *\~french
		 *\return		La dimension de la texture.
		 */
		C3D_API TextureType getType()const;
		/**
		 *\~english
		 *\return		The texture.
		 *\~french
		 *\return		La texture.
		 */
		inline castor3d::TextureLayoutSPtr getTexture()const
		{
			return m_texture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture channel.
		 *\return		The value.
		 *\~french
		 *\brief		Récupère le canal de la texture.
		 *\return		La valeur.
		 */
		inline castor3d::TextureChannel getChannel()const
		{
			return m_channel;
		}
		/**
		 *\~english
		 *\brief		Sets the texture channel.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le canal de la texture.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setChannel( castor3d::TextureChannel value )
		{
			m_channel = value;
		}
		/**
		 *\~english
		 *\return		The unit index.
		 *\~french
		 *\return		L'index de l'unité.
		 */
		inline uint32_t getIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\brief		Sets the unit index.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit l'index de l'unité.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setIndex( uint32_t value )
		{
			m_index = value;
		}
		/**
		 *\~english
		 *\brief		Sets the target holding the texture.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la cible contenant la texture.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setRenderTarget( RenderTargetSPtr value )
		{
			m_renderTarget = value;
		}
		/**
		 *\~english
		 *\brief		Defines the texture sampler.
		 *\param[in]	value	The sampler.
		 *\~french
		 *\brief		Définit le sampler de la texture.
		 *\param[in]	value	Le sampler.
		 */
		inline void setSampler( SamplerSPtr value )
		{
			m_sampler = value;
		}
		/**
		 *\~english
		 *\return		The sampler.
		 *\~french
		 *\return		L'échantillonneur.
		 */
		inline SamplerSPtr getSampler()const
		{
			return m_sampler.lock();
		}
		/**
		 *\~english
		 *\return		\p false if the texture is null.
		 *\~french
		 *\return		\p false si la texture est nulle.
		 */
		inline bool isTextured()const
		{
			return m_texture != nullptr;
		}
		/**
		 *\~english
		 *\brief		Sets the auto mipmaps generation status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut d'auto génération des mipmaps.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setAutoMipmaps( bool value )
		{
			m_autoMipmaps = value;
		}
		/**
		 *\~english
		 *\return		The the auto mipmaps generation status.
		 *\~french
		 *\return		Le statut d'auto génération des mipmaps.
		 */
		inline bool getAutoMipmaps()const
		{
			return m_autoMipmaps;
		}
		/**
		 *\~english
		 *\return		The render target.
		 *\~french
		 *\return		La cible de rendu.
		 */
		inline RenderTargetSPtr getRenderTarget()const
		{
			return m_renderTarget.lock();
		}

	private:
		friend class TextureRenderer;
		//!\~english	The unit index inside it's pass.
		//!\~french		L'index de l'unité dans sa passe.
		uint32_t m_index;
		//!\see TextureChannel
		//\~english		The unit channel inside it's pass.
		//!\~french		Le canal de l'unité dans sa passe.
		TextureChannel m_channel;
		//!\~english	The unit transformations.
		//!\~french		Les transformations de l'unité.
		castor::Matrix4x4r m_transformations;
		//!\~english	The unit texture.
		//!\~french		La texture de l'unité.
		TextureLayoutSPtr m_texture;
		//!\~english	The render target used to compute the texture, if this unit is a render target.
		//!\~french		La render target utilisée pour générer la texture si cette unité est une render target.
		RenderTargetWPtr m_renderTarget;
		//!\~english	The sampler state assigned to this unit.
		//!\~french		Le sampler state affecté à cette unité.
		SamplerWPtr m_sampler;
		//!\~english	Tells mipmaps must be regenerated after each texture data change.
		//!\~french		Dit que les mipmaps doivent être regénérés après chaque changement des données de la texture.
		bool m_autoMipmaps;
		//!\~english	Tells the texture data has changed.
		//!\~french		Dit que les données de la texture ont changé.
		mutable bool m_changed;
	};
	/**
	 *\~english
	 *\brief			Stream operator.
	 *\param[in,out]	streamOut	The stream receiving texture's data.
	 *\param[in]		texture		The input texture.
	 *\return			A reference to the stream.
	 *\~french
	 *\brief			Opérateur de flux.
	 *\param[in,out]	streamOut	Le flux qui reçoit les données de la texture.
	 *\param[in]		texture		La texture.
	 *\return			Une référence sur le flux.
	 */
	inline std::ostream & operator<<( std::ostream & streamOut, TextureUnitSPtr const & texture )
	{
		streamOut << texture->getIndex();
		return streamOut;
	}
}

#endif
