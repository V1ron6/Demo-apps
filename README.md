# Counter Demo Apps

Two small counter applications:

- `android-counter/` - Kotlin Android app
- `windows-counter/` - Native C++ Win32 app for Windows 10

Both apps support incrementing, decrementing, and resetting a counter.

## Android

Open `android-counter` in Android Studio and run it on an Android device or emulator.

Requires Android Studio with Android SDK 35.

To install the required Linux packages and build a debug APK:

```bash
cd android-counter
./build.sh
```

## Windows

Open a Developer Command Prompt for VS 2022, then run:

```bat
cmake -S windows-counter -B build/windows-counter
cmake --build build/windows-counter --config Release
```

Run `build/windows-counter/Release/CounterApp.exe`.

To install the required Windows packages and build the app:

```powershell
Set-ExecutionPolicy -Scope Process Bypass
.\windows-counter\build.ps1
```
