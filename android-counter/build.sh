#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"

JDK_VERSION="${JDK_VERSION:-21}"
ANDROID_COMPILE_SDK="${ANDROID_COMPILE_SDK:-35}"
ANDROID_BUILD_TOOLS="${ANDROID_BUILD_TOOLS:-35.0.0}"
ANDROID_CMDLINE_TOOLS_VERSION="${ANDROID_CMDLINE_TOOLS_VERSION:-13114758}"
ANDROID_SDK_ROOT="${ANDROID_SDK_ROOT:-$HOME/Android/Sdk}"

CMDLINE_TOOLS_URL="https://dl.google.com/android/repository/commandlinetools-linux-${ANDROID_CMDLINE_TOOLS_VERSION}_latest.zip"

install_linux_packages() {
    if command -v apt-get >/dev/null 2>&1; then
        echo "Installing Android build packages..."
        sudo apt-get update
        sudo apt-get install -y curl unzip ca-certificates "openjdk-${JDK_VERSION}-jdk"
    elif command -v dnf >/dev/null 2>&1; then
        sudo dnf install -y curl unzip ca-certificates "java-${JDK_VERSION}-openjdk-devel"
    elif command -v pacman >/dev/null 2>&1; then
        sudo pacman -Sy --needed curl unzip ca-certificates "jdk${JDK_VERSION}-openjdk"
    else
        echo "Unsupported package manager. Install curl, unzip, and JDK ${JDK_VERSION} manually." >&2
        exit 1
    fi
}

configure_java() {
    if [[ -d "/usr/lib/jvm/java-${JDK_VERSION}-openjdk-amd64" ]]; then
        export JAVA_HOME="/usr/lib/jvm/java-${JDK_VERSION}-openjdk-amd64"
        export PATH="$JAVA_HOME/bin:$PATH"
    fi

    if ! command -v java >/dev/null 2>&1; then
        echo "JDK ${JDK_VERSION} is unavailable after package installation." >&2
        exit 1
    fi
}

install_android_sdk() {
    if [[ -x "$ANDROID_SDK_ROOT/cmdline-tools/latest/bin/sdkmanager" ]] &&
       [[ -d "$ANDROID_SDK_ROOT/platforms/android-$ANDROID_COMPILE_SDK" ]] &&
       [[ -d "$ANDROID_SDK_ROOT/build-tools/$ANDROID_BUILD_TOOLS" ]]; then
        return
    fi

    mkdir -p "$ANDROID_SDK_ROOT/cmdline-tools"
    local archive="/tmp/android-commandlinetools.zip"
    rm -f "$archive"
    curl --fail --location --retry 3 --output "$archive" "$CMDLINE_TOOLS_URL"
    rm -rf "$ANDROID_SDK_ROOT/cmdline-tools/latest"
    unzip -q "$archive" -d "$ANDROID_SDK_ROOT/cmdline-tools"
    mv "$ANDROID_SDK_ROOT/cmdline-tools/cmdline-tools" \
       "$ANDROID_SDK_ROOT/cmdline-tools/latest"

    yes | "$ANDROID_SDK_ROOT/cmdline-tools/latest/bin/sdkmanager" \
        --sdk_root="$ANDROID_SDK_ROOT" \
        --licenses >/dev/null

    "$ANDROID_SDK_ROOT/cmdline-tools/latest/bin/sdkmanager" \
        --sdk_root="$ANDROID_SDK_ROOT" \
        "platform-tools" \
        "platforms;android-$ANDROID_COMPILE_SDK" \
        "build-tools;$ANDROID_BUILD_TOOLS"
}

install_linux_packages
configure_java
install_android_sdk

export ANDROID_SDK_ROOT
export ANDROID_HOME="$ANDROID_SDK_ROOT"
printf 'sdk.dir=%s\n' "$ANDROID_SDK_ROOT" > "$PROJECT_DIR/local.properties"

cd "$PROJECT_DIR"
./gradlew assembleDebug

APK="$PROJECT_DIR/app/build/outputs/apk/debug/app-debug.apk"
echo "Debug APK: $APK"
