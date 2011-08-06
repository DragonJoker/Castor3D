//*************************************************************************************************

template <> struct ShaderProgramFunctor<GlslShaderProgram>
{
	static GlslShaderProgramPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createGlslShaderProgram();
	}
	static void Add( RenderSystem * p_pRS, GlslShaderProgramPtr p_pToAdd)
	{
		p_pRS->_addShaderProgram( static_pointer_cast<ShaderProgramBase>( p_pToAdd));
	}
};

template <> struct ShaderProgramFunctor<HlslShaderProgram>
{
	static HlslShaderProgramPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createHlslShaderProgram();
	}
	static void Add( RenderSystem * p_pRS, HlslShaderProgramPtr p_pToAdd)
	{
		p_pRS->_addShaderProgram( static_pointer_cast<ShaderProgramBase>( p_pToAdd));
	}
};

template <> struct ShaderProgramFunctor<CgShaderProgram>
{
	static CgShaderProgramPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createCgShaderProgram();
	}
	static void Add( RenderSystem * p_pRS, CgShaderProgramPtr p_pToAdd)
	{
		p_pRS->_addShaderProgram( static_pointer_cast<ShaderProgramBase>( p_pToAdd));
	}
};

//*************************************************************************************************

template <> struct RendererFunctor<SubmeshRenderer>
{
	static SubmeshRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createSubmeshRenderer();
	}
	static void Add( RenderSystem * p_pRS, SubmeshRendererPtr p_pRenderer)
	{
		p_pRS->m_submeshesRenderers.push_back( p_pRenderer);
	}
};

template <> struct RendererFunctor<TextureRenderer>
{
	static TextureRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createTextureRenderer();
	}
	static void Add( RenderSystem * p_pRS, TextureRendererPtr p_pRenderer)
	{
		p_pRS->m_textureRenderers.push_back( p_pRenderer);
	}
};

template <> struct RendererFunctor<PassRenderer>
{
	static PassRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createPassRenderer();
	}
	static void Add( RenderSystem * p_pRS, PassRendererPtr p_pRenderer)
	{
		p_pRS->m_passRenderers.push_back( p_pRenderer);
	}
};

template <> struct RendererFunctor<CameraRenderer>
{
	static CameraRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createCameraRenderer();
	}
	static void Add( RenderSystem * p_pRS, CameraRendererPtr p_pRenderer)
	{
		p_pRS->m_cameraRenderers.push_back( p_pRenderer);
	}
};

template <> struct RendererFunctor<LightRenderer>
{
	static LightRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createLightRenderer();
	}
	static void Add( RenderSystem * p_pRS, LightRendererPtr p_pRenderer)
	{
		p_pRS->m_lightRenderers.push_back( p_pRenderer);
	}
};

template <> struct RendererFunctor<WindowRenderer>
{
	static WindowRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createWindowRenderer();
	}
	static void Add( RenderSystem * p_pRS, WindowRendererPtr p_pRenderer)
	{
		p_pRS->m_windowRenderers.push_back( p_pRenderer);
	}
};

template <> struct RendererFunctor<OverlayRenderer>
{
	static OverlayRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createOverlayRenderer();
	}
	static void Add( RenderSystem * p_pRS, OverlayRendererPtr p_pRenderer)
	{
		p_pRS->m_overlayRenderers.push_back( p_pRenderer);
	}
};

//*************************************************************************************************

template <class Ty>
shared_ptr<Ty> RenderSystem :: CreateRenderer()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	shared_ptr<Ty> l_pReturn;

	if (sm_singleton)
	{
		l_pReturn = RendererFunctor<Ty>::Create( sm_singleton);

		if (l_pReturn)
		{
			RendererFunctor<Ty>::Add( sm_singleton, l_pReturn);
		}
	}

	return l_pReturn;
}

template <class Ty>
shared_ptr<Ty> RenderSystem :: CreateShaderProgram()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	shared_ptr<Ty> l_pReturn;

	if (sm_singleton)
	{
		l_pReturn = ShaderProgramFunctor<Ty>::Create( sm_singleton);

		if (l_pReturn)
		{
			ShaderProgramFunctor<Ty>::Add( sm_singleton, l_pReturn);
		}
	}

	return l_pReturn;
}

//*************************************************************************************************
