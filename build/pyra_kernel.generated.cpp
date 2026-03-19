#define PYRA_BARE_MODE 1
#define PYRA_FREESTANDING_MODE 1


#include <stdint.h>

struct MultibootInfo {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t framebuffer_bpp;
    uint8_t framebuffer_type;
    uint16_t framebuffer_reserved;
} __attribute__((packed));

static volatile uint32_t* g_framebuffer = nullptr;
static uint32_t g_pitch = 0;
static uint32_t g_width = 0;
static uint32_t g_height = 0;

static bool freestanding_init(uint32_t multiboot_magic, uint32_t multiboot_info_addr) {
    if (multiboot_magic != 0x2BADB002) {
        return false;
    }

    MultibootInfo* info = (MultibootInfo*)multiboot_info_addr;
    bool has_framebuffer = (info->flags & (1u << 12)) != 0;
    if (!has_framebuffer || info->framebuffer_type != 1 || info->framebuffer_bpp != 32) {
        return false;
    }

    g_framebuffer = (volatile uint32_t*)(uintptr_t)info->framebuffer_addr;
    g_pitch = info->framebuffer_pitch;
    g_width = info->framebuffer_width;
    g_height = info->framebuffer_height;

    return g_framebuffer != nullptr && g_width != 0 && g_height != 0;
}

static uint32_t screen_width() {
    return g_width;
}

static uint32_t screen_height() {
    return g_height;
}

static uint32_t rgb(uint32_t red, uint32_t green, uint32_t blue) {
    return ((red & 0xFFu) << 16) | ((green & 0xFFu) << 8) | (blue & 0xFFu);
}

static void put_pixel(uint32_t x, uint32_t y, uint32_t color) {
    if (!g_framebuffer || x >= g_width || y >= g_height) {
        return;
    }
    uint8_t* row = (uint8_t*)g_framebuffer + (y * g_pitch);
    ((volatile uint32_t*)row)[x] = color;
}

static void fill_rect(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
    for (uint32_t row = 0; row < height; ++row) {
        for (uint32_t col = 0; col < width; ++col) {
            put_pixel(x + col, y + row, color);
        }
    }
}

static void clear(uint32_t color) {
    fill_rect(0, 0, g_width, g_height, color);
}

static void halt() {
    for (;;) {
        asm volatile ("hlt");
    }
}

static void panic(uint32_t color) {
    clear(color);
    fill_rect(0, 0, g_width, 10, rgb(255, 255, 255));
    halt();
}


extern "C" int pyra_kernel_entry(uint32_t multiboot_magic, uint32_t multiboot_info_addr) {
    if (!freestanding_init(multiboot_magic, multiboot_info_addr)) {
        panic(rgb(0, 0, 170));
    }

    clear(rgb(43, 87, 154));

    uint32_t width = screen_width();
    uint32_t height = screen_height();

    uint32_t taskbar_height = 56;
    fill_rect(0, height - taskbar_height, width, taskbar_height, rgb(22, 50, 92));
    fill_rect(0, height - taskbar_height - 2, width, 2, rgb(111, 168, 220));

    uint32_t pane_w = 90;
    uint32_t pane_h = 90;
    uint32_t gap = 18;
    uint32_t logo_x = (width / 2) - pane_w - (gap / 2);
    uint32_t logo_y = (height / 2) - pane_h - 50;

    fill_rect(logo_x, logo_y, pane_w, pane_h, rgb(127, 219, 255));
    fill_rect(logo_x + pane_w + gap, logo_y, pane_w, pane_h, rgb(58, 150, 221));
    fill_rect(logo_x, logo_y + pane_h + gap, pane_w, pane_h, rgb(87, 199, 255));
    fill_rect(logo_x + pane_w + gap, logo_y + pane_h + gap, pane_w, pane_h, rgb(30, 111, 184));

    uint32_t card_width = 520;
    uint32_t card_height = 140;
    uint32_t card_x = (width - card_width) / 2;
    uint32_t card_y = logo_y + (pane_h * 2) + gap + 48;
    fill_rect(card_x, card_y, card_width, card_height, rgb(234, 243, 255));
    fill_rect(card_x, card_y, card_width, 8, rgb(169, 208, 245));
    fill_rect(card_x + 16, card_y + 24, card_width - 32, 18, rgb(187, 215, 242));
    fill_rect(card_x + 16, card_y + 56, card_width - 120, 14, rgb(201, 221, 243));
    fill_rect(card_x + 16, card_y + 82, card_width - 180, 14, rgb(201, 221, 243));
    fill_rect(card_x + 16, card_y + 108, 160, 12, rgb(159, 191, 226));

    fill_rect(32, height - 44, 32, 32, rgb(100, 181, 246));
    fill_rect(80, height - 40, 140, 24, rgb(44, 79, 125));
    fill_rect(width - 140, height - 38, 108, 18, rgb(39, 71, 111));

    halt();
    return 0;
}
