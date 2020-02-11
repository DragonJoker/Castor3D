/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderToneMappingModule_H___
#define ___C3D_RenderToneMappingModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
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
	class HdrConfig;
	/**
	*\~english
	*\brief
	*	Base class for ToneMapping plug-ins.
	*\remarks
	*	The inheriting classes will have to write their fragment shader.
	*	This one will have the uniform buffer slot 0 reserved for
	*	a provided HdrConfigUbo (whether it uses it or not).
	*\~french
	*\brief
	*	Classe de base pour les plug-ins de tone mapping.
	*\remarks
	*	Les classes héritières devront écrire leur fragment shader.
	*	Celui-ci devra réserver le slot 0 d'uniform buffer pour
	*	un HdrConfigUbo, qu'il l'utilise ou pas.
	*/
	class ToneMapping;
	/**
	\author 	Sylvain DOREMUS
	\version	0.11.0
	\date		16/05/2018
	\~english
	\brief		Post effect visitor base class.
	\~french
	\brief		Classe de base d'un visiteur d'effet post rendu.
	*/
	class ToneMappingVisitor;

	CU_DeclareSmartPtr( ToneMapping );
	/**
	*\~english
	*\brief
	*	Tone mapping factory.
	*\~french
	*\brief
	*	Fabrique de mappage de tons.
	*/
	using ToneMappingFactory = castor::Factory< ToneMapping
		, castor::String
		, ToneMappingSPtr
		, std::function< ToneMappingSPtr( Engine &, HdrConfig &, Parameters const & ) > >;;

	//@}
	//@}
}

#endif
