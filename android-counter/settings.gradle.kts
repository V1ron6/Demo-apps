pluginManagement {
    repositories {
        google()
        mavenCentral()
        gradlePluginPortal()
    }
}

dependencyResolutionManagement {
    repositoriesMode.set(RepositoriesMode.FAIL_ON_PROJECT_REPOS)
    repositories {
        google()
        mavenCentral()
    }
}

rootProject.name = "Android Counter"
include(":app")

// Triskelion update SDK (the repo is a sibling folder: Demo-apps-main/triskelion)
include(":triskelion")
project(":triskelion").projectDir = file("../triskelion/sdk-android")
