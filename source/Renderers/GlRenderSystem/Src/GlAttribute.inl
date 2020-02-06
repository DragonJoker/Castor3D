namespace GlRender
{
	template< typename T > struct GlTyper;

	template <> struct GlTyper< int			>
	{
		enum { Value = eGL_TYPE_INT				};
	};
	template <> struct GlTyper< uint32_t	>
	{
		enum { Value = eGL_TYPE_UNSIGNED_INT	};
	};
	template <> struct GlTyper< short		>
	{
		enum { Value = eGL_TYPE_SHORT			};
	};
	template <> struct GlTyper< uint16_t	>
	{
		enum { Value = eGL_TYPE_UNSIGNED_SHORT	};
	};
	template <> struct GlTyper< char		>
	{
		enum { Value = eGL_TYPE_BYTE			};
	};
	template <> struct GlTyper< uint8_t		>
	{
		enum { Value = eGL_TYPE_UNSIGNED_BYTE	};
	};
	template <> struct GlTyper< float		>
	{
		enum { Value = eGL_TYPE_FLOAT			};
	};
	template <> struct GlTyper< double		>
	{
		enum { Value = eGL_TYPE_DOUBLE			};
	};

	template <typename T, uint32_t Count>
	GlAttribute<T, Count>::GlAttribute( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem, Castor::String const & p_strAttribName )
		:	GlAttributeBase( p_gl, p_pRenderSystem, p_strAttribName, eGL_TYPE( GlTyper< T >::Value ), Count )
	{
	}

	template <typename T, uint32_t Count>
	GlAttribute<T, Count>::~GlAttribute()
	{
	}
}
