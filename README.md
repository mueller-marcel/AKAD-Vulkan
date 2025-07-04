# AKAD Vulkan

Dieses Repository enthält den Code für die Prüfungsaufgabe des Moduls ROB80. Das Repository enthält den Code für die Darstellung eines Dreiecks mit Farbverlauf und einen rotierenden Würfel.
In den folgenden Branches ist der Code enthalten.

1. **Einfarbiges Dreieck mit Farbverlauf**  
   Branch: [`release/triangle`](https://github.com/mueller-marcel/AKAD-Vulkan/tree/release/triangle)  
   Zeigt ein einfaches Dreieck mit Vertex-basiertem Farbverlauf.

2. **Rotierender Würfel**  
   Branch: [`release/cube`](https://github.com/mueller-marcel/AKAD-Vulkan/tree/release/cube)  
   Zeigt einen 3D-Würfel mit kontinuierlicher Rotation.

## 🔧 Build-Anleitung

Die Applikation wurden mit Cmake entwickelt. Folgende Schritte können durchgeführt werden, um die ausführbaren Dateien zu bauen.

### Voraussetzungen

- Visual Studio 2022 mit C++-Entwicklungstools
- Vulkan SDK
- Cmake

1. Repository klonen
```bash
   git clone https://github.com/mueller-marcel/AKAD-Vulkan.git
```

2. Erstellen des Build Verzeichnisses
```bash
   mkdir build
   cd build
```

3. Build Konfiguration ausführen
```bash
   cmake .. -G "Visual Studio 17 2022"
```

4. Build starten
```bash
   cmake --build . --config Release
```

5. Die erstellte ausführbare Datei befindet sich im Verzeichnis `bin/Release` und kann von dort ausgeführt werden.

> [!NOTE]  
> Cmake unterstützt die Generierung des Projekts für viele Buildsysteme.
> Während der Entwicklung wurde Visual Studio 2022 genutzt. Der Build hinsichtlich dieses Cmake-Generators getestet.

## 📚 Bibliotheken

Im Verzeichnis `third_party` wurden alle Bibliotheken abgelegt, um den Build zu vereinfachen und die Abhängigkeit zu externen Paket-Managern zu eliminieren.
Folgende Bibliotheken wurden verwendet:

- SDL
- fastgltf
- fmt
- glm
- imgui
- stb_image
- tinyobjloader
- vkbootstrap
- vma
- volk
