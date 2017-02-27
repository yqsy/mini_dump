// reference:https://msdn.microsoft.com/zh-cn/library/windows/desktop/ee416349(v=vs.85).aspx


#ifndef mini_dump_h__
#define mini_dump_h__


namespace minidump
{
    void MiniDumpBegin(const char* app_name, const char* app_version);
}

#endif // mini_dump_h__
