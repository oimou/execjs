#include <uv.h>
#include "timer.h"

static void timer_cb(uv_timer_t* handle) {
  Timer* timer = static_cast<Timer*>(handle->data);
  timer->InvokeCallback();

  if (handle->repeat == 0) {
    delete timer;
  }
}

Timer::Timer(
  Isolate* _isolate,
  Handle<Function> _callback,
  int64_t _delay
) {
  isolate = _isolate;
  callback.Reset(isolate, _callback);
  delay = _delay;

  uv_timer_init(uv_default_loop(), &watcher);
  watcher.data = this;
  uv_timer_start(&watcher, timer_cb, delay, 0);
}

Timer::~Timer() {
  uv_timer_stop(&watcher);
}

void Timer::InvokeCallback() {
  HandleScope scope(isolate);

  Local<Function> f = Local<Function>::New(
    isolate,
    callback
  );

  const int size = 1;
  Handle<Value> argv[size];
  argv[0] = Number::New(isolate, delay);

  f->Call(
    isolate->GetCurrentContext()->Global(),
    size,
    argv
  );
}
