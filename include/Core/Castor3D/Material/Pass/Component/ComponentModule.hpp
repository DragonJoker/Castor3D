/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialPassComponentModule_H___
#define ___C3D_MaterialPassComponentModule_H___

#include "Castor3D/Limits.hpp"
#include "Castor3D/Castor3DModule.hpp"
#include "Castor3D/Material/MaterialModule.hpp"

#include <CastorUtils/Config/Macros.hpp>
#include <CastorUtils/Design/DesignModule.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>

#include <ShaderWriter/ShaderWriterPrerequisites.hpp>

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
		eAlphaBlending,
		eNormals,
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
		eAlphaBlending = 0x0001u << uint16_t( ComponentMode::eAlphaBlending ),
		eNormals = 0x0001u << uint16_t( ComponentMode::eNormals ),
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
	\brief		Pass components registrar.
	\~french
	\brief		Registre des composants de passe.
	*/
	class PassComponentRegister;

	/**@name Base Components */
	//@{
	/**
	\~english
	\brief		Component holding colour and alpha blend modes.
	\~french
	\brief		Composant détenant les modes de mélange couleur et alpha.
	*/
	struct BlendComponent;
	/**
	\~english
	\brief		Component to enable fractal mapping.
	\~french
	\brief		Composant pour activer le fractal mapping.
	*/
	struct FractalMappingComponent;
	/**
	\~english
	\brief		Component holding the normal.
	\~french
	\brief		Composant détenant la normale.
	*/
	struct NormalComponent;
	/**
	\~english
	\brief		Component holding  base pass data.
	\~french
	\brief		Composant détenant les données de base d'une passe.
	*/
	struct PassHeaderComponent;
	/**
	\~english
	\brief		Component to enable picking.
	\~french
	\brief		Composant pour activer le picking.
	*/
	struct PickableComponent;
	/**
	\~english
	\brief		Component holding textures count.
	\~french
	\brief		Composant détenant le nombre de textures.
	*/
	struct TextureCountComponent;
	/**
	\~english
	\brief		Component holding textures.
	\~french
	\brief		Composant détenant les textures.
	*/
	struct TexturesComponent;
	/**
	\~english
	\brief		Component to enable two-sided pass.
	\~french
	\brief		Composant pour activer le fait que la passe est two-sided.
	*/
	struct TwoSidedComponent;
	/**
	\~english
	\brief		Component to enable untiled mapping.
	\~french
	\brief		Composant pour activer l'untiled mapping.
	*/
	struct UntileMappingComponent;

	CU_DeclareSmartPtr( castor3d, BlendComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, FractalMappingComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, NormalComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassHeaderComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, PickableComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TextureCountComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TexturesComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TwoSidedComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, UntileMappingComponent, C3D_API );
	//@}
	/**@name Lighting Components */
	//@{
	/**
	\~english
	\brief		Component holding ambient lighting data.
	\~french
	\brief		Composant détenant les données d'éclairage ambiant.
	*/
	struct AmbientComponent;
	/**
	\~english
	\brief		Component holding attenuation data.
	\~french
	\brief		Composant détenant les données d'atténuation.
	*/
	struct AttenuationComponent;
	/**
	\~english
	\brief		Component holding clearcoat data.
	\~french
	\brief		Composant détenant les données de clearcoat.
	*/
	struct ClearcoatComponent;
	/**
	\~english
	\brief		Component holding emissive data.
	\~french
	\brief		Composant détenant les données d'émission.
	*/
	struct EmissiveComponent;
	/**
	\~english
	\brief		Component holding iridescence data.
	\~french
	\brief		Composant détenant les données d'iridescence.
	*/
	struct IridescenceComponent;
	/**
	\~english
	\brief		Component holding lighting model data.
	\~french
	\brief		Composant détenant les données de modèle d'éclairage.
	*/
	struct LightingModelComponent;
	/**
	\~english
	\brief		Component holding metalness data.
	\~french
	\brief		Composant détenant les données de metalness.
	*/
	struct MetalnessComponent;
	/**
	\~english
	\brief		Component holding roughness data.
	\~french
	\brief		Composant détenant les données de roughness.
	*/
	struct RoughnessComponent;
	/**
	\~english
	\brief		Component holding sheen data.
	\~french
	\brief		Composant détenant les données de sheen.
	*/
	struct SheenComponent;
	/**
	\~english
	\brief		Component holding specular data.
	\~french
	\brief		Composant détenant les données de spéculaire.
	*/
	struct SpecularComponent;
	/**
	\~english
	\brief		Component holding subsurface scattering data.
	\~french
	\brief		Composant détenant les données de subsurface scattering.
	*/
	struct SubsurfaceScatteringComponent;
	/**
	\~english
	\brief		Component holding thickness data.
	\~french
	\brief		Composant détenant les données d'épaisseur.
	*/
	struct ThicknessComponent;
	/**
	\~english
	\brief		Component holding transmission data.
	\~french
	\brief		Composant détenant les données de transmission.
	*/
	struct TransmissionComponent;

	CU_DeclareSmartPtr( castor3d, AmbientComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, AttenuationComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ClearcoatComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, EmissiveComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, IridescenceComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, LightingModelComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, MetalnessComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, RoughnessComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SheenComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SpecularComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubsurfaceScatteringComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ThicknessComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TransmissionComponent, C3D_API );
	//@}
	/**@name Texture Components */
	//@{
	/**
	\~english
	\brief		Component holding attenuation map.
	\~french
	\brief		Composant détenant la texture d'atténuation.
	*/
	struct AttenuationMapComponent;
	/**
	\~english
	\brief		Component holding clearcoat map.
	\~french
	\brief		Composant détenant la texture de clearcoat.
	*/
	struct ClearcoatMapComponent;
	/**
	\~english
	\brief		Component holding clearcoat's normal map.
	\~french
	\brief		Composant détenant la texture de normales pour le clearcoat.
	*/
	struct ClearcoatNormalMapComponent;
	/**
	\~english
	\brief		Component holding clearcoat's roughness map.
	\~french
	\brief		Composant détenant la texture de roughness pour le clearcoat.
	*/
	struct ClearcoatRoughnessMapComponent;
	/**
	\~english
	\brief		Component holding the colour map.
	\~french
	\brief		Composant détenant la texture de couleurs.
	*/
	struct ColourMapComponent;
	/**
	\~english
	\brief		Component holding the emissive map.
	\~french
	\brief		Composant détenant la texture d'e couleurs'émissive.
	*/
	struct EmissiveMapComponent;
	/**
	\~english
	\brief		Component holding the glossiness map.
	\~french
	\brief		Composant détenant la texture de glossiness.
	*/
	struct GlossinessMapComponent;
	/**
	\~english
	\brief		Component holding the height map.
	\~french
	\brief		Composant détenant la texture de hauteurs.
	*/
	struct HeightMapComponent;
	/**
	\~english
	\brief		Component holding the iridescence map.
	\~french
	\brief		Composant détenant la texture d'iridescence.
	*/
	struct IridescenceMapComponent;
	/**
	\~english
	\brief		Component holding the iridescent layer thickness map.
	\~french
	\brief		Composant détenant la texture d'épaisseur de la couche iridescente.
	*/
	struct IridescenceThicknessMapComponent;
	/**
	\~english
	\brief		Component holding the metalness map.
	\~french
	\brief		Composant détenant la texture de metalness.
	*/
	struct MetalnessMapComponent;
	/**
	\~english
	\brief		Component holding the normal map.
	\~french
	\brief		Composant détenant la texture de normales.
	*/
	struct NormalMapComponent;
	/**
	\~english
	\brief		Component holding the occlusion map.
	\~french
	\brief		Composant détenant la texture d'occlusion.
	*/
	struct OcclusionMapComponent;
	/**
	\~english
	\brief		Component holding the opacity map.
	\~french
	\brief		Composant détenant la texture d'opacité.
	*/
	struct OpacityMapComponent;
	/**
	\~english
	\brief		Component holding the roughness map.
	\~french
	\brief		Composant détenant la texture de roughness.
	*/
	struct RoughnessMapComponent;
	/**
	\~english
	\brief		Component holding the sheen map.
	\~french
	\brief		Composant détenant la texture de sheen.
	*/
	struct SheenMapComponent;
	/**
	\~english
	\brief		Component holding the sheen's roughness map.
	\~french
	\brief		Composant détenant la texture de roughness pour le sheen.
	*/
	struct SheenRoughnessMapComponent;
	/**
	\~english
	\brief		Component holding the specular map.
	\~french
	\brief		Composant détenant la texture de spéculaire.
	*/
	struct SpecularMapComponent;
	/**
	\~english
	\brief		Component holding the transmission map.
	\~french
	\brief		Composant détenant la texture de transmission.
	*/
	struct TransmissionMapComponent;
	/**
	\~english
	\brief		Component holding the transmittance map (for subsurface scattering).
	\~french
	\brief		Composant détenant la texture de transmittance (pour le subsurface scattering).
	*/
	struct TransmittanceMapComponent;

	CU_DeclareSmartPtr( castor3d, AttenuationMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ClearcoatMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ClearcoatNormalMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ClearcoatRoughnessMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ColourMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, EmissiveMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, GlossinessMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, HeightMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, IridescenceMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, IridescenceThicknessMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, MetalnessMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, NormalMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, OcclusionMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, OpacityMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, RoughnessMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SheenMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SheenRoughnessMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, SpecularMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TransmissionMapComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, TransmittanceMapComponent, C3D_API );
	//@}
	/**@name Other components */
	//@{
	/**
	\~english
	\brief		Component holding alpha test data.
	\~french
	\brief		Composant détenant les données d'alpha test.
	*/
	struct AlphaTestComponent;
	/**
	\~english
	\brief		Component holding colour data.
	\~french
	\brief		Composant détenant les données de couleur.
	*/
	struct ColourComponent;
	/**
	\~english
	\brief		Component holding height data.
	\~french
	\brief		Composant détenant les données de hauteur.
	*/
	struct HeightComponent;
	/**
	\~english
	\brief		Component holding opacity data.
	\~french
	\brief		Composant détenant les données d'opacité.
	*/
	struct OpacityComponent;
	/**
	\~english
	\brief		Component to enable reflections.
	\~french
	\brief		Composant pour activer les réflexions.
	*/
	struct ReflectionComponent;
	/**
	\~english
	\brief		Component holding refraction data.
	\~french
	\brief		Composant détenant les données de réfraction.
	*/
	struct RefractionComponent;

	CU_DeclareSmartPtr( castor3d, AlphaTestComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ColourComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, HeightComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, OpacityComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, ReflectionComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, RefractionComponent, C3D_API );
	//@}
	/**@name Shader */
	//@{
	/**@name Buffers */
	//@{
	class PassBuffer;
	//@}
	namespace shader
	{
		struct BlendComponents;
		struct SurfaceBase;
		class Utils;
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

	struct PassComponentCombine
	{
		PassComponentCombineID baseId{};
		PassComponentFlagsSet flags{};
		// Computed from \p flags
		bool hasTransmissionFlag{};
		bool hasAlphaTestFlag{};
		bool hasAlphaBlendingFlag{};
		bool hasParallaxOcclusionMappingOneFlag{};
		bool hasParallaxOcclusionMappingRepeatFlag{};
	};

	C3D_API bool operator==( PassComponentCombine const & lhs, PassComponentCombine const & rhs );

	C3D_API bool hasAny( PassComponentCombine const & lhs
		, PassComponentFlag rhs );
	C3D_API void remFlags( PassComponentCombine & lhs
		, PassComponentFlag rhs );
	C3D_API void remFlags( PassComponentCombine & lhs
		, PassComponentFlagsSet const & rhs );
	C3D_API void addFlags( PassComponentCombine & lhs
		, PassComponentFlag rhs );
	C3D_API void addFlags( PassComponentCombine & lhs
		, PassComponentFlagsSet const & rhs );
	C3D_API bool contains( PassComponentCombine const & cont
		, PassComponentFlag test );
	C3D_API bool contains( PassComponentCombine const & cont
		, PassComponentCombine const & test );

	CU_DeclareSmartPtr( castor3d, PassComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassComponentRegister, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassComponentPlugin, C3D_API );

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
	using FinishComponent = std::function< void( shader::SurfaceBase const & surface
		, sdw::Vec3 const worldEye
		, shader::Utils & utils
		, shader::BlendComponents & components ) >;

	using CreatePassComponentPlugin = std::function< PassComponentPluginUPtr( PassComponentRegister const & ) >;
	//@}
	//@}
	//@}
}

#endif
