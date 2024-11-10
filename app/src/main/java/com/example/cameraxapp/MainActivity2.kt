package com.example.cameraxapp

import android.content.Intent
import android.graphics.Color
import android.os.Bundle
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.ViewCompat
import androidx.core.view.WindowInsetsCompat
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

// MethodItem.kt
// MethodItem.kt
data class MethodItem(val name: String, val description: String, val activityClass: Class<*>)


// MainActivity2.kt
class MainActivity2 : AppCompatActivity() {

    private lateinit var methodList: List<MethodItem>
    private lateinit var recyclerView: RecyclerView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main2)
//        findViewById<RecyclerView>(R.id.recyclerview).setBackgroundColor(Color.parseColor("#F5F5F5"))
        // Initialize the list of methods with three example activities
        methodList = listOf(
            MethodItem("OOK Single Source", "This implements a luminosity thresholding algorithm", MainActivity::class.java),
//            MethodItem("Method 2", "Description of Method 2", Method2Activity::class.java),
//            MethodItem("Method 3", "Description of Method 3", Method3Activity::class.java)
        )

        recyclerView = findViewById(R.id.recyclerview)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = MethodAdapter(methodList) { item ->
            startActivity(Intent(this, item.activityClass))
        }
    }

}



