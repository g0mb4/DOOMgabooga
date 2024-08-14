#include "doomgeneric/doomgeneric.h"
#include "doomgeneric/doomkeys.h"
#include "doomgeneric/m_argv.h"

Gfx_Image *image = NULL;
f64 start_time = 0;

typedef struct {
  bool pressed;
  u8 key;
} Key_Event;

// ring buffer for keys
#define KEY_RBUFFER_SIZE 16
static Key_Event key_rbuffer[KEY_RBUFFER_SIZE];
static u32 key_rbuffer_write_index = 0;
static u32 key_rbuffer_read_index = 0;

static void add_key(Key_Event e){
  key_rbuffer[key_rbuffer_write_index] = e;
  key_rbuffer_write_index++;
  key_rbuffer_write_index %= KEY_RBUFFER_SIZE;
}

static int get_key(int* pressed, u8* doomKey){
  // buffer is empty
  if (key_rbuffer_read_index == key_rbuffer_write_index){
    return 0;
  }

  Key_Event e = key_rbuffer[key_rbuffer_read_index];
  key_rbuffer_read_index++;
  key_rbuffer_read_index %= KEY_RBUFFER_SIZE;

  *pressed = e.pressed;
  *doomKey = e.key;

  return 1;
}

static void poll_keys(void){

#define KP(ok, dk)                                      \
  if(is_key_just_pressed((ok))){                        \
    consume_key_just_pressed((ok));                     \
    add_key((Key_Event) { .pressed = 1, .key = (dk)});  \
  }

#define KR(ok, dk)                                      \
  if(is_key_just_released((ok))) {                      \
    consume_key_just_released((ok));                    \
    add_key((Key_Event) { .pressed = 0, .key = (dk)});  \
  }

#define K(ok, dk) \
  KP((ok), (dk));     \
  KR((ok), (dk))

  K(KEY_ENTER, KEY_ENTER);
  K(KEY_ESCAPE, KEY_ESCAPE);
  K(KEY_ESCAPE, KEY_ESCAPE);
  K(KEY_ARROW_LEFT, KEY_LEFTARROW);
  K(KEY_ARROW_RIGHT, KEY_RIGHTARROW);
  K(KEY_ARROW_UP, KEY_UPARROW);
  K(KEY_ARROW_DOWN, KEY_DOWNARROW);
  K(KEY_CTRL, KEY_FIRE);
  K(KEY_SPACEBAR, KEY_USE);
  K(KEY_SHIFT, KEY_RSHIFT);

  for(char c = 'A'; c <= 'Z'; ++c){
    char lower = c - 'A' + 'a';
    K(c, lower);
  }

  K('_', '_');
  K('.', '.');
}

Gfx_Image * create_image(int width, int height){
  Gfx_Image *image = alloc(get_heap_allocator(), sizeof(Gfx_Image));

  image->width = width;
  image->height = height;
  image->gfx_handle = GFX_INVALID_HANDLE;  // This is handled in gfx
  image->allocator = get_heap_allocator();
  image->channels = 4;

  void *data = alloc(get_heap_allocator(), width * height * sizeof(u32));
  // assert(data, "Unable to allocate image data\n"); // TODO: does not find oogaboogas's assert, find out why

  gfx_init_image(image, data);

  return image;
}

void DG_Init() {
  window.scaled_width = DOOMGENERIC_RESX;
  window.scaled_height = DOOMGENERIC_RESY;
  window.x = 100;
  window.y = 100;
  window.clear_color = hex_to_rgba(0x181818FF);
  window.title = STR("DOOMgabooga");

  image = create_image(DOOMGENERIC_RESX, DOOMGENERIC_RESY);
  start_time = os_get_current_time_in_seconds();
}

void DG_DrawFrame() {
  draw_frame.projection = m4_make_orthographic_projection(0, window.width, 0, window.height, -1, 10);
  draw_frame.view = m4_scalar(1.0);

  gfx_set_image_data(image, 0, 0, DOOMGENERIC_RESX, DOOMGENERIC_RESY, DG_ScreenBuffer);
	draw_image(image, v2(0, 0), v2(DOOMGENERIC_RESX, DOOMGENERIC_RESY), COLOR_WHITE);

  gfx_update();
}

void DG_SleepMs(u32 ms) { 
  os_update();
  poll_keys();

  os_sleep(ms);
}

u32 DG_GetTicksMs() {
  const f64 elapsed = os_get_current_time_in_seconds() - start_time;
  return (u32)(elapsed * 1000.0);
}

int DG_GetKey(int *pressed, unsigned char *doomKey) {
  return get_key(pressed, doomKey);
}

void DG_SetWindowTitle(const char *title) { 
  window.title = STR(title); 
}

int entry(int argc, char **argv) {
  doomgeneric_Create(argc, argv);

  while (!window.should_close) {
    reset_temporary_storage();

    doomgeneric_Tick();
  }

  return 0;
}