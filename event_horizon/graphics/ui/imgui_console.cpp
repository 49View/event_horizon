//
// Created by Dado on 10/08/2018.
//

#include "imgui_console.h"
#include "core/math/rect2f.h"

ImGuiConsole::ImGuiConsole(CommandQueue& _cq) : cq(_cq) {
    ClearLog();
    memset(InputBuf, 0, sizeof(InputBuf));
    HistoryPos = -1;
    Commands.push_back("HELP");
    Commands.push_back("HISTORY");
    Commands.push_back("CLEAR");
    Commands.push_back("CLASSIFY");  // "classify" is only here to provide an example of "C"+[tab] completing to "CL" and displaying matches.
    hookLogCallbackFunction( std::bind( &ImGuiConsole::AddLogLine, this, std::placeholders::_1) );
}

ImGuiConsole::~ImGuiConsole() {
    ClearLog();
    for (int i = 0; i < History.Size; i++)
        free(History[i]);
}

void ImGuiConsole::ClearLog() {
    for (int i = 0; i < Items.Size; i++)
        free(Items[i]);
    Items.clear();
    ScrollToBottom = true;
}

void ImGuiConsole::AddLogLine( const char *buf ) {
    Items.push_back(Strdup(buf));
    ScrollToBottom = true;
}

void ImGuiConsole::AddLog( const char *fmt, ... ) {
    // FIXME-OPT
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);
    buf[IM_ARRAYSIZE(buf)-1] = 0;
    va_end(args);
    AddLogLine(buf);
}

