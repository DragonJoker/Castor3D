//*************************************************************************************************

template <> struct ShaderObjectCreator<GlslShaderObject>
{
	static GlslShaderObjectPtr Vertex( RenderSystem * p_pRS)
	{
		return p_pRS->_createGlslVertexShader();
	}
	static GlslShaderObjectPtr Pixel( RenderSystem * p_pRS)
	{
		return p_pRS->_createGlslFragmentShader();
	}
	static GlslShaderObjectPtr Geometry( RenderSystem * p_pRS)
	{
		return p_pRS->_createGlslGeometryShader();
	}
};

template <> struct ShaderObjectCreator<HlslShaderObject>
{
	static HlslShaderObjectPtr Vertex( RenderSystem * p_pRS)
	{
		return p_pRS->_createHlslVertexShader();
	}
	static HlslShaderObjectPtr Pixel( RenderSystem * p_pRS)
	{
		return p_pRS->_createHlslFragmentShader();
	}
	static HlslShaderObjectPtr Geometry( RenderSystem * p_pRS)
	{
		return p_pRS->_createHlslGeometryShader();
	}
};

template <> struct ShaderObjectCreator<CgShaderObject>
{
	static CgShaderObjectPtr Vertex( RenderSystem * p_pRS)
	{
		return p_pRS->_createCgVertexShader();
	}
	static CgShaderObjectPtr Pixel( RenderSystem * p_pRS)
	{
		return p_pRS->_createCgFragmentShader();
	}
	static CgShaderObjectPtr Geometry( RenderSystem * p_pRS)
	{
		return p_pRS->_createCgGeometryShader();
	}
};

//*************************************************************************************************

template <> struct ShaderProgramFunctor<GlslShaderProgram>
{
	static GlslShaderProgramPtr Create( RenderSystem * p_pRS, const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	{
		return p_pRS->_createGlslShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
	}
	static void Add( RenderSystem * p_pRS, GlslShaderProgramPtr p_pToAdd)
	{
		p_pRS->_addGlslShaderProgram( p_pToAdd);
	}
};

template <> struct ShaderProgramFunctor<HlslShaderProgram>
{
	static HlslShaderProgramPtr Create( RenderSystem * p_pRS, const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	{
		return p_pRS->_createHlslShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
	}
	static void Add( RenderSystem * p_pRS, HlslShaderProgramPtr p_pToAdd)
	{
		p_pRS->_addHlslShaderProgram( p_pToAdd);
	}
};

template <> struct ShaderProgramFunctor<CgShaderProgram>
{
	static CgShaderProgramPtr Create( RenderSystem * p_pRS, const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
	{
		return p_pRS->_createCgShaderProgram( p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);
	}
	static void Add( RenderSystem * p_pRS, CgShaderProgramPtr p_pToAdd)
	{
		p_pRS->_addCgShaderProgram( p_pToAdd);
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

template <> struct RendererFunctor<TextureEnvironmentRenderer>
{
	static TextureEnvironmentRendererPtr Create( RenderSystem * p_pRS)
	{
		return p_pRS->_createTexEnvRenderer();
	}
	static void Add( RenderSystem * p_pRS, TextureEnvironmentRendererPtr p_pRenderer)
	{
		p_pRS->m_texEnvRenderers.push_back( p_pRenderer);
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

template <> struct VertexAttribCreator<VertexAttribsBufferBool, bool, 1>
{
	static VertexAttribsBufferBoolPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create1BoolVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferInt, int, 1>
{
	static VertexAttribsBufferIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create1IntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferUInt, unsigned int, 1>
{
	static VertexAttribsBufferUIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create1UIntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferFloat, float, 1>
{
	static VertexAttribsBufferFloatPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create1FloatVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferDouble, double, 1>
{
	static VertexAttribsBufferDoublePtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create1DoubleVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferBool, bool, 2>
{
	static VertexAttribsBufferBoolPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create2BoolVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferInt, int, 2>
{
	static VertexAttribsBufferIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create2IntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferUInt, unsigned int, 2>
{
	static VertexAttribsBufferUIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create2UIntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferFloat, float, 2>
{
	static VertexAttribsBufferFloatPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create2FloatVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferDouble, double, 2>
{
	static VertexAttribsBufferDoublePtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create2DoubleVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferBool, bool, 3>
{
	static VertexAttribsBufferBoolPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create3BoolVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferInt, int, 3>
{
	static VertexAttribsBufferIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create3IntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferUInt, unsigned int, 3>
{
	static VertexAttribsBufferUIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create3UIntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferFloat, float, 3>
{
	static VertexAttribsBufferFloatPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create3FloatVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferDouble, double, 3>
{
	static VertexAttribsBufferDoublePtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create3DoubleVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferBool, bool, 4>
{
	static VertexAttribsBufferBoolPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create4BoolVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferInt, int, 4>
{
	static VertexAttribsBufferIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create4IntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferUInt, unsigned int, 4>
{
	static VertexAttribsBufferUIntPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create4UIntVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferFloat, float, 4>
{
	static VertexAttribsBufferFloatPtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create4FloatVertexAttribsBuffer( p_strArg);
	}
};

template <> struct VertexAttribCreator<VertexAttribsBufferDouble, double, 4>
{
	static VertexAttribsBufferDoublePtr Create( RenderSystem * p_pRS, const String & p_strArg)
	{
		return p_pRS->_create4DoubleVertexAttribsBuffer( p_strArg);
	}
};

//*************************************************************************************************

template <class Ty>
typename shared_ptr<Ty> RenderSystem :: CreateRenderer()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	typename shared_ptr<Ty> l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = RendererFunctor<Ty>::Create( sm_singleton);

		if (l_pReturn != NULL)
		{
			RendererFunctor<Ty>::Add( sm_singleton, l_pReturn);
		}
	}

	return l_pReturn;
}

template <class Ty, typename T, size_t Count>
typename shared_ptr<Ty> RenderSystem :: CreateBuffer( const String & p_strArg)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	typename shared_ptr<Ty> l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = VertexAttribCreator<Ty, T, Count>::Create( sm_singleton, p_strArg);
	}

	return l_pReturn;
}

template <class Ty>
typename shared_ptr<Ty> RenderSystem :: CreateVertexShader()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	typename shared_ptr<Ty> l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = ShaderObjectCreator<Ty>::Vertex( sm_singleton);
	}

	return l_pReturn;
}

template <class Ty>
typename shared_ptr<Ty> RenderSystem :: CreateFragmentShader()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	typename shared_ptr<Ty> l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = ShaderObjectCreator<Ty>::Pixel( sm_singleton);
	}

	return l_pReturn;
}

template <class Ty>
typename shared_ptr<Ty> RenderSystem :: CreateGeometryShader()
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	typename shared_ptr<Ty> l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = ShaderObjectCreator<Ty>::Geometry( sm_singleton);
	}

	return l_pReturn;
}

template <class Ty>
typename shared_ptr<Ty> RenderSystem :: CreateShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
{
	CASTOR_RECURSIVE_MUTEX_AUTO_SCOPED_LOCK();
	typename shared_ptr<Ty> l_pReturn;

	if (sm_singleton != NULL)
	{
		l_pReturn = ShaderProgramFunctor<Ty>::Create( sm_singleton, p_vertexShaderFile, p_fragmentShaderFile, p_geometryShaderFile);

		if (l_pReturn != NULL)
		{
			ShaderProgramFunctor<Ty>::Add( sm_singleton, l_pReturn);
		}
	}

	return l_pReturn;
}

//*************************************************************************************************