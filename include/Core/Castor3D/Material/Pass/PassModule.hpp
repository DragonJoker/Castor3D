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

namespace c3d::shader
{
	class BufferBase;
	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d::shader, BufferBase, C3D_API );
	/** @endcond */
}

namespace c3d
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
		eNone = 0,
		eOne = 1,
		eRepeat = 2,
		CU_ScopedEnumBounds( eNone, eRepeat )
	};
	C3D_API String getName( ParallaxOcclusionMode value );
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
		eNoBlend = 0,
		//!\~english Order independent, add the components.
		//!\~french Mélange indépendant de l'ordre, additionnant les composantes.
		eAdditive = 1,
		//!\~english Order independent, multiply the components.
		//!\~french Indépendant de l'ordre, multipliant les composantes.
		eMultiplicative = 2,
		//!\~english Order dependent, interpolate the components.
		//!\~french Indépendant de l'ordre, interpolant les composantes.
		eInterpolative = 3,
		//!\~english Order independent, using A-buffer, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant les A-Buffer (non implémenté).
		eABuffer = 4,
		//!\~english Order independent, using depth peeling, not implemented yet.
		//!\~french Indépendant de l'ordre, utilisant le pelage en profondeur (non implémenté).
		eDepthPeeling = 5,
		CU_ScopedEnumBounds( eNoBlend, eDepthPeeling )
	};
	C3D_API String getName( BlendMode value );
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

	using OnPassChangedFunction = Function< void( Pass const & pass
		, PassComponentCombineID oldComponents
		, PassComponentCombineID newComponents ) >;
	using OnPassChanged = SignalT< OnPassChangedFunction >;
	using OnPassChangedConnection = OnPassChanged::connection;

	struct RenderPassRegisterInfo;
	struct PassRegisterInfo;

	struct SpecificsBuffer
	{
		using ShaderBufferCreator = Function< ShaderBufferUPtr( RenderDevice const & ) >;
		using ShaderBufferUpdater = Function< void( ShaderBuffer &
			, Pass const & ) >;
		using ShaderBufferDeclarator = Function< shader::BufferBaseUPtr( sdw::ShaderWriter & writer
			, uint32_t binding
			, uint32_t set ) >;

		ShaderBufferCreator create;
		ShaderBufferUpdater update;
		ShaderBufferDeclarator declare;
	};

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, Pass, C3D_API );
	CU_DeclareSmartPtr( c3d, PassComponent, C3D_API );
	CU_DeclareSmartPtr( c3d, PassFactory, C3D_API );
	CU_DeclareSmartPtr( c3d, RenderPassRegisterInfo, C3D_API );
	CU_DeclareSmartPtr( c3d, SubsurfaceScattering, C3D_API );

	CU_DeclareMap( PassComponentID, PassComponentUPtr, PassComponent );
	//! Pass array
	CU_DeclareVector( Pass, Pass );
	//! Pass pointer array
	CU_DeclareVector( PassUPtr, PassPtr );
	/** @endcond */

	using PassCreator = Function< PassUPtr( LightingModelID, Material & ) >;

	struct PassFactoryEntry
	{
		String key;
		PassCreator create;
		PassTypeID id;
	};

	using PassFactoryBase = Factory< Pass
		, String
		, PassUPtr
		, PassCreator
		, PassTypeID
		, PassFactoryEntry >;
	using SpecificsBuffers = StringMap< Pair< SpecificsBuffer, ShaderBufferUPtr > >;

	C3D_API PassRPtr getComponentPass( PassComponent const & component );
	C3D_API String const & getPassComponentType( PassComponent const & component );
	C3D_API Engine * getEngine( Pass const & pass );
	//@}
	//@}
}

#endif
