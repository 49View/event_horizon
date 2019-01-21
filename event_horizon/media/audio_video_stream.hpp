#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#include <string>

extern "C" {
#include <libavutil/common.h>
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}

class StreamingMediator;

class AudioVideoStream {
public:
	AudioVideoStream( StreamingMediator& mediator );
	int main_decode( const std::string& tname );
	void advanceFrame();
	const std::string& Name() const;
private:
    std::string name;
	AVFormatContext *fmt_ctx = NULL;
	AVCodecContext *video_dec_ctx = NULL, *audio_dec_ctx;
	int width, height;
	enum AVPixelFormat pix_fmt;
	AVStream *video_stream = NULL, *audio_stream = NULL;

	uint8_t *video_dst_data[4] = { NULL };
	int      video_dst_linesize[4];

	int video_stream_idx = -1, audio_stream_idx = -1;
	AVFrame *frame = NULL;
	AVPacket pkt;

	/* Enable or disable frame reference counting. You are not supposed to support
	* both paths in your application but pick the one most appropriate to your
	* needs. Look for the use of refcount in this example to see what are the
	* differences of API usage between them. */
	int refcount = 0;
	bool loaded = false;

	StreamingMediator& mediator;
private:
	int decode_packet( int *got_frame, int cached );
	int open_codec_context( int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type );
};
