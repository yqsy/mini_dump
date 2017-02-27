#include "mini_dump.h"

#include <windows.h>
#include <shellapi.h>

#include <string>

// Is dbghelp.h dbghelp.lib dbghelp.dll windows build-in?
#include <dbghelp.h>
#pragma comment (lib,"dbghelp.lib")

// Visual Studio 2005 compatible 
#define snprintf(buf,len, format,...) _snprintf_s(buf, len,len, format, __VA_ARGS__)

namespace minidump
{
    // needn't delete
    std::string* app_name = 0;

    std::string* app_version = 0;

    // "c:\folder1\xxx.exe" to "c:\folder1\"
    std::string GetDirectory(const std::string& execution)
    {
        std::string path("");
        size_t pos = execution.find_last_of("\\");
        if (pos != std::string::npos)
            path = execution.substr(0, pos + 1);
        return path;
    }

    // Generage dump file to avoid file name collisions
    std::string GetDumpFileMark()
    {
        SYSTEMTIME system_local_time;
        GetLocalTime(&system_local_time);

        char file_name[MAX_PATH];

        snprintf(file_name, MAX_PATH, "%s%s-%04d%02d%02d-%02d%02d%02d-%ld-%ld.dmp",
            app_name->c_str(), app_version->c_str(),
            system_local_time.wYear, system_local_time.wMonth, system_local_time.wDay,
            system_local_time.wHour, system_local_time.wMinute, system_local_time.wSecond,
            GetCurrentProcessId(), GetCurrentThreadId());

        return file_name;
    }

    // Generate dump file whole name in GetModuleFileName()'s directory
    // Like: F:\reference\project\TestMiniDump\VsProject\build\Debug\dump\20161015-110026-8552-9344.dmp
    std::string GetDumpFileName()
    {
        char file_path[MAX_PATH];
        GetModuleFileName(NULL, file_path, MAX_PATH);

        std::string path = GetDirectory(file_path);

        path += "dump\\";

        std::string file_mark = GetDumpFileMark();

        path += file_mark;

        return path;
    }

    MINIDUMP_EXCEPTION_INFORMATION GetExpParam(EXCEPTION_POINTERS* exception_pointers)
    {
        MINIDUMP_EXCEPTION_INFORMATION exp_param;
        exp_param.ThreadId = GetCurrentThreadId();
        exp_param.ExceptionPointers = exception_pointers;
        exp_param.ClientPointers = TRUE;
        return exp_param;
    }

    int GenerateDump(EXCEPTION_POINTERS* exception_pointers)
    {
        std::string dump_file_name = GetDumpFileName();

        // The "\dump" whole directory
        std::string dump_file_dir = GetDirectory(dump_file_name);

        CreateDirectory(dump_file_dir.c_str(), NULL);

        HANDLE dump_file_handle = CreateFile(dump_file_name.c_str(), 
            GENERIC_READ| GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ,
             0, CREATE_ALWAYS, 0, 0);

        MINIDUMP_EXCEPTION_INFORMATION exp_param = 
            GetExpParam(exception_pointers);

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), 
            dump_file_handle, MiniDumpNormal, &exp_param, NULL, NULL);

        return 0;
    }
    
    LONG _stdcall GolbalExceptionHandler(EXCEPTION_POINTERS* exception_pointers)
    {
        GenerateDump(exception_pointers);
        return EXCEPTION_EXECUTE_HANDLER;
    }
    
    void IntiializeMemory()
    {
        app_name = new std::string();
        app_version = new std::string();
    }

    void MiniDumpBegin(const char* app_name, const char* app_version)
    {
        IntiializeMemory();

        *minidump::app_name = app_name;
        *minidump::app_version = app_version;

        SetUnhandledExceptionFilter(GolbalExceptionHandler);
    }
}
