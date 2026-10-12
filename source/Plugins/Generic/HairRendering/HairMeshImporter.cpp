#include "HairRendering/HairMeshImporter.hpp"

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include "HairRendering/cyHairFile.h"
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp>
#include <Castor3D/Scene/Scene.hpp>

namespace hair
{
	MeshImporter::MeshImporter( c3d::Engine & engine
		, c3d::MeshImportConfig const & config )
		: c3d::MeshImporter{ engine, cuT( "Hair" ), config }
	{
	}

	bool MeshImporter::doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )
	{
		auto const & materialName = mesh.getName();
		auto & scene = *mesh.getScene();
		auto material = scene.tryFindMaterial( materialName );
		if ( !material )
		{
			auto mat = getOwner()->createMaterial( materialName
				, *getOwner()
				, getOwner()->getDefaultLightingModel() );
			material = scene.addMaterial( materialName, mat, true );
			material->createPass();
		}

		auto pass = material->getPass( 0 );
		pass->createComponent< c3d::TwoSidedComponent >()->setTwoSided( true );

		auto submesh = mesh.createSubmesh();
		submesh->setDefaultMaterial( material );
		submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );

		// Open the file and read the header
		cy::HairFile hairfile;
		auto filename = c3d::toUtf8( m_file->getFileName() );
		auto result = hairfile.LoadFromFile( filename.c_str() );

		// Check for errors
		if ( result < 0 )
		{
			switch ( result )
			{
			case CY_HAIR_FILE_ERROR_CANT_OPEN_FILE:
				c3d::log::error << "Error: Cannot open hair file!\n";
				break;
			case CY_HAIR_FILE_ERROR_CANT_READ_HEADER:
				c3d::log::error << "Error: Cannot read hair file header!\n";
				break;
			case CY_HAIR_FILE_ERROR_WRONG_SIGNATURE:
				c3d::log::error << "Error: File has wrong signature!\n";
				break;
			case CY_HAIR_FILE_ERROR_READING_SEGMENTS:
				c3d::log::error << "Error: Cannot read hair segments!\n";
				break;
			case CY_HAIR_FILE_ERROR_READING_POINTS:
				c3d::log::error << "Error: Cannot read hair points!\n";
				break;
			case CY_HAIR_FILE_ERROR_READING_COLORS:
				c3d::log::error << "Error: Cannot read hair colors!\n";
				break;
			case CY_HAIR_FILE_ERROR_READING_THICKNESS:
				c3d::log::error << "Error: Cannot read hair thickness!\n";
				break;
			case CY_HAIR_FILE_ERROR_READING_TRANSPARENCY:
				c3d::log::error << "Error: Cannot read hair transparency!\n";
				break;
			default:
				c3d::log::error << "Error: Unknown error!\n";
				break;
			}

			return false;
		}

		c3d::log::info << "Hair file \"" << filename << "\" loaded.\n";
		auto pointCount = c3d::u32( hairfile.GetHeader().point_count );
		submesh->createComponent< c3d::DefaultRenderComponent >();

		// Copy positions
		if ( hairfile.GetPointsArray() )
		{
			auto & positions = submesh->createComponent< c3d::PositionsComponent >()->getData().getData();
			positions.resize( pointCount );
			auto src = std::bit_cast< c3d::Point3f * >( hairfile.GetPointsArray() );
			std::copy( src, src + pointCount, positions.begin() );
		}

		// Copy colours, if present
		if ( hairfile.GetColorsArray() )
		{
			auto & colours = submesh->createComponent< c3d::ColoursComponent >()->getData().getData();
			colours.resize( pointCount );
			auto src = std::bit_cast< c3d::Point3f * >( hairfile.GetColorsArray() );
			std::copy( src, src + pointCount, colours.begin() );
		}

		// Copy normals
		auto & normals = submesh->createComponent< c3d::NormalsComponent >()->getData().getData();
		normals.resize( pointCount );
		hairfile.FillDirectionArray( normals.data()->ptr() );

		auto mapping = submesh->createComponent< c3d::LineMapping >();
		auto hairCount = c3d::u32( hairfile.GetHeader().hair_count );

		if ( const auto * segments = hairfile.GetSegmentsArray() )
		{
			// If segments array exists
			auto & lines = mapping->getData().getFaces();
			c3d::u32 totalSegmentCount = 0u;
			for ( c3d::u32 hairIndex = 0; hairIndex < hairCount; ++hairIndex )
				totalSegmentCount += segments[hairIndex] + 1u;
			lines.reserve( totalSegmentCount * 2u );

			c3d::u32 globalPointIndex = 0;
			for ( c3d::u32 hairIndex = 0; hairIndex < hairCount; ++hairIndex )
			{
				for ( c3d::u32 segmentPointIndex = 0; segmentPointIndex < segments[hairIndex]; ++segmentPointIndex )
				{
					lines.emplace_back( globalPointIndex, globalPointIndex + 1u );
					++globalPointIndex;
				}

				// Increment to skip to the next hair's starting point
				++globalPointIndex;
			}
		}
		else
		{
			// If segments array does not exist, use default segment count
			auto & lines = mapping->getData().getFaces();
			auto defaultSegments = c3d::u32( hairfile.GetHeader().d_segments );
			c3d::u32 totalSegmentCount = hairCount * ( defaultSegments + 1u );
			lines.reserve( totalSegmentCount * 2u );

			c3d::u32 globalPointIndex = 0;
			for ( c3d::u32 hairIndex = 0; hairIndex < hairCount; ++hairIndex )
			{
				for ( c3d::u32 segmentPointIndex = 0; segmentPointIndex < defaultSegments; ++segmentPointIndex )
				{
					lines.emplace_back( globalPointIndex, globalPointIndex + 1u );
					++globalPointIndex;
				}

				// Increment to skip to the next hair's starting point
				++globalPointIndex;
			}
		}

		return true;
	}

	//*********************************************************************************************
}
