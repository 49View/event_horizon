#include <string>
#include <memory>

class StreamingMediator;

class AudioVideoStream {
public:
	explicit AudioVideoStream( StreamingMediator& mediator );
	int main_decode( const std::string& tname );
	void advanceFrame();
	const std::string& Name() const;

private:
	class AudioVideoStreamImpl;
	std::unique_ptr<AudioVideoStreamImpl> pimpl;
};
