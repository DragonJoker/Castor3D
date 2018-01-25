/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassRenderNode_H___
#define ___C3D_PassRenderNode_H___

#include "Castor3DPrerequisites.hpp"

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date
	\~english
	\brief		Helper structure used link a pass and a shader program.
	\~french
	\brief		Structure d'aide utilisée pour lier une passe et un programme shader.
	*/
	struct PassRenderNode
	{
		C3D_API PassRenderNode( Pass & pass );

		//!\~english	The pass.
		//!\~french		La passe.
		Pass & m_pass;
	};
}

#endif
