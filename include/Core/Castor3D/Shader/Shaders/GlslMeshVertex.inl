#include "Castor3D/Shader/Shaders/GlslMeshVertex.hpp"

#include <ShaderWriter/Intrinsics/Intrinsics.hpp>

namespace castor3d::shader
{
	template< ast::var::Flag FlagT >
	MeshVertexT< FlagT >::MeshVertexT( sdw::ShaderWriter & writer
		, sdw::expr::ExprPtr expr
		, bool enabled )
		: MeshVertexBase{ writer, castor::move( expr ), enabled }
	{
	}

	template< ast::var::Flag FlagT >
	ast::type::IOStructPtr MeshVertexT< FlagT >::makeIOType( ast::type::TypesCache & cache
		, sdw::EntryPoint entryPoint
		, SubmeshShaders const & submeshShaders )
	{
		return MeshVertexBase::makeIOType( cache, entryPoint, submeshShaders, FlagT );
	}

	template< ast::var::Flag FlagT >
	ast::type::BaseStructPtr MeshVertexT< FlagT >::makeType( ast::type::TypesCache & cache
		, SubmeshShaders const & submeshShaders )
	{
		return MeshVertexBase::makeType( cache, submeshShaders );
	}
}
