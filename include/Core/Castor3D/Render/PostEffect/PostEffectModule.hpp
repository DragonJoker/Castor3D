/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPostEffectModule_H___
#define ___C3D_RenderPostEffectModule_H___

#include "Castor3D/Miscellaneous/MiscellaneousModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Design/Factory.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name PostEffect */
	//@{

	/**
	*\~english
	*\brief
	*	Holds a command buffer and its semaphore.
	*\~french
	*\brief
	*	Contient un command buffer et son semaphore.
	*/
	struct CommandsSemaphore;
	/**
	*\~english
	*\brief
	*	Post render effect base class.
	*\remarks
	*	A post render effect is an effect applied after 3D rendering and before 2D rendering.
	*	<br />Post render effects are applied in a cumulative way.
	*\~french
	*\brief
	*	Classe de base d'effet post rendu.
	*\remarks
	*	Un effet post rendu est un effet appliqué après le rendu 3D et avant le rendu 2D.
	*	<br />Les effets post rendu sont appliqués de manière cumulative.
	*/
	class PostEffect;
	/**
	*\~english
	*\brief
	*	Post render effect surface structure.
	*\remarks
	*	Holds basic informations for a possible post effect surface: framebuffer and colour texture.
	*\~french
	*\brief
	*	Surface pour effet post rendu.
	*\remarks
	*	Contient les informations basiques de surface d'un effet: framebuffer, texture de couleur.
	*/
	struct PostEffectSurface;
	/**
	*\~english
	*\brief
	*	Post effect factory.
	*\~french
	*\brief
	*	La fabrique d'effets post rendu.
	*/
	using PostEffectFactory = castor::Factory< PostEffect
		, castor::String
		, std::shared_ptr< PostEffect >
		, std::function< std::shared_ptr< PostEffect >( RenderTarget &, RenderSystem &, Parameters const & ) > >;

	CU_DeclareSmartPtr( PostEffect );

	CU_DeclareVector( CommandsSemaphore, CommandsSemaphore );
	CU_DeclareVector( PostEffectSPtr, PostEffectPtr );

	//@}
	//@}
}

#endif
