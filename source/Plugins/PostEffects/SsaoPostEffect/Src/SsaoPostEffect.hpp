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
#ifndef ___C3D_SsaoPostEffect___
#define ___C3D_SsaoPostEffect___

#include <PostEffect.hpp>

#ifndef _WIN32
#	define C3D_Ssao_API
#else
#	ifdef SsaoPostEffect_EXPORTS
#		define C3D_Ssao_API __declspec( dllexport )
#	else
#		define C3D_Ssao_API __declspec( dllimport )
#	endif
#endif

namespace Castor3D
{
	class SsaoPostEffect
		:	public PostEffect
	{
	public:
		SsaoPostEffect( RenderSystem * p_pRenderSystem );
		virtual ~SsaoPostEffect();

		virtual bool Initialise();
		virtual void Cleanup();
		virtual bool Apply( RenderTarget * p_pRenderTarget );
	};
}


#endif
