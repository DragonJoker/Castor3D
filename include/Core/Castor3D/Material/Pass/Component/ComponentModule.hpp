/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialPassComponentModule_H___
#define ___C3D_MaterialPassComponentModule_H___

#include "Castor3D/Limits.hpp"
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

#define C3D_PluginMakePassComponentName( p, x ) C3D_Join3Strings( p, "pass", x )
#define C3D_MakePassComponentName( x ) C3D_PluginMakePassComponentName( "c3d", x )

#define C3D_PluginMakePassBaseComponentName( p, x ) C3D_Join4Strings( p, "pass", "base", x )
#define C3D_MakePassBaseComponentName( x ) C3D_PluginMakePassBaseComponentName( "c3d", x )

#define C3D_PluginMakePassLightingComponentName( p, x ) C3D_Join4Strings( p, "pass", "lighting", x )
#define C3D_MakePassLightingComponentName( x ) C3D_PluginMakePassLightingComponentName( "c3d", x )

#define C3D_PluginMakePassMapComponentName( p, x ) C3D_Join4Strings( p, "pass", "map", x )
#define C3D_MakePassMapComponentName( x ) C3D_PluginMakePassMapComponentName( "c3d", x )

#define C3D_PluginMakePassOtherComponentName( p, x ) C3D_Join4Strings( p, "pass", "other", x )
#define C3D_MakePassOtherComponentName( x ) C3D_PluginMakePassOtherComponentName( "c3d", x )

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
		CU_ScopedEnumBounds( eNone ),
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
		eAll = ( ( 0x0001u << uint16_t( ComponentMode::eCount ) ) - 1u ),
	};
	CU_ImplementFlags( ComponentModeFlag )
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
	\brief		Used to create a component type's shaders and data.
	\~french
	\brief		Utilisé pour créer les shaders et données d'un type de composant.
	*/
	class PassComponentPlugin;
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
		/**
		\~english
		\brief		Base class for all component shaders.
		\~french
		\brief		Classe de base des shaders de composant.
		*/
		struct PassShader;
		/**
		\~english
		\brief		Shaders for components that go through lighting (\see shader::BlendComponents).
		\remarks	Those shaders are enabled if the component is enabled.
		\~french
		\brief		Shaders pour les composants allant jusqu'à l'éclairage (\see shader::BlendComponents).
		\remarks	Ces shaders sont actifs si le composant est actif.
		*/
		struct PassComponentsShader;
		/**
		\~english
		\brief		Shaders for components that need registration to castor3d::PassBuffer.
		\remarks	Those shaders are enabled whether the component is enabled or not (for consistency with the PassBuffer).
		\~french
		\brief		Shaders pour les composants ayant un enregistrement dans castor3d::PassBuffer.
		\remarks	Ces shaders sont actifs que le composant soit actif ou pas (par soucis de cohérence avec le PassBuffer).
		*/
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

	using PassComponentIDSet = std::set< PassComponentID >;

	CU_DeclareSmartPtr( PassComponent );
	CU_DeclareCUSmartPtr( castor3d, PassComponentRegister, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PassComponentPlugin, C3D_API );

	CU_DeclareMap( PassComponentID, PassComponentUPtr, PassComponent );

	C3D_API castor::String const & getPassComponentType( PassComponent const & component );

	using SssProfileChangedFunc = std::function< void( SubsurfaceScatteringComponent const & ) >;
	using OnSssProfileChanged = castor::SignalT< SssProfileChangedFunc >;
	using OnSssProfileChangedConnection = castor::ConnectionT< OnSssProfileChanged >;

	using PassComponentsBitset = castor::DynamicBitsetT< uint16_t >;

	using ComponentConfigFiller = std::function< void( SceneFileContext & parsingContext ) >;
	using ChannelFiller = std::pair< PassComponentTextureFlag, ComponentConfigFiller >;
	using ChannelFillers = std::map< castor::String, ChannelFiller >;

	using UpdateComponent = std::function< void( PassComponentRegister const & passComponents
		, TextureCombine const & combine
		, shader::BlendComponents & components ) >;

	using CreatePassComponentPlugin = std::function< PassComponentPluginUPtr() >;
	//@}
	//@}
	//@}
}

#endif
