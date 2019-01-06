//
// Created by Dado on 2018-10-29.
//

#define IMGUI_DEFINE_MATH_OPERATORS 1

#include "timeline_layout.h"
#include <core/math/anim.h>
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/imgui_internal.h>
//#include <graphics/imgui/ImSequencer.h>
//#include <graphics/imgui/ImCurveEdit.h>
#include <render_scene_graph/scene.hpp>

void ImGuiTimeline( [[maybe_unused]] Scene* p, const Rect2f& _r ) {

    auto tgroups = Timeline::Groups();
    auto gsize = tgroups.size();

    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Timeline",  nullptr, ImGuiWindowFlags_NoCollapse );

    static int currentFrame = 0;
    const static float secondMult = 100.0f;
    const static int secondMultI = static_cast<int>(secondMult);
    int currFrameWidth = 5; // must be dividend of 100

    const char* current_item = ( gsize == 1 ) ? tgroups.begin()->first.c_str() : "None";
    std::string tkey = current_item;

    ImGui::PushItemWidth(130);

    if (ImGui::BeginCombo("##combo", current_item)) // The second parameter is the label previewed before opening the combo.
    {
        for ( const auto& [k,v] : Timeline::Groups() )
        {
            auto item = k.c_str();
            bool is_selected = (current_item == item); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(item, is_selected))
                current_item = item;
            if (is_selected) {
                tkey = current_item;
                ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
        }
        ImGui::EndCombo();
    }
    ImGui::SameLine();

    if ( ImGui::Button( "->" ) ) {
        Timeline::play(current_item, (currentFrame*currFrameWidth) / secondMult);
    }
    ImGui::SameLine();
    if ( ImGui::Button( "|->" ) ) {
        Timeline::play(current_item, 0.0f);
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(90);
    ImGui::InputInt("Frame ", &currentFrame);

    ImGui::SameLine();
    if ( ImGui::Button( "Camera" ) ) {
        if ( current_item ) {
            auto cam = p->CM().getCamera(Name::Foxtrot);
            auto lFrame = (currentFrame*currFrameWidth)/secondMult;
            Timeline::add( current_item, cam->PosAnim(), {lFrame, cam->getPosition() } );
            Timeline::add( current_item, cam->QAngleAnim(), { lFrame, cam->quatAngle() } );
        }
    }

    if ( currentFrame < 0 ) currentFrame = 0;

    if ( gsize == 0 ) {
        ImGui::End();
        return; // Early out, slight crap but inline with ImGui
    }

    ImGuiIO& io = ImGui::GetIO();

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

    auto drawFrameLine = [&]( int currentFrame, ImU32 col ) {
        float currFrameX = canvas_pos.x + titlesWidth + currentFrame*currFrameWidth;
        ImVec2 currFrameTop = ImVec2(currFrameX, canvas_pos.y);
        ImVec2 currFrameTopLong = ImVec2(currFrameX, canvas_pos.y);
        draw_list->AddRectFilled( currFrameTopLong + ImVec2(-1,0), currFrameTopLong + ImVec2(2, canvas_size.y+frameLinesHeight), col, 0);
    };

    if ( auto ct = Timeline::groupAnimTime(current_item); ct > 0.0f ) {
        currentFrame = static_cast<int>(ceil(ct * ( secondMult / currFrameWidth)));
    }

    ImRect lineFramesContainer{ topLineFrame, bottomLineFrame };
    if ( lineFramesContainer.Contains(io.MousePos) ) {
        frameLineCol = 0xFFBFBF00;
        if ( ImGui::IsMouseDown(0) ) {
            currentFrame = static_cast<int>(( io.MousePos.x - topLineFrame.x) / currFrameWidth);
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

    float cellHeight = 20.0f;
    int counter = 0;
    auto cellMargin = ImVec2(1,2);
    auto textMargin = ImVec2(3,2);
    auto cellBlock = ImVec2(currFrameWidth, cellHeight) - cellMargin;
    Timeline::visitGroup( current_item, [&]( const std::string& _name, const std::vector<float>& _values,
                                             TimelineIndex _valueType, int stride ) {
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
            auto keyframe = _values[kc*stride];
            auto kposOff = kpos + ImVec2( keyframe * secondMult, 0.0f);

            auto lTop = topTimeline + kposOff + cellMargin;
            switch (_valueType) {
                case tiFloatIndex:
                    draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "F");
                    break;
                case tiV2fIndex:
                    draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "KK");
                    break;
                case tiV3fIndex:
                    draw_list->AddRectFilled( lTop, lTop + cellBlock, 0xFF4040AF, 0);
                break;
                case tiV4fIndex:
                    draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "KKKK");
                    break;
                case tiQuatIndex:
                    draw_list->AddRectFilled( lTop, lTop + cellBlock, 0xFFA040AF, 0);
                    break;
                default:
                    break;
            }
        }
        counter++;
    } );

    ImGui::End();

}
