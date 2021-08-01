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
	*	The render pass used to display the background.
	*\~french
	*\brief
	*	La passe de rendu utilisée pour l'affichage du background.
	*/
	class BackgroundPass;
	/**
	*\~english
	*\brief
	*	Holds all necessary GPU objects to render background.
	*\~french
	*\brief
	*	Contient tous les objets GPU nécessaires au reundu du background.
	*/
	class BackgroundRenderer;
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
	*	Used to build a CombinePass variable.
	*\~french
	*\brief
	*	Utilisé pour construire une variable CombinePass.
	*/
	class CombinePassBuilder;
	/**
	*\~english
	*\brief
	*	The configuration to create a CombinePass.
	*\~french
	*\brief
	*	La configuration pour créer une CombinePass.
	*/
	template< template< typename ValueT > typename WrapperT >
	struct CombinePassConfigT;
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
	*	Downscales multiple views to a given dimension.
	*\~french
	*\brief
	*	Downscale plusieurs vues vers une dimension donnée.
	*/
	class DownscalePass;
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
	*	Overlays render pass.
	*\~french
	*\brief
	*	Passe de rendu des overlays.
	*/
	class OverlayPass;
	/**
	*\~english
	*\brief
	*	Picking render pass.
	*\~french
	*\brief
	*	Passe de rendu pour le picking.
	*/
	class PickingPass;
	/**
	*\~english
	*\brief
	*	Class used to render a texture into a 3D grid.
	*\~french
	*\brief
	*	Classe utilisée pour rendre une texture dans une grille 3D.
	*/
	class RenderGrid;
	/**
	*\~english
	*\brief
	*	Used to create a RenderGrid variable.
	*\~french
	*\brief
	*	Utilisée pour créer une variable RenderGrid.
	*/
	template< typename ConfigT, typename BuilderT >
	class RenderGridBuilderT;
	/**
	*\~english
	*\brief
	*	Used to create a RenderGrid variable.
	*\~french
	*\brief
	*	Utilisée pour créer une variable RenderGrid.
	*/
	class RenderGridBuilder;
	/**
	*\~english
	*\brief
	*	Class used to render a texture into a quad.
	*\~french
	*\brief
	*	Classe utilisée pour rendre une texture dans un quad.
	*/
	class RenderQuad;
	/**
	*\~english
	*\brief
	*	Used to create a RenderQuad variable.
	*\~french
	*\brief
	*	Utilisée pour créer une variable RenderQuad.
	*/
	template< typename ConfigT, typename BuilderT >
	class RenderQuadBuilderT;
	/**
	*\~english
	*\brief
	*	Used to create a RenderQuad variable.
	*\~french
	*\brief
	*	Utilisée pour créer une variable RenderQuad.
	*/
	class RenderQuadBuilder;
	/**
	*\~english
	*\brief
	*	Stencil pre-pass for light passes needing a mesh.
	*\~french
	*\brief
	*	Pré-passe de stencil pour les passes d'éclairage nécessitant un maillage.
	*/
	class StencilPass;

	CU_DeclareCUSmartPtr( castor3d, BackgroundRenderer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, CombinePass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, DepthPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, DownscalePass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, GaussianBlur, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, LineariseDepthPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, PickingPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderQuad, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, RenderGrid, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, StencilPass, C3D_API );

	CU_DeclareVector( CommandsSemaphore, CommandsSemaphore );

	//@}
	//@}
}

#endif
