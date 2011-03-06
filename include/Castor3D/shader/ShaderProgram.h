/*
This source file is part of Castor3D (http://dragonjoker.co.cc

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_ShaderProgram___
#define ___C3D_ShaderProgram___

#include "../Prerequisites.h"
#include "../render_system/RenderSystem.h"
#include "FrameVariable.h"
#include "ShaderObject.h"

namespace Castor3D
{
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.2
	\date 24/01/2011
	*/
	class C3D_API ShaderProgramBase
	{
	public:
		typedef enum
		{
			eGlslShader,
			eHlslShader,
			eCgShader,
		}
		eSHADER_TYPE;

	protected:
		bool m_isLinked;					//!< Tells if the program is linked
		bool m_bError;
		bool m_usesGeometryShader;			//!< Tells if the program uses a geometry shader
		eSHADER_TYPE m_eType;
		bool m_enabled;						//!< Tells if the shader is enabled

	public:
		/**
		 * Constructor
		 */
		ShaderProgramBase( eSHADER_TYPE p_eType)
			:	m_isLinked( false)
			,	m_bError( false)
			,	m_usesGeometryShader( false)
			,	m_enabled( true)
			,	m_eType( p_eType)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~ShaderProgramBase()
		{
		}
		/**
		 * Initialises the program
		 */
		virtual void Initialise()=0;
		/**
		 * Cleans up the program
		 */
		virtual void Cleanup()=0;
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;

		/**@name Inherited methods from Textable */
		//@{
		virtual bool Write( File & p_file)const
		{
			return true;
		}
		virtual bool Read( File & p_file)
		{
			return false;
		}
		//@}

		/**@name Inherited methods from Serialisable */
		//@{
		virtual bool Save( File & p_file)const
		{
			return true;
		}
		virtual bool Load( File & p_file)
		{
			return true;
		}
		//@}

		/**@name Accessors */
		//@{
		inline void 	UsesGeometryShader	( bool p_use)	{ m_usesGeometryShader = p_use;}
		inline void 	Enable				()				{ m_enabled = true; }
		inline void 	Disable				()				{ m_enabled = false; }
		inline bool 		UsesGeometryShader	()const	{ return m_usesGeometryShader; }
		inline bool			IsLinked			()const { return m_isLinked; }
		inline eSHADER_TYPE	GetType				()const	{ return m_eType; }
		//@}
	};
	/*!
	\author Sylvain DOREMUS
	\version 0.6.1.2
	\date 24/01/2011
	*/
	template <class ShObj>
	class C3D_API ShaderProgram : public ShaderProgramBase
	{
	private:
		typedef shared_ptr<ShObj> ShObjPtr;

	protected:
		ShObjPtr m_pShaders[eNbShaderTypes];
		FrameVariablePtrStrMap m_mapFrameVariables;

	public:
		/**
		 * Constructor
		 *@param p_vertexShaderFile : [in] The vertex shader file
		 *@param p_fragmentShaderFile : [in] The fragment shader file
		 *@param p_geometryShaderFile : [in] The geometry shader file
		 */
		ShaderProgram( eSHADER_TYPE p_eType, const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile)
			:	ShaderProgramBase( p_eType)
		{
			m_pShaders[eVertexShader] = ShObjPtr( RenderSystem::CreateVertexShader<ShObj>());
			m_pShaders[eVertexShader]->SetParent( this);
			m_pShaders[ePixelShader] = ShObjPtr( RenderSystem::CreateFragmentShader<ShObj>());
			m_pShaders[ePixelShader]->SetParent( this);

			if (RenderSystem::HasGeometryShader())
			{
				m_pShaders[eGeometryShader] = ShObjPtr( RenderSystem::CreateGeometryShader<ShObj>());
				m_pShaders[eGeometryShader]->SetParent( this);
			}

			SetVertexFile( p_vertexShaderFile);
			SetFragmentFile( p_fragmentShaderFile);
			SetGeometryFile( p_geometryShaderFile);
		}
		/**
 		 * Default Constructor
		 */
		ShaderProgram( eSHADER_TYPE p_eType)
			:	ShaderProgramBase( p_eType)
		{
		}
		/**
		 * Destructor
		 */
		virtual ~ShaderProgram()
		{
			for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
			{
				m_pShaders[i].reset();
			}

			m_mapFrameVariables.clear();
		}
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 *@param p_pObject : [in] The target shader object
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable, ShObjPtr p_pObject)
		{
			if (p_pVariable != NULL)
			{
				if (m_mapFrameVariables.find( p_pVariable->GetName()) == m_mapFrameVariables.end())
				{
					m_mapFrameVariables.insert( FrameVariablePtrStrMap::value_type( p_pVariable->GetName(), p_pVariable));
				}
			}

			p_pObject->AddFrameVariable( p_pVariable);
 		}
		/**
		 * Adds a uniform variable to pass to the shader objects
		 *@param p_pVariable : the variable to pass
		 *@param p_eType : [in] The target shader object type
		 */
		virtual void AddFrameVariable( FrameVariablePtr p_pVariable, eSHADER_PROGRAM_TYPE p_eType)
		{
			CASTOR_ASSERT( p_eType > eShaderNone && p_eType < eNbShaderTypes);
			AddFrameVariable( p_pVariable, m_pShaders[p_eType]);
		}
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup()
		{
			for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
			{
				if (m_pShaders[i] != NULL)
				{
					m_pShaders[i]->DestroyProgram();
				}
			}

			m_mapFrameVariables.clear();
		}
		/** 
		 * Initialises the program
		 */
		virtual void Initialise()
		{
			m_usesGeometryShader = m_pShaders[eGeometryShader] != NULL && ! m_pShaders[eGeometryShader]->GetSource().empty();

			for (int i = eVertexShader ; i < eNbShaderTypes ; i++)
			{
				if (m_pShaders[i] != NULL && ! m_pShaders[i]->GetSource().empty())
				{
					m_pShaders[i]->CreateProgram();

					if ( ! m_pShaders[i]->Compile())
					{
						Logger::LogError( CU_T( "ShaderProgram :: LoadfromMemory - ***COMPILER ERROR"));
						Cleanup();
						return;
					}
				}
			}

			if ( ! Link())
			{
				Logger::LogError( CU_T( "ShaderProgram :: LoadfromMemory - **LINKER ERROR"));
				Cleanup();
				return;
			}

			Logger::LogMessage( CU_T( "ShaderProgram :: LoadfromMemory - ***Program Linked successfully"));
		}
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;
		/**
		 * Loads and sets the vertex program to the one contained in the file
		 *@param p_strFile : The file address
		 */
		void SetVertexFile( const String & p_strFile)
		{
			_setProgramFile( p_strFile, eVertexShader);
		}
		/**
		 * Loads and sets the fragment program to the one contained in the file
		 *@param p_strFile : The file address
		 */
		void SetFragmentFile( const String & p_strFile)
		{
			_setProgramFile( p_strFile, ePixelShader);
		}
		/**
		 * Loads and sets the geometry program to the one contained in the file
		 *@param p_strFile : The file address
		 */
		void SetGeometryFile( const String & p_strFile)
		{
			if (RenderSystem::HasGeometryShader())
			{
				_setProgramFile( p_strFile, eGeometryShader);
			}
		}
		/**
		 * Loads and sets the vertex program to the one contained in the text
		 *@param p_strContent : The shader code
		 */
		void SetVertexText( const String & p_strContent)
		{
			_setProgramText( p_strContent, eVertexShader);
		}
		/**
		 * Loads and sets the fragment program to the one contained in the text
		 *@param p_strContent : The shader code
		 */
		void SetFragmentText( const String & p_strContent)
		{
			_setProgramText( p_strContent, ePixelShader);
		}
		/**
		 * Loads and sets the geometry program to the one contained in the text
		 *@param p_strContent : The shader code
		 */
		void SetGeometryText( const String & p_strContent)
		{
			if (RenderSystem::HasGeometryShader())
			{
				_setProgramText( p_strContent, eGeometryShader);
			}
		}
		/**
		 * Retrieves the uniform variables the program holds
		 *@return the map of variables held by the program
		 */
		FrameVariablePtrStrMap GetFrameVariables()const { return m_mapFrameVariables; }

	public:
		inline ShObjPtr		GetProgram			( eSHADER_PROGRAM_TYPE p_eType)const	{ return m_pShaders[p_eType]; }
		inline ShObjPtr		GetVertexProgram	()const 								{ return m_pShaders[eVertexShader]; }
		inline ShObjPtr		GetFragmentProgram	()const 								{ return m_pShaders[ePixelShader]; }
		inline ShObjPtr		GetGeometryProgram	()const 								{ return m_pShaders[eGeometryShader]; }

	private:
		void _setProgramFile( const String & p_strFile, eSHADER_PROGRAM_TYPE p_eType)
		{
			m_pShaders[eVertexShader]->SetFile( p_strFile);
			m_isLinked = false;
			m_bError = false;
		}
		void _setProgramText( const String & p_strFile, eSHADER_PROGRAM_TYPE p_eType)
		{
			m_pShaders[p_eType]->SetFile( C3DEmptyString);
			m_pShaders[p_eType]->SetSource( p_strFile);
			m_isLinked = false;
			m_bError = false;
		}
	};
	//! GLSL Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API GlslShaderProgram : public ShaderProgram<GlslShaderObject>, public MemoryTraced<GlslShaderProgram>
	{
	protected:
		friend class ShaderManager;

	public:
		/**
		 * Constructor
		 *@param p_vertexShaderFile : [in] The vertex shader file
		 *@param p_fragmentShaderFile : [in] The fragment shader file
		 *@param p_geometryShaderFile : [in] The geometry shader file
		 */
		GlslShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		GlslShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~GlslShaderProgram();
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup();
		/** 
		 * Initialises the program
		 */
		virtual void Initialise();
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;

		inline GlslShaderObjectPtr	GetProgram( eSHADER_PROGRAM_TYPE p_eType)const	{ return ShaderProgram<GlslShaderObject>::GetProgram( p_eType); }
	};
	//! HLSL Shader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API HlslShaderProgram : public ShaderProgram<HlslShaderObject>, public MemoryTraced<HlslShaderProgram>
	{
	protected:
		friend class ShaderManager;

	public:
		/**
		 * Constructor
		 *@param p_vertexShaderFile : [in] The vertex shader file
		 *@param p_fragmentShaderFile : [in] The fragment shader file
		 *@param p_geometryShaderFile : [in] The geometry shader file
		 */
		HlslShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		HlslShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~HlslShaderProgram();
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup();
		/** 
		 * Initialises the program
		 */
		virtual void Initialise();
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;

		inline HlslShaderObjectPtr	GetProgram( eSHADER_PROGRAM_TYPE p_eType)const	{ return ShaderProgram<HlslShaderObject>::GetProgram( p_eType); }
	};
	//! CgShader program representation
	/*!
	A shader programs holds it's geometry, fragment and vertex shader objects
	\author Sylvain DOREMUS
	\date 14/02/2010
	*/
	class C3D_API CgShaderProgram : public ShaderProgram<CgShaderObject>
	{
	protected:
		CGcontext m_cgContext;	//!< Cg context, used for programs creation

	public:
		/**
		 * Constructor
		 *@param p_vertexShaderFile : [in] The vertex shader file
		 *@param p_fragmentShaderFile : [in] The fragment shader file
		 *@param p_geometryShaderFile : [in] The geometry shader file
		 */
		CgShaderProgram( const String & p_vertexShaderFile, const String & p_fragmentShaderFile, const String & p_geometryShaderFile);
		CgShaderProgram();
		/**
		 * Destructor
		 */
		virtual ~CgShaderProgram();
		/**
		 * Cleans the program up
		 */
		virtual void Cleanup();
		/** 
		 * Initialises the program
		 */
		virtual void Initialise();
		/**
		 * Links all Shaders held by the program
		 */
		virtual bool Link()=0;
		/**
		 * Begins the program
		 */
		virtual void Begin()=0;
		/**
		 * Pass all the variables to the shader objects
		 */
		virtual void ApplyAllVariables()=0;
		/**
		 * Ends the program
		 */
		virtual void End()=0;
		/**
		 * Retrieves the uniform variables the program holds
		 *@return the map of variables held by the program
		 */
		FrameVariablePtrStrMap GetFrameVariables()const { return m_mapFrameVariables; }

		inline CGcontext			GetContext					()const						 			{ return m_cgContext; }

		inline CgShaderObjectPtr	GetProgram( eSHADER_PROGRAM_TYPE p_eType)const	{ return ShaderProgram<CgShaderObject>::GetProgram( p_eType); }
	};
}

#endif
