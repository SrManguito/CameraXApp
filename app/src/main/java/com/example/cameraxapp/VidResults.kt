package com.example.cameraxapp

import android.app.Activity
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.*
import androidx.lifecycle.lifecycleScope
import com.example.cameraxapp.databinding.ActivityVidResultsBinding
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Locale

private lateinit var openMediaFolderLauncher: ActivityResultLauncher<Intent>

class VidResults : AppCompatActivity() {
    private lateinit var viewBinding: ActivityVidResultsBinding
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        viewBinding = ActivityVidResultsBinding.inflate(layoutInflater)
        setContentView(viewBinding.root)

        openMediaFolderLauncher = registerForActivityResult(ActivityResultContracts.StartActivityForResult()) { result ->
            if (result.resultCode == Activity.RESULT_OK) {
                val videoUri = result.data?.data
                if (videoUri != null) {

                    // Show loading indicator immediately

                    viewBinding.textView.text = "Processing video, please wait..."

                    // Process in background
                    lifecycleScope.launch {
                        try {
                            val output = withContext(Dispatchers.IO) {
                                // All heavy processing happens on background thread
                                val filePath = getFileFromContentUri(videoUri)
                                val outputFilename = SimpleDateFormat(FILENAME_FORMAT, Locale.US)
                                    .format(System.currentTimeMillis())
                                val file = File(baseContext.getExternalFilesDir(null), "${outputFilename}.txt")
                                val path = file.absolutePath

                                // This heavy operation runs on background thread
                                MyClass.stringFromJNI(filePath.toString(), path)
                            }

                            // Update UI on main thread when processing is complete
                            viewBinding.textView.text = output

                        } catch (e: Exception) {
                            // Handle errors on main thread
                            viewBinding.textView.text = "Error processing video: ${e.message}"
//                            Log.e(TAG, "Error processing video", e)
                        }
                    }
                }
            }
        }

        viewBinding.openFolderButton.setOnClickListener {
            val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
                addCategory(Intent.CATEGORY_OPENABLE)
                type = "video/*"
            }
            openMediaFolderLauncher.launch(intent)
        }

    }
    class MyClass {

        companion object {
            init {
                System.loadLibrary("cameraxapp")
            }
            external fun stringFromJNI(input :String, out_path : String) :String

        }
    }

    companion object {
        private const val TAG = "CameraXApp"
        private const val FILENAME_FORMAT = "yyyy-MM-dd-HH-mm-ss-SSS"
    }

    private fun getFileFromContentUri(uri: Uri): File? {
        val tempFile = File.createTempFile("video_temp", ".mp4", cacheDir)
        contentResolver.openInputStream(uri)?.use { inputStream ->
            FileOutputStream(tempFile).use { outputStream ->
                inputStream.copyTo(outputStream)
            }
        }
        return tempFile
    }

}