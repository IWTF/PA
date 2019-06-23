#include "common.h"

extern _RegSet* do_syscall(_RegSet *r);
extern _RegSet* schedule(_RegSet *prev);

static _RegSet* do_event(_Event e, _RegSet* r) {
  // Log("e.event is:%d\n", e.event);
  // Log("_EVENT_SYSCALL is:%d\n", _EVENT_SYSCALL);
  // Log("_EVENT_TRAP is:%d\n", _EVENT_IRQ_TIME);
  switch (e.event) {
  	case _EVENT_SYSCALL: return schedule(do_syscall(r)); break;
  	case _EVENT_TRAP: return schedule(r); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
