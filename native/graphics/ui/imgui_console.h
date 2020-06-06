//
// Created by Dado on 10/08/2018.
//

#pragma once

#include "core/command.hpp"
#include "graphics/imgui/imgui.h"

namespace  JMATH {
    class Rect2f;
}

struct ImGuiConsole
{
    char                  InputBuf[512]{};
    ImVector<char*>       Items;
    bool                  ScrollToBottom{};
    ImVector<char*>       History;
    int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
    ImVector<const char*> Commands;
    ImVec2 mPos;
    ImVec2 mSize;

    ImGuiConsole( CommandQueue& _cq );
    ~ImGuiConsole();

    // Portable helpers
    static int   Stricmp(const char* str1, const char* str2)         { int d; while ((d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; } return d; }
    static int   Strnicmp(const char* str1, const char* str2, int n) { int d = 0; while (n > 0 && (d = toupper(*str2) - toupper(*str1)) == 0 && *str1) { str1++; str2++; n--; } return d; }
    static char* Strdup(const char *str)                             { size_t len = strlen(str) + 1; void* buff = malloc(len); return (char*)memcpy(buff, (const void*)str, len); }
    static void  Strtrim(char* str)                                  { char* str_end = str + strlen(str); while (str_end > str && str_end[-1] == ' ') str_end--; *str_end = 0; }

    void ClearLog();
    void AddLogLine(const char* buf );
    void AddLog(const char* fmt, ...) IM_FMTARGS(2);
    void Draw( const JMATH::Rect2f& _r );
    void ExecCommand(const char* command_line);

    static int TextEditCallbackStub(ImGuiTextEditCallbackData* data);
    int TextEditCallback(ImGuiTextEditCallbackData* data);
private:
    CommandQueue& cq;
};



