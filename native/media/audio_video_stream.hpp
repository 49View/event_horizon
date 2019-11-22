#include <string>
#include <memory>
#include <functional>
#include <core/math/vector2i.h>
#include <core/streaming_mediator.hpp>

class AudioVideoStreamFFmpeg : public AudioVideoStream {
public:
	explicit AudioVideoStreamFFmpeg( const std::string& tname, StreamingMediator& mediator, AVInitCallback avc );
	virtual ~AudioVideoStreamFFmpeg();

	void advanceFrame() override;
	V2i streamDim() const override;
	const std::string& Name() const override;

private:
	class AudioVideoStreamImpl;
	std::unique_ptr<AudioVideoStreamImpl> pimpl;
};
