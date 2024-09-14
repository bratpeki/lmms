INCLUDE(BuildPlugin)

BUILD_PLUGIN(waveanalzyer WaveAnalyzer.cpp WaveAnalyzerControls.cpp WaveAnalyzerControlDialog.cpp MOCFILES WaveAnalyzerControls.h WaveAnalyzerControlDialog.h EMBEDDED_RESOURCES artwork.png logo.png)
