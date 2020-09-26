/*
See LICENSE file in root folder
*/
#ifndef ___C3D_BackgroundVisitor_H___
#define ___C3D_BackgroundVisitor_H___

#include "BackgroundModule.hpp"

namespace castor3d
{
	class BackgroundVisitor
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API BackgroundVisitor();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~BackgroundVisitor();
		/**
		*\~english
		*\brief
		*	Visits a colour background.
		*\~french
		*\brief
		*	Visite un fond de type couleur.
		*/
		C3D_API virtual void visit( ColourBackground & background ) = 0;
		/**
		*\~english
		*\brief
		*	Visits a skybox background.
		*\~french
		*\brief
		*	Visite un fond de type skybox.
		*/
		C3D_API virtual void visit( SkyboxBackground & background ) = 0;
		/**
		*\~english
		*\brief
		*	Visits an image background.
		*\~french
		*\brief
		*	Visite un fond de type image.
		*/
		C3D_API virtual void visit( ImageBackground & background ) = 0;
	};
}

#endif
