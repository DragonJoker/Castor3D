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
#ifndef ___CU_TEXTURE_UNIT_H___
#define ___CU_TEXTURE_UNIT_H___

#include "Castor3DPrerequisites.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Texture unit representation
	\remark		A texture unit is a texture with few options, like channel, blend modes, transformations.
	\~french
	\brief		Représentation d'une unité de texture
	\remark		Une unité de texture se compose d'une texture avec des options telles que son canal, modes de mélange, transformations.
	*/
	class TextureUnit
		: public Castor::OwnedBy< Engine >
	{
	public:
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		TextureUnit loader
		\~french
		\brief		Loader de TextureUnit
		*/
		class TextLoader
			: public Castor::TextLoader< TextureUnit >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API TextLoader( Castor::String const & p_tabs, Castor::File::eENCODING_MODE p_encodingMode = Castor::File::eENCODING_MODE_ASCII );
			/**
			 *\~english
			 *\brief		Writes a TextureUnit into a text file
			 *\param[in]	p_file	The file
			 *\param[in]	p_unit	The TextureUnit
			 *\~french
			 *\brief		Ecrit une TextureUnit dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_unit	La TextureUnit
			 */
			C3D_API virtual bool operator()( TextureUnit const & p_unit, Castor::TextFile & p_file );
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param		p_engine	The Engine
		 *\~french
		 *\brief		Constructeur
		 *\param		p_engine	Le moteur
		 */
		C3D_API TextureUnit( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~TextureUnit();
		/**
		 *\~english
		 *\brief		Initialises the texture, id est : fills the image buffer, creates the texture in the render system
		 *\~french
		 *\brief		Initialise la texture, id est : remplit le buffer d'image, cree la texture au niveau du renderer
		 */
		C3D_API void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the texture
		 *\~french
		 *\brief		Nettoie l'objet
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the texture
		 *\param[in]	p_texture	The texture
		 *\~french
		 *\brief		Definit la texture
		 *\param[in]	p_texture	La texture
		 */
		C3D_API void SetTexture( TextureLayoutSPtr p_texture );
		/**
		 *\~english
		 *\brief		Applies the texture unit
		 *\~french
		 *\brief		Applique la texture
		 */
		C3D_API void Bind()const;
		/**
		 *\~english
		 *\brief		Removes the texture unit from the stack, in order not to interfere with other ones
		 *\~french
		 *\brief		Desactive la texture
		 */
		C3D_API void Unbind()const;
		/**
		 *\~english
		 *\brief		Loads the texture image from the given path
		 *\param[in]	p_pathFile	The image file path
		 *\return		\p false if any problem occured
		 *\~french
		 *\brief		Charge l'image de la texture à partir du chemin donné
		 *\param[in]	p_pathFile	Le chemin du fichier image
		 *\return		\p false si un problème quelconque est arrivé
		 */
		C3D_API bool LoadTexture( Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		C3D_API TextureType GetType()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture initalisation status
		 *\return		\p false if the texture is null or uninitialised
		 *\~french
		 *\brief		Récupère le statut d'initialisation de la texture
		 *\return		\p false si la texture est nulle ou non initialisée
		 */
		C3D_API bool IsTextureInitialised()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture file path
		 *\return		The value
		 *\~french
		 *\brief		Récupère le chemin du fichier de la texture
		 *\return		La valeur
		 */
		inline const Castor::Path & GetTexturePath()const
		{
			return m_pathTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The value
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La valeur
		 */
		inline Castor3D::TextureLayoutSPtr GetTexture()const
		{
			return m_pTexture;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha blending function
		 *\return		The value
		 *\~french
		 *\brief		Récupère la fonction de mélange d'alpha
		 *\return		La valeur
		 */
		inline Castor3D::AlphaBlendFunc GetAlpFunction()const
		{
			return m_eAlpFunction;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha blending function
		 *\param[in]	p_func	The new value
		 *\~french
		 *\brief		Définit la fonction de mélange d'alpha
		 *\param[in]	p_func	La nouvelle valeur
		 */
		inline void SetAlpFunction( Castor3D::AlphaBlendFunc p_func )
		{
			m_eAlpFunction = p_func;
		}
		/**
		 *\~english
		 *\brief		Retrieves RGB blending function
		 *\return		The value
		 *\~french
		 *\brief		Récupère fonction de mélange RGB
		 *\return		La valeur
		 */
		inline Castor3D::RGBBlendFunc GetRgbFunction()const
		{
			return m_eRgbFunction;
		}
		/**
		 *\~english
		 *\brief		Sets RGB blending function
		 *\param[in]	p_func	The new value
		 *\~french
		 *\brief		Définit fonction de mélange RGB
		 *\param[in]	p_func	La nouvelle valeur
		 */
		inline void SetRgbFunction( Castor3D::RGBBlendFunc p_func )
		{
			m_eRgbFunction = p_func;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha function
		 *\return		The value
		 *\~french
		 *\brief		Récupère la fonction d'alpha
		 *\return		La valeur
		 */
		inline Castor3D::AlphaFunc GetAlphaFunc()const
		{
			return m_eAlphaFunc;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha function
		 *\param[in]	p_func	The new value
		 *\~french
		 *\brief		Définit la fonction d'alpha
		 *\param[in]	p_func	La nouvelle valeur
		 */
		inline void SetAlphaFunc( Castor3D::AlphaFunc p_func )
		{
			m_eAlphaFunc = p_func;
		}
		/**
		 *\~english
		 *\brief		Retrieves the alpha reference value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de référence pour l'alpha
		 *\return		La valeur
		 */
		inline float GetAlphaValue()const
		{
			return m_fAlphaValue;
		}
		/**
		 *\~english
		 *\brief		Sets the alpha reference value
		 *\param[in]	p_fAlpha	The new value
		 *\~french
		 *\brief		Définit la valeur de référence pour l'alpha
		 *\param[in]	p_fAlpha	La nouvelle valeur
		 */
		inline void SetAlphaValue( float p_fAlpha )
		{
			m_fAlphaValue = p_fAlpha;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture channel
		 *\return		The value
		 *\~french
		 *\brief		Récupère le canal de la texture
		 *\return		La valeur
		 */
		inline Castor3D::TextureChannel GetChannel()const
		{
			return m_eChannel;
		}
		/**
		 *\~english
		 *\brief		Sets the texture channel
		 *\param[in]	p_channel	The new value
		 *\~french
		 *\brief		Définit le canal de la texture
		 *\param[in]	p_channel	La nouvelle valeur
		 */
		inline void SetChannel( Castor3D::TextureChannel p_channel )
		{
			m_eChannel = p_channel;
		}
		/**
		 *\~english
		 *\brief		Retrieves the unit index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'index de l'unité
		 *\return		La valeur
		 */
		inline uint32_t GetIndex()const
		{
			return m_index;
		}
		/**
		 *\~english
		 *\brief		Sets the unit index
		 *\param[in]	p_index	The new value
		 *\~french
		 *\brief		Définit l'index de l'unité
		 *\param[in]	p_index	La nouvelle valeur
		 */
		inline void SetIndex( uint32_t p_index )
		{
			m_index = p_index;
		}
		/**
		 *\~english
		 *\brief		Retrieves the blend colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur de mélange
		 *\return		La valeur
		 */
		inline const Castor::Colour	& GetBlendColour()const
		{
			return m_clrBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the blend colour
		 *\param[in]	p_clrBlend	The new value
		 *\~french
		 *\brief		Définit la couleur de mélange
		 *\param[in]	p_clrBlend	La nouvelle valeur
		 */
		inline void SetBlendColour( const Castor::Colour & p_clrBlend )
		{
			m_clrBlend = p_clrBlend;
		}
		/**
		 *\~english
		 *\brief		Sets the target holding the texture
		 *\param[in]	p_pTarget	The new value
		 *\~french
		 *\brief		Définit la cible contenant la texture
		 *\param[in]	p_pTarget	La nouvelle valeur
		 */
		inline void SetRenderTarget( RenderTargetSPtr p_pTarget )
		{
			m_renderTarget = p_pTarget;
		}
		/**
		 *\~english
		 *\brief		Defines the texture sampler
		 *\param[in]	p_pSampler	The sampler
		 *\~french
		 *\brief		Définit le sampler de la texture
		 *\param[in]	p_pSampler	Le sampler
		 */
		inline void SetSampler( SamplerSPtr p_pSampler )
		{
			m_pSampler = p_pSampler;
		}
		/**
		 *\~english
		 *\brief		Retrieves the texture sampler
		 *\return		The sampler
		 *\~french
		 *\brief		Récupère le sampler de la texture
		 *\return		Le buffer
		 */
		inline SamplerSPtr GetSampler()const
		{
			return m_pSampler.lock();
		}
		/**
		 *\~english
		 *\brief		Retrieves the RGB blend parameter
		 *\param[in]	p_index	The parameter index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le paramètre de mélange RGB
		 *\param[in]	p_index	L'index du paramètre
		 *\return		La valeur
		 */
		inline BlendSource GetRgbArgument( BlendSrcIndex p_index )const
		{
			return m_eRgbArguments[uint32_t( p_index )];
		}
		/**
		 *\~english
		 *\brief		Sets the RGB blend parameter
		 *\param[in]	p_index	The parameter index
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit le paramètre de mélange RGB
		 *\param[in]	p_index	L'index du paramètre
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetRgbArgument( BlendSrcIndex p_index, BlendSource p_value )
		{
			m_eRgbArguments[uint32_t( p_index )] = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the Alpha blend parameter
		 *\param[in]	p_index	The parameter index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le paramètre de mélange Alpha
		 *\param[in]	p_index	L'index du paramètre
		 *\return		La valeur
		 */
		inline BlendSource GetAlpArgument( BlendSrcIndex p_index )const
		{
			return m_eAlpArguments[uint32_t( p_index )];
		}
		/**
		 *\~english
		 *\brief		Sets the Alpha blend parameter
		 *\param[in]	p_index	The parameter index
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit le paramètre de mélange Alpha
		 *\param[in]	p_index	L'index du paramètre
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetAlpArgument( BlendSrcIndex p_index, BlendSource p_value )
		{
			m_eAlpArguments[uint32_t( p_index )] = p_value;
		}
		/**
		 *\~english
		 *\brief		Tells if the unit has a texture
		 *\return		\p false if the texture is null
		 *\~french
		 *\brief		Dit si l'unité a une texture
		 *\return		\p false si la texture est nulle
		 */
		inline bool IsTextured()const
		{
			return m_pTexture != nullptr;
		}
		/**
		 *\~english
		 *\brief		Sets the auto mipmaps generation status
		 *\param[in]	p_value		The new value
		 *\~french
		 *\brief		Définit le statut d'auto génération des mipmaps
		 *\param[in]	p_value		La nouvelle valeur
		 */
		inline void SetAutoMipmaps( bool p_value )
		{
			m_bAutoMipmaps = p_value;
		}
		/**
		 *\~english
		 *\brief		Retrieves the auto mipmaps generation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'auto génération des mipmaps
		 *\return		La valeur
		 */
		inline bool GetAutoMipmaps()const
		{
			return m_bAutoMipmaps;
		}
		/**
		 *\~english
		 *\brief		Retrieves the render target
		 *\return		The value
		 *\~french
		 *\brief		Récupère la cible de rendu
		 *\return		La valeur
		 */
		inline RenderTargetSPtr GetRenderTarget()const
		{
			return m_renderTarget.lock();
		}

	private:
		friend class TextureRenderer;
		//!\~english The unit index inside it's pass	\~french L'index de l'unité dans sa passe
		uint32_t m_index;
		//!\see TextureChannel	\~english The unit channel inside it's pass	\~french Le canal de l'unité dans sa passe
		TextureChannel m_eChannel;
		//!\~english The reference alpha value for alpha comparison	\~french La valeur d'alpha de référence pour la comparaison d'alpha
		float m_fAlphaValue;
		//!\~english The alpha function for alpha comparison	\~french La fonction d'alpha utilisée lors de la comparaison d'alpha
		AlphaFunc m_eAlphaFunc;
		//!\~english The RGB blending parameters	\~french Les paramètres de mélange RGB
		BlendSource m_eRgbArguments[uint32_t( BlendSrcIndex::Count )];
		//!\~english The Alpha blending parameters	\~french Les paramètres de mélange Alpha
		BlendSource m_eAlpArguments[uint32_t( BlendSrcIndex::Count )];
		//!\~english The RGB blending function	\~french La fonction de mélange RGB
		RGBBlendFunc m_eRgbFunction;
		//!\~english The Alpha blending function	\~french La fonction de mélange Alpha
		AlphaBlendFunc m_eAlpFunction;
		//!\~english The unit transformations	\~french Les transformations de l'unité
		Castor::Matrix4x4r m_mtxTransformations;
		//!\~english The unit texture	\~french La texture de l'unité
		TextureLayoutSPtr m_pTexture;
		//!\~english The unit texture's path	\~french Le chemin de la texture de l'unité
		Castor::Path m_pathTexture;
		//!\~english The render target used to compute the texture, if this unit is a render target	\~french La render target utilisée pour générer la texture si cette unité est une render target
		RenderTargetWPtr m_renderTarget;
		//!\~english The sampler state assigned to this unit	\~french Le sampler state affecté à cette unité
		SamplerWPtr m_pSampler;
		//!\~english The blend colour when it is enabled	\~french Couleur de mélange lorsque celui-ci est actif
		Castor::Colour m_clrBlend;
		//!\~english The matrix mode in use berfore this unit's transformations are applied	\~french Le mode de matrice en cours avant l'application des transformations de cette unité
		eMTXMODE m_ePrevMtxMode;
		//!\~english Tells mipmaps must be regenerated after each texture data change	\~french Dit que les mipmaps doivent être regénérés après chaque changement des données de la texture
		bool m_bAutoMipmaps;
		//!\~english Tells the texture data has changed	\~french Dit que les données de la texture ont changé
		mutable bool m_changed;
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
	inline std::ostream & operator <<( std::ostream & p_streamOut, TextureUnitSPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
}

#endif
