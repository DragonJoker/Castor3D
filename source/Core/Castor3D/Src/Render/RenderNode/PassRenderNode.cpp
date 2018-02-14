#include "PassRenderNode.hpp"

#include "Material/Pass.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Castor3DPrerequisites.hpp"
#include "Texture/TextureUnit.hpp"

using namespace castor;

namespace castor3d
{
	PassRenderNode::PassRenderNode( Pass & pass )
		: m_pass{ pass }
	{
	}
}
