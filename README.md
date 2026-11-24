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

## Updates with Triskelion

Both apps embed the Triskelion update library (`triskelion/` folder, a copy of the Triskelion repo; use a git submodule in real projects).

Both apps are already pointed at `https://triskelion.onrender.com` with the public key of `counter.key` (`CounterApp.kt` and `main.cpp`). App id is `counter` on both platforms. Register the app once on the server with that public key (see `triskelion/USAGE_INSTRUCTIONS.md`, section 2). Versions must be `x.y.z` (`versionName` in `app/build.gradle.kts`, `COUNTER_VERSION` in `windows-counter/CMakeLists.txt`).

Android: `settings.gradle.kts` already includes the SDK module; it needs the NDK and CMake from the SDK Manager. Windows: `build.ps1` installs OpenSSL through vcpkg.

Publish a new version (see `triskelion/README.md`):

```bash
node triskelion/tools/sign/sign.js publish --key counter.key --app counter --platform android \
  --version 1.1.0 --file app-release.apk
node triskelion/tools/sign/sign.js publish --key counter.key --app counter --platform windows \
  --version 1.1.0 --file CounterApp.exe
```

Build the new version with the version number bumped first, otherwise the app would see itself as outdated forever.
