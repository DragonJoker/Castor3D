/*
See LICENSE file in root folder
*/
#ifndef ___C3D_OpaqueSsaoModule_H___
#define ___C3D_OpaqueSsaoModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <CastorUtils/CastorUtilsPrerequisites.hpp>
#include <CastorUtils/Config/SmartPtr.hpp>

namespace castor3d
{
	/**@name Render */
	//@{
	/**@name SSAO */
	//@{

	/**
	*\~english
	*\brief
	*	SSAO blur pass.
	*\~french
	*\brief
	*	Passe de flou du SSAO.
	*/
	class SsaoBlurPass;
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
	*	SSAO configuration UBO.
	*\~french
	*\brief
	*	UBO de configuration du SSAO.
	*/
	class SsaoConfigUbo;
	/**
	*\~english
	*\brief
	*	Raw SSAO pass.
	*\~french
	*\brief
	*	Passe de SSAO.
	*/
	class SsaoRawAOPass;

	CU_DeclareCUSmartPtr( castor3d, SsaoBlurPass, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, SsaoRawAOPass, C3D_API );

	//@}
	//@}
}

#endif
