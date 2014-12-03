#include "Dx9FrameVariableBuffer.hpp"
#include "Dx9OneFrameVariable.hpp"
#include "Dx9PointFrameVariable.hpp"
#include "Dx9MatrixFrameVariable.hpp"

using namespace Castor3D;

namespace Dx9Render
{
	template< eFRAME_VARIABLE_TYPE > struct Dx9VariableApplyer;

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_SAMPLER > : public DxVariableApplyerBase
	{
		typedef TextureBaseRPtr	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxOneFrameVariable< value_type > > l_pVariable = std::static_pointer_cast< DxOneFrameVariable< value_type > >( p_pVariable );

			if ( l_pVariable->GetOccCount() <= 1 && l_pVariable->GetValue() )
			{
				IDirect3DTexture9 * l_pDxTexture = NULL;
				Castor3D::TextureBaseRPtr l_pTexture = l_pVariable->GetValue();

				if ( l_pTexture )
				{
					if ( l_pTexture->GetType() == Castor3D::eTEXTURE_TYPE_STATIC )
					{
						l_pDxTexture = static_cast< DxStaticTexture * >( l_pTexture )->GetDxTexture();
					}
					else
					{
						l_pDxTexture = static_cast< DxDynamicTexture * >( l_pTexture )->GetDxTexture();
					}
				}

				HRESULT l_hr = p_pEffect->SetTexture( LPCSTR( p_handle ), l_pDxTexture );
				dxCheckError( l_hr, "ApplyVariable texture" );
			}
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_INT > : public DxVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxOneFrameVariable< value_type > > l_pVariable = std::static_pointer_cast< DxOneFrameVariable< value_type > >( p_pVariable );
			HRESULT l_hr;

			if ( l_pVariable->GetOccCount() <= 1 )
			{
				l_hr = p_pEffect->SetInt( p_handle, l_pVariable->GetValue() );
			}
			else
			{
				l_hr = p_pEffect->SetIntArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() );
			}

			dxCheckError( l_hr, "ApplyVariable int" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_FLOAT > : public DxVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxOneFrameVariable< value_type > > l_pVariable = std::static_pointer_cast< DxOneFrameVariable< value_type > >( p_pVariable );
			HRESULT l_hr;

			if ( l_pVariable->GetOccCount() <= 1 )
			{
				l_hr = p_pEffect->SetFloat( p_handle, l_pVariable->GetValue() );
			}
			else
			{
				l_hr = p_pEffect->SetFloatArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() );
			}

