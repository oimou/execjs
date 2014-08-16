#include <iostream>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include <v8.h>
#include <uv.h>

#include "timer.h"

using namespace v8;

/**
 *  Log
 */
void Log(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 1) {
    return;
  }

  HandleScope scope(args.GetIsolate());

  String::Utf8Value text(args[0]->ToString());
  std::cout << *text << std::endl;
}

/**
 *  SetTimer
 */
void SetTimer(const FunctionCallbackInfo<Value>& args) {
  if (args.Length() < 2) {
    return;
  }

  HandleScope scope(args.GetIsolate());

  int64_t delay = args[0]->IntegerValue();
  Local<Function> callback = args[1].As<Function>();
  Timer* timer = new Timer(
    args.GetIsolate(),
    callback,
    delay
  );
}

/**
 *  ReadFile
 */
static Handle<String>
ReadFile (Isolate* isolate, const std::string& name) 
{
  FILE* file = fopen(name.c_str(), "rb");
  if (file == NULL) return Handle<String>();

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);

  char* chars = new char[size + 1];
  chars[size] = '\0';
  for (int i = 0; i < size; i++) {
    chars[i] = fgetc(file);
  }
  fclose(file);

  Handle<String> result = String::NewFromUtf8(isolate, chars);
  delete[] chars;
  return result;
}

/**
 *  main
 */
int main(int argc, char* argv[]) {
  if (argc != 2) {
    fprintf(stderr, "No script was specified.\n");
    return 1;
  }

  std::string filename(argv[1]);

  // Create a new Isolate and make it the current one.
  Isolate* isolate = Isolate::New();
  Isolate::Scope isolate_scope(isolate);

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // Set log function to global
  Handle<ObjectTemplate> global = ObjectTemplate::New(isolate);

  global->Set(
    String::NewFromUtf8(isolate, "setTimer"),
    FunctionTemplate::New(isolate, SetTimer)
  );

  global->Set(
    String::NewFromUtf8(isolate, "log"),
    FunctionTemplate::New(isolate, Log)
  );

  // Create a new context
  Handle<Context> context = Context::New(isolate, NULL, global);

  // Enter the context for compiling and running the hello world script.
  Context::Scope context_scope(context);

  // Create a string containing the JavaScript source code.
  Local<String> source = ReadFile(isolate, filename);

  // Compile the source code.
  Local<Script> script = Script::Compile(source);

  // Run the script to get the result.
  Local<Value> result = script->Run();

  uv_run(uv_default_loop(), UV_RUN_DEFAULT);

  return 0;
}
