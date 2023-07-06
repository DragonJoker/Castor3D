/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureModule_H___
#define ___C3D_TextureModule_H___

#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Material */
	//@{
	/**@name Texture */
	//@{

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
	C3D_API castor::String getName( TextureSpace value );
	C3D_API castor::String getName( TextureSpaces value );
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
		ePositiveX,
		//!\~english Face on negative X
		//!\~french Face des X négatifs.
		eNegativeX,
		//!\~english Face on positive Y
		//!\~french Face des Y positifs.
		ePositiveY,
		//!\~english Face on negative Y
		//!\~french Face des Y négatifs.
		eNegativeY,
		//!\~english Face on positive Z
		//!\~french Face des Z positifs.
		ePositiveZ,
		//!\~english Face on negative Z
		//!\~french Face des Z négatifs.
		eNegativeZ,
		CU_ScopedEnumBounds( ePositiveX )
	};
	C3D_API castor::String getName( CubeMapFace value );
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

	CU_DeclareSmartPtr( castor3d, Sampler, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureData, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureLayout, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureSource, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureUnit, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureView, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureUnitData, C3D_API );

	//! TextureUnit array
	CU_DeclareVector( TextureUnit, TextureUnit );
	// ARGB => 4 components => 4 channels per configuration.
	using TextureFlagConfigurations = std::array< TextureFlagConfiguration, 4u >;

	using TextureUnitDatas = std::vector< TextureUnitDataUPtr >;
	using TextureUnitDataRefs = std::vector< TextureUnitDataRPtr >;
	using TextureUnitDataSet = std::map< PassComponentTextureFlag, TextureUnitDataRPtr >;

	using OnTextureUnitChangedFunction = std::function< void( TextureUnit const & ) >;
	using OnTextureUnitChanged = castor::SignalT< OnTextureUnitChangedFunction >;
	using OnTextureUnitChangedConnection = OnTextureUnitChanged::connection;

	std::ostream & operator<<( std::ostream & stream, castor::ImageLayout const & rhs );
	std::ostream & operator<<( std::ostream & stream, castor::Image const & rhs );
	std::ostream & operator<<( std::ostream & stream, TextureLayout const & layout );
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
	struct PtrCacheTraitsT< Sampler, castor::String >
		: PtrCacheTraitsBaseT< Sampler, castor::String >
	{
		using ResT = Sampler;
		using KeyT = castor::String;
		using Base = PtrCacheTraitsBaseT< ResT, KeyT >;
		using ElementT = typename Base::ElementT;
		using ElementPtrT = typename Base::ElementPtrT;

		C3D_API static const castor::String Name;
	};

	using SamplerCacheTraits = PtrCacheTraitsT< Sampler, castor::String >;
	using SamplerCache = castor::ResourceCacheT< Sampler
		, castor::String
		, SamplerCacheTraits >;

	using SamplerPtr = SamplerCacheTraits::ElementPtrT;
	using SamplerObs = SamplerCacheTraits::ElementObsT;

	CU_DeclareSmartPtr( castor3d, SamplerCache, C3D_API );

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

	C3D_API castor::Image & getBufferImage( Engine & engine
		, castor::String const & name
		, castor::String const & type
		, castor::ByteArray const & data );
	C3D_API castor::Image & getFileImage( Engine & engine
		, castor::String const & name
		, castor::Path const & folder
		, castor::Path const & relative );
	C3D_API TextureLayoutUPtr createTextureLayout( Engine const & engine
		, castor::Path const & relative
		, castor::Path const & folder
		, castor::ImageLoaderConfig config = { true, true, true } );
	C3D_API TextureLayoutUPtr createTextureLayout( Engine const & engine
		, castor::String const & name
		, castor::PxBufferBaseUPtr buffer
		, bool isStatic = false );
	C3D_API uint32_t getMipLevels( VkExtent3D const & extent
		, VkFormat format );

	//@}
	//@}
}

#endif
