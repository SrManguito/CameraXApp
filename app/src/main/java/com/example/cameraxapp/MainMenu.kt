package com.example.cameraxapp

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

// MethodItem.kt
// MethodItem.kt
data class MethodItem(val name: String, val description: String, val activityClass: Class<*>, val imageResId: Int)


// MainMenu.kt
class MainMenu : AppCompatActivity() {

    private lateinit var methodList: List<MethodItem>
    private lateinit var recyclerView: RecyclerView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main_menu)
//        findViewById<RecyclerView>(R.id.recyclerview).setBackgroundColor(Color.parseColor("#F5F5F5"))
        // Initialize the list of methods with three example activities
        methodList = listOf(
            MethodItem("OOK Single Source", "This implements a luminosity thresholding algorithm",
                RealTime::class.java, R.drawable.onoff),
            MethodItem("VideoCapLayout", "This template of a videocapture layout",
                VidCapTemplate::class.java,R.drawable.config),
            MethodItem("VideoRecording", "This method records video and decodes after",
                VidProcPost::class.java,R.drawable.ic_start)
        )

        recyclerView = findViewById(R.id.recyclerview)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = MethodAdapter(methodList) { item ->
            startActivity(Intent(this, item.activityClass))
        }
    }

}



