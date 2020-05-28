#pragma once

#include <core/serialization.hpp>

JSONDATA( FontParamsJSONAble, fontRevision, unitsPerEm, createdDate, modifiedData, bbox, macStyle, lowestRecPPEM,
          ascent, descent, lineGap, caretSlope, caretOffset, minLeftSideBearing, minRightSideBearing, advanceWidthMax )

    FontParamsJSONAble( float fontRevision, int unitsPerEm, uint64_t createdDate, uint64_t modifiedData,
                        const V4f& bbox, int macStyle, int lowestRecPpem, float ascent, float descent, float lineGap,
                        const V2f& caretSlope, float caretOffset, float minLeftSideBearing, float minRightSideBearing,
                        float advanceWidthMax ) : fontRevision( fontRevision ), unitsPerEm( unitsPerEm ),
                                                  createdDate( createdDate ), modifiedData( modifiedData ),
                                                  bbox( bbox ), macStyle( macStyle ), lowestRecPPEM( lowestRecPpem ),
                                                  ascent( ascent ), descent( descent ), lineGap( lineGap ),
                                                  caretSlope( caretSlope ), caretOffset( caretOffset ),
                                                  minLeftSideBearing( minLeftSideBearing ),
                                                  minRightSideBearing( minRightSideBearing ),
                                                  advanceWidthMax( advanceWidthMax ) {}

    float fontRevision = 0.0f; // information purposes only
    int   unitsPerEm = 0;
    uint64_t createdDate = 0;  // seconds since 12:00 midnight, January 1, 1904
    uint64_t modifiedData = 0; // seconds since 12:00 midnight, January 1, 1904
    V4f bbox = V4f::ZERO;
    int macStyle = 0;        // flags (bold, underline, ect...)
    int lowestRecPPEM = 0;   // smallest size this font is legible

    float ascent = 0.0f;
    float descent = 0.0f;     // distance in 'font units' of the glyphs will go from y = 0 (baseline)
    float lineGap = 0.0f;             // distance in 'font units' between lines
    V2f   caretSlope = V2fc::ZERO;          // caret slope, vector in 'font units'
    float caretOffset = 0.0f;         // caret offset in font units
    float minLeftSideBearing = 0.0f;  // minimum (smallest) left side bearing
    float minRightSideBearing = 0.0f; // minimum right side bearing (why not the max? not sure, maybe this is for right to left text?)
    float advanceWidthMax = 0.0f;     // maximum advanceWidth
};
