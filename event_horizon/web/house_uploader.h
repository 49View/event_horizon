#pragma	once

class HouseUploader {
public:
	static HouseUploader& getInstance() {
		static HouseUploader instance; // Guaranteed to be destroyed.
		return instance;// Instantiated on first use.
	}

	void upload();

private:
	HouseUploader() {}
	HouseUploader( HouseUploader const& ) = delete;
	void operator=( HouseUploader const& ) = delete;
};

#define HU HouseUploader::getInstance()