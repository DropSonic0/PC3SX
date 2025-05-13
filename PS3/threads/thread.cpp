#include "thread.hpp"

namespace Threads {
   namespace Internal {

      void* _Entry(void *data)
      {
         Callable *call = reinterpret_cast<Internal::Callable*>(data);
         call->run();
         delete call;
         pthread_exit(NULL);
         return NULL;
      }

   }
}

