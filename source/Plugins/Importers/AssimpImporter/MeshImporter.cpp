#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/MeshImporter.hpp"

#include "AssimpImporter/AssimpHelpers.hpp"

#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Model/Vertex.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/Face.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Design/ArrayView.hpp>

namespace c3d_assimp
{
	//*********************************************************************************************

	namespace meshes
	{
		template< typename aiMeshType >
		static castor3d::InterleavedVertexArray createVertexBuffer( aiMeshType const & aiMesh )
		{
			castor3d::InterleavedVertexArray vertices{ aiMesh.mNumVertices };
			uint32_t index{ 0u };

			for ( auto & vertex : vertices )
			{
				vertex.pos[0] = float( aiMesh.mVertices[index].x );
				vertex.pos[1] = float( aiMesh.mVertices[index].y );
				vertex.pos[2] = float( aiMesh.mVertices[index].z );
				++index;
			}

			if ( aiMesh.HasNormals() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.nml[0] = float( aiMesh.mNormals[index].x );
					vertex.nml[1] = float( aiMesh.mNormals[index].y );
					vertex.nml[2] = float( aiMesh.mNormals[index].z );
					++index;
				}
			}

			if ( aiMesh.HasTangentsAndBitangents() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.tan[0] = float( aiMesh.mTangents[index].x );
					vertex.tan[1] = float( aiMesh.mTangents[index].y );
					vertex.tan[2] = float( aiMesh.mTangents[index].z );
					++index;
				}
			}

