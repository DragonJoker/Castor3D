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

	enum class SkyboxFace : uint8_t
	{
		eRight = 0,
		eLeft = 1,
		eBottom = 2,
		eTop = 3,
		eBack = 4,
		eFront = 5,
		CU_ScopedEnumBounds( eLeft, eBack )
	};
	castor::StringView getName( SkyboxFace face );

	using BackgroundModelID = uint8_t;
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

	CU_DeclareSmartPtr( castor3d, SceneBackground, C3D_API );
	CU_DeclareSmartPtr( castor3d, ColourBackground, C3D_API );
	CU_DeclareSmartPtr( castor3d, ImageBackground, C3D_API );
	CU_DeclareSmartPtr( castor3d, SkyboxBackground, C3D_API );

	using BackgroundChangedFunc = castor::Function< void( SceneBackground const & ) >;
	using OnBackgroundChanged = castor::SignalT< BackgroundChangedFunc >;
	using OnBackgroundChangedConnection = OnBackgroundChanged::connection;

	//@}
	//@}
}

#endif