void ImGuiConsole::Draw( const JMATH::Rect2f& _r ) {
    mPos = ImVec2{ _r.origin().x(), _r.origin().y() };
    mSize = ImVec2{ _r.size().x(), _r.size().y() };
    ImGui::SetNextWindowPos(mPos);
    ImGui::SetNextWindowSize(mSize);
    if (!ImGui::Begin("Command Console", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
    {
        ImGui::End();
        return;
    }

    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
    if (ImGui::BeginPopupContextWindow())
    {
        if (ImGui::Selectable("Clear")) ClearLog();
        ImGui::EndPopup();
    }

    // Display every line as a separate entry so we can change their color or add custom widgets. If you only want raw text you can use ImGui::TextUnformatted(log.begin(), log.end());
    // NB- if you have thousands of entries this approach may be too inefficient and may require user-side clipping to only process visible items.
    // You can seek and display only the lines that are visible using the ImGuiListClipper helper, if your elements are evenly spaced and you have cheap random access to the elements.
    // To use the clipper we could replace the 'for (int i = 0; i < Items.Size; i++)' loop with:
    //     ImGuiListClipper clipper(Items.Size);
    //     while (clipper.Step())
    //         for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
    // However, note that you can not use this code as is if a filter is active because it breaks the 'cheap random-access' property. We would need random-access on the post-filtered list.
    // A typical application wanting coarse clipping and filtering may want to pre-compute an array of indices that passed the filtering test, recomputing this array when user changes the filter,
    // and appending newly elements as they are inserted. This is left as a task to the user until we can manage to improve this example code!
    // If your items are of variable size you may want to implement code similar to what ImGuiListClipper does. Or split your data into fixed height items to allow random-seeking into your list.
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
    ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    for (int i = 0; i < Items.Size; i++)
    {
        const char* item = Items[i];
        ImVec4 col = col_default_text;
        if (strstr(item, "[ERROR]")) {
            col = ImColor(1.0f,0.2f,0.2f,1.0f);
        } else if (strncmp(item, "# ", 2) == 0) {
            col = ImColor(1.0f,0.78f,0.58f,1.0f);
        } else if (strstr(item, "[HTTP-GET]")) {
            if (strstr(item, "[HTTP-GET] http")) {
                col = ImColor( 0.2f, 0.98f, 0.28f, 1.0f );
            } else if (strstr(item, "[HTTP-GET] Response code: 2")) {
                col = ImColor( 0.2f, 0.78f, 0.28f, 1.0f );
            } else  {
                col = ImColor( 1.0f,0.48f,0.38f,1.0f );
            }
        }
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::TextUnformatted(item);
        ImGui::PopStyleColor();
    }
    if (ScrollToBottom)
        ImGui::SetScrollHere(1.0f);
    ScrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // Command-line
    bool reclaim_focus = false;
    ImGui::PushItemWidth(mSize.x);
    if (ImGui::InputText("", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue|ImGuiInputTextFlags_CallbackCompletion|ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
    {
        Strtrim(InputBuf);
        if (InputBuf[0])
            ExecCommand(InputBuf);
        strcpy(InputBuf, "");
        reclaim_focus = true;
    }

    // Demonstrate keeping focus on the input box
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget

    ImGui::End();
}

void ImGuiConsole::ExecCommand( const char *command_line ) {
    cq.script( command_line );

    AddLog("# %s\n", command_line);

    // Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
    HistoryPos = -1;
    for (int i = History.Size-1; i >= 0; i--)
        if (Stricmp(History[i], command_line) == 0)
        {
            free(History[i]);
            History.erase(History.begin() + i);
            break;
        }
    History.push_back(Strdup(command_line));

    // Process command
    if (Stricmp(command_line, "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(command_line, "HELP") == 0)
    {
        AddLog("Commands:");
        for (int i = 0; i < Commands.Size; i++)
            AddLog("- %s", Commands[i]);
    }
    else if (Stricmp(command_line, "HISTORY") == 0)
    {
        int first = History.Size - 10;
        for (int i = first > 0 ? first : 0; i < History.Size; i++)
            AddLog("%3d: %s\n", i, History[i]);
    }
//    else
//    {
//        AddLog("Unknown command: '%s'\n", command_line);
//    }
}

int ImGuiConsole::TextEditCallbackStub( ImGuiTextEditCallbackData *data ) // In C++11 you are better off using lambdas for this sort of forwarding callbacks
{
    ImGuiConsole* console = (ImGuiConsole*)data->UserData;
    return console->TextEditCallback(data);
}

int ImGuiConsole::TextEditCallback( ImGuiTextEditCallbackData *data ) {
    //AddLog("cursor: %d, selection: %d-%d", data->CursorPos, data->SelectionStart, data->SelectionEnd);
    switch (data->EventFlag)
    {
        case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Example of TEXT COMPLETION

            // Locate beginning of current word
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build a list of candidates
            ImVector<const char*> candidates;
            for (int i = 0; i < Commands.Size; i++)
                if (Strnicmp(Commands[i], word_start, (int)(word_end-word_start)) == 0)
                    candidates.push_back(Commands[i]);

            if (candidates.Size == 0)
            {
                // No match
                AddLog("No match for \"%.*s\"!\n", (int)(word_end-word_start), word_start);
            }
            else if (candidates.Size == 1)
            {
                // Single match. Delete the beginning of the word and replace it entirely so we've got nice casing
                data->DeleteChars((int)(word_start-data->Buf), (int)(word_end-word_start));
                data->InsertChars(data->CursorPos, candidates[0]);
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches. Complete as much as we can, so inputing "C" will complete to "CL" and display "CLEAR" and "CLASSIFY"
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int i = 0; i < candidates.Size && all_candidates_matches; i++)
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end-word_start));
                    data->InsertChars(data->CursorPos, candidates[0], candidates[0] + match_len);
                }

                // List matches
                AddLog("Possible matches:\n");
                for (int i = 0; i < candidates.Size; i++)
                    AddLog("- %s\n", candidates[i]);
            }

            break;
        }
        case ImGuiInputTextFlags_CallbackHistory:
        {
            // Example of HISTORY
            const int prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Size - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Size)
                        HistoryPos = -1;
            }

            // A better implementation would preserve the data on the current input line along with cursor position.
            if (prev_history_pos != HistoryPos)
            {
                data->CursorPos = data->SelectionStart = data->SelectionEnd = data->BufTextLen = (int)snprintf(data->Buf, (size_t)data->BufSize, "%s", (HistoryPos >= 0) ? History[HistoryPos] : "");
                data->BufDirty = true;
            }
        }
    }
    return 0;
}
