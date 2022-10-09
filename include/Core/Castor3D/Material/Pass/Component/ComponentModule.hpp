/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialPassComponentModule_H___
#define ___C3D_MaterialPassComponentModule_H___

#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Config/Macros.hpp>
#include <CastorUtils/Config/SmartPtr.hpp>
#include <CastorUtils/Design/DesignModule.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

#define C3D_Stringify( x ) C3DX_Stringify(x)
#define C3DX_Stringify( x ) #x
#define C3D_Join2Strings( l, r ) l "." r
#define C3D_Join3Strings( l, m, r ) C3D_Join2Strings(l, C3D_Join2Strings(m, r))
#define C3D_Join4Strings( l, ml, mr, r ) C3D_Join3Strings(l, ml, C3D_Join2Strings(mr, r))

#define C3D_PluginMakePassComponentName( p, x ) C3D_Join3Strings(p, "pass", x)
#define C3D_MakePassComponentName( x ) C3D_PluginMakePassComponentName( "c3d", x )

#define C3D_PluginMakePassMapComponentName( p, x ) C3D_Join4Strings(p, "map", "pass", x)
#define C3D_MakePassMapComponentName( x ) C3D_PluginMakePassMapComponentName( "c3d", x )

namespace castor3d
{
	class SceneFileContext;
	/**@name Shader */
	//@{
	/**@name Shaders */
	//@{
	namespace shader
	{
		struct BlendComponents;
	}
	//@}
	/**@name Buffers */
	//@{
	class PassBuffer;
	//@}
	//@}
	/**@name Material */
	//@{
	/**@name Texture */
	//@{
	struct TextureConfiguration;
	//@}
	/**@name Pass */
	//@{
	class Pass;
	/**@name Component */
	//@{
	enum class ComponentMode: uint16_t
	{
		eNone,
		eOpacity,
		eGeometry,
		eColour,
		eDiffuseLighting,
		eSpecularLighting,
		eSpecifics,
		eDerivTex,
		eOcclusion,
	};
	enum class ComponentModeFlag : uint16_t
	{
		eNone = 0x0000u,
		eOpacity = 0x0001u << uint16_t( ComponentMode::eOpacity ),
		eGeometry = 0x0001u << uint16_t( ComponentMode::eGeometry ),
		eColour = 0x0001u << uint16_t( ComponentMode::eColour ),
		eDiffuseLighting = 0x0001u << uint16_t( ComponentMode::eDiffuseLighting ),
		eSpecularLighting = 0x0001u << uint16_t( ComponentMode::eSpecularLighting ),
		eSpecifics = 0x0001u << uint16_t( ComponentMode::eSpecifics ),
		eDerivTex = 0x0001u << uint16_t( ComponentMode::eDerivTex ),
		eOcclusion = 0x0001u << uint16_t( ComponentMode::eOcclusion ),
	};
	CU_ImplementFlags( ComponentModeFlag );
	/**
	\~english
	\brief		Pass component holding base pass data.
	\~french
	\brief		Composant de passe détenant des données basiques d'une passe.
	*/
	template< typename DataT >
	struct BaseDataPassComponentT;
	/**
	\~english
	\brief		Pass component base class.
	\~french
	\brief		Classe de base d'un composant de passe.
	*/
	struct PassComponent;
	/**
	\~english
	\brief		Pass texture component base class.
	\~french
	\brief		Classe de base d'un composant de texture de passe.
	*/
	struct PassMaskComponent;
	/**
	\~english
	\brief		Pass components registrar.
	\~french
	\brief		Registre des composants de passe.
	*/
	class PassComponentRegister;
	/**@name Shader */
	//@{
	namespace shader
	{
		struct PassShader;
		struct PassComponentsShader;
		struct PassMaterialShader;
		/**
		\~english
		\brief		Holds the shaders for all the components of a pass.
		\~french
		\brief		Contient tous les shaders pour tous les composants d'une passe.
		*/
		class PassShaders;

		using PassComponentsShaderPtr = std::unique_ptr< PassComponentsShader >;
		using PassMaterialShaderPtr = std::unique_ptr< PassMaterialShader >;
	}
	//@}

	struct SubsurfaceScatteringComponent;

	CU_DeclareSmartPtr( PassComponent );
	CU_DeclareCUSmartPtr( castor3d, PassComponentRegister, C3D_API );

	CU_DeclareMap( uint32_t, PassComponentUPtr, PassComponent );

	C3D_API castor::String const & getPassComponentType( PassComponent const & component );

	using SssProfileChangedFunc = std::function< void( SubsurfaceScatteringComponent const & ) >;
	using OnSssProfileChanged = castor::SignalT< SssProfileChangedFunc >;
	using OnSssProfileChangedConnection = castor::ConnectionT< OnSssProfileChanged >;

	using PassComponentsBitset = castor::DynamicBitsetT< uint16_t >;

	using ComponentConfigFiller = std::function< void( SceneFileContext & parsingContext ) >;
	using ChannelFiller = std::pair< uint32_t, ComponentConfigFiller >;
	using ChannelFillers = std::map< castor::String, ChannelFiller >;

	using ParsersFiller = std::function< void( castor::AttributeParsers & parsers
		, ChannelFillers & channelFillers ) >;
	using SectionsFiller = std::function< void( castor::StrUInt32Map & sections ) >;
	using ZeroBuffer = std::function< void( Pass const &
		, shader::PassMaterialShader const &
		, PassBuffer & ) >;
	using FillRemapMask = std::function< void( uint32_t maskValue
		, TextureConfiguration & configuration ) >;
	using WriteTextureConfig = std::function< bool( TextureConfiguration const & configuration
		, castor::String const & tabs
		, castor::StringStream & file ) >;
	using NeedsMapComponent = std::function< bool( TextureConfiguration const & configuration ) >;
	using IsComponentNeeded = std::function< bool( TextureFlags const &, ComponentModeFlags const & ) >;
	using CreateMapComponent = void( * )( Pass & pass
		, std::vector< PassComponentUPtr > & result );
	using CreateComponentsShader = std::function< shader::PassComponentsShaderPtr() >;
	using CreateMaterialShader = std::function< shader::PassMaterialShaderPtr() >;
	using UpdateComponent = std::function< void( TextureFlags const & texturesFlags
		, shader::BlendComponents const & components ) >;

	struct ComponentData
	{
		ZeroBuffer zeroBuffer{};
		FillRemapMask fillRemapMask{};
		WriteTextureConfig writeTextureConfig{};
		NeedsMapComponent needsMapComponent{};
		CreateMapComponent createMapComponent{};
		IsComponentNeeded isComponentNeeded{};
		CreateComponentsShader createComponentsShader{};
		UpdateComponent updateComponent{};
	};
	//@}
	//@}
	//@}
}

#endif
