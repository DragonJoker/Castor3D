/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueSsaoModule_H___
#define ___C3D_OpaqueSsaoModule_H___

#include <CastorUtils/CastorUtilsPrerequisites.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name Technique */
	//@{
	/**@name Opaque */
	//@{
	/**@name SSAO */
	//@{

	/**
	*\~english
	*\brief
	*	Ambient occlusion blur pass.
	*\~french
	*\brief
	*	Passe de flou de l'occlusion ambiante.
	*/
	class SsaoBlurPass;
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
	*	Raw ambient occlusion pass.
	*\~french
	*\brief
	*	Passe d'occlusion ambiante.
	*/
	class RawSsaoPass;
	/**
	*\~english
	*\brief
	*	SSAO configuration values.
	*\~french
	*\brief
	*	Valeurs de configuration du SSAO.
	*/
	struct SsaoConfig;
	/**
	*\~english
	*\brief
	*	Ambient occlusion configuration UBO.
	*\~french
	*\brief
	*	UBO de configuration de l'occlusion ambiante.
	*/
	class SsaoConfigUbo;

	//@}
	//@}
	//@}
	//@}
}

#endif
