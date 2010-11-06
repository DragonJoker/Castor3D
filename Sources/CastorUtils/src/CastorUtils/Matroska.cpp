#include "PrecompiledHeader.h"

#ifndef __GNUG__
#	include <ebml/StdIOCallback.h>
#	include <ebml/EbmlHead.h>
#	include <ebml/EbmlSubHead.h>
#	include <ebml/EbmlVoid.h>
#	include <matroska/FileKax.h>
#	include <matroska/KaxSegment.h>
#	include <matroska/KaxTracks.h>
#	include <matroska/KaxTrackEntryData.h>
#	include <matroska/KaxTrackAudio.h>
#	include <matroska/KaxTrackVideo.h>
#	include <matroska/KaxCluster.h>
#	include <matroska/KaxClusterData.h>
#	include <matroska/KaxSeekHead.h>
#	include <matroska/KaxCues.h>
#	include <matroska/KaxInfo.h>
#	include <matroska/KaxInfoData.h>
#	include <matroska/KaxTags.h>
#	include <matroska/KaxTag.h>
#	include <matroska/KaxChapters.h>
#	include <matroska/KaxContentEncoding.h>
#	include <matroska/KaxBlockData.h>
#	include <windows.h>

using namespace libebml;
using namespace libmatroska;
#endif

#include "Matroska.h"

using namespace Castor::Resource;

#ifndef MKFOURCC
#	define MKFOURCC( d, c, b, a) (DWORD)((char)a | (char)b << 8 | (char)c << 16 | (char)d << 24)
#endif

#ifdef __GNUG__
struct RECT
{
	long	left;
	long	top;
	long	right;
	long	bottom;
};

struct BITMAPINFOHEADER
{
	unsigned long	biSize;
	long			biWidth;
	long			biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned long	biCompression;
	unsigned long	biSizeImage;
	long			biXPelsPerMeter;
	long			biYPelsPerMeter;
	unsigned long	biClrUsed;
	unsigned long	biClrImportant;
};
#endif

const unsigned long long  TIMECODE_SCALE = 1000000;

bool FreeBuffer( const DataBuffer & p_buffer)
{
	delete [] p_buffer.Buffer();

	return true;
}

MatroskaFile :: MatroskaFile( const String & p_file, bool p_saveDefaults)
	:	m_segment( NULL),
		m_segmentSize( 0),
		m_tracks( NULL),
		m_tracksSize( 0),
		m_metaSeek( NULL),
		m_infoSize( 0),
		m_tagsSize( 0),
		m_chapterSize( 0),
		m_cueSize( 0),
		m_clusterSize( 0),
		m_firstFrame( true),
		m_timeIndex( 0),
		m_currentCluster( NULL),
		m_currentClusterTimeIndex( 0),
		m_trackUID( 7),
		m_currentTrackNumber( 1),
		m_duration( 0),
		m_initialised( false),
		m_cues( NULL),
		m_saveDefaults( p_saveDefaults)
{
	m_file = new StdIOCallback( p_file.char_str(), MODE_CREATE);
}

MatroskaFile :: ~MatroskaFile()
{
	Cleanup();
}

void MatroskaFile :: Cleanup()
{
	Finalise();

	if (m_cues != NULL)
	{
		m_cues->RemoveAll();
		delete m_cues;
		m_cues = NULL;
	}
	if (m_metaSeek != NULL)
	{
		m_metaSeek->RemoveAll();
		delete m_metaSeek;
		m_metaSeek = NULL;
	}
	if (m_tracks != NULL)
	{
		m_tracks->RemoveAll();
		delete m_tracks;
		m_tracks = NULL;
	}
	if (m_segment != NULL)
	{
		m_segment->RemoveAll();
		delete m_segment;
		m_segment = NULL;
	}
}

void MatroskaFile :: Initialise( const String & p_muxingAppName, const String & p_writingAppName)
{
	if (m_initialised)
	{
		return;
	}

	m_firstFrame = true;

	_writeHeader();

	_initialiseSegment();

	_writeSegmentInfos( p_muxingAppName, p_writingAppName);

	m_cues = new KaxCues;
	m_cues->SetGlobalTimecodeScale( TIMECODE_SCALE);
	m_initialised = true;
}

