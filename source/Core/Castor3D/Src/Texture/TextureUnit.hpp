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
#ifndef ___CU_TEXTURE_UNIT_H___
#define ___CU_TEXTURE_UNIT_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>

namespace Castor3D
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
		: public Castor::OwnedBy< Engine >
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
			: public Castor::TextWriter< TextureUnit >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a TextureUnit into a text file.
			 *\param[in]	p_file	The file.
			 *\param[in]	p_unit	The TextureUnit.
			 *\~french
			 *\brief		Ecrit une TextureUnit dans un fichier texte.
			 *\param[in]	p_file	Le fichier.
			 *\param[in]	p_unit	La TextureUnit.
			 */
			C3D_API bool operator()( TextureUnit const & p_unit, Castor::TextFile & p_file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param		p_engine	The engine.
		 *\~french
		 *\brief		Constructeur.
		 *\param		p_engine	Le moteur.
		 */
		C3D_API explicit TextureUnit( Engine & p_engine );
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
		C3D_API bool Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the texture.
		 *\~french
		 *\brief		Nettoie l'objet.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the texture.
		 *\param[in]	p_texture	The texture.
		 *\~french
		 *\brief		Definit la texture.
		 *\param[in]	p_texture	La texture.
		 */
		C3D_API void SetTexture( TextureLayoutSPtr p_texture );
		/**
		 *\~english
		 *\brief		Applies the texture unit.
		 *\~french
		 *\brief		Applique la texture.
		 */
		C3D_API void Bind()const;
		/**
		 *\~english
		 *\brief		Removes the texture unit from the stack, in order not to interfere with other ones.
		 *\~french
		 *\brief		Désactive la texture.
		 */
		C3D_API void Unbind()const;
		/**
		 *\~english
		 *\return		The texture dimension.
		 *\~french
		 *\return		La dimension de la texture.
		 */
		C3D_API TextureType GetType()const;
		/**
		 *\~english
		 *\return		The texture.
		 *\~french
		 *\return		La texture.
		 */
		inline Castor3D::TextureLayoutSPtr GetTexture()const
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
		inline Castor3D::TextureChannel GetChannel()const
		{
			return m_channel;
		}
		/**
		 *\~english
		 *\brief		Sets the texture channel.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le canal de la texture.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetChannel( Castor3D::TextureChannel p_value )
		{
			m_channel = p_value;
		}
		/**
		 *\~english
		 *\return		The unit index.
		 *\~french
		 *\return		L'index de l'unité.
		 */
		inline uint32_t GetIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\brief		Sets the unit index.
		 *\param[in]	p_index	The new value.
		 *\~french
		 *\brief		Définit l'index de l'unité.
		 *\param[in]	p_index	La nouvelle valeur.
		 */
		inline void SetIndex( uint32_t p_value )
		{
			m_index = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the target holding the texture.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit la cible contenant la texture.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetRenderTarget( RenderTargetSPtr p_value )
		{
			m_renderTarget = p_value;
		}
		/**
		 *\~english
		 *\brief		Defines the texture sampler.
		 *\param[in]	p_value	The sampler.
		 *\~french
		 *\brief		Définit le sampler de la texture.
		 *\param[in]	p_value	Le sampler.
		 */
		inline void SetSampler( SamplerSPtr p_value )
		{
			m_sampler = p_value;
		}
		/**
		 *\~english
		 *\return		The sampler.
		 *\~french
		 *\return		L'échantillonneur.
		 */
		inline SamplerSPtr GetSampler()const
		{
			return m_sampler.lock();
		}
		/**
		 *\~english
		 *\return		\p false if the texture is null.
		 *\~french
		 *\return		\p false si la texture est nulle.
		 */
		inline bool IsTextured()const
		{
			return m_texture != nullptr;
		}
		/**
		 *\~english
		 *\brief		Sets the auto mipmaps generation status.
		 *\param[in]	p_value	The new value.
		 *\~french
		 *\brief		Définit le statut d'auto génération des mipmaps.
		 *\param[in]	p_value	La nouvelle valeur.
		 */
		inline void SetAutoMipmaps( bool p_value )
		{
			m_autoMipmaps = p_value;
		}
		/**
		 *\~english
		 *\return		The the auto mipmaps generation status.
		 *\~french
		 *\return		Le statut d'auto génération des mipmaps.
		 */
		inline bool GetAutoMipmaps()const
		{
			return m_autoMipmaps;
		}
		/**
		 *\~english
		 *\return		The render target.
		 *\~french
		 *\return		La cible de rendu.
		 */
		inline RenderTargetSPtr GetRenderTarget()const
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
		Castor::Matrix4x4r m_transformations;
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
	 *\param[in,out]	p_streamOut	The stream receiving texture's data.
	 *\param[in]		p_texture	The input texture.
	 *\return			A reference to the stream.
	 *\~french
	 *\brief			Opérateur de flux.
	 *\param[in,out]	p_streamOut	Le flux qui reçoit les données de la texture.
	 *\param[in]		p_texture	La texture.
	 *\return			Une référence sur le flux.
	 */
	inline std::ostream & operator<<( std::ostream & p_streamOut, TextureUnitSPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
}

#endif
