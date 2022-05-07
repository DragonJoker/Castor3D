/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextMaterial_H___
#define ___CSE_TextMaterial_H___

#include <Castor3D/Material/Material.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Material >
		: public TextWriterT< castor3d::Material >
	{
	public:
		explicit TextWriter( String const & tabs
			, Path const & folder
			, String const & subfolder );
		bool operator()( castor3d::Material const & material
			, castor::StringStream & file )override;

	private:
		Path m_folder;
		String m_subfolder;
	};
}

#endif
