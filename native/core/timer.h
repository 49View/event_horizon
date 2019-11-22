#ifndef __UTIL_TIMER_H_
#define __UTIL_TIMER_H_

class Timer {
public:
	Timer() {
		mTimeLeft = 0.0f;
	}

	// Start the timer
	void start( float time ) {
		mTimeLeft = time;
	}

	// Return true if the timer hit 0 this frame
	bool update( float timeStep ) {
		if ( mTimeLeft > 0.0f ) {
			mTimeLeft -= timeStep;
			return mTimeLeft <= 0.0f;
		}
		return false;
	}

	bool isRunning() {
		return mTimeLeft > 0.0f;
	}

	float getTimeLeft() {
		return mTimeLeft;
	}

private:
	float mTimeLeft;
};

#endif
