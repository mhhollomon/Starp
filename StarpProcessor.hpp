#pragma once

#include "Algorithm.hpp"

#include <juce_audio_processors/juce_audio_processors.h>

#include <fstream>

struct speed_value {
    juce::String name;
    double multiplier;
};

struct played_note {
    int note_value;
    double end_slot;

};

bool operator==(const played_note& lhs, const played_note& rhs);
bool operator<(const played_note& lhs, const played_note& rhs);


//==============================================================================
class StarpProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    StarpProcessor();
    ~StarpProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout&) const override { return true; };

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override {return JucePlugin_Name;};

    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return true; };
    bool isMidiEffect() const override { return true; };
    double getTailLengthSeconds() const override { return 0.0; };

    //==============================================================================
    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };
    void setCurrentProgram (int) override { };
    const juce::String getProgramName (int) override { return {}; };
    void changeProgramName (int, const juce::String&) override {};

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:
    //==============================================================================
    juce::AudioParameterChoice* speed;
    juce::AudioParameterChoice* algorithm_parm;
    juce::AudioParameterFloat*  gate;
    juce::AudioParameterInt*    velocity;
    juce::AudioParameterInt*    velo_range;
    juce::AudioParameterInt*    probability;
    juce::AudioParameterFloat*  timing_delay;
    juce::AudioParameterFloat*  timing_advance;

    juce::int64 randomKey = 0L;

    int current_algo_index = -1;
    double rate;

    juce::SortedSet<int> notes;

    juce::Array<played_note> *active_notes = nullptr;

    juce::FileLogger *dbgout = nullptr;

    AlgorithmBase *algo = nullptr;

    double getSpeedFactor();
    double getGate();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StarpProcessor)
};
