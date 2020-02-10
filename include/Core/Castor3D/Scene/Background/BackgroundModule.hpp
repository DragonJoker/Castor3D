/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SceneBackgroundModule_H___
#define ___C3D_SceneBackgroundModule_H___

#include "Castor3D/Scene/SceneModule.hpp"

#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	/**@name Scene */
	//@{
	/**@name Background */
	//@{

	/**
	*\~english
	*\brief
	*	Scene background types enumeration.
	*\~french
	*\brief
	*	Enumération des types de fond de scène.
	*/
	enum class BackgroundType
	{
		eColour,
		eImage,
		eSkybox,
		CU_ScopedEnumBounds( eColour )
	};
	castor::String getName( BackgroundType value );
	/**
	*\~english
	*\brief
	*	Background visitor used to prepare the frame command buffer.
	*\~french
	*\brief
	*	Visiteur de fond utilisé pour préparer le tampon de commandes d'une frame.
	*/
	class BackgroundFramePreparator;
	/**
	*\~english
	*\brief
	*	Background text serialiser.
	*\~french
	*\brief
	*	Sérialiseur texte de fond.
	*/
	class BackgroundTextWriter;
	/**
	*\~english
	*\brief
	*	Base class for a background visitor.
	*\~french
	*\brief
	*	Classe de base pour un visiteur de fond.
	*/
	class BackgroundVisitor;
	/**
	*\~english
	*\brief
	*	A skybox with no image but a colour.
	*\~french
	*\brief
	*	Une skybox sans image mais avec une couleur.
	*/
	class ColourBackground;
	/**
	*\~english
	*\brief
	*	Simple image background.
	*\~french
	*\brief
	*	Simple image de fond.
	*/
	class ImageBackground;
	/**
	*\~english
	*\brief
	*	Scene background base class.
	*\~french
	*\brief
	*	Classe de base du fond d'une scène.
	*/
	class SceneBackground;
	/**
	*\~english
	*\brief
	*	Skybox implementation.
	*\~french
	*\brief
	*	Implémentation de Skybox.
	*/
	class SkyboxBackground;

	CU_DeclareSmartPtr( SceneBackground );

	using BackgroundChangedFunc = std::function< void( SceneBackground const & ) >;
	using OnBackgroundChanged = castor::Signal< BackgroundChangedFunc >;
	using OnBackgroundChangedConnection = OnBackgroundChanged::connection;

	//@}
	//@}
}

#endif
