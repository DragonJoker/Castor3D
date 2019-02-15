/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassRenderNode_H___
#define ___C3D_PassRenderNode_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

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
		C3D_API explicit PassRenderNode( Pass & pass );
		void fillDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ashes::DescriptorSet & descriptorSet
			, bool opacityOnly = false );
		void fillDescriptor( ashes::DescriptorSetLayout const & layout
			, uint32_t & index
			, ashes::WriteDescriptorSetArray & writes
			, bool opacityOnly = false );

		//!\~english	The pass.
		//!\~french		La passe.
		Pass & pass;
	};
}

#endif
