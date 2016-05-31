#include "BinaryExportTest.hpp"

#include <Engine.hpp>
#include <MeshManager.hpp>
#include <PluginManager.hpp>
#include <SceneManager.hpp>

#include <Animation/Animation.hpp>
#include <Animation/KeyFrame.hpp>
#include <Animation/SkeletonAnimationBone.hpp>
#include <Animation/SkeletonAnimationNode.hpp>
#include <Animation/SkeletonAnimationObject.hpp>
#include <Manager/ManagerView.hpp>
#include <Mesh/Importer.hpp>
#include <Mesh/Submesh.hpp>
#include <Mesh/Buffer/IndexBuffer.hpp>
#include <Mesh/Buffer/VertexBuffer.hpp>
#include <Mesh/Skeleton/Bone.hpp>
#include <Mesh/Skeleton/Skeleton.hpp>
#include <Miscellaneous/Parameter.hpp>
#include <Plugin/ImporterPlugin.hpp>
#include <Render/RenderLoop.hpp>
#include <Scene/SceneFileParser.hpp>

using namespace Castor;
using namespace Castor3D;

namespace Testing
{
	//*********************************************************************************************

	template<>
	inline String to_string< Mesh >( Mesh const & p_value )
	{
		return p_value.GetName();
	}

	template<>
	inline String to_string< Submesh >( Submesh const & p_value )
	{
		return String{};
	}

	template<>
	inline String to_string< Skeleton >( Skeleton const & p_value )
	{
		return String{};
	}

	template<>
	inline String to_string< Bone >( Bone const & p_value )
	{
		return p_value.GetName();
	}

	template<>
	inline String to_string< Animation >( Animation const & p_value )
	{
		return p_value.GetName();
	}

	template<>
	inline String to_string< AnimationObject >( AnimationObject const & p_value )
	{
		return p_value.GetName();
	}

	template<>
	inline String to_string< KeyFrame >( KeyFrame const & p_value )
	{
		return String{};
	}

	//*********************************************************************************************

	BinaryExportTest::BinaryExportTest( Engine & p_engine )
		: C3DTestCase{ "BinaryExportTest", p_engine }
	{
	}

	BinaryExportTest::~BinaryExportTest()
	{
	}

	void BinaryExportTest::DoRegisterTests()
	{
		DoRegisterTest( "BinaryExportTest::SimpleMesh", std::bind( &BinaryExportTest::SimpleMesh, this ) );
		DoRegisterTest( "BinaryExportTest::AnimatedMesh", std::bind( &BinaryExportTest::AnimatedMesh, this ) );
	}

