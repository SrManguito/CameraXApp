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
        // Initialize the list of methods with three example activities
        methodList = listOf(
            MethodItem("VideoRecording", "This section records video for experiments",
            VidProcPost::class.java,R.drawable.ic_start),
            MethodItem("Results visualizer", "This section allows the user to select a video to be decodified",
                VidResults::class.java, R.drawable.onoff),
            MethodItem("VideoCapLayout", "This section allows the developer to try UI elements isolated from other activities",
                VidCapTemplate::class.java,R.drawable.config),

        )

        recyclerView = findViewById(R.id.recyclerview)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = MethodAdapter(methodList) { item ->
            startActivity(Intent(this, item.activityClass))
        }
    }

}



