#include <am.h>
#include <x86.h>

#define RTC_PORT 0x48   // Note that this is not standard

#define I8042_DATA_PORT 0x60
#define I8042_STATUS_PORT 0X64
static unsigned long boot_time;

void _ioe_init() {
  boot_time = inl(RTC_PORT);
}

unsigned long _uptime() {
  unsigned long now_time;
	now_time = inl(RTC_PORT);
	return now_time -  boot_time;
}

uint32_t* const fb = (uint32_t *)0x40000;

_Screen _screen = {
  .width  = 400,
  .height = 300,
};

extern void* memcpy(void *, const void *, int);

void _draw_rect(const uint32_t *pixels, int x, int y, int w, int h) {
  int len = sizeof(uint32_t) * ( (x + w >= _screen.width) ? _screen.width - x : w );
  uint32_t *p_fb = &fb[y * _screen.width + x];
  for (int j = 0; j < h; j ++) {
    if (y + j < _screen.height) {
      memcpy(p_fb, pixels, len);
    }
    else {
      break;
    }
    p_fb += _screen.width;
    pixels += w;
  }
}

void _draw_sync() {
}

int _read_key() {
  int ret = _KEY_NONE;
	if(inb(I8042_STATUS_PORT)){//端口开启
		ret = inl(I8042_DATA_PORT);//获取数据
	}
	return ret;
}
