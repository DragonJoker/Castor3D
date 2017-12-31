/* See LICENSE file in root folder */
#ifndef ___GLT_GlTextureTest_H___
#define ___GLT_GlTextureTest_H___

#include "GlRenderSystemTestPrerequisites.hpp"

#include <Design/ArrayView.hpp>

#include <cstring>

namespace Testing
{
	class GlTextureTest
		: public GlTestCase
	{
	public:
		explicit GlTextureTest( castor3d::Engine & engine );
		virtual ~GlTextureTest();

		void upload( castor3d::TextureLayout & p_storage, castor::ArrayView< uint8_t > const & p_view );
		void download( castor3d::TextureLayout & p_storage, std::vector< uint8_t > & p_dst );
		void Compare( std::array< uint8_t, 8 * 8 * 3 > const & p_src, std::vector< uint8_t > const & p_dst );

	private:
		void doRegisterTests()override;

	private:
		void ImmutableStorage();
		void DirectStorage();
		void PboStorage();
		void GpuOnlyStorage();
		void TboStorage();
	};
}

#endif
