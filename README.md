<h1 align="center">
  Elevator
</h1>

This is a C library DLL that uses the "mock directories" method to bypass Windows User Account Control (UAC) and get administrator rights noninteractively. The API is a single function that works like the `system(3)` call. This is just one of many implementations of this particular method, and it's also just one of many methods available for bypassing UAC. You'll need to do additional work to access `NT AUTHORITY\SYSTEM`.
