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
#ifndef ___GLSL_CONSTANTS_H___
#define ___GLSL_CONSTANTS_H___

#include "GlslKeywords.hpp"

namespace GLSL
{
	class ConstantsBase
	{
	public:
		GlslWriter_API static GLSL::ConstantsBase * Get( GlslWriterConfig const & p_rs );
		GlslWriter_API virtual Castor::String Matrices() = 0;
		GlslWriter_API virtual Castor::String Scene() = 0;
		GlslWriter_API virtual Castor::String Pass() = 0;
		GlslWriter_API virtual Castor::String Billboard() = 0;
	};

	class ConstantsStd : public ConstantsBase
	{
	public:
		GlslWriter_API virtual Castor::String Matrices();
		GlslWriter_API virtual Castor::String Scene();
		GlslWriter_API virtual Castor::String Pass();
		GlslWriter_API virtual Castor::String Billboard();
	};

	class ConstantsUbo : public ConstantsBase
	{
	public:
		GlslWriter_API virtual Castor::String Matrices();
		GlslWriter_API virtual Castor::String Scene();
		GlslWriter_API virtual Castor::String Pass();
		GlslWriter_API virtual Castor::String Billboard();
	};

	static ConstantsStd constantsStd;
	static ConstantsUbo constantsUbo;
}

#endif