void MatroskaFile :: AddVideoTrack( const String & p_codec, unsigned long long p_width, unsigned long long p_height, bool p_default)
{
	KaxTrackEntry * l_track = _addTrack( track_video, p_codec, p_default);

	if (l_track == NULL)
	{
		return;
	}

	*(static_cast <EbmlUInteger *>( & GetChild <KaxTrackMinCache>( * l_track))) = 1;

	KaxTrackVideo & l_videoTrack = GetChild <KaxTrackVideo>( * l_track);
	*(static_cast <EbmlUInteger *>( & GetChild <KaxVideoPixelWidth>( l_videoTrack))) = p_width;
	*(static_cast <EbmlUInteger *>( & GetChild <KaxVideoPixelHeight>( l_videoTrack))) = p_height;
	*(static_cast <EbmlUInteger *>( & GetChild <KaxVideoDisplayWidth>( l_videoTrack))) = p_width;
	*(static_cast <EbmlUInteger *>( & GetChild <KaxVideoDisplayHeight>( l_videoTrack))) = p_height;

	if (p_codec == C3D_T( "V_UNCOMPRESSED"))
	{
		int l_space = 0x002EB524;
		unsigned char * l_char = new unsigned char[4];
		memcpy( l_char, & l_space, 4);
		GetChild <KaxVideoColourSpace>( l_videoTrack).SetBuffer( l_char, 4);
	}

	RECT l_rect;
	l_rect.top = 0;
	l_rect.left = 0;
	l_rect.right = (long)p_width;
	l_rect.bottom = (long)p_height;

	BITMAPINFOHEADER * l_format = new BITMAPINFOHEADER;
	memset( l_format, 0, sizeof( BITMAPINFOHEADER));
	l_format->biWidth = (long)p_width;
	l_format->biHeight = (long)p_height;
	l_format->biPlanes = 1;
	l_format->biBitCount = 16;
	l_format->biCompression = MKFOURCC( 'Y', 'U', 'Y', '2');
	l_format->biSize = sizeof( BITMAPINFOHEADER);
	l_format->biSizeImage = (long)(p_width * p_height * 2);
	
	KaxCodecPrivate & l_codec = GetChild <KaxCodecPrivate>( * l_track);
	l_codec.SetBuffer( (binary *)l_format, sizeof( *l_format));
}

void MatroskaFile :: AddAudioTrack( const String & p_codec, bool p_default)
{
	KaxTrackEntry * l_track = _addTrack( track_audio, p_codec, p_default);

	if (l_track == NULL)
	{
		return;
	}

	GetChild <KaxTrackAudio>( * l_track);
}

void MatroskaFile :: AddSubsTrack( bool p_default)
{
	KaxTrackEntry * l_track = _addTrack( track_subtitle, C3D_T( "S_TEXT/UTF8"), p_default);

	if (l_track == NULL)
	{
		return;
	}
	*(static_cast <EbmlUInteger *>( & GetChild <KaxTrackMinCache>( * l_track))) = 0;
}

void MatroskaFile :: EndCluster()
{
	if (m_currentCluster != NULL)
	{
		m_clusterSize += m_currentCluster->Render( * m_file, * m_cues, m_saveDefaults);
		m_currentCluster->ReleaseFrames();
		m_metaSeek->IndexThis( * m_currentCluster, * m_segment);
		delete m_currentCluster;
		m_currentCluster = NULL;
	}
}

void MatroskaFile :: StartCluster()
{
	EndCluster();

	m_currentCluster = new KaxCluster;
	m_currentCluster->SetParent( * m_segment);
	m_currentCluster->SetPreviousTimecode( m_timeIndex, TIMECODE_SCALE);
	m_currentCluster->EnableChecksum();
	m_currentClusterTimeIndex = 0;
}

void MatroskaFile :: AddFrame( const EBML_TRACK_FRAME & p_frame)
{
	if (m_firstFrame)
	{
		m_firstFrame = false;
		m_tracksSize = m_tracks->Render( * m_file, m_saveDefaults);
		m_metaSeek->IndexThis( * m_tracks, * m_segment);
	}

	TrackMap::iterator l_it = m_trackEntries.find( p_frame.m_track);
	if (l_it == m_trackEntries.end())
	{
		return;
	}

	m_currentClusterTimeIndex += p_frame.m_duration * TIMECODE_SCALE;

	m_timeIndex += p_frame.m_duration * TIMECODE_SCALE;

	DataBuffer * l_buffer = new DataBuffer( p_frame.m_data, p_frame.m_size, & FreeBuffer);
	KaxBlockGroup * l_group;
	if (m_currentCluster->AddFrame( * l_it->second->m_track, m_timeIndex, * l_buffer, l_group, LACING_AUTO) && l_group != NULL)
	{
		KaxBlockMap::iterator l_currentIt = m_currentGroups.find( p_frame.m_track);
		KaxBlockMap::iterator l_previousIt = m_previousGroups.find( p_frame.m_track);
		l_currentIt->second = l_group;
		m_cues->AddBlockGroup( * l_group);
		l_previousIt->second = l_group;
	}
}

void MatroskaFile :: Finalise()
{
	if (m_file != NULL)
	{
		if (m_initialised)
		{
			if (m_firstFrame)
			{
				m_firstFrame = false;
				m_tracksSize = m_tracks->Render( * m_file, m_saveDefaults);
				m_metaSeek->IndexThis( * m_tracks, * m_segment);
			}

			EndCluster();

			unsigned int l_cueSize = m_cues->Render( * m_file, m_saveDefaults);
			m_metaSeek->IndexThis( * m_cues, * m_segment);

			unsigned long long l_metaSeekSize = m_dummy.ReplaceWith( * m_metaSeek, * m_file, m_saveDefaults);
			unsigned long long l_tracksSize = 0;

			for (TrackMap::iterator l_it = m_trackEntries.begin() ; l_it != m_trackEntries.end() ; ++l_it)
			{
				l_tracksSize += l_it->second->m_size;
			}

			if (m_segment->ForceSize( m_segmentSize - m_segment->HeadSize() + l_metaSeekSize + m_tracksSize + l_tracksSize +
									  m_clusterSize + l_cueSize + m_infoSize + m_tagsSize + m_chapterSize))
			{
					m_segment->OverwriteHead( * m_file);
			}
		}

		m_file->close();
		delete m_file;
		m_file = NULL;
	}
}

