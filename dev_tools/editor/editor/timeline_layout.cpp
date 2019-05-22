//
// Created by Dado on 2018-10-29.
//

#define IMGUI_DEFINE_MATH_OPERATORS 1

#include "timeline_layout.h"
#include <core/math/anim.h>
#include <core/camera.h>
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/imgui_internal.h>
#include <render_scene_graph/render_orchestrator.h>
#include <render_scene_graph/layouts/layout_helper.hpp>
#include <core/resources/resource_manager.hpp>

static int currentFrame = 0;
const static float secondMult = 100.0f;
const static int secondMultI = static_cast<int>(secondMult);
int currFrameWidth = 5; // must be dividend of 100

auto currframeToTime = []() -> float { return (currentFrame*currFrameWidth)/secondMult; };

static std::string currTimelineName = "None";

namespace GuiTimeline {

    std::string TimeLineName() {
        return currTimelineName;
    }
    float CurrentTime() {
        return currframeToTime();
    }

};

void ImGuiTimeline::renderImpl( SceneGraph& _sg, RenderOrchestrator& _rsg, Rect2f& _r ) {

    auto tgroups = Timeline::Groups();
    auto gsize = tgroups.size();

    ImGuiIO& io = ImGui::GetIO();

    const char* timelineNameCStr = ( gsize == 1 ) ? tgroups.begin()->first.c_str() : "None";
    currTimelineName = std::string(timelineNameCStr);

    ImGui::PushItemWidth(130);

    if (ImGui::BeginCombo("##combo", timelineNameCStr)) // The second parameter is the label previewed before opening the combo.
    {
        for ( const auto& [k,v] : Timeline::Groups() )
        {
            auto item = k.c_str();
            bool is_selected = (timelineNameCStr == item); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(item, is_selected))
                timelineNameCStr = item;
            if (is_selected) {
                currTimelineName = timelineNameCStr;
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();

    if ( ImGui::Button( "->" ) ) {
//        Timeline::play(timelineNameCStr, currframeToTime());
    }
    ImGui::SameLine();
    if ( ImGui::Button( "|->" ) ) {
//        Timeline::play(timelineNameCStr, 0.0f);
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(90);
    if ( ImGui::InputInt("Frame ", &currentFrame) ) {
//        Timeline::playOneFrame(timelineNameCStr, currframeToTime() );
    } else {
        if ( timelineNameCStr ) {
            if ( auto ct = Timeline::groupAnimTime( timelineNameCStr ); ct > 0.0f ) {
                currentFrame = static_cast<int>(ceil( ct * ( secondMult / currFrameWidth )));
            }
        }
    }

    ImGui::SameLine();
    if ( ImGui::Button( "Camera" ) && timelineNameCStr ) {
        Timeline::addLinked( timelineNameCStr, _sg.CM().get(Name::Foxtrot)->getMainCamera(), currframeToTime() );
    }

    if ( currentFrame < 0 ) currentFrame = 0;

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available

    float frameLinesHeight = 20.0f;
    float titlesWidth = 200.0f;
    ImVec2 mainTop = canvas_pos + ImVec2( 0.0f, frameLinesHeight);

    draw_list->AddRectFilled(mainTop, mainTop + ImVec2(titlesWidth, canvas_size.y), 0xFF404040, 0);
    auto topTimeline = mainTop+ ImVec2(titlesWidth, 0.0f);
    auto bottomTimeline = topTimeline + ImVec2(canvas_size.x - topTimeline.x, canvas_size.y);

    // Frame lines
    int incCounter = 0;
    auto lineFrameStartX = canvas_pos.x + titlesWidth;
    auto topLineFrame = ImVec2( lineFrameStartX, canvas_pos.y);
    auto bottomLineFrame = topLineFrame + ImVec2( canvas_size.x - titlesWidth, frameLinesHeight );
    ImU32 frameLineCol = 0xFF606060;

    static bool bStartCellDragging = false;
    static uint64_t ktimelineDragging = 0;
    static uint64_t ktimelineDraggingIndex = 0;

    if ( !ImGui::IsMouseDown(0) ) bStartCellDragging = false;

    auto seCurrentFrameFromMouseX = [&]() {
        currentFrame = std::max(0, static_cast<int>(( io.MousePos.x - topLineFrame.x) / currFrameWidth));
    };

    auto drawFrameLine = [&]( int currentFrame, ImU32 col ) {
        float currFrameX = canvas_pos.x + titlesWidth + currentFrame*currFrameWidth;
        ImVec2 currFrameTopLong = ImVec2(currFrameX, canvas_pos.y);
        draw_list->AddRectFilled( currFrameTopLong, currFrameTopLong + ImVec2(currFrameWidth, canvas_size.y+frameLinesHeight), col, 0);
    };

    auto isDragging = [&]( uint64_t _k, uint64_t _kc ) -> bool {
        return ( ktimelineDragging == _k) &&
                ( ktimelineDraggingIndex == _kc);

    };

    ImRect lineFramesContainer{ topLineFrame, bottomLineFrame };
    if ( lineFramesContainer.Contains(io.MousePos) ) {
        frameLineCol = 0xFFBFBF00;
        if ( ImGui::IsMouseDown(0) ) {
            seCurrentFrameFromMouseX();
//            Timeline::playOneFrame(timelineNameCStr, currframeToTime() );
        }
    }
    for ( int fl = 0; fl < canvas_size.x; fl+=currFrameWidth ) {
        float lineX = lineFrameStartX + fl;
        bool bOnASecondEdge = fl % secondMultI == 0;
        bool bOnHalfASecondEdge = fl % (secondMultI/2) == 0;
        float lhc = bOnASecondEdge ? 0 : frameLinesHeight*0.8f;
        if ( bOnHalfASecondEdge && ! bOnASecondEdge ) lhc = frameLinesHeight*0.5f;
        draw_list->AddLine( ImVec2(lineX, canvas_pos.y+frameLinesHeight), ImVec2(lineX, canvas_pos.y+lhc), frameLineCol, 1);
        if ( bOnASecondEdge ) {
            std::string sec = std::to_string(fl/secondMultI) + "s";
            draw_list->AddText( ImVec2(lineX+4, canvas_pos.y), 0xFFB0B0B0, sec.c_str());
        }
        auto ccol = incCounter++ % 2 == 0 ? 0xFF804000 : 0xFF603020;
        draw_list->AddRectFilled(ImVec2(lineX,topTimeline.y), ImVec2(lineX+currFrameWidth,bottomTimeline.y), ccol, 0);
    }

    // current frame
    drawFrameLine( currentFrame, 0x7F40A0FF );

    if ( bStartCellDragging && ImGui::IsMouseDown(0) ) {
        auto lcf = currentFrame;
        seCurrentFrameFromMouseX();
        if ( lcf != currentFrame )
            Timeline::updateKeyTime( timelineNameCStr, ktimelineDragging, ktimelineDraggingIndex, currframeToTime() );
    }

    float cellHeight = 20.0f;
    int counter = 0;
    auto cellMargin = ImVec2(1,2);
    auto textMargin = ImVec2(3,2);
    auto cellBlock = ImVec2(currFrameWidth, cellHeight) - cellMargin;
    Timeline::visitGroup( timelineNameCStr, [&]( const std::string& _name, const std::vector<float>& _values,
                                             TimelineIndex _k, TimelineIndex _valueType, int stride ) {
        auto kpos = ImVec2(0.0f, counter*cellHeight);
        auto rowNameTop = mainTop + ImVec2( 0.0f, cellHeight*counter );
        auto rowNameBottom = rowNameTop + ImVec2(titlesWidth, cellHeight );
        bool oddLine = counter % 2 == 0;
        ImU32 lTextBackCol = oddLine ? 0x7F4040AF : 0x7F40407F;
        ImRect rowContainer{ rowNameTop, rowNameBottom };
        if (rowContainer.Contains(io.MousePos)) {
            lTextBackCol = 0xFFFFFF00;
        }
        draw_list->AddRectFilled( rowContainer.Min, rowContainer.Max, lTextBackCol, 0);
        draw_list->AddText( mainTop + kpos + textMargin, 0xFFFFFFFF, _name.c_str());
        auto lineBreakTop = rowNameTop + ImVec2( 0.0f, cellHeight );
        draw_list->AddLine( lineBreakTop, lineBreakTop+ImVec2(canvas_size.x, 0.0f), 0xFF909040, 1);
        size_t keyframeCount = _values.size() / stride;
        for ( size_t kc = 0; kc < keyframeCount; kc++ ) {
            auto kStrideIndex = kc*stride;
            auto keyframe = _values[kStrideIndex];
            auto kposOff = kpos + ImVec2( keyframe * secondMult, 0.0f);

            auto lTop = topTimeline + kposOff + cellMargin;
            auto lBottom = lTop + cellBlock;

            ImRect frameRect{ lTop, lBottom };
            ImU32 cellCol = 0xFF4040AF;
            if ( frameRect.Contains(io.MousePos) ) {
                cellCol = 0xFFFFFF00;
                ImGui::SetNextWindowPos( lTop + ImVec2(10.0f, 0.0f) );
                ImGui::SetNextWindowSize( ImVec2(200,cellHeight) );
                std::ostringstream sname;
                sname << _name << _values[kStrideIndex];
                ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar);
                ImGui::BeginChild(sname.str().c_str());
                std::ostringstream ss{};
                for ( int t = 1; t < stride; t++ ) {
                    ss << " x: " << _values[kStrideIndex+t];
                }
                auto s = ss.str();
                ImGui::TextColored( {1.0f, 1.0f, 1.0f, 1.0f}, "%s", s.c_str() );
                ImGui::EndChild();
                ImGui::End();
                if ( ImGui::IsMouseClicked(1) ) {
                    Timeline::deleteKey( timelineNameCStr, _k, kc );
                    break;
                }
                if ( ImGui::IsMouseDown(0) ) {
                    bStartCellDragging = true;
                    ktimelineDragging = _k;
                    ktimelineDraggingIndex = kc;
                }
            }
            if ( isDragging(_k, kc) )  cellCol = 0xFFFFFF00;
            draw_list->AddRectFilled( lTop, lBottom, cellCol, 0);
        }
        counter++;
    } );
}
