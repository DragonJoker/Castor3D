//*************************************************************************************************

template <> struct RendererCreator< SubmeshRenderer >
{
	SubmeshRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateSubmeshRenderer();
	}
};

template <> struct RendererCreator< TextureRenderer >
{
	TextureRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateTextureRenderer();
	}
};

template <> struct RendererCreator< PassRenderer >
{
	PassRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreatePassRenderer();
	}
};

template <> struct RendererCreator< CameraRenderer >
{
	CameraRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateCameraRenderer();
	}
};

template <> struct RendererCreator< LightRenderer >
{
	LightRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateLightRenderer();
	}
};

template <> struct RendererCreator< WindowRenderer >
{
	WindowRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateWindowRenderer();
	}
};
/*
template <> struct RendererCreator< OverlayRenderer >
{
	OverlayRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateOverlayRenderer();
	}
};
*/
template <> struct RendererCreator< TargetRenderer >
{
	TargetRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateTargetRenderer();
	}
};

template <> struct RendererCreator< SamplerRenderer >
{
	SamplerRendererSPtr operator()( RenderSystem * p_pRS )
	{
		return p_pRS->DoCreateSamplerRenderer();
	}
};

//*************************************************************************************************

template <> struct RendererAdder< SubmeshRenderer >
{
	void operator()( RenderSystem * p_pRS, SubmeshRendererSPtr p_pRenderer )
	{
		p_pRS->m_submeshRenderers.push_back( p_pRenderer );
	}
};

template <> struct RendererAdder< TextureRenderer >
{
	void operator()( RenderSystem * p_pRS, TextureRendererSPtr p_pRenderer )
	{
		p_pRS->m_textureRenderers.push_back( p_pRenderer );
	}
};

template <> struct RendererAdder< PassRenderer >
{
	void operator()( RenderSystem * p_pRS, PassRendererSPtr p_pRenderer )
	{
		p_pRS->m_passRenderers.push_back( p_pRenderer );
	}
};

template <> struct RendererAdder< CameraRenderer >
{
	void operator()( RenderSystem * p_pRS, CameraRendererSPtr p_pRenderer )
	{
		p_pRS->m_cameraRenderers.push_back( p_pRenderer );
	}
};

template <> struct RendererAdder< LightRenderer >
{
	void operator()( RenderSystem * p_pRS, LightRendererSPtr p_pRenderer )
	{
		p_pRS->m_lightRenderers.push_back( p_pRenderer );
	}
};

template <> struct RendererAdder< WindowRenderer >
{
	void operator()( RenderSystem * p_pRS, WindowRendererSPtr p_pRenderer )
	{
		p_pRS->m_windowRenderers.push_back( p_pRenderer );
	}
};
/*
template <> struct RendererAdder< OverlayRenderer >
{
	void operator()( RenderSystem * p_pRS, OverlayRendererSPtr p_pRenderer )
	{
		p_pRS->m_overlayRenderers.push_back( p_pRenderer );
	}
};
*/
template <> struct RendererAdder< TargetRenderer >
{
	void operator()( RenderSystem * p_pRS, TargetRendererSPtr p_pRenderer )
	{
		p_pRS->m_targetRenderers.push_back( p_pRenderer );
	}
};

template <> struct RendererAdder< SamplerRenderer >
{
	void operator()( RenderSystem * p_pRS, SamplerRendererSPtr p_pRenderer )
	{
		p_pRS->m_arraySamplerRenderers.push_back( p_pRenderer );
	}
};

//*************************************************************************************************

template< class Ty >
std::shared_ptr< Ty > RenderSystem :: CreateRenderer()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	std::shared_ptr< Ty > l_pReturn;

	l_pReturn = RendererCreator<Ty>()( this );

	if( l_pReturn )
	{
		RendererAdder<Ty>()( this, l_pReturn );
	}

	return l_pReturn;
}

//*************************************************************************************************
