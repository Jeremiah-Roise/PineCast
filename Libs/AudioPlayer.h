#pragma once
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include <stdio.h>
/// The AudioPlayer class nicely encapsulates the primary functionallity needed in the application
class AudioPlayer {
    private:
        ma_result result;
        ma_engine engine;
        ma_sound sound;
        uint32_t sampleRate;
        bool isPlaying = false;

    public:
        // initializes the audio engine
        AudioPlayer(){
            result = ma_engine_init(NULL, &engine);
            if (result != MA_SUCCESS) {
                printf("Failed to initialize audio engine.");
            }
        }

        /// loads the audio file to be played
        void load_file(const char* file){
            ma_sound_init_from_file(&engine,file,MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION, NULL, NULL, &sound);
            sampleRate = ma_engine_get_sample_rate(&engine);
            isPlaying = false;
        }
        
        /// returns true for playin and false for not playing
        /// imperically sets play state
        bool setPlaying(bool should_play){
            if (should_play == true)
            {
                ma_sound_start(&sound);
                isPlaying = true;
            }
            if (should_play == false)
            {
                ma_sound_stop(&sound);
                isPlaying = false;
            }
            return isPlaying;
        }

        /// returns true for playin and false for not playing
        /// alternates playing and not playing
        bool pausePlay(){
            if (isPlaying == true)
            {
                ma_sound_stop(&sound);
            }
            if (isPlaying == false)
            {
                ma_sound_start(&sound);
            }
            isPlaying = !isPlaying;
            return isPlaying;
        }

        /// takes a positive number to jump forward and a negetive number to reverse
        void seek(int seconds){
            // if the number of seconds is zero ignore it.
            if (seconds > 0)
            {
                uint64_t pos = ma_sound_get_time_in_pcm_frames(&sound);
                ma_sound_seek_to_pcm_frame(&sound,pos + (sampleRate * seconds));
            }
            if(seconds < 0)
            {
                seconds = abs(seconds);
                uint64_t pos = ma_sound_get_time_in_pcm_frames(&sound);
                ma_sound_seek_to_pcm_frame(&sound,pos - (sampleRate * seconds));
            }
            return;
        }
};