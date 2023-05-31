#include <juce_audio_processors/juce_audio_processors.h>

#include "TinySHA1.hpp"

class AlgorithmBase {

protected:
    int last_index = -1;

public:

    AlgorithmBase() = default;
    AlgorithmBase(AlgorithmBase *o){
        last_index = o->last_index;
    }
    virtual int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed)  = 0;

    virtual ~AlgorithmBase() {}

};

class UpAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    int getNextNote(double, const juce::SortedSet<int> &notes, bool notes_changed) override {
        if (notes.size() > 0) {
            last_index = (++last_index) % notes.size();
            return notes[last_index];
        } else {
            last_index = -1;
            return -1;
        }
    }

};

class DownAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    int getNextNote(double, const juce::SortedSet<int> &notes, bool notes_changed) override {
        if (notes.size() > 0) {

            if (last_index <= 0) {
                last_index = notes.size();
            }
            last_index = --last_index;
            return notes[last_index];
        } else {
            last_index = -1;
            return -1;
        }
    }

};

class RandomAlgorithm : public AlgorithmBase {
    using AlgorithmBase::AlgorithmBase;

    juce::int64 key_;

    juce::SortedSet<int> available_notes;

    int last_note = -1;

    juce::FileLogger *dbgout_ = nullptr;


public:
    void setKey(juce::int64 key) {
        key_ = key;
    }

    void setDebug(juce::FileLogger *logger) {
        dbgout_ = logger;
    }

    int getNextNote(double timeline_slot, const juce::SortedSet<int> &notes, bool notes_changed) override {

        if (available_notes.isEmpty()) {
            available_notes.addSet(notes);
        } else if (notes_changed) {
            available_notes.clear();
            // Add all the stuff from the new set.
            available_notes.addSet(notes);
        }

        int num_notes = available_notes.size();
        if (dbgout_) {
            std::stringstream x;
            x << "available note count = " << num_notes;
            dbgout_->logMessage(x.str());
        }

        if (num_notes > 0) {
            if (num_notes == 1) {
                last_note = available_notes[0];
                available_notes.clear();
                return last_note;
            } else {
                last_note = getRandom(timeline_slot, last_note, available_notes);
                available_notes.removeValue(last_note);
                return last_note;
            }
        } else {
            return -1;
        }
    }

    virtual ~RandomAlgorithm() = default;

private :
    int getRandom(double slot, int note_to_avoid, const juce::SortedSet<int> & notes) {
        std::stringstream buf;
        buf << std::hex << key_ << slot;

        sha1::SHA1 hash;
        hash.processBytes(buf.str().c_str(), buf.str().size());

        uint8_t digest[20];
        hash.getDigestBytes(digest);

        for (int j=0; j < 20; ++j) {
            // need to do better, but this is an okay proof of concept
           int maybe =  notes[digest[j] % notes.size()];
           if (maybe != note_to_avoid)
            return maybe;
        }

        return -1;
    }

};
