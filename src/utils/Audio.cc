// Audio.cc
#include "Audio.h"
#ifdef __linux__
#include <AL/al.h>
#include <AL/alc.h>
#else
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#endif
#include <AL/alut.h>
#include <string.h>
#include <iostream>
#include <map>
using std::cout;
using std::map;
using std::string;

namespace {
  ALCdevice* device = nullptr;
  ALCcontext* context = nullptr;
  map<string, ALuint> sourceManager;
  map<string, ALuint> bufferManager;
};

void Audio::init() {
  // initialize library
  ::device = alcOpenDevice(NULL);
  if (!::device) {
    cout << "======================================\n";
    cout << "ERROR::Audio: Failed to initialize OpenAL\n";
    cout << "======================================\n";
    return;
  }

  // check enumeration
  ALboolean enumeration;
  enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATION_EXT");
  if (enumeration == AL_FALSE) {
    cout << "======================================\n";
    cout << "ERROR::Audio: Enumeration not supported\n";
    cout << "======================================\n";
    return;
  }

  // create context
  context = alcCreateContext(device, NULL);
  if (!alcMakeContextCurrent(context)) {
    cout << "======================================\n";
    cout << "ERROR::Audio: Failed to create context\n";
    cout << "======================================\n";
    return;
  }

  // set up position, velocity and orientation
  ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

  alListener3f(AL_POSITION, 0, 0, 1.0f);
  alListener3f(AL_VELOCITY, 0, 0, 0);
  alListenerfv(AL_ORIENTATION, listenerOri);

  Audio::playAudio("ouch");
  Audio::playAudio("tink");
}

void Audio::playAudio(std::string audioName) {
  ALuint source;
  if (sourceManager.find(audioName) == sourceManager.end()) {
    ALuint buffer;
    // load buffer
    ALuint source;
    alGenSources((ALuint)1, &source);
    alGenBuffers((ALuint)1, &buffer);
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);
    ALsizei size, freq;
    ALenum format;
    ALvoid *data;

    char file[256];
    strcpy(file, "../resources/sounds/");
    strcat(file, audioName.c_str());
    strcat(file, ".wav");
#ifdef __linux__
    // not sure what the last parameter does
    alutLoadWAVFile((ALbyte*)file, &format, &data, &size, &freq, false);
#else
    alutLoadWAVFile((ALbyte*)file, &format, &data, &size, &freq);
#endif
    alBufferData(buffer, format, data, size, freq);
    alSourcei(source, AL_BUFFER, buffer);

    sourceManager[audioName] = source;
    bufferManager[audioName] = buffer;
  } else {
    source = sourceManager[audioName];
  }

  alSourcePlay(source);
}

void Audio::cleanUp() {
  for (auto& source: sourceManager) {
    alDeleteSources(1, &source.second);
  }
  for (auto& buffer: bufferManager) {
    alDeleteBuffers(1, &buffer.second);
  }
  device = alcGetContextsDevice(context);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(context);
  alcCloseDevice(device);
}
