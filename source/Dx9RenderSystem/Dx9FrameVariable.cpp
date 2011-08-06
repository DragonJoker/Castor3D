#include "Dx9RenderSystem/PrecompiledHeader.hpp"

#include "Dx9RenderSystem/Dx9FrameVariable.hpp"

using namespace Castor3D;

void Dx9FrameVariableBase :: GetVariableLocation( LPCSTR p_pVarName)
{
	m_hDx9Handle = ( * m_pConstants)->GetConstantByName( nullptr, p_pVarName);
}