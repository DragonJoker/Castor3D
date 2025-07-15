/*
See LICENSE file in root folder
*/
#ifndef ___CSE_TextMesh_H___
#define ___CSE_TextMesh_H___

#include <Castor3D/Model/Mesh/Mesh.hpp>

#include <CastorUtils/Data/TextWriter.hpp>

namespace c3d
{
	template<>
	class TextWriter< Mesh >
		: public TextWriterT< Mesh >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & subfolder );
		bool operator()( Mesh const & material
			, StringStream & file )override;

	private:
		String m_subfolder;
	};
}

#endif
