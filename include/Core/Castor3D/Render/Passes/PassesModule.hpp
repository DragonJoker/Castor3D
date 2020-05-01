/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderPassesModule_H___
#define ___C3D_RenderPassesModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Passes */
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
	*	Combines two textures into one, using an addition.
	*\~french
	*\brief
	*	Combine deux textures en une, en les additionnant.
	*/
	class CombinePass;
	/**
	*\~english
	*\brief
	*	Deferred lighting Render technique pass.
	*\~french
	*\brief
	*	Classe de passe de technique de rendu implémentant le Deferred lighting.
	*/
	class DepthPass;
	/**
	*\~english
	*\brief
	*	Gaussian blur pass.
	*\~french
	*\brief
	*	Passe flou gaussien.
	*/
	class GaussianBlur;
	/**
	*\~english
	*\brief
	*	Depth linearisation pass, with mipmaps generation.
	*\~french
	*\brief
	*	Passe linéarisation de profondeur, avec génération des mipmaps.
	*/
	class LineariseDepthPass;
	/**
	*\~english
	*\brief
	*	Stencil pre-pass for light passes needing a mesh.
	*\~french
	*\brief
	*	Pré-passe de stencil pour les passes d'éclairage nécessitant un maillage.
	*/
	class StencilPass;

	CU_DeclareSmartPtr( CombinePass );
	CU_DeclareSmartPtr( DepthPass );
	CU_DeclareSmartPtr( GaussianBlur );
	CU_DeclareSmartPtr( LineariseDepthPass );
	CU_DeclareSmartPtr( StencilPass );

	CU_DeclareVector( CommandsSemaphore, CommandsSemaphore );

	//@}
	//@}
}

#endif
