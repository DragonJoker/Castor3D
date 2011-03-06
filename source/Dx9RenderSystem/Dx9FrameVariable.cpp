#include "Dx9RenderSystem/PrecompiledHeader.h"

#include "Dx9RenderSystem/Dx9FrameVariable.h"

using namespace Castor3D;

void Dx9FrameVariableBase :: GetVariableLocation( LPCSTR p_pVarName)
{
	m_hDx9Handle = m_pConstants->GetConstantByName( NULL, p_pVarName);
}