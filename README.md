
# How to develop in the Camera X App
***
## Section Index
1. [Introduction](#introduction)
2. [Android](#android-studio)
3. [Kotlin](#kotlin)
4. [JNI](#jni)
5. [C++](#c)
6. [Types of developer](#types-of-developers)
***
## Introduction
CameraXApp is a platform created to help in the development of OCC based systems, the final result is a Android based application that let the end-user access the camera of the device in order to make experiments, try new implementations and see the results in the same app.

We believe that the growth of OCC in the electrical engineer community (Or for any developer interested) should not be stopped by the technical knowledge needed on Android, Java, Kotlin, etc. This platform has as target $number types of user based on their base knowledge and depth of implentation intended.

User cases:
* App user: This user only uses the app and the base methods already there to make experiments, doing minor changes in the parameters of the receptor.
* C++ Developer: This user is trying to implement their own OCC methods using the base Kotlin architecture and modifying the C++ section.
* UI/UX Developer: This user wants to modify also the app interface, moving objets, adding buttons for a better user experience.
* Android Developer: This is the more advance user that will make changes in all the source code, adding extra methods, interfaces

The next chapters contain some basic information about the main topics of the platform and a link to the specific documentation in case is needed a more in-depth view.

## Android Studio

The app use two programming languages, Kotlin and C++, but it's mainly developed in Kotlin. In the proyect you will find the following structure, where **AndroidManifest.xml** controls the details, parameteres and the relation between interfaces, **cpp** is for C++ files/related, **cameraxapp** is for the Kotlin code and **res** in the resources folder where you can find all the resources like, images, layouts, parameteres, etc.  

```
├───app
    │   
    └───src
        ├───main
            │   AndroidManifest.xml
            │
            ├───cpp
            │       cameraxapp.cpp
            │       CMakeLists.txt
            │       opencv-utils.cpp
            │       opencv-utils.h
            │
            ├───java
            │   └───com
            │       └───example
            │           └───cameraxapp
            │                   MainMenu.kt
            │                   MethodAdapter.kt
            │                   RealTime.kt
            │                   VidCapTemplate.kt
            │                   VidProcPost.kt                
            │
            └───res
                │
                ├───drawable
                │
                ├───layout
                │
                |───values
                |
                :
```
The most important part from every file/folder mentioned before are:
* AndroidManifest.xml: This file is where you define the permissions that the app will need, the definition of the main interface and some parameters.
* cpp: folder where all the cpp code is stored, where you define the libraries and the connection to Kotlin via JNI
* ../cameraxapp: this folder contains all the Kotlin codes fot the activities and auxiliar classes. (Consider an activity as an interface)
* res: folder with all the constant parameter and images for the app.
    * drawable: folder with the images used for all the icons and background
    * layout: xml's with the layout for each interface and auxiliar elements on them
    * values: xml's with the hexadecimal colors used and parameter values for the app.

## Kotlin

For the Kotlin part there is the cameraxapp folder, where you can find this files:

* MainMenu.kt: This is the main activity, sets the interfaces and manage the access to other activities
* MethodAdapter.kt: This is an aux class to handle the navigation from the main activity to the others
* RealTime.kt: This activity for real time image processing, it's not fully implemented yet, but there is the base method to analyze frame by frame in real time what the camera sees. 
* VidCapTemplate.kt: This activity acts as a sandbox object interface, where the developer can place, delete and try different interface objects without breaking other interfaces.
* VidProcPost.kt: This activity is for video capture and postprocessing, where you can record videos and select any video from the movile to process and decodificate its information.


## JNI
<!-- TODO Muy pelao, explicar mjr como se utiliza JNI -->
JNI is the Java Natural Interface which serves as a connector between Kotlin and C++, it follows the following structure

``` cpp 
1 extern "C" JNIEXPORT j<type_output> JNICALL
2 Java_<package>_<file>_<Class>_<function>(JNIEnv *env, jobject p_this, j<type> input1, j<type> input2) {
3    <type> output = someFunction(input1, input2);
4    return output;
5 }
```

* first: define the language and the types of the outputs, for example jstring
* second: call the full dir of the function you wanna use, for example: com_example_cameraxapp_VidProcPost_MyClass_stringFromJNI
* third: define the variables of input and enviroment, for example: jstring input1, jint input2
* fourth: call the C++ functions in order to get the defined output.
* fifth: return the variable (you have to do some extra steps in case there isn't a direct transformation between types, for example: C++ String to Kotlin String)

## C++
For the C++ part there is the cpp folder, where you can find this files:

* CMakeList.txt: check the libraries and files of C++.
* cameraxapp.cpp: connects Kotlin with C++ using JNI
* opencv-utils.cpp: contains all the C++ functions and classes needed for the image/video processing

This is the most important part related to the image and video processing, once is setup everything in the CMakeList.txt including core libraries as OpenCV, it's needed to define (being consistent with JNI and Kotlin) the way that C++ interacts with Kotlin. 

There are two ways implemented for each case of use:

* Image Processing: For this method Kotlin sends each frame from the image buffer to C++ via JNI, after the detection, the binary result is given back to Kotlin
* Video Processing: For this method Kotlin sends to C++ the path of a buffer where the selected video is streamed (Because we don't want to override something in the original video) and a path to an external file for the app to write the binary output to. With both paths C++ can open the video and write on a file as it would do in any other enviroment.

## Types of developers

According to the types of user from the [Introduction](#introduction), there will be 4 other documentations doing a more in-depth description of the files of interest and a guide of use for them. Consider that for each level you should also check from the others above so you can have a better vision of the proyect.

* App user [User Documentation](docs/user_doc.md)
* C++ Developer [C++ Documentation](docs/cpp.md)
* UI/UX Developer [UI/UX Documentation](docs/cpp.md)
* Android Developer [Kotlin Documentation](docs/cpp.md)

<!-- TODO Add the other documentation links -->
