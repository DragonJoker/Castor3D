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
#ifndef ___GLSL_VARIABLES_H___
#define ___GLSL_VARIABLES_H___

#include "GlslConstants.hpp"

namespace GlRender
{
	namespace GLSL
	{
		class VariablesBase
		{
		public:
			C3D_Gl_API static GLSL::VariablesBase * Get( OpenGl const & p_gl );
			C3D_Gl_API virtual Castor::String GetVertexOutMatrices()const = 0;
			C3D_Gl_API virtual Castor::String GetVertexMatrixCopy()const = 0;
			C3D_Gl_API virtual Castor::String GetPixelInMatrices()const = 0;
			C3D_Gl_API virtual Castor::String GetPixelMtxModelView()const = 0;
		};

		class VariablesUbo
			: public VariablesBase
		{
		public:
			C3D_Gl_API virtual Castor::String GetVertexOutMatrices()const;
			C3D_Gl_API virtual Castor::String GetVertexMatrixCopy()const;
			C3D_Gl_API virtual Castor::String GetPixelInMatrices()const;
			C3D_Gl_API virtual Castor::String GetPixelMtxModelView()const;
		};

		class VariablesStd
			: public VariablesBase
		{
		public:
			C3D_Gl_API virtual Castor::String GetVertexOutMatrices()const;
			C3D_Gl_API virtual Castor::String GetVertexMatrixCopy()const;
			C3D_Gl_API virtual Castor::String GetPixelInMatrices()const;
			C3D_Gl_API virtual Castor::String GetPixelMtxModelView()const;
		};

		static VariablesUbo variablesUbo;
		static VariablesStd variablesStd;
	}
}

#endif
