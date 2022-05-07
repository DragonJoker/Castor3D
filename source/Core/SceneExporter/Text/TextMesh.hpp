/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextMesh_H___
#define ___CSE_TextMesh_H___

#include <Castor3D/Model/Mesh/Mesh.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor
{
	template<>
	class TextWriter< castor3d::Mesh >
		: public TextWriterT< castor3d::Mesh >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & subfolder );
		bool operator()( castor3d::Mesh const & material
			, castor::StringStream & file )override;

	private:
		String m_subfolder;
	};
}

#endif
