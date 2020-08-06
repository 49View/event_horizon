//
// Created by Dado on 2019-08-20.
//

static int gMultiSampleCount = 8;

namespace WindowHandling {

    void setMultiSampleCount( int count ) {
        gMultiSampleCount = count;
    }

    int getMultiSampleCount() {
        return gMultiSampleCount;
    }

}