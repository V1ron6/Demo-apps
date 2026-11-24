package com.v1ron6.counter

import android.os.Bundle
import android.widget.Button
import android.widget.TextView
import com.triskelion.sdk.Triskelion
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private var counter = 0
    private lateinit var counterView: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        counterView = findViewById(R.id.counter_value)
        counter = savedInstanceState?.getInt(STATE_COUNTER) ?: 0
        updateCounter()

        findViewById<Button>(R.id.button_increment).setOnClickListener {
            counter++
            updateCounter()
        }

        findViewById<Button>(R.id.button_decrement).setOnClickListener {
            counter--
            updateCounter()
        }

        val statusView = findViewById<TextView>(R.id.update_status)
        val version = packageManager.getPackageInfo(packageName, 0).versionName
        findViewById<TextView>(R.id.version_label).text = "Version $version"
        statusView.text = CounterApp.status
        CounterApp.listener = { statusView.text = it }
        findViewById<Button>(R.id.button_check_updates).setOnClickListener {
            statusView.text = "Checking..."
            Triskelion.checkNow()
        }

        findViewById<Button>(R.id.button_reset).setOnClickListener {
            counter = 0
            updateCounter()
        }
    }

    override fun onDestroy() {
        CounterApp.listener = null
        super.onDestroy()
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        outState.putInt(STATE_COUNTER, counter)
    }

    private fun updateCounter() {
        counterView.text = counter.toString()
    }

    companion object {
        private const val STATE_COUNTER = "counter"
    }
}
