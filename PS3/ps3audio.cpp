#include "ps3audio.h"
#include <cell/audio.h>
#include <sys/event.h>
#include <pthread.h>
#include "buffer.h"
#include "resampler.h"

#define AUDIO_BLOCKS 16 // 8 or 16. Guess what we choose? :)
#define AUDIO_CHANNELS 2 // All hail glorious stereo!
#define AUDIO_OUT_RATE (48000.0)

#include <limits>
template<typename T>
inline void array_to_float(float * __restrict__ out, T * __restrict__ in, size_t samples)
{
   for (size_t i = 0; i < samples; i++)
      out[i] = (float)in[i]/(std::numeric_limits<T>::max() + 1);
}

typedef struct
{
   float tmp_data[CELL_AUDIO_BLOCK_SAMPLES * AUDIO_CHANNELS];
   uint32_t audio_port;
   bool nonblocking;
   volatile bool quit_thread;
   fifo_buffer_t *buffer;
   uint64_t input_rate;

   pthread_t thread;
   pthread_mutex_t lock;
   pthread_mutex_t cond_lock;
   pthread_cond_t cond;
} ps3_audio_t;

static ps3_audio_t *aud;

static size_t drain_fifo(void *cb_data, float **data)
{
   ps3_audio_t *aud = (ps3_audio_t*)cb_data;

   audio_input_t tmp[CELL_AUDIO_BLOCK_SAMPLES * AUDIO_CHANNELS];

   if (fifo_read_avail(aud->buffer) >= sizeof(tmp))
   {
      pthread_mutex_lock(&aud->lock);
      fifo_read(aud->buffer, tmp, sizeof(tmp));
      pthread_mutex_unlock(&aud->lock);
      array_to_float(aud->tmp_data, tmp, CELL_AUDIO_BLOCK_SAMPLES * AUDIO_CHANNELS);
   }
   else
   {
      memset(aud->tmp_data, 0, sizeof(aud->tmp_data));
   }
   *data = aud->tmp_data;
   return CELL_AUDIO_BLOCK_SAMPLES;
}

static void *event_loop(void *data)
{
   ps3_audio_t *aud = (ps3_audio_t*)data;
   sys_event_queue_t id;
   sys_ipc_key_t key;
   sys_event_t event;

   cellAudioCreateNotifyEventQueue(&id, &key);
   cellAudioSetNotifyEventQueue(key);

   resampler_t *resampler = resampler_new(drain_fifo, AUDIO_OUT_RATE/aud->input_rate, 2, data);

   float out_tmp[CELL_AUDIO_BLOCK_SAMPLES * AUDIO_CHANNELS] __attribute__((aligned(16)));

   while (!aud->quit_thread)
   {
      sys_event_queue_receive(id, &event, SYS_NO_TIMEOUT);
      resampler_cb_read(resampler, CELL_AUDIO_BLOCK_SAMPLES, out_tmp);
      cellAudioAddData(aud->audio_port, out_tmp, CELL_AUDIO_BLOCK_SAMPLES, 1.0);
      pthread_cond_signal(&aud->cond);
   }

   cellAudioRemoveNotifyEventQueue(key);
   resampler_free(resampler);
   pthread_exit(NULL);
   return NULL;
}

bool cellAudioPortInit(uint64_t samplerate, uint64_t buffersize)
{
   aud = (ps3_audio_t*)calloc(1, sizeof(*aud));
   if (aud == NULL)
      return false;

   CellAudioPortParam params;

   cellAudioInit();

   params.nChannel = AUDIO_CHANNELS;
   params.nBlock = AUDIO_BLOCKS;
   params.attr = 0;

   if (cellAudioPortOpen(&params, &aud->audio_port) != CELL_OK)
   {
      cellAudioQuit();
      free(aud);
      return false;
   }

   // Create a small fifo buffer. :)
   aud->buffer = fifo_new(buffersize * sizeof(audio_input_t));
   aud->input_rate = samplerate;

   pthread_mutex_init(&aud->lock, NULL);
   pthread_mutex_init(&aud->cond_lock, NULL);
   pthread_cond_init(&aud->cond, NULL);

   cellAudioPortStart(aud->audio_port);
   pthread_create(&aud->thread, NULL, event_loop, aud);
   return true;
}


// Should make some noise at least. :)
void cellAudioPortWrite(const audio_input_t* buf, uint64_t samples)
{
   // We will continuously write slightly more data than we should per second, and rely on blocking mechanisms to ensure we don't write too much. 
   while (fifo_write_avail(aud->buffer) < samples * sizeof(audio_input_t))
   {
      pthread_mutex_lock(&aud->cond_lock);
      pthread_cond_wait(&aud->cond, &aud->lock);
      pthread_mutex_unlock(&aud->cond_lock);
   }

   pthread_mutex_lock(&aud->lock);
   fifo_write(aud->buffer, buf, samples * sizeof(audio_input_t));
   pthread_mutex_unlock(&aud->lock);
}

uint64_t cellAudioPortWriteAvail()
{
   return fifo_write_avail(aud->buffer)/sizeof(audio_input_t);
}

void cellAudioPortExit()
{
   aud->quit_thread = true;
   pthread_join(aud->thread, NULL);

   cellAudioPortStop(aud->audio_port);
   cellAudioPortClose(aud->audio_port);
   cellAudioQuit();
   fifo_free(aud->buffer);

   pthread_mutex_destroy(&aud->lock);
   pthread_mutex_destroy(&aud->cond_lock);
   pthread_cond_destroy(&aud->cond);
   free(aud);
}
   


