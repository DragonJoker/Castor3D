/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureUnit_H___
#define ___C3D_TextureUnit_H___
#pragma once

#include "Castor3DPrerequisites.hpp"

#include <Descriptor/WriteDescriptorSet.hpp>

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
		 *\return		The texture dimension.
		 *\~french
		 *\return		La dimension de la texture.
		 */
		C3D_API renderer::TextureType getType()const;
		/**
		 *\~english
		 *\return		The texture.
		 *\~french
		 *\return		La texture.
		 */
		inline TextureLayoutSPtr getTexture()const
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
		/**
		 *\~english
		 *\return		The descriptor used for the texture.
		 *\~french
		 *\return		Le descripteur utilisé pour la texture.
		 */
		inline renderer::WriteDescriptorSet getDescriptor()const
		{
			return m_descriptor;
		}

	private:
		friend class TextureRenderer;
		TextureChannel m_channel;
		castor::Matrix4x4r m_transformations;
		TextureLayoutSPtr m_texture;
		RenderTargetWPtr m_renderTarget;
		SamplerWPtr m_sampler;
		bool m_autoMipmaps;
		renderer::WriteDescriptorSet m_descriptor;
		mutable bool m_changed;
	};
}

#endif
