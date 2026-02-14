/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderToneMappingModule_H___
#define ___C3D_RenderToneMappingModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

#include <RenderGraph/FrameGraphPrerequisites.hpp>

namespace c3d
{
	/**@name Render */
	//@{
	/**@name ToneMapping */
	//@{

	/**
	*\~english
	*\brief
	*	HDR configuration (expsure and gamma correction).
	*\~french
	*\brief
	*	Configuration HDR (exposition et correction gamma).
	*/
	struct HdrConfig;
	/**
	*\~english
	*\brief
	*	Tone mapping wrapper.
	*\~french
	*\brief
	*	Classe de tone mapping.
	*/
	class ToneMapping;
	/**
	*\~english
	*\brief
	*	Base class for tone mapping implementation.
	*\remarks
	*	The inheriting classes will have to write their fragment shader.
	*	This one will have the uniform buffer slot 0 reserved for
	*	a provided RenderUbo (whether it uses it or not).
	*\~french
	*\brief
	*	Classe de base pour les implémentations de tone mapping.
	*\remarks
	*	Les classes héritières devront écrire leur fragment shader.
	*	Celui-ci devra réserver le slot 0 d'uniform buffer pour
	*	un RenderUbo, qu'il l'utilise ou pas.
	*/
	class ToneMappingImpl;

	/** @cond !Doxygen */
	CU_DeclareSmartPtr( c3d, ToneMapping, C3D_API );
	CU_DeclareSmartPtr( c3d, ToneMappingImpl, C3D_API );
	/** @endcond */
	/**
	*\~english
	*\brief
	*	Tone mapping factory.
	*\~french
	*\brief
	*	Fabrique de mappage de tons.
	*/
	using ToneMappingFactory = Factory< ToneMapping
		, String
		, ToneMappingImplUPtr
		, Function< ToneMappingImplUPtr( c3d::ToneMapping const &, c3d::RenderDevice const &, Parameters ) > >;

	//@}
	//@}
}

#endif
