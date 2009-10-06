#include "node.h"
#include "signal_handler.h"
#include <assert.h>

using namespace v8;
using namespace node;

Persistent<FunctionTemplate> SignalHandler::constructor_template;

void
SignalHandler::Initialize (Handle<Object> target)
{
  HandleScope scope;

  Local<FunctionTemplate> t = FunctionTemplate::New(SignalHandler::New);
  constructor_template = Persistent<FunctionTemplate>::New(t);
  constructor_template->Inherit(EventEmitter::constructor_template);
  constructor_template->InstanceTemplate()->SetInternalFieldCount(1);
  constructor_template->SetClassName(String::NewSymbol("SignalHandler"));

  NODE_SET_PROTOTYPE_METHOD(constructor_template, "stop", SignalHandler::Stop);

  target->Set(String::NewSymbol("SignalHandler"), constructor_template->GetFunction());
}

void
SignalHandler::OnSignal (EV_P_ ev_signal *watcher, int revents)
{
  SignalHandler *handler = static_cast<SignalHandler*>(watcher->data);

  assert(revents == EV_SIGNAL);

  handler->Emit("signal", 0, NULL);
}

SignalHandler::~SignalHandler ()
{
  ev_signal_stop (EV_DEFAULT_UC_ &watcher_);
}

Handle<Value>
SignalHandler::New (const Arguments& args)
{
  HandleScope scope;

  if (args.Length() != 1 || !args[0]->IsInt32())
    return ThrowException(String::New("Bad arguments"));

  int sig = args[0]->Int32Value();

  SignalHandler *handler = new SignalHandler();

  ev_signal_init(&handler->watcher_, SignalHandler::OnSignal, sig);
  handler->watcher_.data = handler;
  ev_signal_start(EV_DEFAULT_UC_ &handler->watcher_);

  handler->Attach();

  handler->Wrap(args.Holder());
  return args.This();
}

Handle<Value>
SignalHandler::Stop (const Arguments& args)
{
  SignalHandler *handler = ObjectWrap::Unwrap<SignalHandler>(args.Holder());
  ev_signal_stop(EV_DEFAULT_UC_ &handler->watcher_);
  handler->Detach();
  return Undefined();
}
