plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.v1ron6.counter"
    compileSdk = 35

    defaultConfig {
        applicationId = "com.v1ron6.counter"
        minSdk = 24
        targetSdk = 35
        versionCode = 1
        versionName = "1.0.0" // must be x.y.z: Triskelion compares it with the published version
    }

    buildTypes {
        release {
            isMinifyEnabled = false
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }

    kotlinOptions {
        jvmTarget = "17"
    }
}

dependencies {
    implementation(project(":triskelion"))
    implementation("androidx.core:core-ktx:1.15.0")
    implementation("androidx.appcompat:appcompat:1.7.0")
    implementation("com.google.android.material:material:1.12.0")
}
