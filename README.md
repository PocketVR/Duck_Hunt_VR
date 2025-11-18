# 🦆 Duck Hunt AR: Cardboard Edition

A comprehensive, low-latency implementation of the classic Duck Hunt game, leveraging WebAssembly (WASM) for native performance, and augmented reality (AR) for a truly immersive, physical gaming experience via Google Cardboard.

This project showcases the integration of C++ graphics engines with modern browser sensor APIs and AR frameworks.

## 🚀 Key Technologies

| Technology | Role |
| :--- | :--- |
| **C++ & Emscripten** | Core game logic, physics, and graphics rendering (via the `Ungine` engine). |
| **WebAssembly (WASM)** | Provides native-level performance crucial for smooth 60 FPS VR/AR rendering. |
| **ARToolKit (or similar)** | Handles real-time detection and tracking of the AR marker, anchoring the ducks in the real world. |
| **AbsoluteOrientationSensor** | Uses the Android device's gyroscope to provide precise, low-latency aim control. |
| **Google Cardboard** | Provides the stereoscopic view necessary for the VR/AR experience. |
| **raylib** | The Core Graphics Engine. Handles all 2D/3D rendering and texture loading. |

## ✨ Features

* **Physical Aiming:** Control the crosshair by physically turning your head (using the phone's gyroscope).
* **AR Duck Tracking:** Ducks are rendered in 3D, tracked, and anchored to a physical AR marker in your environment.
* **Browser-Based:** Runs entirely within a web browser, making deployment simple and cross-platform (though optimized for Android devices).
* **High Performance:** Built for 60 FPS performance thanks to the C++/WASM compilation pipeline.

## 🛠️ Build and Setup

### Prerequisites

1.  **Emscripten SDK:** Must be installed and configured.
3.  **The AR Marker:** Download and print the required marker pattern.
    * [Link to AR Marker PDF]

## Showcase