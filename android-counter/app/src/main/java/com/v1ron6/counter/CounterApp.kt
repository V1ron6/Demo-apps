package com.v1ron6.counter

import android.app.Application
import android.os.Handler
import android.os.Looper
import com.triskelion.sdk.Triskelion

class CounterApp : Application() {
    override fun onCreate() {
        super.onCreate()
        Triskelion.init(
            this,
            Triskelion.Config(
                serverUrl = SERVER_URL,
                appId = APP_ID,
                publicKeyB64 = PUBLIC_KEY,
                useFcm = false, // no Firebase in this demo: updates arrive by the timer, the live stream while open, and the button
                onResult = { outcome, detail, version ->
                    val text = when (outcome) {
                        Triskelion.Outcome.UP_TO_DATE -> "Up to date"
                        Triskelion.Outcome.APPLIED -> "Update $version ready: confirm the install prompt"
                        else -> "$outcome ${detail}".trim()
                    }
                    status = text
                    Handler(Looper.getMainLooper()).post { listener?.invoke(text) }
                },
            ),
        )
    }

    companion object {
        // Public key of counter.key (the private key never goes in the app).
        // Server: your Triskelion service on Render.
        const val SERVER_URL = "https://triskelion.onrender.com"
        const val APP_ID = "counter"
        const val PUBLIC_KEY = "IQncJwj/zAPAdrIxZ3kQk9fc5MJmZyghNyo1i4cZwTw="

        @Volatile var status: String = ""
        @Volatile var listener: ((String) -> Unit)? = null
    }
}
