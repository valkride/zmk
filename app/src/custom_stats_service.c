#include <zephyr/bluetooth/gatt.h>
#include <zephyr/kernel.h>
#include <string.h>

// Use your generated UUIDs
#define STATS_SERVICE_UUID BT_UUID_DECLARE_128(0xe72063c8,0x961b,0x4111,0xb757,0x5147b859b6ea)
#define STATS_CHAR_UUID    BT_UUID_DECLARE_128(0x9a89ab4a,0x340d,0x4140,0xb6f0,0xdb8a50c0604d)

// Buffer to hold the latest stats string
char stats_buffer[64] = {0};

static ssize_t stats_write(struct bt_conn *conn, const struct bt_gatt_attr *attr,
                           const void *buf, uint16_t len, uint16_t offset, uint8_t flags)
{
    size_t copy_len = len < sizeof(stats_buffer) - 1 ? len : sizeof(stats_buffer) - 1;
    memcpy(stats_buffer, buf, copy_len);
    stats_buffer[copy_len] = 0; // Null-terminate for safety
    // Optionally: trigger OLED redraw here
    return len;
}

// Register the service and characteristic
BT_GATT_SERVICE_DEFINE(stats_svc,
    BT_GATT_PRIMARY_SERVICE(STATS_SERVICE_UUID),
    BT_GATT_CHARACTERISTIC(STATS_CHAR_UUID,
        BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_WRITE,
        NULL, stats_write, NULL),
);