void MatroskaFile :: _writeHeader()
{
	EbmlHead l_head;

	*(static_cast <EbmlString *>( & GetChild<EDocType>( l_head))) = "matroska";
	*(static_cast <EbmlUInteger *>( & GetChild<EDocTypeVersion>( l_head))) = 1;
	*(static_cast <EbmlUInteger *>( & GetChild<EDocTypeReadVersion>( l_head))) = 1;

	l_head.Render( * m_file, m_saveDefaults);
}

void MatroskaFile :: _initialiseSegment()
{
	m_segment = new KaxSegment;
	m_segmentSize = m_segment->WriteHead( * m_file, 5, m_saveDefaults);

	m_tracks = & GetChild <KaxTracks>( * m_segment);
	m_tracks->EnableChecksum();

	// création d'un bloc vide pour placer le seek head une fois le fichier rempli
	m_dummy.SetSize( 30000);
	m_dummy.Render( * m_file, m_saveDefaults);
	m_metaSeek = new KaxSeekHead();
}

void MatroskaFile :: _writeSegmentInfos( const String & p_muxingAppName, const String & p_writingAppName)
{
	KaxInfo & l_infos = GetChild <KaxInfo>( * m_segment);
	*(static_cast <EbmlUInteger *>( & GetChild <KaxTimecodeScale>( l_infos))) = TIMECODE_SCALE;
	*(static_cast <EbmlFloat *>( & GetChild <KaxDuration>( l_infos))) = 0.0;
	*((EbmlUnicodeString *) & GetChild <KaxMuxingApp>( l_infos)) = p_muxingAppName.wchar_str();
	*((EbmlUnicodeString *) & GetChild <KaxWritingApp>( l_infos)) = p_writingAppName.wchar_str();
	*((EbmlFloat *) & GetChild <KaxDuration>( l_infos)) = 100.0f * (m_duration / TIMECODE_SCALE);

	GetChild <KaxWritingApp>( l_infos).SetDefaultSize( 25);

	unsigned char l_segUID[128 / 8];
	srand( unsigned int( time( NULL)));
	for (int i = 0; i < 128 / 8; i++)
	{
		l_segUID[i] = rand();
	}
	GetChild <KaxSegmentUID>( l_infos).CopyBuffer( l_segUID, 128 / 8);

	m_infoSize = l_infos.Render( * m_file);

	m_metaSeek->IndexThis( l_infos, * m_segment);
}

KaxTrackEntry * MatroskaFile :: _addTrack( track_type p_type, const String & p_codec, bool p_default)
{
	if (m_trackEntries.find( p_type) != m_trackEntries.end())
	{
		return NULL;
	}

	KaxBlockGroup * l_group;
	m_currentGroups.insert( KaxBlockMap::value_type( p_type, l_group));
	m_previousGroups.insert( KaxBlockMap::value_type( p_type, (KaxBlockGroup *)NULL));

	KaxTrackEntry * l_track = & AddNewChild <KaxTrackEntry>( * m_tracks);
	l_track->SetGlobalTimecodeScale( TIMECODE_SCALE);
	l_track->EnableChecksum();

	*(static_cast <EbmlUInteger *>	( & GetChild <KaxTrackNumber>		( * l_track))) = m_currentTrackNumber++;
	*(static_cast <EbmlUInteger *>	( & GetChild <KaxTrackUID>			( * l_track))) = m_trackUID++;
	*(static_cast <EbmlUInteger *>	( & GetChild <KaxTrackType>			( * l_track))) = p_type;
	*(static_cast <EbmlString *>	( & GetChild <KaxCodecID>			( * l_track))) = p_codec.char_str();
	*(static_cast <EbmlUInteger *>	( & GetChild <KaxTrackFlagEnabled>	( * l_track))) = 1;
	*(static_cast <EbmlUInteger *>	( & GetChild <KaxTrackFlagLacing>	( * l_track))) = 0;
	*(static_cast <EbmlUInteger *>	( & GetChild <KaxTrackFlagDefault>	( * l_track))) = (p_default ? 1 : 0);
	*(static_cast <EbmlFloat *>		( & GetChild <KaxTrackTimecodeScale>( * l_track))) = 1;
	*(static_cast <EbmlString *>	( & GetChild <KaxTrackLanguage>		( * l_track))) = "fre";

	EBML_TRACK * l_ebmlTrack = new EBML_TRACK;

	l_ebmlTrack->m_size = 0;
	l_ebmlTrack->m_track = l_track;

	m_trackEntries.insert( TrackMap::value_type( p_type, l_ebmlTrack));

	return l_track;
}