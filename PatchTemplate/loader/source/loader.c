typedef unsigned int u32;
typedef u32 Handle;
typedef u32 Result;

Result svcOpenProcess(Handle* process, u32 processId);
Result svcGetProcessId(u32* out, Handle handle);
void svcBreak(u32 breakReason);
Result svcControlProcessMemory(Handle process, u32 addr0, u32 addr1, u32 size, u32 type, u32 perm);

Handle getCurrentProcessHandle();

void LoaderMain()
{
    Result res;
    
    // TODO: Get these addresses from a generated file
    u32 address =       0x006C5000;
    u32 neededMemory =  0x00001000;

    res = svcControlProcessMemory(getCurrentProcessHandle(), address, address, neededMemory, 6, 7);
    
    if (res < 0)
        svcBreak(1);
}


Handle getCurrentProcessHandle()
{
    Handle handle = 0;
    u32 currentPid = 0;
    Result res;

    svcGetProcessId(&currentPid, 0xffff8001);
    res = svcOpenProcess(&handle, currentPid);
    
    if (res != 0)
        return 0;

    return handle;
}
