/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_OBJECT_H___
#define ___C3DGLES3_OBJECT_H___

#include "Common/GlES3Holder.hpp"

namespace GlES3Render
{
	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to create an object on GPU
	@remarks
		Specialization for glGenBuffers, glGenTextures, ...
	*/
	template<>
	struct CreatorHelper< std::function< void( int, uint32_t * ) > >
	{
		typedef std::function< void( int, uint32_t * ) > CreateFunction;

		static inline uint32_t Apply( CreateFunction p_creator )
		{
			uint32_t l_return = -1;
			p_creator( 1, &l_return );
			return l_return;
		}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to create an object on GPU
	@remarks
		Specialization for glCreateShader
	*/
	template<>
	struct CreatorHelper< std::function< uint32_t() > >
	{
		typedef std::function< uint32_t() > CreateFunction;

		static inline uint32_t Apply( CreateFunction p_creator )
		{
			return p_creator();
		}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to destroy an object on GPU
	@remarks
		Specialization for glDeleteBuffers, glDeleteTextures, ...
	*/
	template<>
	struct DestroyerHelper< std::function< void( int, uint32_t const * ) > >
	{
		typedef std::function< void( int, uint32_t const * ) > DestroyFunction;

		static inline void Apply( DestroyFunction p_destroyer, uint32_t p_glName )
		{
			p_destroyer( 1, &p_glName );
		}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Helper class to destroy an object on GPU
	@remarks
		Specialization for glDeleteShader
	*/
	template<>
	struct DestroyerHelper< std::function< void( uint32_t ) > >
	{
		typedef std::function< void( uint32_t ) > DestroyFunction;

		static inline void Apply( DestroyFunction p_destroyer, uint32_t p_glName )
		{
			p_destroyer( p_glName );
		}
	};

	/*!
	@author
		Sylvain DOREMUS
	@date
		19/11/2015
	@version
		0.8.0
	@brief
		Base class for each OpenGL object (with glGen* and glDelete* functions)
	*/
	template< typename CreateFunction, typename DestroyFunction >
	class Object
		: public Holder
	{
	protected:
		typedef std::function< bool( uint32_t ) > ValidatorFunction;

	public:
		/** Constructor
		@param[in] p_openGlES3
			The OpenGlES3 instance
		@param[in] p_typeName
			The object type name
		@param[in] p_creator
			The creation function
		@param[in] p_destroyer
			The destruction function
		@param[in] p_validator
			The validation function
		*/
		inline Object( OpenGlES3 & p_openGlES3, const char * p_typeName, CreateFunction p_creator, DestroyFunction p_destroyer, ValidatorFunction p_validator )
			: Holder( p_openGlES3 )
			, m_creator( p_creator )
			, m_destroyer( p_destroyer )
			, m_validator( p_validator )
			, m_glName( GlES3InvalidIndex )
			, m_typeName( p_typeName )
		{
		}

		/** Destructor
		*/
		~Object()
		{
		}

		/** Creates the object on GPU
		@return
			false if not created successfully
		*/
		inline bool Create()
		{
			bool l_return = IsValid();

			if ( !l_return )
			{
				l_return = DoCreate();
			}

			if ( l_return )
			{
				glTrack( Holder::GetOpenGlES3(), m_typeName, this );

#if !defined( NDEBUG )

				m_hasBeenValid = true;

#endif
			}

			return l_return;
		}

		/** Destroys the object on GPU, if valid.
		 */
		inline void Destroy()
		{
#if !defined( NDEBUG )

			if ( m_hasBeenValid )
			{
				m_hasBeenValid = !glUntrack( Holder::GetOpenGlES3(), this );
			}

#endif

			if ( IsValid() )
			{
				DoDestroy();
			}
			else
			{
				m_glName = GlES3InvalidIndex;
			}
		}

		/** Tells if the object is created
		@return
			The status
		*/
		inline bool IsValid()const
		{
			return m_glName != GlES3InvalidIndex;
		}

		/** Tells if the object is a valid OpenGL object.
		@return
			The status.
		*/
		inline bool Validate()const
		{
			return IsValid() && m_validator( m_glName );
		}

		/** Retrieves the OpenGlES3 instance
		@return
			The instance
		*/
		inline uint32_t const & GetGlES3Name()const
		{
			return m_glName;
		}

		/** Retrieves the OpenGlES3 instance
		@return
			The instance
		*/
		inline uint32_t & GetGlES3Name()
		{
			return m_glName;
		}

		/** Retrieves the OpenGlES3 type name.
		@return
			The name.
		*/
		inline const char * GetGlES3TypeName()
		{
			return m_typeName;
		}

	protected:
		/** Effectively creates the object on GPU.
		@return
			false if not created successfully.
		 */
		inline bool DoCreate()
		{
			m_glName = CreatorHelper< CreateFunction >::Apply( m_creator );
			return m_glName != GlES3InvalidIndex;
		}

		/** Effectively destroys the object on GPU.
		 */
		inline void DoDestroy()
		{
			DestroyerHelper< DestroyFunction >::Apply( m_destroyer, m_glName );
			m_glName = GlES3InvalidIndex;
		}

	protected:
		//! The creation function
		CreateFunction m_creator;
		//! The destruction function
		DestroyFunction m_destroyer;
		//! The destruction function
		ValidatorFunction m_validator;
		//! The GPU object name
		uint32_t m_glName;
		//! The object type name
		const char * m_typeName;

#if !defined( NDEBUG )

		bool m_hasBeenValid{ false };

#endif
	};
}

#define IMPLEMENT_GL_OBJECT()

#endif
