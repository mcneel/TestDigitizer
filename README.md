# TestDigitizer
Test 3-D digitizer plug-in source code, useful for testing and debugging digitizer issues.

## Prerequisites
The following tools are required to build TestDigitizer:
* Rhino 6 (http://www.rhino3d.com)
* Rhino 6 C/C++ SDK (https://developer.rhino3d.com/)
* Microsoft Visual C++ 2017

## Notes
TestDigitizer uses the Win32 [Raw Input](https://docs.microsoft.com/en-us/windows/win32/inputdev/about-raw-input) APIs to allow a second mouse, attached to your system, to simulate the behavior of a 3-D digitizer.

Just plug-in a second mouse in your system, launch Rhino, run the `Digitize` command, and select `TestDigitizer`. 