//
// Created by Dado on 2018-10-29.
//

#define IMGUI_DEFINE_MATH_OPERATORS 1

#include "timeline_layout.h"
#include <core/math/anim.h>
#include <graphics/imgui/imgui.h>
#include <graphics/imgui/ImSequencer.h>
#include <graphics/imgui/imgui_internal.h>
#include <graphics/imgui/ImCurveEdit.h>
#include <render_scene_graph/scene.hpp>

//
//
// ImSequencer interface
//
//
static const char* SequencerItemTypeNames[] = { "Camera","Music", "ScreenEffect", "FadeIn", "Animation" };


struct RampEdit : public ImCurveEdit::Delegate
{
    RampEdit()
    {
        mPts[0][0] = ImVec2(-10.f, 0);
        mPts[0][1] = ImVec2(20.f, 0.6f);
        mPts[0][2] = ImVec2(25.f, 0.2f);
        mPts[0][3] = ImVec2(70.f, 0.4f);
        mPts[0][4] = ImVec2(120.f, 1.f);
        mPointCount[0] = 5;

        mPts[1][0] = ImVec2(-50.f, 0.2f);
        mPts[1][1] = ImVec2(33.f, 0.7f);
        mPts[1][2] = ImVec2(80.f, 0.2f);
        mPts[1][3] = ImVec2(82.f, 0.8f);
        mPointCount[1] = 4;


        mPts[2][0] = ImVec2(40.f, 0);
        mPts[2][1] = ImVec2(60.f, 0.1f);
        mPts[2][2] = ImVec2(90.f, 0.82f);
        mPts[2][3] = ImVec2(150.f, 0.24f);
        mPts[2][4] = ImVec2(200.f, 0.34f);
        mPts[2][5] = ImVec2(250.f, 0.12f);
        mPointCount[2] = 6;
        mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
        mMax = ImVec2(1.f, 1.f);
        mMin = ImVec2(0.f, 0.f);
    }
    size_t GetCurveCount()
    {
        return 3;
    }

    bool IsVisible(size_t curveIndex)
    {
        return mbVisible[curveIndex];
    }
    size_t GetPointCount(size_t curveIndex)
    {
        return mPointCount[curveIndex];
    }

    uint32_t GetCurveColor(size_t curveIndex)
    {
        uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
        return cols[curveIndex];
    }
    ImVec2* GetPoints(size_t curveIndex)
    {
        return mPts[curveIndex];
    }
    virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
    virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
    {
        mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
        SortValues(curveIndex);
        for (size_t i = 0; i < GetPointCount(curveIndex); i++)
        {
            if (mPts[curveIndex][i].x == value.x)
                return i;
        }
        return pointIndex;
    }
    virtual void AddPoint(size_t curveIndex, ImVec2 value)
    {
        if (mPointCount[curveIndex] >= 8)
            return;
        mPts[curveIndex][mPointCount[curveIndex]++] = value;
        SortValues(curveIndex);
    }
    virtual ImVec2& GetMax() { return mMax; }
    virtual ImVec2& GetMin() { return mMin; }
    virtual unsigned int GetBackgroundColor() { return 0; }
    ImVec2 mPts[3][7];
    size_t mPointCount[3];
    bool mbVisible[3];
    ImVec2 mMin;
    ImVec2 mMax;
private:
    void SortValues(size_t curveIndex)
    {
        auto b = std::begin(mPts[curveIndex]);
        auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
        std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

    }
};

struct MySequence : public ImSequencer::SequenceInterface
{
    // interface with sequencer

    virtual int GetFrameMin() const {
        return mFrameMin;
    }
    virtual int GetFrameMax() const {
        return mFrameMax;
    }
    virtual int GetItemCount() const { return (int)myItems.size(); }

    virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames)/sizeof(char*); }
    virtual const char *GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
    virtual const char *GetItemLabel(int index) const
    {
        static char tmps[512];
        sprintf(tmps, "[%02d] %s", index, myItems[index].mName.c_str());
        return tmps;
    }

    virtual void Get(int index, int *type, unsigned int *color)
    {
        MySequenceItem &item = myItems[index];
        if (color)
            *color = 0xFFAA8080; // same color for everyone, return color based on type
        if (type)
            *type = item.mType;
    }
    virtual void Add(int type) { myItems.push_back(MySequenceItem{ "", type, false }); };
    virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
    virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }

    virtual size_t GetCustomHeight(int index) { return myItems[index].mExpanded ? 300 : 0; }

    // my datas
    MySequence() : mFrameMin(0), mFrameMax(0) {}
    int mFrameMin, mFrameMax;
    struct MySequenceItem
    {
        std::string mName;
        int mType;
        bool mExpanded;
    };
    std::vector<MySequenceItem> myItems;

    virtual void DoubleClick(int index) {
        if (myItems[index].mExpanded)
        {
            myItems[index].mExpanded = false;
            return;
        }
        for (auto& item : myItems)
            item.mExpanded = false;
        myItems[index].mExpanded = !myItems[index].mExpanded;
    }

    virtual void CustomDraw(int index, ImDrawList* draw_list, const ImRect& rc, const ImRect& legendRect, const ImRect& clippingRect, const ImRect& legendClippingRect)
    {
        static const char *labels[] = { "Translation", "Rotation", "Scale" };
        static RampEdit rampEdit;
        rampEdit.mMax = ImVec2(mFrameMax, 1.f);
        rampEdit.mMin = ImVec2(mFrameMin, 0.f);
        draw_list->PushClipRect(legendClippingRect.Min, legendClippingRect.Max, true);
        for (int i = 0; i < 3; i++)
        {
            ImVec2 pta(legendRect.Min.x + 30, legendRect.Min.y + i * 14.f);
            ImVec2 ptb(legendRect.Max.x, legendRect.Min.y + (i+1) * 14.f);
            draw_list->AddText(pta, rampEdit.mbVisible[i]?0xFFFFFFFF:0x80FFFFFF, labels[i]);
            if (ImRect(pta, ptb).Contains(ImGui::GetMousePos()) && ImGui::IsMouseClicked(0))
                rampEdit.mbVisible[i] = !rampEdit.mbVisible[i];
        }
        draw_list->PopClipRect();

        ImGui::SetCursorScreenPos(rc.Min);
        auto mmm = rc.Max-rc.Min;
        ImCurveEdit::Edit(rampEdit, mmm, 137 + index, &clippingRect);
    }
};

void ImGuiTimeline( [[maybe_unused]] Scene* p, const Rect2f& _r ) {

    static MySequence mySequence;
    mySequence.mFrameMin = 0;
    mySequence.mFrameMax = 1000;

    mySequence.myItems.clear();
//    for ( const auto& [k,v] : Timeline::Timelines().tmapf ) {
//        mySequence.myItems.push_back(MySequence::MySequenceItem{ v.Name(), 0, false });
//    }
//    for ( const auto& [k,v] : Timeline::Timelines().tmapV3 ) {
//        mySequence.myItems.push_back(MySequence::MySequenceItem{ v.Name(), 0, true });
//    }

    ImGui::SetNextWindowPos( ImVec2{ _r.origin().x(), _r.origin().y() } );
    ImGui::SetNextWindowSize( ImVec2{ _r.size().x(), _r.size().y() } );
    ImGui::Begin( "Timeline",  nullptr, ImGuiWindowFlags_NoCollapse );

    // let's create the sequencer
//    static int selectedEntry = -1;
//    static int firstFrame = 0;
//    static bool expanded = true;
    static int currentFrame = 0;

    auto tgroups = Timeline::Groups();
    auto gsize = tgroups.size();

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

    ImGui::Button( "Play" );
    ImGui::SameLine();
    ImGui::Button( "Reset" );
    ImGui::SameLine();
    ImGui::PushItemWidth(90);
    ImGui::InputInt("Frame ", &currentFrame);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    ImVec2 canvas_pos = ImGui::GetCursorScreenPos();            // ImDrawList API uses screen coordinates!
    ImVec2 canvas_size = ImGui::GetContentRegionAvail();        // Resize canvas to what's available

    float titlesWidth = 200.0f;
    draw_list->AddRectFilled(canvas_pos, canvas_pos + ImVec2(titlesWidth, canvas_size.y), 0xFF404040, 0);
    auto topTimeline = canvas_pos+ ImVec2(titlesWidth, 0.0f);
    auto bottomTimeline = topTimeline + ImVec2(canvas_size.x - topTimeline.x, canvas_size.y);
    draw_list->AddRectFilled(topTimeline, bottomTimeline, 0xFF804000, 0);

    float cellHeight = 40.0f;
    int counter = 0;
    for ( const auto& i : mySequence.myItems ) {
        draw_list->AddText( canvas_pos + ImVec2(0.0f, counter*cellHeight), 0xFFFFFFFF, i.mName.c_str());
        counter++;
    }

//    Sequencer(&mySequence, &currentFrame, &expanded, &selectedEntry, &firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND | ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL | ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);

    ImGui::End();

//    bool show_demo_window = true;
//    ImGui::ShowDemoWindow(&show_demo_window);
}
