/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___Cg_ModuleShader___
#define ___Cg_ModuleShader___

#if defined( _WIN32 )
#	if defined( ShaderProgramCg_EXPORTS )
#		define C3D_Cg_API __declspec(dllexport)
#	else
#		define C3D_Cg_API __declspec(dllimport)
#	endif
#else
#	define C3D_Cg_API
#endif

namespace CgShader
{
	template< typename T										> class CgOneFrameVariable;
	template< typename T, uint32_t Count						> class CgPointFrameVariable;
	template< typename T, uint32_t Rows,	uint32_t Columns	> class CgMatrixFrameVariable;

	class CgShaderObject;
	class CgShaderProgram;
	class CgFrameVariable;

	typedef std::shared_ptr<	CgFrameVariable				>	CgFrameVariablePtr;
	typedef std::shared_ptr<	CgShaderProgram				>	CgShaderProgramPtr;
	typedef std::shared_ptr<	CgShaderObject				>	CgShaderObjectPtr;

#if defined( Cg_Direct3D9 )
	template< typename T									> class CgD3D9OneFrameVariable;
	template< typename T, uint32_t Count					> class CgD3D9PointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns	> class CgD3D9MatrixFrameVariable;

	class CgD3D9ShaderProgram;
	class CgD3D9ShaderObject;
	class CgD3D9VertexShader;
	class CgD3D9FragmentShader;
	class CgD3D9GeometryShader;
	class CgD3D9FrameVariableBase;
	class CgD3D9IntFrameVariable;
	class CgD3D9RealFrameVariable;

	typedef std::shared_ptr<	CgD3D9ShaderProgram			>	CgD3D9ShaderProgramPtr;
	typedef std::shared_ptr<	CgD3D9VertexShader			>	CgD3D9VertexShaderPtr;
	typedef std::shared_ptr<	CgD3D9FragmentShader		>	CgD3D9FragmentShaderPtr;
	typedef std::shared_ptr<	CgD3D9GeometryShader		>	CgD3D9GeometryShaderPtr;
	typedef std::shared_ptr<	CgD3D9FrameVariableBase		>	CgD3D9FrameVariablePtr;

	typedef std::vector<	CgD3D9ShaderProgramPtr						>	CgD3D9ShaderProgramPtrArray;
	typedef std::map<		Castor::String,		CgD3D9FrameVariablePtr	>	CgD3D9FrameVariablePtrStrMap;
#endif

#if defined( Cg_Direct3D10 )
	template< typename T									> class CgD3D10OneFrameVariable;
	template< typename T, uint32_t Count					> class CgD3D10PointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns	> class CgD3D10MatrixFrameVariable;

	class CgD3D10ShaderProgram;
	class CgD3D10ShaderObject;
	class CgD3D10VertexShader;
	class CgD3D10FragmentShader;
	class CgD3D10GeometryShader;
	class CgD3D10FrameVariableBase;
	class CgD3D10IntFrameVariable;
	class CgD3D10RealFrameVariable;

	typedef std::shared_ptr<	CgD3D10ShaderProgram		>	CgD3D10ShaderProgramPtr;
	typedef std::shared_ptr<	CgD3D10VertexShader			>	CgD3D10VertexShaderPtr;
	typedef std::shared_ptr<	CgD3D10FragmentShader		>	CgD3D10FragmentShaderPtr;
	typedef std::shared_ptr<	CgD3D10GeometryShader		>	CgD3D10GeometryShaderPtr;
	typedef std::shared_ptr<	CgD3D10FrameVariableBase	>	CgD3D10FrameVariablePtr;

	typedef std::vector<	CgD3D10ShaderProgramPtr						>	CgD3D10ShaderProgramPtrArray;
	typedef std::map<		Castor::String,		CgD3D10FrameVariablePtr	>	CgD3D10FrameVariablePtrStrMap;
#endif

#if defined( Cg_Direct3D11 )
	template< typename T									> class CgD3D11OneFrameVariable;
	template< typename T, uint32_t Count					> class CgD3D11PointFrameVariable;
	template< typename T, uint32_t Rows, uint32_t Columns	> class CgD3D11MatrixFrameVariable;

	class CgD3D11ShaderProgram;
	class CgD3D11ShaderObject;
	class CgD3D11VertexShader;
	class CgD3D11FragmentShader;
	class CgD3D11GeometryShader;
	class CgD3D11FrameVariableBase;
	class CgD3D11IntFrameVariable;
	class CgD3D11RealFrameVariable;

	typedef std::shared_ptr<	CgD3D11ShaderProgram			>	CgD3D11ShaderProgramPtr;
	typedef std::shared_ptr<	CgD3D11VertexShader				>	CgD3D11VertexShaderPtr;
	typedef std::shared_ptr<	CgD3D11FragmentShader			>	CgD3D11FragmentShaderPtr;
	typedef std::shared_ptr<	CgD3D11GeometryShader			>	CgD3D11GeometryShaderPtr;
	typedef std::shared_ptr<	CgD3D11FrameVariableBase		>	CgD3D11FrameVariablePtr;

	typedef std::vector<	CgD3D11ShaderProgramPtr						>	CgD3D11ShaderProgramPtrArray;
	typedef std::map<		Castor::String,		CgD3D11FrameVariablePtr	>	CgD3D11FrameVariablePtrStrMap;
#endif

#if defined( Cg_OpenGL )
	template <typename T									> class CgGlOneFrameVariable;
	template <typename T, uint32_t Count					> class CgGlPointFrameVariable;
	template <typename T, uint32_t Rows, uint32_t Columns	> class CgGlMatrixFrameVariable;

	class CgGlShaderProgram;
	class CgGlShaderObject;
	class CgGlFrameVariableBase;

	typedef std::shared_ptr<	CgGlShaderProgram			>	CgGlShaderProgramPtr;
	typedef std::shared_ptr<	CgGlShaderObject			>	CgGlShaderObjectPtr;
	typedef std::shared_ptr<	CgGlFrameVariableBase		>	CgGlFrameVariablePtr;

	typedef std::vector<	CgShaderProgramPtr							>	CgShaderProgramPtrArray;
	typedef std::vector<	CgShaderObjectPtr							>	CgShaderObjectPtrArray;
	typedef std::map<		Castor::String,		CgFrameVariablePtr		>	CgFrameVariablePtrStrMap;

	typedef std::vector<	CgGlShaderProgramPtr						>	CgGlShaderProgramPtrArray;
	typedef std::vector<	CgGlShaderObjectPtr							>	CgGlShaderObjectPtrArray;
	typedef std::map<		Castor::String,		CgGlFrameVariablePtr	>	CgGlFrameVariablePtrStrMap;
#endif

	void CgCheckError( Castor::String const & p_strText );

#   if !defined( NDEBUG )
#	    define CheckCgError( x, txt ) { (x);CgCheckError( txt ); }
#   else
#	    define CheckCgError( x, txt )
#   endif
}

#endif
