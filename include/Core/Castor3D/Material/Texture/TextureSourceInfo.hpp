/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureSourceInfo_H___
#define ___C3D_TextureSourceInfo_H___

#include "TextureModule.hpp"

#include "Castor3D/Render/RenderModule.hpp"

#include <CastorUtils/Data/Path.hpp>

#include <ashespp/Image/ImageCreateInfo.hpp>

namespace castor3d
{
	class TextureSourceInfo
	{
	public:
		C3D_API TextureSourceInfo( SamplerRes sampler
			, castor::Path folder
			, castor::Path relative
			, bool allowCompression = true
			, bool generateMips = true );
		C3D_API TextureSourceInfo( SamplerRes sampler
			, RenderTargetSPtr renderTarget );

		SamplerRes sampler()const
		{
			return m_sampler;
		}

		RenderTargetSPtr renderTarget()const
		{
			return m_renderTarget;
		}

		castor::Path const & folder()const
		{
			CU_Require( !m_renderTarget );
			return m_folder;
		}

		castor::Path const & relative()const
		{
			CU_Require( !m_renderTarget );
			return m_relative;
		}

		bool allowCompression()const
		{
			CU_Require( !m_renderTarget );
			return m_allowCompression;
		}

		bool generateMips()const
		{
			CU_Require( !m_renderTarget );
			return m_generateMips;
		}

	private:
		SamplerRes m_sampler{};
		RenderTargetSPtr m_renderTarget{};
		castor::Path m_folder{};
		castor::Path m_relative{};
		bool m_allowCompression{};
		bool m_generateMips{};
	};
}

#endif