			if ( aiMesh.HasTextureCoords( 0 ) )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.tex[0] = float( aiMesh.mTextureCoords[0][index].x );
					vertex.tex[1] = float( aiMesh.mTextureCoords[0][index].y );
					vertex.tex[2] = float( aiMesh.mTextureCoords[0][index].z );
					++index;
				}
			}

			return vertices;
		}

		static std::map< castor::String, aiMeshMorphAnim const * > findMorphAnims( uint32_t aiMeshIndex
			, uint32_t aiNumAnimMeshes
			, aiNode const & rootNode
			, castor::ArrayView< aiAnimation * > animations )
		{
			std::map< castor::String, aiMeshMorphAnim const * > result;

			for ( auto anim : animations )
			{
				auto morphChannels = castor::makeArrayView( anim->mMorphMeshChannels, anim->mNumMorphMeshChannels );
				auto morphIt = std::find_if( morphChannels.begin()
					, morphChannels.end()
					, [aiNumAnimMeshes, aiMeshIndex, &rootNode]( aiMeshMorphAnim const * morphChannel )
					{
						auto node = rootNode.FindNode( morphChannel->mName );
						bool res = node != nullptr
							&& morphChannel->mNumKeys > 0u
							&& morphChannel->mKeys->mNumValuesAndWeights == aiNumAnimMeshes
							&& morphChannel->mKeys[morphChannel->mNumKeys - 1u].mTime > 0.0;

						if ( res )
						{
							auto meshes = castor::makeArrayView( node->mMeshes, node->mNumMeshes );
							res = meshes.end() != std::find( meshes.begin()
								, meshes.end()
								, aiMeshIndex );
						}

						return res;
					} );

				if ( morphIt != morphChannels.end() )
				{
					result.emplace( anim->mName.C_Str(), *morphIt );
				}
			}

			return result;
		}

		static std::vector< castor3d::InterleavedVertexArray > gatherMeshAnimBuffers( castor3d::Submesh const & submesh
			, castor::ArrayView< aiAnimMesh * > animMeshes )
		{
			std::vector< castor3d::InterleavedVertexArray > result;
			auto const & cmapping = static_cast< castor3d::TriFaceMapping const & >( *submesh.getIndexMapping() );
			auto & srcPoints = submesh.getPoints();

			for ( auto aiAnimMesh : animMeshes )
			{
				auto points = meshes::createVertexBuffer( *aiAnimMesh );

				if ( aiAnimMesh->HasPositions() )
				{
					auto it = points.begin();

					for ( auto & point : srcPoints )
					{
						it->pos -= point.pos;
						++it;
					}
				}

				if ( aiAnimMesh->HasNormals() )
				{
					auto it = points.begin();

					for ( auto & point : srcPoints )
					{
						it->nml -= point.nml;
						++it;
					}
				}

				if ( aiAnimMesh->HasTextureCoords( 0u ) )
				{
					auto it = points.begin();

					for ( auto & point : srcPoints )
					{
						it->tex -= point.tex;
						++it;
					}
				}

				if ( aiAnimMesh->HasTangentsAndBitangents() )
				{
					auto it = points.begin();

					for ( auto & point : srcPoints )
					{
						it->tan -= point.tan;
						++it;
					}
				}
				if ( !aiAnimMesh->HasNormals()
					|| !aiAnimMesh->HasTextureCoords( 0u ) )
				{
					cmapping.computeTangentsFromNormals( points );
				}

				result.emplace_back( std::move( points ) );
			}

			return result;
		}

		static void fillKeyFrame( std::vector< castor3d::InterleavedVertexArray > const & meshAnimBuffers
			, castor::ArrayView< uint32_t > values
			, castor::ArrayView< double > weights
			, castor3d::Submesh const & submesh
			, castor3d::MeshAnimationKeyFrame & keyFrame )
		{
			castor3d::InterleavedVertexArray points{ submesh.getPoints() };
			auto valueIt = values.begin();
			auto weightIt = weights.begin();

			while ( valueIt != values.end() )
			{
				auto value = *valueIt;
				auto weight = float( *weightIt );
				CU_Require( value < meshAnimBuffers.size() );
				auto buffer = meshAnimBuffers[value];

				for ( size_t index = 0u; index < points.size(); ++index )
				{
					auto & point = points[index];
					auto & buf = buffer[index];
					point.pos += buf.pos * weight;
					point.nml += buf.nml * weight;
					point.tan += buf.tan * weight;
					point.tex += buf.tex * weight;
				}

				++valueIt;
				++weightIt;
			}

			keyFrame.addSubmeshBuffer( submesh, points );
		}

		static bool operator==( castor::Point3f const & lhs
			, castor::Point3f const & rhs )
		{
			return castor::point::distance( lhs, rhs ) < 0.0001;
		}

		static bool operator==( castor3d::InterleavedVertex const & lhs
			, castor3d::InterleavedVertex const & rhs )
		{
			return lhs.pos == rhs.pos
				&& lhs.nml == rhs.nml
				&& lhs.tan == rhs.tan
				&& lhs.tex == rhs.tex;
		}

		static bool operator==( castor3d::Submesh const & lhs
			, castor3d::Submesh const & rhs )
		{
			if ( lhs.getPointsCount() != rhs.getPointsCount()
				|| lhs.getIndexMapping()->getComponentsCount() != rhs.getIndexMapping()->getComponentsCount()
				|| lhs.getIndexMapping()->getCount() != rhs.getIndexMapping()->getCount() )
			{
				return false;
			}

			auto lhsIt = lhs.getPoints().begin();
			auto rhsIt = rhs.getPoints().begin();
			bool result = true;

			while ( result && lhsIt != lhs.getPoints().end() )
			{
				result = *lhsIt == *rhsIt;
				++lhsIt;
				++rhsIt;
			}

			return result;
		}

		static bool operator==( castor3d::Mesh const & lhs
			, castor3d::Mesh const & rhs )
		{
			if ( lhs.getName() == rhs.getName() )
			{
				return true;
			}

			if ( lhs.getSubmeshCount() != rhs.getSubmeshCount() )
			{
				return false;
			}

			auto lhsIt = lhs.begin();
			auto rhsIt = rhs.begin();
			bool result = true;

			while ( result && lhsIt != lhs.end() )
			{
				auto & lhsSubmesh = **lhsIt;
				auto & rhsSubmesh = **rhsIt;
				result = lhsSubmesh == rhsSubmesh;
				++lhsIt;
				++rhsIt;
			}

			return result;
		}
	}

	using meshes::operator==;

	//*********************************************************************************************

	MeshesImporter::MeshesImporter( castor3d::MeshImporter & importer
		, MaterialImporter const & materials
		, SkeletonImporter const & skeletons
		, castor3d::MeshPtrStrMap & meshes )
		: m_importer{ importer }
		, m_materials{ materials }
		, m_skeletons{ skeletons }
		, m_meshes{ meshes }
	{
	}

	MeshIndices MeshesImporter::import( castor::String const & prefix
		, castor::Path const & fileName
		, aiScene const & aiScene
		, castor3d::Mesh & mesh )
	{
		m_prefix = prefix;
		m_fileName = fileName;
		return doProcessMeshAndAnims( aiScene
			, castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes )
			, mesh );
	}

	std::map< uint32_t, MeshData * > MeshesImporter::import( castor::String const & prefix
		, castor::Path const & fileName
		, aiScene const & aiScene
		, castor3d::Scene & scene )
	{
		m_prefix = prefix;
		m_fileName = fileName;
		doProcessMeshesAndAnims( aiScene
			, scene
			, castor::makeArrayView( aiScene.mMeshes, aiScene.mNumMeshes ) );
		return doRemoveDuplicateMeshes( scene );
	}

	std::map< uint32_t, MeshData * > MeshesImporter::doRemoveDuplicateMeshes( castor3d::Scene & scene )
	{
		std::map< uint32_t, MeshData * > result;
		std::vector< MeshData * > meshes;

		for ( auto & mesh : m_loadedMeshes )
		{
			auto it = std::find_if( meshes.begin()
				, meshes.end()
				, [&mesh]( MeshData const * lookup )
				{
					return *lookup->mesh == *mesh.mesh;
				} );

			if ( it == meshes.end() )
			{
				meshes.push_back( &mesh );
				result.emplace( *mesh.aiMeshIndices.begin(), &mesh );
			}
			else
			{
				auto & meshRes = **mesh.mesh;
				meshRes.cleanup();
				mesh.mesh.reset();
				result.emplace( *mesh.aiMeshIndices.begin(), *it );

				for ( auto matIt : mesh.submeshes )
				{
					( *it )->submeshes.emplace( matIt );
				}
			}
		}

		for ( auto & mesh : meshes )
		{
			for ( auto submesh : *mesh->mesh )
			{
				scene.getListener().postEvent( castor3d::makeGpuInitialiseEvent( *submesh ) );
			}

			mesh->lmesh = scene.getMeshCache().add( mesh->mesh->getName()
				, mesh->mesh
				, true );

			if ( !mesh->mesh )
			{
				mesh->mesh = mesh->lmesh.lock();
			}

			m_meshes.emplace( mesh->mesh->getName(), mesh->mesh );
		}

		return result;
	}

	void MeshesImporter::doProcessMeshesAndAnims( aiScene const & aiScene
		, castor3d::Scene & scene
		, castor::ArrayView< aiMesh * > aiMeshes )
	{
		uint32_t index = 0u;

		for ( auto & aiMesh : aiMeshes )
		{
			castor::String name = normalizeName( m_prefix + aiMesh->mName.C_Str() );

			if ( name.size() > 150u )
			{
				name = m_prefix + m_fileName.getFileName() + "-" + castor::string::toString( m_registeredMeshes.size() );
			}

			auto regIt = m_registeredMeshes.find( name );

			if ( regIt == m_registeredMeshes.end() )
			{
				auto skeleton = m_skeletons.getSkeleton( *aiMesh );
				regIt = std::find_if( m_registeredMeshes.begin()
					, m_registeredMeshes.end()
					, [&skeleton]( castor3d::MeshPtrStrMap::value_type const & lookup )
					{
						return lookup.second.lock()->getSkeleton() == skeleton;
					} );
			}

			castor3d::MeshRes mesh;
			castor3d::MeshResPtr lmesh;

			if ( regIt != m_registeredMeshes.end() )
			{
				lmesh = regIt->second;
				mesh = lmesh.lock();
			}
			else
			{
				mesh = scene.getMeshCache().create( name, scene );
				lmesh = mesh;
				m_registeredMeshes.emplace( name, lmesh );
			}

			if ( !doProcessMeshAndAnims( aiScene, castor::makeArrayView( &aiMesh, 1u ), *mesh ).empty() )
			{
				if ( regIt == m_registeredMeshes.end() )
				{
					// new mesh
					SubmeshData submesh{ { mesh->getSubmesh( mesh->getSubmeshCount() - 1u )->getDefaultMaterial() } };
					m_loadedMeshes.push_back( { mesh
						, lmesh
						, { index }
						, { { index, submesh } } } );
				}
				else
				{
					// additional submesh
					auto it = std::find_if( m_loadedMeshes.begin()
						, m_loadedMeshes.end()
						, [&mesh]( MeshData const & lookup )
						{
							return lookup.mesh == mesh;
						} );
					CU_Require( it != m_loadedMeshes.end() );
					it->aiMeshIndices.insert( index );
					it->submeshes.begin()->second.materials.push_back( mesh->getSubmesh( mesh->getSubmeshCount() - 1u )->getDefaultMaterial() );
				}
			}

			++index;
		}
	}

	MeshIndices MeshesImporter::doProcessMeshAndAnims( aiScene const & aiScene
		, castor::ArrayView< aiMesh * > aiMeshes
		, castor3d::Mesh & mesh )
	{
		bool create = true;
		uint32_t aiMeshIndex = 0u;
		castor3d::SubmeshSPtr submesh;
		castor3d::SkeletonSPtr skeleton;
		MeshIndices result;
		auto it = result.begin();

		for ( auto aiMesh : aiMeshes )
		{
			if ( create )
			{
				submesh = mesh.createSubmesh();
				result.emplace( aiMeshIndex, submesh ).first;
			}

			auto curSkeleton = m_skeletons.getSkeleton( *aiMesh );
			CU_Require( !skeleton || curSkeleton == skeleton );
			skeleton = curSkeleton;

			if ( aiMesh->HasFaces() && aiMesh->HasPositions() )
			{
				create = doProcessMesh( *mesh.getScene(), mesh, skeleton, *aiMesh, aiMeshIndex, aiScene, *submesh );

				if ( aiMesh->mNumAnimMeshes )
				{
					doProcessAnim( aiScene, *aiMesh, aiMeshIndex, mesh, *submesh );
				}
			}

			++aiMeshIndex;
		}

		if ( skeleton )
		{
			mesh.setSkeleton( skeleton );
		}

		return result;
	}

	bool MeshesImporter::doProcessMesh( castor3d::Scene & scene
		, castor3d::Mesh & mesh
		, castor3d::SkeletonSPtr skeleton
		, aiMesh const & aiMesh
		, uint32_t aiMeshIndex
		, aiScene const & aiScene
		, castor3d::Submesh & submesh )
	{
		bool result = false;
		castor3d::MaterialResPtr material;
		castor3d::log::info << cuT( "Mesh found: [" ) << m_prefix + aiMesh.mName.C_Str() << cuT( "]" ) << std::endl;

		if ( aiMesh.mMaterialIndex < aiScene.mNumMaterials )
		{
			material = m_materials.getMaterial( aiMesh.mMaterialIndex );
		}

		if ( material.lock() )
		{
			auto faces = castor::makeArrayView( aiMesh.mFaces, aiMesh.mNumFaces );
			auto count = uint32_t( std::count_if( faces.begin()
				, faces.end()
				, []( aiFace const & face )
				{
					return face.mNumIndices == 3
						|| face.mNumIndices == 4;
				} ) );

			if ( count > 0 )
			{
				submesh.setDefaultMaterial( material.lock().get() );
				submesh.addPoints( meshes::createVertexBuffer( aiMesh ) );

				if ( aiMesh.HasBones() && skeleton )
				{
					std::vector< castor3d::VertexBoneData > bonesData( aiMesh.mNumVertices );
					doFillBonesData( *skeleton, castor::makeArrayView( aiMesh.mBones, aiMesh.mNumBones ), bonesData );
					auto bones = std::make_shared< castor3d::BonesComponent >( submesh );
					bones->addBoneDatas( bonesData );
					submesh.addComponent( bones );
				}

				auto mapping = std::make_shared< castor3d::TriFaceMapping >( submesh );

				for ( auto face : faces )
				{
					if ( face.mNumIndices == 3 )
					{
						mapping->addFace( face.mIndices[0], face.mIndices[2], face.mIndices[1] );
					}
					else if ( face.mNumIndices == 4 )
					{
						mapping->addQuadFace( face.mIndices[0], face.mIndices[2], face.mIndices[1], face.mIndices[2] );
					}
				}

				if ( !aiMesh.HasNormals() )
				{
					mapping->computeNormals( true );
				}
				else if ( !aiMesh.HasTangentsAndBitangents() )
				{
					mapping->computeTangentsFromNormals();
				}

				submesh.setIndexMapping( mapping );
				result = true;
			}
		}

		return result;
	}

	void MeshesImporter::doProcessAnim( aiScene const & aiScene
		, aiMesh const & aiMesh
		, uint32_t aiMeshIndex
		, castor3d::Mesh & mesh
		, castor3d::Submesh & submesh )
	{
		auto anims = meshes::findMorphAnims( aiMeshIndex
			, aiMesh.mNumAnimMeshes
			, *aiScene.mRootNode
			, castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) );

		if ( !anims.empty() )
		{
			auto meshAnimBuffers = meshes::gatherMeshAnimBuffers( submesh
				, castor::makeArrayView( aiMesh.mAnimMeshes, aiMesh.mNumAnimMeshes ) );

			for ( auto anim : anims )
			{
				castor::String name{ normalizeName( m_prefix + castor::string::stringCast< castor::xchar >( anim.first ) ) };
				castor3d::log::info << cuT( "  Mesh Animation found: [" ) << name << cuT( "] " ) << std::endl;
				auto & animation = mesh.createAnimation( name );
				castor3d::MeshAnimationSubmesh animSubmesh{ animation, submesh };
				animation.addChild( std::move( animSubmesh ) );

				if ( anim.second->mKeys->mTime > 0.0 )
				{
					auto it = animation.find( 0_ms );
					castor3d::MeshAnimationKeyFrame * kf{};

					if ( it == animation.end() )
					{
						auto keyFrame = std::make_unique< castor3d::MeshAnimationKeyFrame >( animation, 0_ms );
						kf = keyFrame.get();
						animation.addKeyFrame( std::move( keyFrame ) );
					}
					else
					{
						kf = &static_cast< castor3d::MeshAnimationKeyFrame & >( **it );
					}

					kf->addSubmeshBuffer( submesh, submesh.getPoints() );
				}

				for ( auto & morphKey : castor::makeArrayView( anim.second->mKeys, anim.second->mNumKeys ) )
				{
					auto timeIndex = castor::Milliseconds{ uint64_t( morphKey.mTime ) };
					auto it = animation.find( timeIndex );
					castor3d::MeshAnimationKeyFrame * kf{};

					if ( it == animation.end() )
					{
						auto keyFrame = std::make_unique< castor3d::MeshAnimationKeyFrame >( animation, timeIndex );
						kf = keyFrame.get();
						animation.addKeyFrame( std::move( keyFrame ) );
					}
					else
					{
						kf = &static_cast< castor3d::MeshAnimationKeyFrame & >( **it );
					}

					meshes::fillKeyFrame( meshAnimBuffers
						, castor::makeArrayView( morphKey.mValues, morphKey.mNumValuesAndWeights )
						, castor::makeArrayView( morphKey.mWeights, morphKey.mNumValuesAndWeights )
						, submesh
						, *kf );
				}
			}
		}
	}

	void MeshesImporter::doFillBonesData( castor3d::Skeleton & skeleton
		, castor::ArrayView< aiBone * > aiBones
		, std::vector< castor3d::VertexBoneData > & arrayVertices )
	{
		for ( auto aiBone : aiBones )
		{
			castor::String name = m_prefix + castor::string::stringCast< castor::xchar >( aiBone->mName.C_Str() );
			auto node = skeleton.findNode( name );
			CU_Require( node && node->getType() == castor3d::SkeletonNodeType::eBone );
			auto bone = &static_cast< castor3d::BoneNode & >( *node );

			for ( auto weight : castor::makeArrayView( aiBone->mWeights, aiBone->mNumWeights ) )
			{
				arrayVertices[weight.mVertexId].addBoneData( bone->getId(), weight.mWeight );
			}
		}
	}

	//*********************************************************************************************
}
