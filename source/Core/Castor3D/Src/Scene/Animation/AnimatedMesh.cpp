#include "AnimatedMesh.hpp"

#include "Animation/Animable.hpp"
#include "Animation/Mesh/MeshAnimation.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/Animation/Mesh/MeshAnimationInstance.hpp"
#include "Shader/MatrixFrameVariable.hpp"

using namespace Castor;

namespace Castor3D
{
	AnimatedMesh::AnimatedMesh( String const & p_name, Mesh & p_mesh )
		: AnimatedObject{ p_name }
		, m_mesh{ p_mesh }
	{
	}

	AnimatedMesh::~AnimatedMesh()
	{
	}

	AnimationInstanceSPtr AnimatedMesh::DoAddAnimation( String const & p_name )
	{
		MeshAnimationInstanceSPtr l_instance;
		auto l_it = m_animations.find( p_name );

		if ( l_it == m_animations.end() )
		{
			auto l_animation = std::static_pointer_cast< MeshAnimation >( m_mesh.GetAnimation( p_name ) );

			if ( l_animation )
			{
				l_instance = std::make_shared< MeshAnimationInstance >( *this, *l_animation );
				m_animations.insert( { p_name, l_instance } );
			}
		}

		return l_instance;
	}

	void AnimatedMesh::DoFillShader( Matrix4x4rFrameVariable & p_variable )
	{
	}
}
