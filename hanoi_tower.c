#define STB_IMAGE_IMPLEMENTATION

// no file io
#define STBI_NO_STDIO

// these rely on math.h, we don't have it
#define STBI_NO_LINEAR
#define STBI_NO_HDR

// we don't have proper assert.h, redefine our own assert
#include <stdio.h> // for printf (to remove)
#define STBI_ASSERT(x)                    \
    do {                                  \
        if (!(x))                         \
            printf("Assertion failed\n"); \
    } while (0)

#include <stdlib.h>
#include <string.h>
void *
my_realloc(void *ptr, size_t old_size, size_t new_size)
{
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }

    void *new_ptr = malloc(new_size);
    if (!new_ptr) {
        return NULL; // out of memory
    }

    size_t copy_size = (new_size < old_size) ? new_size : old_size;

    memcpy(new_ptr, ptr, copy_size);

    free(ptr);

    return new_ptr;
}
#define STBI_MALLOC(sz) malloc(sz)
#define STBI_FREE(p) free(p)
#define STBI_REALLOC_SIZED(p, oldsz, newsz) my_realloc(p, oldsz, newsz)

// our stdlib does not provide this
int
abs(int x)
{
    if (x < 0) {
        return -x;
    }
    else {
        return x;
    }
}

#include "stb_image.h"
#include "phantom_api.h"

// `image_contents` - phantom string
// returns phantom bitmap
phantom_object_t
decode_image(phantom_object_t image_contents)
{
    int buffer_size;
    unsigned char *buffer = ph_unpack_string(image_contents, &buffer_size);

    // Load bitmaps
    int x, y, ch_cnt;
    unsigned char *image =
        stbi_load_from_memory(buffer, buffer_size, &x, &y, &ch_cnt, 3);
    free(buffer);
    int img_length = x * y * ch_cnt;
    int ppm_length =
        img_length + 64; // 64 extra should be enough for any header
    char *ppm = malloc(ppm_length);

    // write the header
    int offset = snprintf(ppm, ppm_length, "P6\n%d %d\n%d\n", x, y, 255);
    memcpy(ppm + offset, image, img_length);
    free(image);

    // create bitmap object
    phantom_object_t bmp = ph_load_bitmap(ppm, img_length);
    free(ppm);

    return bmp;
}

struct block_data {
    phantom_object_t asset;
    uint32_t x, y, w, h;
};

static void
__hanoi_tower__render(phantom_object_t *screen, const struct block_data *blocks,
                      const uint32_t nblocks)
{
    for (uint32_t i = 0; i < nblocks; i++) {
        phantom_object_t params[] = { phantom_int(blocks[i].x),
                                      phantom_int(blocks[i].y),
                                      blocks[i].asset };

        ph_syscall(*screen, PHANTOM_WINDOW_DRAW_IMAGE, params, 3);
    }

    ph_syscall0(*screen, PHANTOM_WINDOW_UPDATE);
}

void
hanoi_tower(phantom_object_t background, phantom_object_t red_block,
            phantom_object_t green_block, phantom_object_t blue_block)
{
    phantom_object_t screen =
        ph_new_window("Hanoi Tower", 250, 250, -1, -1, 0xFFCCCCb4);

    struct block_data blocks[] = { { decode_image(background), 0, 0, 250, 250 },
                                   { decode_image(red_block), 0, 0, 100, 50 },
                                   { decode_image(green_block), 0, 50, 75, 50 },
                                   { decode_image(blue_block), 0, 100, 50,
                                     50 } };

    phantom_object_t sleep = ph_new_timer();
    phantom_object_t sleep_params[] = { PHANTOM_NULL_OBJECT,
                                        phantom_int(5000 /* ms */) };

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[3].x = 200;
    blocks[3].y = 0;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[2].x = 100;
    blocks[2].y = 0;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[3].x = 100;
    blocks[3].y = 50;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[1].x = 200;
    blocks[1].y = 0;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[3].x = 0;
    blocks[3].y = 0;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[2].x = 200;
    blocks[2].y = 50;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);

    blocks[3].x = 200;
    blocks[3].y = 100;

    __hanoi_tower__render(&screen, blocks, 4);
    ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);
}

static void
__loopbreaker__render(phantom_object_t *screen, const struct block_data *blocks,
                      const uint32_t nblocks)
{
    for (uint32_t i = 0; i < nblocks; ++i) {
        phantom_object_t params[] = { phantom_int(blocks[i].x),
                                      phantom_int(blocks[i].y),
                                      blocks[i].asset };

        ph_syscall(*screen, PHANTOM_WINDOW_DRAW_IMAGE, params, 3);
        ph_syscall0(*screen, PHANTOM_WINDOW_UPDATE);
    }
}

void
loopbreaker(phantom_object_t image)
{
    phantom_object_t screen =
        ph_new_window("loopbreaker", 250, 250, -1, -1, 0xFFCCCCb4);

    struct block_data *blocks = malloc(sizeof(struct block_data));
    blocks[0].asset = decode_image(image);
    blocks[0].x = 0;
    blocks[0].y = 0;
    blocks[0].w = 250;
    blocks[0].h = 250;

    phantom_object_t sleep = ph_new_timer();
    phantom_object_t sleep_params[] = { PHANTOM_NULL_OBJECT,
                                        phantom_int(1000 /* ms */) };
    while (1) {
        blocks[0].w = 234;
        blocks[0].h = 122;

        __loopbreaker__render(&screen, blocks, 1);
        ph_syscall(sleep, PHANTOM_CONNECTION_BLOCK, sleep_params, 2);
    }
}