	void BinaryExportTest::SimpleMesh()
	{
		String l_name = cuT( "TestMesh" );
		auto l_src = m_engine.GetMeshManager().Create( l_name, eMESH_TYPE_ICOSAHEDRON, { 20 }, { 20.0_r } );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		Path l_path{ cuT( "TestMesh.cmsh" ) };
		CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, BinaryFile{ l_path, File::eOPEN_MODE_WRITE } ) );

		auto l_dst = m_engine.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, BinaryFile{ l_path, File::eOPEN_MODE_READ } ) );

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_src.reset();
		l_dst.reset();
		m_engine.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
		m_engine.GetMeshManager().Remove( l_name );
	}

	void BinaryExportTest::AnimatedMesh()
	{
		SceneSPtr l_scene;
		SceneFileParser l_parser{ m_engine };
		CT_REQUIRE( l_parser.ParseFile( TEST_DATA_FOLDER / cuT( "Anim.zip" ) ) );
		CT_REQUIRE( l_parser.ScenesBegin() != l_parser.ScenesEnd() );
		l_scene = l_parser.ScenesBegin()->second;
		CT_REQUIRE( l_scene->GetMeshView().begin() != l_scene->GetMeshView().end() );
		String l_name = *l_scene->GetMeshView().begin();
		auto l_src = l_scene->GetMeshView().Find( l_name );

		for ( auto l_submesh : *l_src )
		{
			l_submesh->Initialise();
		}

		Path l_path{ cuT( "TestMesh.cmsh" ) };
		{
			CT_CHECK( BinaryWriter< Mesh >{}.Write( *l_src, BinaryFile{ l_path, File::eOPEN_MODE_WRITE } ) );
		}

		auto l_dst = m_engine.GetMeshManager().Create( l_name + cuT( "_imp" ), eMESH_TYPE_CUSTOM );
		{
			CT_CHECK( BinaryParser< Mesh >{}.Parse( *l_dst, BinaryFile{ l_path, File::eOPEN_MODE_READ } ) );
		}

		for ( auto l_submesh : *l_dst )
		{
			l_submesh->Initialise();
		}

		CT_EQUAL( *l_src, *l_dst );
		File::DeleteFile( l_path );
		l_dst.reset();
		m_engine.GetMeshManager().Remove( l_name + cuT( "_imp" ) );
		l_src.reset();
		m_engine.GetMeshManager().Remove( l_name );
		l_scene->GetMeshView().Remove( l_name );
		m_engine.GetRenderLoop().Cleanup();
		m_engine.GetSceneManager().Remove( l_scene->GetName() );
		l_scene->Cleanup();
	}

	bool BinaryExportTest::compare( Mesh const & p_a, Mesh const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetSubmeshCount(), p_b.GetSubmeshCount() );
		auto l_itA = p_a.begin();
		auto const l_endItA = p_a.end();
		auto l_itB = p_b.begin();
		auto const l_endItB = p_b.end();

		while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
		{
			l_return = CT_EQUAL( *( *( l_itA ) ), *( *( l_itB ) ) );
			++l_itA;
			++l_itB;
		}

		l_return &= CT_EQUAL( p_a.GetSkeleton() != nullptr, p_b.GetSkeleton() != nullptr );

		if ( l_return && p_a.GetSkeleton() )
		{
			l_return = CT_EQUAL( *p_a.GetSkeleton(), *p_b.GetSkeleton() );
		}

		return l_return;
	}

	bool BinaryExportTest::compare( Submesh const & p_a, Submesh const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetPointsCount(), p_b.GetPointsCount() ) };
		l_return &= CT_EQUAL( std::make_pair( p_a.GetVertexBuffer().data(), p_a.GetVertexBuffer().GetSize() )
							  , std::make_pair( p_b.GetVertexBuffer().data(), p_b.GetVertexBuffer().GetSize() ) );
		l_return &= CT_EQUAL( p_a.HasIndexBuffer(), p_b.HasIndexBuffer() );

		if ( l_return && p_a.HasIndexBuffer() )
		{
			l_return &= CT_EQUAL( std::make_pair( p_a.GetIndexBuffer().data(), p_a.GetIndexBuffer().GetSize() )
								  , std::make_pair( p_b.GetIndexBuffer().data(), p_b.GetIndexBuffer().GetSize() ) );
		}

		l_return &= CT_EQUAL( p_a.HasBonesBuffer(), p_b.HasBonesBuffer() );

		if ( l_return && p_a.HasBonesBuffer() )
		{
			l_return &= CT_EQUAL( std::make_pair( p_a.GetBonesBuffer().data(), p_a.GetBonesBuffer().GetSize() )
								  , std::make_pair( p_b.GetBonesBuffer().data(), p_b.GetBonesBuffer().GetSize() ) );
		}

		l_return &= CT_EQUAL( p_a.HasMatrixBuffer(), p_b.HasMatrixBuffer() );

		if ( l_return && p_a.HasMatrixBuffer() )
		{
			l_return &= CT_EQUAL( std::make_pair( p_a.GetMatrixBuffer().data(), p_a.GetMatrixBuffer().GetSize() )
								  , std::make_pair( p_b.GetMatrixBuffer().data(), p_b.GetMatrixBuffer().GetSize() ) );
		}

		return l_return;
	}

	bool BinaryExportTest::compare( Skeleton const & p_a, Skeleton const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetGlobalInverseTransform(), p_b.GetGlobalInverseTransform() ) };

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetBonesCount(), p_b.GetBonesCount() );
			auto l_itA = p_a.begin();
			auto const l_endItA = p_a.end();

			while ( l_return && l_itA != l_endItA )
			{
				auto l_boneA = *l_itA;
				auto l_boneB = p_b.FindBone( l_boneA->GetName() );
				CT_REQUIRE( l_boneB != nullptr );
				l_return = CT_EQUAL( *l_boneA, *l_boneB );
				++l_itA;
			}
		}

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetAnimations().size(), p_b.GetAnimations().size() );
			auto l_itA = p_a.GetAnimations().begin();
			auto const l_endItA = p_a.GetAnimations().end();
			auto l_itB = p_b.GetAnimations().begin();
			auto const l_endItB = p_b.GetAnimations().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				l_return = CT_EQUAL( l_itA->first, l_itB->first );
				l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool BinaryExportTest::compare( Bone const & p_a, Bone const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetParent() == nullptr, p_b.GetParent() == nullptr );

		if ( l_return && p_a.GetParent() )
		{
			l_return = CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		l_return &= CT_EQUAL( p_a.GetOffsetMatrix(), p_b.GetOffsetMatrix() );
		l_return &= CT_EQUAL( p_a.GetFinalTransformation(), p_b.GetFinalTransformation() );
		l_return &= CT_EQUAL( p_a.GetChildren().size(), p_b.GetChildren().size() );
		auto l_itA = p_a.GetChildren().begin();
		auto const l_endItA = p_a.GetChildren().end();
		auto l_itB = p_b.GetChildren().begin();
		auto const l_endItB = p_b.GetChildren().end();

		while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
		{
			l_return = CT_EQUAL( *( *( l_itA ) ), *( *( l_itB ) ) );
			++l_itA;
			++l_itB;
		}

		return l_return;
	}

	bool BinaryExportTest::compare( Animation const & p_a, Animation const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		l_return &= CT_EQUAL( p_a.GetObjects().size(), p_b.GetObjects().size() );
		auto l_itA = p_a.GetObjects().begin();
		auto const l_endItA = p_a.GetObjects().end();
		auto l_itB = p_b.GetObjects().begin();
		auto const l_endItB = p_b.GetObjects().end();

		while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
		{
			l_return = CT_EQUAL( l_itA->first, l_itB->first );
			l_return &= CT_EQUAL( *l_itA->second, *l_itB->second );
			++l_itA;
			++l_itB;
		}

		return l_return;
	}

	bool BinaryExportTest::compare( AnimationObject const & p_a, AnimationObject const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetName(), p_b.GetName() ) };
		l_return &= CT_EQUAL( p_a.GetInterpolationMode(), p_b.GetInterpolationMode() );
		l_return &= CT_EQUAL( p_a.GetNodeTransform(), p_b.GetNodeTransform() );
		l_return &= CT_EQUAL( p_a.GetLength(), p_b.GetLength() );
		l_return &= CT_EQUAL( p_a.GetType(), p_b.GetType() );
		l_return &= CT_EQUAL( p_a.GetParent() == nullptr, p_b.GetParent() == nullptr );

		if ( l_return && p_a.GetParent() )
		{
			l_return = CT_EQUAL( p_a.GetParent()->GetName(), p_b.GetParent()->GetName() );
		}

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetKeyFrames().size(), p_b.GetKeyFrames().size() );
			auto l_itA = p_a.GetKeyFrames().begin();
			auto const l_endItA = p_a.GetKeyFrames().end();
			auto l_itB = p_b.GetKeyFrames().begin();
			auto const l_endItB = p_b.GetKeyFrames().end();

			while ( l_return && l_itA != l_endItA )
			{
				l_return = CT_EQUAL( *l_itA, *l_itB );
				++l_itA;
				++l_itB;
			}
		}

		if ( l_return )
		{
			l_return = CT_EQUAL( p_a.GetChildren().size(), p_b.GetChildren().size() );
			auto l_itA = p_a.GetChildren().begin();
			auto const l_endItA = p_a.GetChildren().end();
			auto l_itB = p_b.GetChildren().begin();
			auto const l_endItB = p_b.GetChildren().end();

			while ( l_return && l_itA != l_endItA && l_itB != l_endItB )
			{
				auto l_objectA = *l_itA;
				auto l_objectB = *l_itB;
				CT_REQUIRE( l_objectA->GetType() == l_objectB->GetType() );
				l_return = CT_EQUAL( *l_objectA, *l_objectB );
				++l_itA;
				++l_itB;
			}
		}

		return l_return;
	}

	bool BinaryExportTest::compare( KeyFrame const & p_a, KeyFrame const & p_b )
	{
		bool l_return{ CT_EQUAL( p_a.GetRotate(), p_a.GetRotate() ) };
		l_return &= CT_EQUAL( p_a.GetTranslate(), p_b.GetTranslate() );
		l_return &= CT_EQUAL( p_a.GetScale(), p_b.GetScale() );
		l_return &= CT_EQUAL( p_a.GetTimeIndex(), p_b.GetTimeIndex() );
		return l_return;
	}

	//*********************************************************************************************
}
