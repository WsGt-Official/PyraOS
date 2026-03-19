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

static int __pyra_kernel_main() {
    clear(rgb(43.0, 87.0, 154.0));
    auto width = screen_width();
    auto height = screen_height();
    auto taskbar_height = 56.0;
    fill_rect(0.0, (height - taskbar_height), width, taskbar_height, rgb(22.0, 50.0, 92.0));
    fill_rect(0.0, ((height - taskbar_height) - 2.0), width, 2.0, rgb(111.0, 168.0, 220.0));
    auto pane_w = 90.0;
    auto pane_h = 90.0;
    auto gap = 18.0;
    auto logo_x = (((width / 2.0) - pane_w) - (gap / 2.0));
    auto logo_y = (((height / 2.0) - pane_h) - 50.0);
    fill_rect(logo_x, logo_y, pane_w, pane_h, rgb(127.0, 219.0, 255.0));
    fill_rect(((logo_x + pane_w) + gap), logo_y, pane_w, pane_h, rgb(58.0, 150.0, 221.0));
    fill_rect(logo_x, ((logo_y + pane_h) + gap), pane_w, pane_h, rgb(87.0, 199.0, 255.0));
    fill_rect(((logo_x + pane_w) + gap), ((logo_y + pane_h) + gap), pane_w, pane_h, rgb(30.0, 111.0, 184.0));
    auto card_width = 520.0;
    auto card_height = 140.0;
    auto card_x = ((width - card_width) / 2.0);
    auto card_y = (((logo_y + (pane_h * 2.0)) + gap) + 48.0);
    fill_rect(card_x, card_y, card_width, card_height, rgb(234.0, 243.0, 255.0));
    fill_rect(card_x, card_y, card_width, 8.0, rgb(169.0, 208.0, 245.0));
    fill_rect((card_x + 16.0), (card_y + 24.0), (card_width - 32.0), 18.0, rgb(187.0, 215.0, 242.0));
    fill_rect((card_x + 16.0), (card_y + 56.0), (card_width - 120.0), 14.0, rgb(201.0, 221.0, 243.0));
    fill_rect((card_x + 16.0), (card_y + 82.0), (card_width - 180.0), 14.0, rgb(201.0, 221.0, 243.0));
    fill_rect((card_x + 16.0), (card_y + 108.0), 160.0, 12.0, rgb(159.0, 191.0, 226.0));
    fill_rect(32.0, (height - 44.0), 32.0, 32.0, rgb(100.0, 181.0, 246.0));
    fill_rect(80.0, (height - 40.0), 140.0, 24.0, rgb(44.0, 79.0, 125.0));
    fill_rect((width - 140.0), (height - 38.0), 108.0, 18.0, rgb(39.0, 71.0, 111.0));
    halt();
    return 0;
}

extern "C" int pyra_kernel_entry(uint32_t multiboot_magic, uint32_t multiboot_info_addr) {
    if (!freestanding_init(multiboot_magic, multiboot_info_addr)) {
        panic(rgb(0, 0, 170));
        return 1;
    }
    return __pyra_kernel_main();
}
