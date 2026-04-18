#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_network/juce_network.h>
#include <juce_audio_formats/juce_audio_formats.h>

using namespace juce;

class AudioStreamer : public AudioIODeviceCallback {
public:
    AudioStreamer() { socket.bindToPort(0); formatManager.registerBasicFormats(); }
    void startRecording(const File& file) {
        if (currentSampleRate > 0) {
            fileStream.reset(new FileOutputStream(file));
            WavAudioFormat wavFormat;
            writer.reset(wavFormat.createWriterFor(fileStream.get(), currentSampleRate, 2, 24, {}, 0));
            isRecording = true;
        }
    }
    void stopRecording() { isRecording = false; writer.reset(); fileStream.release(); }
    void audioDeviceIOCallbackWithContext(const float** inputChannelData, int numInputChannels, float** outputChannelData, int numOutputChannels, int numSamples, const AudioIODeviceCallbackContext& context) override {
        if (isRecording && writer != nullptr && inputChannelData[0] != nullptr) {
            AudioBuffer<float> buffer(const_cast<float**>(inputChannelData), numInputChannels, numSamples);
            writer->writeFromAudioSampleBuffer(buffer, 0, numSamples);
        }
        if (inputChannelData[0] != nullptr) {
            for (int i = 0; i < numSamples; i += 128) {
                socket.write("255.255.255.255", 16384, inputChannelData[0] + i, 128 * sizeof(float));
            }
        }
        for (int i = 0; i < numOutputChannels; ++i) if (outputChannelData[i]) FloatVectorOperations::clear(outputChannelData[i], numSamples);
    }
    void audioDeviceAboutToStart(AudioIODevice* device) override { currentSampleRate = device->getCurrentSampleRate(); }
    void audioDeviceStopped() override {}
private:
    DatagramSocket socket; AudioFormatManager formatManager; std::unique_ptr<AudioFormatWriter> writer;
    std::unique_ptr<FileOutputStream> fileStream; double currentSampleRate = 0; bool isRecording = false;
};

class MainComponent : public Component {
public:
    MainComponent() {
        setSize(500, 350); addAndMakeVisible(recButton);
        recButton.setButtonText("GRAVAR NO PC");
        recButton.onClick = [this] {
            if (!recording) { streamer.startRecording(File::getSpecialLocation(File::userDesktopDirectory).getChildFile("db_studio_rec.wav")); recButton.setButtonText("GRAVANDO..."); recording = true; }
            else { streamer.stopRecording(); recButton.setButtonText("GRAVAR NO PC"); recording = false; }
        };
        deviceManager.initialiseWithDefaultDevices(2, 2); deviceManager.addAudioCallback(&streamer);
    }
    ~MainComponent() { deviceManager.removeAudioCallback(&streamer); }
    void paint(Graphics& g) override { g.fillAll(Colour(0xFF0A0A0A)); g.setColour(Colours::yellow); g.setFont(24.0f); g.drawText("dB STUDIO HOST", getLocalBounds().removeFromTop(60), Justification::centred); }
    void resized() override { recButton.setBounds(100, 150, 300, 60); }
private:
    AudioDeviceManager deviceManager; AudioStreamer streamer; TextButton recButton; bool recording = false;
};

class MainWindow : public DocumentWindow {
public:
    MainWindow(String name) : DocumentWindow(name, Colours::black, allButtons) { setUsingNativeTitleBar(true); setContentOwned(new MainComponent(), true); setResizable(false, false); centreWithSize(getWidth(), getHeight()); setVisible(true); }
    void closeButtonPressed() override { JUCEApplication::getInstance()->systemRequestedQuit(); }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

class DBStudioApplication : public JUCEApplication {
public:
    const String getApplicationName() override { return "dB Studio"; }
    const String getApplicationVersion() override { return "1.0.0"; }
    void initialise(const String&) override { mainWindow.reset(new MainWindow(getApplicationName())); }
    void shutdown() override { mainWindow = nullptr; }
private:
    std::unique_ptr<MainWindow> mainWindow;
};
START_JUCE_APPLICATION(DBStudioApplication)
