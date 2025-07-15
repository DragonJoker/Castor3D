/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureModule_H___
#define ___C3D_TextureModule_H___

#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace c3d
{
	/**@name Material */
	//@{
	/**@name Texture */
	//@{

	using crg::FilterMode;
	using crg::MipmapMode;
	using crg::WrapMode;

	enum class BorderColour
	{
		eFloatTransparentBlack,
		eIntTransparentBlack,
		eFloatOpaqueBlack,
		eIntOpaqueBlack,
		eFloatOpaqueWhite,
		eIntOpaqueWhite,
		CU_ScopedEnumBounds( eFloatTransparentBlack, eIntOpaqueWhite )
	};
	C3D_API String getName( BorderColour v );
	C3D_API VkBorderColor convert( BorderColour v );
	C3D_API BorderColour convert( VkBorderColor v );
	/**
	*\~english
	*\brief
	*	Lists the possible spaces for a texture.
	*\~french
	*\brief
	*	Liste les espaces possibles pour une texture.
	*/
	enum class TextureSpace
		: uint16_t
	{
		eNone = 0x0000,
		//!\~english Modifier: Normalized value (Colour in [0, 1] range, depth in [0, 1] range).
		//!\~french Modificateur: Valeur normalisée (Couleur dans l'intervalle [0, 1], profondeur dans l'intervalle [0, 1]).
		eNormalised = 0x0001 << 0,
		//!\~english Modifier: Y inverted space.
		//!\~french Modificateur: Espace Y inversé.
		eYInverted = 0x0001 << 1,
		//!\~english Colour texture.
		//!\~french Texture couleur.
		eColour = 0x0001 << 2,
		//!\~english Depth in [near, far] range.
		//!\~french Profondeur dans l'intervalle [near, far].
		eDepth = 0x0001 << 3,
		//!\~english Tangent space data.
		//!\~french Données en espace tangent.
		eTangentSpace = 0x0001 << 4,
		//!\~english Object space data.
		//!\~french Données en espace objet.
		eObjectSpace = 0x0001 << 5,
		//!\~english World space data.
		//!\~french Données en espace monde.
		eWorldSpace = 0x0001 << 6,
		//!\~english View space data.
		//!\~french Données en espace vue.
		eViewSpace = 0x0001 << 7,
		//!\~english Clip space data.
		//!\~french Données en espace de clipping.
		eClipSpace = 0x0001 << 8,
		//!\~english Stencil data.
		//!\~french Données de stencil.
		eStencil = 0x0001 << 9,
		//!\~english Allows SRGB formats.
		//!\~french Autorise les formats SRGB.
		eAllowSRGB = 0x0001 << 10,
	};
	CU_ImplementFlags( TextureSpace )
	C3D_API String getName( TextureSpace value );
	C3D_API String getName( TextureSpaces value );
	/**
	*\~english
	*\brief
	*	Cube map faces.
	*\~french
	*\brief
	*	Les faces d'une cube map.
	*/
	enum class CubeMapFace
		: uint8_t
	{
		//!\~english Face on positive X
		//!\~french Face des X positifs.
		ePositiveX = 0,
		//!\~english Face on negative X
		//!\~french Face des X négatifs.
		eNegativeX = 1,
		//!\~english Face on positive Y
		//!\~french Face des Y positifs.
		ePositiveY = 2,
		//!\~english Face on negative Y
		//!\~french Face des Y négatifs.
		eNegativeY = 3,
		//!\~english Face on positive Z
		//!\~french Face des Z positifs.
		ePositiveZ = 4,
		//!\~english Face on negative Z
		//!\~french Face des Z négatifs.
		eNegativeZ = 5,
		CU_ScopedEnumBounds( ePositiveX, eNegativeZ )
	};
	C3D_API String getName( CubeMapFace value );
	/**
	*\~english
	*\brief
	*	Defines a sampler for a texture
	*\~french
	*\brief
	*	Définit un sampler pour une texture
	*/
	class Sampler;
	/**
	*\~english
	*\brief
	*	Specifies the usages of a texture, per image component.
	*\~french
	*\brief
	*	Définit les utilisations d'une texture, par composante d'image.
	*/
	struct TextureConfiguration;
	/**
	*\~english
	*\brief
	*	The configuration for one texture flag.
	*\~french
	*\brief
	*	La configuration pour un indicateur de texture.
	*/
	struct TextureFlagConfiguration;
	/**
	*\~english
	*\brief
	*	Specifies the transformations for a texture.
	*\~french
	*\brief
	*	Définit les transformations pour une texture.
	*/
	struct TextureTransform;
	/**
	*\~english
	*\brief
	*	The configuration of a texture in a pass.
	*\~french
	*\brief
	*	La configuration d'une texture de passe.
	*/
	struct PassTextureConfig;
	/**
	*\~english
	*\brief
	*	Texture base class
	*\~french
	*\brief
	*	Class de base d'une texture
	*/
	class TextureLayout;
	/**
	*\~english
	*\brief
	*	Texture source base data.
	*\~french
	*\brief
	*	Données de base d'une source de texture.
	*/
	class TextureSourceInfo;
	/**
	*\~english
	*\brief
	*	Hashes a TextureSourceInfo.
	*\~french
	*\brief
	*	Hashe une TextureSourceInfo.
	*/
	struct TextureSourceInfoHasher;
	/**
	*\~english
	*\brief
	*	Texture unit representation.
	*\remarks
	*	A texture unit is a texture with few options, like channel, blend modes, transformations...
	*\~french
	*\brief
	*	Représentation d'une unité de texture.
	*\remarks
	*	Une unité de texture se compose d'une texture avec des options telles que son canal, modes de mélange, transformations...
	*/
	class TextureUnit;
	/**
	*\~english
	*\brief
	*	Data for a texture.
	*\~french
	*\brief
	*	Données pour une texture.
	*/
	struct TextureData;
	/**
	*\~english
	*\brief
	*	Data for a texture unit.
	*\~french
	*\brief
	*	Données pour une unité de texture.
	*/
	struct TextureUnitData;
	/**
	*\~english
	*\brief
	*	Texture image source.
	*\~french
	*\brief
	*	Classe de source d'image de texture.
	*/
	class TextureSource;
	/**
	*\~english
	*\brief
	*	Texture image.
	*\remarks
	*	Holds the GPU texture storage.
	*\~french
	*\brief
	*	Classe d'image de texture.
	*\remarks
	*	Contient le stockage de la texture au niveau GPU.
	*/
	class TextureView;

	using OnTextureUnitChangedFunction = Function< void( TextureUnit const & ) >;
	using OnTextureUnitChanged = SignalT< OnTextureUnitChangedFunction >;
	using OnTextureUnitChangedConnection = OnTextureUnitChanged::connection;

	// ARGB => 4 components => 4 channels per configuration.
	using TextureFlagConfigurations = Array< TextureFlagConfiguration, 4u >;

	OutputStream & operator<<( OutputStream & stream, ImageMemoryLayout const & rhs );
	OutputStream & operator<<( OutputStream & stream, Image const & rhs );
	OutputStream & operator<<( OutputStream & stream, TextureLayout const & layout );
	/**
	*\~english
	*	Helper structure to specialise a cache behaviour.
	*\remarks
	*	Specialisation for Sampler.
	*\~french
	*	Structure permettant de spécialiser le comportement d'un cache.
	*\remarks
	*	Spécialisation pour Sampler.
	*/
	template<>
	struct PtrCacheTraitsT< Sampler, String >
		: PtrCacheTraitsBaseT< Sampler, String >
	{
		using ResT = Sampler;
		using KeyT = String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const String Name;
	};

	using SamplerCacheTraits = PtrCacheTraitsT< Sampler, String >;
	using SamplerCache = ResourceCacheT< Sampler
		, String
		, SamplerCacheTraits >;

	using SamplerPtr = SamplerCacheTraits::ElementPtrT;
	using SamplerObs = SamplerCacheTraits::ElementObsT;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Sampler, C3D_API );
	CU_DeclareSmartPtr( c3d, SamplerCache, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureData, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureLayout, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureSource, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureSourceInfo, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureUnit, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureUnitData, C3D_API );
	CU_DeclareSmartPtr( c3d, TextureView, C3D_API );

	//! TextureUnit array
	CU_DeclareVector( TextureUnit, TextureUnit );
	/** @endcond */

	using TextureUnitDatas = Vector< TextureUnitDataUPtr >;
	using TextureUnitDataRefs = Vector< TextureUnitDataRPtr >;
	using TextureUnitDataSet = Map< PassComponentTextureFlag, TextureUnitDataRPtr >;

	struct TextureSourceInfoHasher
	{
		C3D_API size_t operator()( TextureSourceInfo const & value )const noexcept;
	};

	C3D_API bool operator==( TextureSourceInfo const & lhs
		, TextureSourceInfo const & rhs )noexcept;
	inline bool operator!=( TextureSourceInfo const & lhs
		, TextureSourceInfo const & rhs )noexcept
	{
		return !( lhs == rhs );
	}

	struct PassTextureConfigHasher
	{
		C3D_API size_t operator()( PassTextureConfig const & value )const noexcept;
	};

	C3D_API bool operator==( PassTextureConfig const & lhs
		, PassTextureConfig const & rhs )noexcept;
	inline bool operator!=( PassTextureConfig const & lhs
		, PassTextureConfig const & rhs )noexcept
	{
		return !( lhs == rhs );
	}

	C3D_API Image & getBufferImage( Engine & engine
		, String const & name
		, String const & type
		, ByteArray const & data );
	C3D_API Image & getFileImage( Engine & engine
		, String const & name
		, Path const & folder
		, Path const & relative );
	C3D_API TextureLayoutUPtr createTextureLayout( Engine const & engine
		, Path const & relative
		, Path const & folder );
	C3D_API TextureLayoutUPtr createTextureLayout( Engine const & engine
		, String const & name
		, PxBufferBaseUPtr buffer
		, bool isStatic = false );
	C3D_API uint32_t getMipLevels( Extent3D const & extent
		, PixelFormat format );

	//@}
	//@}
}

#endif
