#include <v8.h>
#include <uv.h>

using namespace v8;

class Timer {
  public:
    Timer(
      Isolate* isolate,
      Handle<Function> callback,
      int64_t delay
    );
    ~Timer();
    void InvokeCallback();

  private:
    uv_timer_t watcher;
    Isolate* isolate;
    Persistent<Function> callback;
    int64_t delay;
};
