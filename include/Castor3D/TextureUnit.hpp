/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___CU_TextureUnit___
#define ___CU_TextureUnit___

#include "Prerequisites.hpp"
#include "Renderable.hpp"
#include "BinaryLoader.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4275 )

namespace Castor
{
}

namespace Castor3D
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.7.0.0
	\date		15/04/2013
	\~english
	\brief		TextureUnit loader
	\~french
	\brief		Loader de TextureUnit
	*/
	class C3D_API UnitBinaryParser : public BinaryParser< TextureUnit >
	{
	private:
		virtual bool DoFill( TextureUnit const & p_unit, stCHUNK & p_chunk );
		virtual bool DoParse( TextureUnit & p_unit, stCHUNK & p_chunk );
	};
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
	class C3D_API TextureUnit : public Renderable< TextureUnit, TextureRenderer >
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
		class C3D_API TextLoader : public Castor::Loader< TextureUnit, Castor::eFILE_TYPE_TEXT, Castor::TextFile >, CuNonCopyable
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			TextLoader( Castor::File::eENCODING_MODE p_eEncodingMode=Castor::File::eENCODING_MODE_ASCII );
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
			virtual bool operator ()( TextureUnit const & p_unit, Castor::TextFile & p_file );
		};
	private:
		friend class TextureRenderer;
		//!\~english	The unit index inside it's pass	\~french	L'index de l'unité dans sa passe
		uint32_t m_uiIndex;
		//!\see eTEXTURE_CHANNEL	\~english	The unit channel inside it's pass	\~french	Le canal de l'unité dans sa passe
		eTEXTURE_CHANNEL m_eChannel;
		//!\~english	The reference alpha value for alpha comparison	\~french	La valeur d'alpha de référence pour la comparaison d'alpha
		float m_fAlphaValue;
		//!\~english	The alpha function for alpha comparison	\~french	La fonction d'alpha utilisée lors de la comparaison d'alpha
		eALPHA_FUNC m_eAlphaFunc;
		//!\~english	The RGB blending parameters	\~french	Les paramètres de mélange RGB
		eBLEND_SOURCE m_eRgbArguments[eBLEND_SRC_INDEX_COUNT];
		//!\~english	The Alpha blending parameters	\~french	Les paramètres de mélange Alpha
		eBLEND_SOURCE m_eAlpArguments[eBLEND_SRC_INDEX_COUNT];
		//!\~english	The RGB blending function	\~french	La fonction de mélange RGB
		eRGB_BLEND_FUNC m_eRgbFunction;
		//!\~english	The Alpha blending function	\~french	La fonction de mélange Alpha
		eALPHA_BLEND_FUNC m_eAlpFunction;
		//!\~english	The unit transformations	\~french	Les transformations de l'unité
		Castor::Matrix4x4r m_mtxTransformations;
		//!\~english	The unit texture	\~french	La texture de l'unité
		TextureBaseSPtr m_pTexture;
		//!\~english	The unit texture's path	\~french	Le chemin de la texture de l'unité
		Castor::Path m_pathTexture;
		//!\~english	The render target used to compute the texture, if this unit is a render target	\~french	La render target utilisée pour générer la texture si cette unité est une render target
		RenderTargetWPtr m_pRenderTarget;
		//!\~english	The sampler state assigned to this unit	\~french	Le sampler state affecté à cette unité
		SamplerWPtr m_pSampler;
		//!\~english	The blend colour when it is enabled	\~french	Couleur de mélange lorsque celui-ci est actif
		Castor::Colour m_clrBlend;
		//!\~english	The matrix mode in use berfore this unit's transformations are applied	\~french	Le mode de matrice en cours avant l'application des transformations de cette unité
		eMTXMODE m_ePrevMtxMode;
		//!\~english	Tells mipmaps must be regenerated after each texture data change	\~french	Dit que les mipmaps doivent être regénérés après chaque changement des données de la texture
		bool m_bAutoMipmaps;
		//!\~english	Tells the texture data has changed	\~french	Dit que les données de la texture ont changé
		bool m_bChanged;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param		p_pEngine	The Engine
		 *\~french
		 *\brief		Constructeur
		 *\param		p_pEngine	Le moteur
		 */
		TextureUnit( Engine * p_pEngine );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		~TextureUnit();
		/**
		 *\~english
		 *\brief		Initialises the texture, id est : fills the image buffer, creates the texture in the render system
		 *\~french
		 *\brief		Initialise la texture, id est : remplit le buffer d'image, cree la texture au niveau du renderer
		 */
		void Initialise();
		/**
		 *\~english
		 *\brief		Cleans up the texture
		 *\~french
		 *\brief		Nettoie l'objet
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Sets the texture
		 *\param[in]	p_pTexture	The texture
		 *\~french
		 *\brief		Definit la texture
		 *\param[in]	p_pTexture	La texture
		 */
		void SetTexture( TextureBaseSPtr p_pTexture );
		/**
		 *\~english
		 *\brief		Applies the texture unit
		 *\~french
		 *\brief		Applique la texture
		 */
		virtual void Render();
		/**
		 *\~english
		 *\brief		Removes the texture unit from the stack, in order not to interfere with other ones
		 *\~french
		 *\brief		Desactive la texture
		 */
		virtual void EndRender();
		/**
		 *\~english
		 *\brief		Uploads current image to the GPU
		 *\param[in]	p_bSync	Tells if the upload must be synchronous or not
		 *\~french
		 *\brief		Charge l'image courante sur le GPU
		 *\param[in]	p_bSync	Dit si le chargement doit être synchrone ou pas
		 */
		void UploadImage( bool p_bSync );
		/**
		 *\~english
		 *\brief		Downloads the current image from the GPU
		 *\param[in]	p_bSync	Tells if the download must be synchronous or not
		 *\~french
		 *\brief		Charge l'image courante à partir du GPU
		 *\param[in]	p_bSync	Dit si le chargement doit être synchrone ou pas
		 */
		void DownloadImage( bool p_bSync );
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
		bool LoadTexture( Castor::Path const & p_pathFile );
		/**
		 *\~english
		 *\brief		Retrieves the texture file path
		 *\return		The value
		 *\~french
		 *\brief		Récupère le chemin du fichier de la texture
		 *\return		La valeur
		 */
		inline const Castor::Path & GetTexturePath()const { return m_pathTexture; }
		/**
		 *\~english
		 *\brief		Retrieves the texture
		 *\return		The value
		 *\~french
		 *\brief		Récupère la texture
		 *\return		La valeur
		 */
		inline Castor3D::TextureBaseSPtr GetTexture()const { return m_pTexture; }
		/**
		 *\~english
		 *\brief		Retrieves the alpha blending function
		 *\return		The value
		 *\~french
		 *\brief		Récupère la fonction de mélange d'alpha
		 *\return		La valeur
		 */
		inline Castor3D::eALPHA_BLEND_FUNC GetAlpFunction()const { return m_eAlpFunction; }
		/**
		 *\~english
		 *\brief		Sets the alpha blending function
		 *\param[in]	p_eFunc	The new value
		 *\~french
		 *\brief		Définit la fonction de mélange d'alpha
		 *\param[in]	p_eFunc	La nouvelle valeur
		 */
		inline void SetAlpFunction( Castor3D::eALPHA_BLEND_FUNC p_eFunc ) { m_eAlpFunction = p_eFunc; }
		/**
		 *\~english
		 *\brief		Retrieves RGB blending function
		 *\return		The value
		 *\~french
		 *\brief		Récupère fonction de mélange RGB
		 *\return		La valeur
		 */
		inline Castor3D::eRGB_BLEND_FUNC GetRgbFunction()const { return m_eRgbFunction; }
		/**
		 *\~english
		 *\brief		Sets RGB blending function
		 *\param[in]	p_eFunc	The new value
		 *\~french
		 *\brief		Définit fonction de mélange RGB
		 *\param[in]	p_eFunc	La nouvelle valeur
		 */
		inline void SetRgbFunction( Castor3D::eRGB_BLEND_FUNC p_eFunc ) { m_eRgbFunction = p_eFunc; }
		/**
		 *\~english
		 *\brief		Retrieves the alpha function
		 *\return		The value
		 *\~french
		 *\brief		Récupère la fonction d'alpha
		 *\return		La valeur
		 */
		inline Castor3D::eALPHA_FUNC GetAlphaFunc()const { return m_eAlphaFunc; }
		/**
		 *\~english
		 *\brief		Sets the alpha function
		 *\param[in]	p_eFunc	The new value
		 *\~french
		 *\brief		Définit la fonction d'alpha
		 *\param[in]	p_eFunc	La nouvelle valeur
		 */
		inline void SetAlphaFunc( Castor3D::eALPHA_FUNC p_eFunc ) { m_eAlphaFunc = p_eFunc; }
		/**
		 *\~english
		 *\brief		Retrieves the alpha reference value
		 *\return		The value
		 *\~french
		 *\brief		Récupère la valeur de référence pour l'alpha
		 *\return		La valeur
		 */
		inline float GetAlphaValue()const { return m_fAlphaValue; }
		/**
		 *\~english
		 *\brief		Sets the alpha reference value
		 *\param[in]	p_fAlpha	The new value
		 *\~french
		 *\brief		Définit la valeur de référence pour l'alpha
		 *\param[in]	p_fAlpha	La nouvelle valeur
		 */
		inline void SetAlphaValue( float p_fAlpha ) { m_fAlphaValue = p_fAlpha; }
		/**
		 *\~english
		 *\brief		Retrieves the texture channel
		 *\return		The value
		 *\~french
		 *\brief		Récupère le canal de la texture
		 *\return		La valeur
		 */
		inline Castor3D::eTEXTURE_CHANNEL GetChannel()const { return m_eChannel; }
		/**
		 *\~english
		 *\brief		Sets the texture channel
		 *\param[in]	p_eChannel	The new value
		 *\~french
		 *\brief		Définit le canal de la texture
		 *\param[in]	p_eChannel	La nouvelle valeur
		 */
		inline void SetChannel( Castor3D::eTEXTURE_CHANNEL p_eChannel ) { m_eChannel = p_eChannel; }
		/**
		 *\~english
		 *\brief		Retrieves the unit index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l'index de l'unité
		 *\return		La valeur
		 */
		inline uint32_t GetIndex()const { return m_uiIndex; }
		/**
		 *\~english
		 *\brief		Sets the unit index
		 *\param[in]	p_uiIndex	The new value
		 *\~french
		 *\brief		Définit l'index de l'unité
		 *\param[in]	p_uiIndex	La nouvelle valeur
		 */
		inline void SetIndex( uint32_t p_uiIndex ) { m_uiIndex = p_uiIndex; }
		/**
		 *\~english
		 *\brief		Retrieves the blend colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur de mélange
		 *\return		La valeur
		 */
		inline const Castor::Colour	& GetBlendColour()const { return m_clrBlend; }
		/**
		 *\~english
		 *\brief		Sets the blend colour
		 *\param[in]	p_clrBlend	The new value
		 *\~french
		 *\brief		Définit la couleur de mélange
		 *\param[in]	p_clrBlend	La nouvelle valeur
		 */
		inline void SetBlendColour( const Castor::Colour & p_clrBlend ) { m_clrBlend = p_clrBlend; }
		/**
		 *\~english
		 *\brief		Retrieves the texture dimension
		 *\return		The texture dimension
		 *\~french
		 *\brief		Récupère la dimension de la texture
		 *\return		La dimension de la texture
		 */
		 eTEXTURE_DIMENSION GetDimension()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture mapping mode
		 *\return		The texture mapping mode
		 *\~french
		 *\brief		Récupère le mode mappage de la texture
		 *\return		Le mode mappage de la texture
		 */
		 eTEXTURE_MAP_MODE GetMappingMode()const;
		/**
		 *\~english
		 *\brief		Defines the texture mapping mode
		 *\param[in]	p_eMode	The texture mapping mode
		 *\~french
		 *\brief		Définit le mode de mappage de la texture
		 *\param[in]	p_eMode	Le mode de mappage de la texture
		 */
		 void SetMappingMode( eTEXTURE_MAP_MODE p_eMode );
		/**
		 *\~english
		 *\brief		Sets the target holding the texture
		 *\param[in]	p_pTarget	The new value
		 *\~french
		 *\brief		Définit la cible contenant la texture
		 *\param[in]	p_pTarget	La nouvelle valeur
		 */
		inline void SetRenderTarget( RenderTargetSPtr p_pTarget ) { m_pRenderTarget = p_pTarget; }
		/**
		 *\~english
		 *\brief		Sets the sampler
		 *\param[in]	p_pSampler	The new value
		 *\~french
		 *\brief		Définit le sampler
		 *\param[in]	p_pSampler	La nouvelle valeur
		 */
		inline void SetSampler( SamplerSPtr p_pSampler ) { m_pSampler = p_pSampler; }
		/**
		 *\~english
		 *\brief		Retrieves the texture image buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le buffer de l'image de la texture
		 *\return		Le buffer
		 */
		uint8_t const * GetImageBuffer()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture image buffer
		 *\return		The buffer
		 *\~french
		 *\brief		Récupère le buffer de l'image de la texture
		 *\return		Le buffer
		 */
		uint8_t * GetImageBuffer();
		/**
		 *\~english
		 *\brief		Retrieves the texture image pixels
		 *\return		The pixels
		 *\~french
		 *\brief		Récupère les pixels de l'image de la texture
		 *\return		Les pixels
		 */
		Castor::PxBufferBaseSPtr const GetImagePixels()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture image pixels
		 *\return		The pixels
		 *\~french
		 *\brief		Récupère les pixels de l'image de la texture
		 *\return		Les pixels
		 */
		Castor::PxBufferBaseSPtr GetImagePixels();
		/**
		 *\~english
		 *\brief		Retrieves the texture image size (in bytes)
		 *\return		The size
		 *\~french
		 *\brief		Récupère la taille de l'image de la texture (en octets)
		 *\return		La taille
		 */
		uint32_t GetImageSize()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture image width
		 *\return		The width
		 *\~french
		 *\brief		Récupère la largeur de l'image de la texture
		 *\return		La largeur
		 */
		uint32_t GetWidth()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture image height
		 *\return		The height
		 *\~french
		 *\brief		Récupère la hauteur de l'image de la texture
		 *\return		La hauteur
		 */
		uint32_t GetHeight()const;
		/**
		 *\~english
		 *\brief		Retrieves the texture image pixel format
		 *\return		The pixel format
		 *\~french
		 *\brief		Récupère le format des pixels de l'image de la texture
		 *\return		Le format des pixels
		 */
		Castor::ePIXEL_FORMAT GetPixelFormat()const;
		/**
		 *\~english
		 *\brief		Retrieves the RGB blend parameter
		 *\param[in]	p_eIndex	The parameter index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le paramètre de mélange RGB
		 *\param[in]	p_eIndex	L'index du paramètre
		 *\return		La valeur
		 */
		inline eBLEND_SOURCE GetRgbArgument( eBLEND_SRC_INDEX p_eIndex )const { return m_eRgbArguments[p_eIndex]; }
		/**
		 *\~english
		 *\brief		Sets the RGB blend parameter
		 *\param[in]	p_eIndex	The parameter index
		 *\param[in]	p_eVal		The new value
		 *\~french
		 *\brief		Définit le paramètre de mélange RGB
		 *\param[in]	p_eIndex	L'index du paramètre
		 *\param[in]	p_eVal		La nouvelle valeur
		 */
		inline void SetRgbArgument( eBLEND_SRC_INDEX p_eIndex, eBLEND_SOURCE p_eVal ) { m_eRgbArguments[p_eIndex] = p_eVal; }
		/**
		 *\~english
		 *\brief		Retrieves the Alpha blend parameter
		 *\param[in]	p_eIndex	The parameter index
		 *\return		The value
		 *\~french
		 *\brief		Récupère le paramètre de mélange Alpha
		 *\param[in]	p_eIndex	L'index du paramètre
		 *\return		La valeur
		 */
		inline eBLEND_SOURCE GetAlpArgument( eBLEND_SRC_INDEX p_eIndex )const { return m_eAlpArguments[p_eIndex]; }
		/**
		 *\~english
		 *\brief		Sets the Alpha blend parameter
		 *\param[in]	p_eIndex	The parameter index
		 *\param[in]	p_eVal		The new value
		 *\~french
		 *\brief		Définit le paramètre de mélange Alpha
		 *\param[in]	p_eIndex	L'index du paramètre
		 *\param[in]	p_eVal		La nouvelle valeur
		 */
		inline void SetAlpArgument( eBLEND_SRC_INDEX p_eIndex, eBLEND_SOURCE val ) { m_eAlpArguments[p_eIndex] = val; }
		/**
		 *\~english
		 *\brief		Retrieves the texture initalisation status
		 *\return		\p false if the texture is null or uninitialised
		 *\~french
		 *\brief		Récupère le statut d'initialisation de la texture
		 *\return		\p false si la texture est nulle ou non initialisée
		 */
		bool IsTextureInitialised()const;
		/**
		 *\~english
		 *\brief		Tells if the unit has a texture
		 *\return		\p false if the texture is null
		 *\~french
		 *\brief		Dit si l'unité a une texture
		 *\return		\p false si la texture est nulle
		 */
		inline bool IsTextured()const { return m_pTexture != nullptr; }
		/**
		 *\~english
		 *\brief		Sets the auto mipmaps generation status
		 *\param[in]	p_bVal		The new value
		 *\~french
		 *\brief		Définit le statut d'auto génération des mipmaps
		 *\param[in]	p_bVal		La nouvelle valeur
		 */
		inline void SetAutoMipmaps( bool p_bVal ) { m_bAutoMipmaps = p_bVal; }
		/**
		 *\~english
		 *\brief		Retrieves the auto mipmaps generation status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'auto génération des mipmaps
		 *\return		La valeur
		 */
		inline bool GetAutoMipmaps()const { return m_bAutoMipmaps; }
	};
	/*!
	\author Sylvain DOREMUS
	\version 0.1
	\date 09/02/2010
	\~english
	\brief Texture unit renderer
	\~french
	\brief Renderer d'unité de texture
	*/
	class C3D_API TextureRenderer : public Renderer< TextureUnit, TextureRenderer >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param		p_pRenderSystem	The render system
		 *\~french
		 *\brief		Constructeur
		 *\param		p_pRenderSystem	Le render system
		 */
		TextureRenderer( RenderSystem * p_pRenderSystem );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~TextureRenderer();
		/**
		 *\~english
		 *\brief		Applies multitexturing parameters
		 *\return		\p true if ok
		 *\~french
		 *\brief		Applique les paramètres de multitexturing
		 *\return		\p true si tout s'est bien passé
		 */
		virtual bool Render()=0;
		/**
		 *\~english
		 *\brief		Removes multitexturing parameters
		 *\~french
		 *\brief		Enlève les paramètres de multitexturing
		 */
		virtual void EndRender()=0;
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
	template< typename CharType > std::basic_ostream< CharType > & operator <<( std::basic_ostream< CharType > & p_streamOut, TextureUnitSPtr const & p_texture )
	{
		p_streamOut << p_texture->GetIndex();
		return p_streamOut;
	}
	/**
	 *\~english
	 *\brief			Stream operator
	 *\param[in,out]	p_streamIn	The stream holding texture's data
	 *\param[in,out]	p_texture	The output texture
	 *\return			A reference to the stream
	 *\~french
	 *\brief			Opérateur de flux
	 *\param[in,out]	p_streamIn	Le flux qui contient les données de la texture
	 *\param[in,out]	p_texture	La texture
	 *\return			Une référence sur le flux
	 */
	template< typename CharType > std::basic_istream< CharType > & operator >>( std::basic_istream< CharType > & p_streamIn, TextureUnitSPtr & p_texture )
	{
		uint32_t l_uiIndex = 0;
		p_streamIn >> l_uiIndex;
		p_texture->SetIndex( l_uiIndex );
		return p_streamIn;
	}
}

#pragma warning( pop )

#endif