			dxCheckError( l_hr, "ApplyVariable float" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC2I > : public DxVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxPointFrameVariable< value_type, 2 > > l_pVariable = std::static_pointer_cast< DxPointFrameVariable< value_type, 2 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetIntArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 2 );
			dxCheckError( l_hr, "ApplyVariable vec2i" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC3I > : public DxVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxPointFrameVariable< value_type, 3 > > l_pVariable = std::static_pointer_cast< DxPointFrameVariable< value_type, 3 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetIntArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 3 );
			dxCheckError( l_hr, "ApplyVariable vec3i" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC4I > : public DxVariableApplyerBase
	{
		typedef int	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxPointFrameVariable< value_type, 4 > > l_pVariable = std::static_pointer_cast< DxPointFrameVariable< value_type, 4 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetIntArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 4 );
			dxCheckError( l_hr, "ApplyVariable vec4i" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC2F > : public DxVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxPointFrameVariable< value_type, 2 > > l_pVariable = std::static_pointer_cast< DxPointFrameVariable< value_type, 2 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetFloatArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 2 );
			dxCheckError( l_hr, "ApplyVariable float2" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC3F > : public DxVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxPointFrameVariable< value_type, 3 > > l_pVariable = std::static_pointer_cast< DxPointFrameVariable< value_type, 3 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetFloatArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 3 );
			dxCheckError( l_hr, "ApplyVariable float3" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC4F > : public DxVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxPointFrameVariable< value_type, 4 > > l_pVariable = std::static_pointer_cast< DxPointFrameVariable< value_type, 4 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetFloatArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 4 );
			dxCheckError( l_hr, "ApplyVariable float4" );
		}
	};

	template<> struct Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4F > : public DxVariableApplyerBase
	{
		typedef float	value_type;

		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			std::shared_ptr< DxMatrixFrameVariable< value_type, 4, 4 > > l_pVariable = std::static_pointer_cast< DxMatrixFrameVariable< value_type, 4, 4 > >( p_pVariable );
			HRESULT l_hr = p_pEffect->SetFloatArray( p_handle, ( value_type const * )l_pVariable->const_ptr(), l_pVariable->GetOccCount() * 4 );
			dxCheckError( l_hr, "ApplyVariable float4x4" );
		}
	};

	template< eFRAME_VARIABLE_TYPE Type > struct Dx9VariableApplyer
	{
		inline void operator()( ID3DXEffect * p_pEffect, D3DXHANDLE p_handle, FrameVariableSPtr p_pVariable )
		{
			CASTOR_EXCEPTION( "VariableApplyer - Unsupported arguments" );
		}
	};

	namespace
	{
		template< eFRAME_VARIABLE_TYPE Type > FrameVariableSPtr Dx9FrameVariableCreator( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc );
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_INT			>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable<		int						> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable<		float					> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxOneFrameVariable<		TextureBaseRPtr			> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable<		int,			2		> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable<		int,			3		> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable<		int,			4		> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable<		float,			2		> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable<		float,			3		> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxPointFrameVariable<		float,			4		> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}
		template<> FrameVariableSPtr Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F		>( DxRenderSystem * p_pRenderSystem, DxShaderProgram * p_pProgram, uint32_t p_uiNbOcc )
		{
			return std::make_shared< DxMatrixFrameVariable<	float,			4,	4	> >( p_pRenderSystem, p_pProgram, p_uiNbOcc );
		}

		template< eFRAME_VARIABLE_TYPE Type > DxVariableApplyerBaseSPtr Dx9ApplyerCreator();
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_INT		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_INT		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_FLOAT		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_FLOAT		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_SAMPLER	>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_SAMPLER	> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2I		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC2I		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3I		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC3I		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4I		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC4I		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2F		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC2F		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3F		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC3F		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4F		>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_VEC4F		> >();
		}
		template<> DxVariableApplyerBaseSPtr Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4F	>()
		{
			return std::make_shared< Dx9VariableApplyer< eFRAME_VARIABLE_TYPE_MAT4X4F	> >();
		}
	}

	DxFrameVariableBuffer::DxFrameVariableBuffer( Castor::String const & p_strName, DxRenderSystem * p_pRenderSystem )
		:	FrameVariableBuffer( p_strName, p_pRenderSystem )
		,	m_pDxRenderSystem( p_pRenderSystem )
		,	m_pShaderProgram( NULL )
	{
	}

	DxFrameVariableBuffer::~DxFrameVariableBuffer()
	{
	}

	FrameVariableSPtr DxFrameVariableBuffer::DoCreateVariable( ShaderProgramBase * p_pProgram, eFRAME_VARIABLE_TYPE p_eType, Castor::String const & p_strName, uint32_t p_uiNbOcc )
	{
		FrameVariableSPtr l_pReturn;
		DxVariableApplyerBaseSPtr l_pApplyer;
		DxShaderProgram * l_pProgram = static_cast< DxShaderProgram * >( p_pProgram );

		switch ( p_eType )
		{
		case eFRAME_VARIABLE_TYPE_INT:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_INT		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_INT		>();
			break;

		case eFRAME_VARIABLE_TYPE_FLOAT:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_FLOAT		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_FLOAT	>();
			break;

		case eFRAME_VARIABLE_TYPE_SAMPLER:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_SAMPLER	>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_SAMPLER	>();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2I:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2I		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2I	>();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3I:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3I		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3I	>();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4I:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4I		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4I	>();
			break;

		case eFRAME_VARIABLE_TYPE_VEC2F:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC2F		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC2F	>();
			break;

		case eFRAME_VARIABLE_TYPE_VEC3F:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC3F		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC3F	>();
			break;

		case eFRAME_VARIABLE_TYPE_VEC4F:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_VEC4F		>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_VEC4F	>();
			break;

		case eFRAME_VARIABLE_TYPE_MAT4X4F:
			l_pReturn = Dx9FrameVariableCreator< eFRAME_VARIABLE_TYPE_MAT4X4F	>( m_pDxRenderSystem, l_pProgram, p_uiNbOcc );
			l_pApplyer = Dx9ApplyerCreator< eFRAME_VARIABLE_TYPE_MAT4X4F	>();
			break;
		}

		if ( l_pReturn )
		{
			l_pReturn->SetName( p_strName );
		}

		return l_pReturn;
	}

	bool DxFrameVariableBuffer::DoInitialise( ShaderProgramBase * p_pProgram )
	{
		return false;
	}

	void DxFrameVariableBuffer::DoCleanup()
	{
	}

	bool DxFrameVariableBuffer::DoBind()
	{
		for ( FrameVariablePtrListIt l_it = m_listInitialised.begin(); l_it != m_listInitialised.end(); ++l_it )
		{
			( *l_it )->Apply();
		}

		return true;
	}

	void DxFrameVariableBuffer::DoUnbind()
	{
	}
}