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

#ifndef ELEVATOR_H
#define ELEVATOR_H

#ifdef __cplusplus
#   define ELEVATOR_EXTERN extern "C"
#else
#   define ELEVATOR_EXTERN extern
#endif

#ifdef _ELEVATOR_INTERNAL
#   define ELEVATOR_EXPORT __declspec(dllexport)
#else
#   define ELEVATOR_EXPORT __declspec(dllimport)
#endif

#define ELEVATOR_API ELEVATOR_EXTERN ELEVATOR_EXPORT
#define ELEVATOR_CALL __cdecl

/**
 * Execute a shell command in an invisible admin command prompt. The calling
 * process itself, however, need not be elevated :O
 *
 * @param command The shell command to execute
 * @return Either zero on success or a nonzero error code
 */
ELEVATOR_API int ELEVATOR_CALL elevator(const char* command);

#endif // #ifndef ELEVATOR_H
