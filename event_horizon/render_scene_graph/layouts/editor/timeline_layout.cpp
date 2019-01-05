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

//    for ( const auto& [k,v] : Timeline::Timelines().tmapf ) {
//        mySequence.myItems.push_back(MySequence::MySequenceItem{ v.Name(), 0, false });
//    }

    auto tgroups = Timeline::Groups();
    auto gsize = tgroups.size();

    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Timeline",  nullptr, ImGuiWindowFlags_NoCollapse );

    if ( gsize == 0 ) {
        ImGui::End();
        return; // Early out, slight crap but inline with ImGui
    }

    // let's create the sequencer
//    static int selectedEntry = -1;
//    static int firstFrame = 0;
//    static bool expanded = true;
    static int currentFrame = 0;

    const char* current_item = ( gsize == 1 ) ? tgroups.begin()->first.c_str() : nullptr;
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

    if ( ImGui::Button( "Play" ) ) {
        Timeline::play(current_item);
    }
    ImGui::SameLine();
    ImGui::Button( "Reset" );
    ImGui::SameLine();
    ImGui::PushItemWidth(90);
    ImGui::InputInt("Frame ", &currentFrame);
    if ( currentFrame < 0 ) currentFrame = 0;

    if ( current_item ) {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
        ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available

        float frameLinesHeight = 20.0f;
        float titlesWidth = 200.0f;
        int currFrameWidth = 5; // must be dividend of 100
        ImVec2 mainTop = canvas_pos + ImVec2( 0.0f, frameLinesHeight);


        draw_list->AddRectFilled(mainTop, mainTop + ImVec2(titlesWidth, canvas_size.y), 0xFF404040, 0);
        auto topTimeline = mainTop+ ImVec2(titlesWidth, 0.0f);
        auto bottomTimeline = topTimeline + ImVec2(canvas_size.x - topTimeline.x, canvas_size.y);
        draw_list->AddRectFilled(topTimeline, bottomTimeline, 0xFF804000, 0);

        // Frame lines
        for ( int fl = 0; fl < canvas_size.x; fl+=currFrameWidth ) {
            float lineX = canvas_pos.x + titlesWidth + fl;
            bool bOnASecondEdge = fl % 100 == 0;
            float lhc = bOnASecondEdge ? 0 : frameLinesHeight*0.8f;
            draw_list->AddLine( ImVec2(lineX, canvas_pos.y+frameLinesHeight), ImVec2(lineX, canvas_pos.y+lhc), 0xFF606060, 1);
            if ( bOnASecondEdge ) {
                std::string sec = std::to_string(fl/100) + "s";
                draw_list->AddText( ImVec2(lineX+4, canvas_pos.y), 0xFFB0B0B0, sec.c_str());
            }
        }

        // current frame
        float currFrameX = canvas_pos.x + titlesWidth + currentFrame*currFrameWidth;
        ImVec2 currFrameTop = ImVec2(currFrameX, canvas_pos.y);
        draw_list->AddRectFilled( currFrameTop, currFrameTop + ImVec2(currFrameWidth, frameLinesHeight), 0xFF4040FF, 0);
        ImVec2 currFrameTopLong = ImVec2(currFrameX + currFrameWidth*0.5f, canvas_pos.y);
        draw_list->AddRectFilled( currFrameTopLong, currFrameTopLong + ImVec2(1, canvas_size.y), 0xFF4040FF, 0);

        float cellHeight = 20.0f;
        int counter = 0;
        Timeline::visitGroup( current_item, [&]( const std::string& _name, const std::vector<float>& _values,
                                                 TimelineIndex _valueType, int stride ) {
            auto kpos = ImVec2(0.0f, counter*cellHeight);
            draw_list->AddText( mainTop + kpos, 0xFFFFFFFF, _name.c_str());
            size_t keyframeCount = _values.size() / stride;
            for ( size_t kc = 0; kc < keyframeCount; kc++ ) {
                auto keyframe = _values[kc*stride];
                auto kposOff = kpos + ImVec2( keyframe * 100.0f, 0.0f);

                switch (_valueType) {
                    case tiFloatIndex:
                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "F");
                        break;
                    case tiV2fIndex:
                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "KK");
                        break;
                    case tiV3fIndex: {
                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "KKK");
//                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, std::to_string(_values[kc*3+0]).c_str());
//                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, std::to_string(_values[kc*3+1]).c_str());
//                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, std::to_string(_values[kc*3+2]).c_str());
                    }
                    break;
                    case tiV4fIndex:
                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "KKKK");
                        break;
                    case tiQuatIndex:
                        draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "QQQ");
                        break;
                    default:
                        break;
                }
            }
            counter++;
        } );
//        for ( const auto& [k,v] : Timeline::timelinesInGroup(current_item).tmapV3 ) {
//            auto kpos = ImVec2(0.0f, counter*cellHeight);
//            draw_list->AddText( canvas_pos + kpos, 0xFFFFFFFF, v.Name().c_str());
//            for ( const auto& keyframe : v.KeyframeTimes() ) {
//                auto kposOff = kpos + ImVec2( keyframe * 100.0f, 0.0f);
//                draw_list->AddText( topTimeline + kposOff, 0xFF0F0FFF, "K");
//            }
//            counter++;
//        }

    }

//    Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);

    ImGui::End();

//    bool show_demo_window = true;
//    ImGui::ShowDemoWindow(&show_demo_window);
}
