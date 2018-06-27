// Audio.cc
#include "Audio.h"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <AL/alut.h>
#include <iostream>
#include <string>
using std::cout;
using std::string;

ALCdevice* device = nullptr;
ALCcontext* context = nullptr;

void Audio::init() {

  // initialize library
  device = alcOpenDevice(NULL);
  if (!device) {
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

  context = alcCreateContext(device, NULL);
  if (!alcMakeContextCurrent(context)) {
    cout << "======================================\n";
    cout << "ERROR::Audio: Failed to create context\n";
    cout << "======================================\n";
    return;
  }

  ALfloat listenerOri[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f };

  alListener3f(AL_POSITION, 0, 0, 1.0f);
// check for errors
  alListener3f(AL_VELOCITY, 0, 0, 0);
// check for errors
  alListenerfv(AL_ORIENTATION, listenerOri);
// check for errors

  ALuint source;


alGenSources((ALuint)1, &source);
// check for errors

alSourcef(source, AL_PITCH, 1);
// check for errors
alSourcef(source, AL_GAIN, 1);
// check for errors
alSource3f(source, AL_POSITION, 0, 0, 0);
// check for errors
alSource3f(source, AL_VELOCITY, 0, 0, 0);
// check for errors
alSourcei(source, AL_LOOPING, AL_FALSE);
// check for errros

ALuint buffer;

alGenBuffers((ALuint)1, &buffer);
// check for errors

ALsizei size, freq;
ALenum format;
ALvoid *data;

ALCenum error;

char file[] = "../resources/sounds/tink.wav";
alutLoadWAVFile(file, &format, &data, &size, &freq);

alBufferData(buffer, format, data, size, freq);
error = alGetError();
if (error != AL_NO_ERROR) {
  cout << "ERROR1\n";
}

alSourcei(source, AL_BUFFER, buffer);
error = alGetError();
if (error != AL_NO_ERROR) {
  cout << "ERROR2\n";
}

alSourcePlay(source);
error = alGetError();
if (error != AL_NO_ERROR) {
  cout << "ERROR3\n";
}
ALint source_state;
alGetSourcei(source, AL_SOURCE_STATE, &source_state);
error = alGetError();
if (error != AL_NO_ERROR) {
  cout << "ERROR4\n";
}
// check for errors
while (source_state == AL_PLAYING) {
        alGetSourcei(source, AL_SOURCE_STATE, &source_state);
      }

// cleanup context
alDeleteSources(1, &source);
alDeleteBuffers(1, &buffer);
device = alcGetContextsDevice(context);
alcMakeContextCurrent(NULL);
alcDestroyContext(context);
alcCloseDevice(device);
}