/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderTechniqueModule_H___
#define ___C3D_RenderTechniqueModule_H___

#include "Castor3D/Render/RenderModule.hpp"

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{

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
	*	Forward render technique pass class.
	*\~french
	*\brief
	*	Classe de passe de technique de rendu de type forward.
	*/
	class ForwardRenderTechniquePass;
	/**
	*\~english
	*\brief
	*	Render technique class
	*\remarks
	*	A render technique is the description of a way to render a render target
	*\~french
	*\brief
	*	Classe de technique de rendu
	*\remarks
	*	Une technique de rendu est la description d'une manière de rendre une cible de rendu
	*/
	class RenderTechnique;
	/**
	\author		Sylvain DOREMUS
	\version	0.9.0
	\date		08/12/2016
	\~english
	\brief		Render technique pass base class.
	\~french
	\brief		Classe de base d'une passe de technique de rendu.
	*/
	class RenderTechniquePass;
	/**
	*\~english
	*\brief
	*	Render technique effect visitor base class.
	*\~french
	*\brief
	*	Classe de base d'un visiteur de technique de rendu.
	*/
	class RenderTechniqueVisitor;
	/**
	*\~english
	*\brief
	*	Writes the image view and sampler descriptor to the given writes.
	*\~french
	*\brief
	*	Ecrit le descripteur de la vue et du sampler dans les writes donnés.
	*/
	void bindTexture( ashes::ImageView const & view
		, ashes::Sampler const & sampler
		, ashes::WriteDescriptorSetArray & writes
		, uint32_t & index );

	CU_DeclareSmartPtr( RenderTechnique );
	CU_DeclareCUSmartPtr( castor3d, RenderTechniquePass, C3D_API );

	//@}
	//@}
}

#endif
