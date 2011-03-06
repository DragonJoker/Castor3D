template <> inline void ApplyVariable<int>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const int * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		p_pConstants->SetInt( Dx9RenderSystem::GetDevice(), p_hDx9Handle, * p_pValue);
	}
	else
	{
		p_pConstants->SetIntArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc);
	}
}

template <> inline void ApplyVariable<int, 1>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const int * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetIntArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 1);
}

template <> inline void ApplyVariable<int, 2>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const int * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetIntArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 2);
}

template <> inline void ApplyVariable<int, 3>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const int * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetIntArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 3);
}

template <> inline void ApplyVariable<int, 4>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const int * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetIntArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 4);
}

template <> inline void ApplyVariable<float>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		p_pConstants->SetFloat( Dx9RenderSystem::GetDevice(), p_hDx9Handle, * p_pValue);
	}
	else
	{
		p_pConstants->SetFloatArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc);
	}
}

template <> inline void ApplyVariable<float, 1>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const float * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetFloatArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 1);
}

template <> inline void ApplyVariable<float, 2>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const float * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetFloatArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 2);
}

template <> inline void ApplyVariable<float, 3>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const float * p_pValue, size_t p_uiOcc)
{
	p_pConstants->SetFloatArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, p_pValue, p_uiOcc * 3);
}

template <> inline void ApplyVariable<float, 4>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		D3DXVECTOR4 l_vector( p_pValue);
		p_pConstants->SetVector( Dx9RenderSystem::GetDevice(), p_hDx9Handle, & l_vector);
	}
	else
	{
		std::vector <D3DXVECTOR4> l_arrayVectors( p_uiOcc);

		for (size_t i = 0 ; i < p_uiOcc ; i++)
		{
			l_arrayVectors[i] = D3DXVECTOR4( p_pValue + (i * 4));
		}

		p_pConstants->SetVectorArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, & l_arrayVectors[0], p_uiOcc);
	}
}

template <> inline void ApplyVariable<float, 4, 4>( ID3DXConstantTable * p_pConstants, D3DXHANDLE p_hDx9Handle, const float * p_pValue, size_t p_uiOcc)
{
	if (p_uiOcc <= 1)
	{
		D3DXMATRIX l_matrix( p_pValue);
		p_pConstants->SetMatrix( Dx9RenderSystem::GetDevice(), p_hDx9Handle, & l_matrix);
	}
	else
	{
		std::vector <D3DXMATRIX> l_arrayMatrices( p_uiOcc);

		for (size_t i = 0 ; i < p_uiOcc ; i++)
		{
			l_arrayMatrices[i] = D3DXMATRIX( p_pValue + (i * 16));
		}

		p_pConstants->SetMatrixArray( Dx9RenderSystem::GetDevice(), p_hDx9Handle, & l_arrayMatrices[0], p_uiOcc);
	}
}

//***********************************************************************************************************************

template <typename Type> 
inline void Dx9FrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_hDx9Handle == NULL)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_hDx9Handle != NULL) 
		{
			ApplyVariable<Type>( m_pConstants, m_hDx9Handle, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Count>
inline void Dx9FrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_hDx9Handle == NULL)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_hDx9Handle != NULL) 
		{
			ApplyVariable<Type, Count>( m_pConstants, m_hDx9Handle, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}
template <typename Type, int Rows, int Columns>
inline void Dx9FrameVariableBase :: _apply( const String & p_strName, const Type * p_pValue, size_t p_uiOcc)
{
	if (m_bPresentInProgram)
	{
		if (m_hDx9Handle == NULL)
		{
			GetVariableLocation( p_strName.char_str());
		}
		if (m_hDx9Handle != NULL) 
		{
			ApplyVariable<Type, Rows, Columns>( m_pConstants, m_hDx9Handle, p_pValue, p_uiOcc);
		}
		else
		{
			m_bPresentInProgram = false;
		}
	}
}

//***********************************************************************************************************************
