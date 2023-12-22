/*
See LICENSE file in root folder
*/
#ifndef ___C3D_MaterialPassModule_H___
#define ___C3D_MaterialPassModule_H___

#include "Castor3D/Material/MaterialModule.hpp"
#include "Castor3D/Scene/Background/BackgroundModule.hpp"
#include "Castor3D/Scene/Light/LightModule.hpp"
#include "Castor3D/Shader/ShaderModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace sdw
{
	class ShaderWriter;
}

namespace castor3d::shader
{
	class BufferBase;
	CU_DeclareSmartPtr( castor3d::shader, BufferBase, C3D_API );
}

namespace castor3d
{
	struct RenderDevice;

	/**@name Material */
	//@{
	/**@name Pass */
	//@{
	/**
	*\~english
	*\brief
	*	Parallax occlusion modes enumeration.
	*\~french
	*\brief
	*	Enumération des modes de parallax occlusion.
	*/
	enum class ParallaxOcclusionMode
	{
		eNone,
		eOne,
		eRepeat,
		CU_ScopedEnumBounds( eNone )
	};
	C3D_API castor::String getName( ParallaxOcclusionMode value );
	/**
	*\~english
	*\brief
	*	Blending modes enumeration
	*\~french
	*\brief
	*	Enumération des modes de mélange
	*/
	enum class BlendMode
		: uint32_t
	{
		//!\~english Order dependent blending.
		//!\~french Mélange dépendant de l'ordre.
		eNoBlend,
		//!\~english Order independent, add the components.
		//!\~french Mélange indépendant de l'ordre, additionnant les composantes.
		eAdditive,
		//!\~english Order independent, multiply the components.
		//!\~french Indépendant de l'ordre, multipliant les composantes.
		eMultiplicative,
		//!\~english Order dependent, interpolate the components.
		//!\~french Indépendant de l'ordre, interpolant les composantes.
		eInterpolative,
		//!\~english Order independent, using A-buffer, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant les A-Buffer (non implémenté).
		eABuffer,
		//!\~english Order independent, using depth peeling, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant le pelage en profondeur (non implémenté).
		eDepthPeeling,
		CU_ScopedEnumBounds( eNoBlend )
	};
	C3D_API castor::String getName( BlendMode value );
	/**
	*\~english
	*\brief
	*	Base class of a material pass.
	*\~french
	*\brief
	*	Classe de base d'une passe d'un matériau.
	*/
	class Pass;
	/**
	\~english
	\brief		Pass component base class.
	\~french
	\brief		Classe de base d'un composant de passe.
	*/
	struct PassComponent;
	/**
	*\~english
	*\brief
	*	Factory for material passes.
	*\~french
	*\brief
	*	Fabrique de passe de matériau.
	*/
	class PassFactory;
	/**
	*\~english
	*\brief
	*	Extended pass information, related to subsurface scattering.
	*\~french
	*\brief
	*	Informations étendues de la passe, relatives au subsurface scattering.
	*/
	class SubsurfaceScattering;

	CU_DeclareSmartPtr( castor3d, Pass, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassComponent, C3D_API );
	CU_DeclareSmartPtr( castor3d, PassFactory, C3D_API );
	CU_DeclareSmartPtr( castor3d, SubsurfaceScattering, C3D_API );

	//! Pass array
	CU_DeclareVector( Pass, Pass );
	//! Pass pointer array
	CU_DeclareVector( PassUPtr, PassPtr );

	CU_DeclareMap( PassComponentID, PassComponentUPtr, PassComponent );

	using OnPassChangedFunction = std::function< void( Pass const & pass
		, PassComponentCombineID oldComponents
		, PassComponentCombineID newComponents ) >;
	using OnPassChanged = castor::SignalT< OnPassChangedFunction >;
	using OnPassChangedConnection = OnPassChanged::connection;

	struct RenderPassRegisterInfo;
	struct PassRegisterInfo;

	struct SpecificsBuffer
	{
		using ShaderBufferCreator = std::function< ShaderBufferUPtr( RenderDevice const & ) >;
		using ShaderBufferUpdater = std::function< void( ShaderBuffer &
			, Pass const & ) >;
		using ShaderBufferDeclarator = std::function< shader::BufferBaseUPtr( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set ) >;

		ShaderBufferCreator create;
		ShaderBufferUpdater update;
		ShaderBufferDeclarator declare;
	};

	using PassCreator = std::function< PassUPtr( LightingModelID, Material & ) >;

	struct PassFactoryEntry
	{
		LightingModelID key;
		PassCreator create;
		PassTypeID id;
		castor::String name;
		bool hasIBLSupport;
	};

	using PassFactoryBase = castor::Factory< Pass
		, LightingModelID
		, PassUPtr
		, PassCreator
		, PassTypeID
		, PassFactoryEntry >;
	using SpecificsBuffers = std::map< std::string, std::pair< SpecificsBuffer, ShaderBufferUPtr > >;

	CU_DeclareSmartPtr( castor3d, RenderPassRegisterInfo, C3D_API );

	C3D_API PassRPtr getComponentPass( PassComponent & component );
	C3D_API castor::String const & getPassComponentType( PassComponent const & component );
	//@}
	//@}
}

#endif
