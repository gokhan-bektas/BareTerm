#include <bareterm_screen.h>
#include <bareterm.h>
#include <widget/bareterm_widget.h>
#include <stdio.h>
#include <string.h>
#include <bareterm_screen_manager.h>
#include "settings_screen.h"
#include "main_screen.h"

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/services/cgms.h>

// --- All the static widget variables from main.c go here ---
static bareterm_widget_t btn1, btn2;
static bareterm_widget_t memo;
static bareterm_widget_t lbl1;
static bareterm_widget_t lbl2;
static bareterm_widget_t bar;
static bareterm_widget_t cb1, cb2;
static bareterm_widget_t rg;
static const char *choices[] = { "SPI", "I2C", "UART", "I2C"};
static bareterm_widget_t status_led;
static bareterm_widget_t listbox;
static const char *list_items[] = {
    "First Item", "Second Item", "A Third, Longer Item", "Item Four",
    "Fifth", "Sixth Sense", "Seventh Heaven", "Eighth Wonder",
    "Ninth Life", "Tenth Planet"
};
static bareterm_widget_t input;
static char buf[64] = "";

/* === Define a GATT service globally === */
/* Battery Service Declaration */
BT_GATT_SERVICE_DEFINE(bas_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_BAS),
    BT_GATT_CHARACTERISTIC(BT_UUID_BAS_BATTERY_LEVEL, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, NULL, NULL, NULL),
);

/* Device Information Service Declaration */
BT_GATT_SERVICE_DEFINE(dis_svc,
    BT_GATT_PRIMARY_SERVICE(BT_UUID_DIS),
    BT_GATT_CHARACTERISTIC(BT_UUID_DIS_MANUFACTURER_NAME, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, NULL, NULL, "Analog Devices Inc."),
    BT_GATT_CHARACTERISTIC(BT_UUID_DIS_MODEL_NUMBER, BT_GATT_CHRC_READ,
                           BT_GATT_PERM_READ, NULL, NULL, "MAX32657-EVKIT-Test"),
);

/* Bluetooth declarations */
#define FIXED_PASSKEY 555555
static bt_addr_le_t stored_bond;
static struct bt_conn *active_conn;
static const struct bt_data ad[] = {
    /* Flags: general discoverable, no BR/EDR */
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    /* Set appearance to Generic Computer, little-endian 0x0080 */
    BT_DATA_BYTES(BT_DATA_GAP_APPEARANCE, 0x80, 0x00),
    /* Advertise two services: Device Information and Battery Service */
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
                  BT_UUID_16_ENCODE(BT_UUID_DIS_VAL),
                  BT_UUID_16_ENCODE(BT_UUID_BAS_VAL)),
};

/* Put the full name in the SCAN RESPONSE to save ADV space */
static const struct bt_data sd[] = {
    BT_DATA(BT_DATA_NAME_COMPLETE, CONFIG_BT_DEVICE_NAME,
            sizeof(CONFIG_BT_DEVICE_NAME) - 1),
};

// --- All the callback functions from main.c go here ---
void on_submit(bareterm_widget_t *ti) {
    // Copy out and display
    char out[64];
    bareterm_textinput_get_text(ti, out, sizeof(out));
    bareterm_move_cursor(1, 20);
    bareterm_printf("You entered: %s", out);
}

static void on_button_click1(void *w) {
    bareterm_label_set_text(&lbl1, "Clicked Button 1!");
    bareterm_label_set_color(&lbl1, bareterm_YELLOW, bareterm_BLUE, bareterm_STYLE_BOLD);
	bareterm_progressbar_set_value(&bar, bar.value - 1);
    static int status_idx = 0;
    bareterm_color_t colors[] = { bareterm_GREEN, bareterm_YELLOW, bareterm_RED, bareterm_WHITE };
    status_idx = (status_idx + 1) % 4;
    bareterm_statusindicator_set_color(&status_led, colors[status_idx]);
}

static void on_button_click2(void *w) {
    // Navigate to the settings screen
    bareterm_screen_manager_set_active(settings_screen_get());
}

//LOG_MODULE_REGISTER(test_rf, LOG_LEVEL_DBG);

static struct bt_conn *default_conn;

/* Bluetooth bağlantı callback fonksiyonları */

K_SEM_DEFINE(bt_connected_sem, 0, 1); // Başlangıçta 0, maksimum 1

static void connected(struct bt_conn *conn, uint8_t err)
{
    if (err) {
        //LOG_ERR("Connection failed (err %u)", err);
        //report_result("BLE_TEST", false);
        //return;
        printk("connection failed\n");
    }

    default_conn = bt_conn_ref(conn);
    //LOG_INF("Connection established");
    k_sem_give(&bt_connected_sem); // Semaforu serbest bırak
}

static void disconnected(struct bt_conn *conn, uint8_t reason)
{
    //LOG_INF("Disconnected (reason %u)", reason);
    printk("disconnected\n");
    if (default_conn) {
        bt_conn_unref(default_conn);
        default_conn = NULL;
    }
}

/* Bluetooth bağlantı callback yapısını tanımla */
static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
};



static void on_toggle(bareterm_widget_t *cb, unsigned char state) {

    //init_bt();
    
    int err;
    /* Bluetooth'u başlat */
    err = bt_enable(NULL);
    if (err) {
        //LOG_ERR("Bluetooth init failed (err %d)", err);
        printk("Bluetooth init failed\n");
        //report_result("BLE_TEST", false);
        return;
    }

    //LOG_INF("Bluetooth initialized");
    printk("Bluetooth initialized\n");

    /* Bağlantı callback'lerini kaydet */
    bt_conn_cb_register(&conn_callbacks);

    const struct bt_le_adv_param *param =
        BT_LE_ADV_PARAM(BT_LE_ADV_OPT_CONNECTABLE |
                        BT_LE_ADV_OPT_USE_IDENTITY, 
                        BT_GAP_ADV_FAST_INT_MIN_2,  
                        BT_GAP_ADV_FAST_INT_MAX_2,  
                        NULL);                      

    
    err = bt_le_adv_start(param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
    if (err) {
        printk("Advertising failed to start (err %d)\n", err);
        return;
    }

    //LOG_INF("Advertising started");
    printk("Advertising started. Waiting for connection named BLE TEST...\n");

    /* Bağlantının tamamlanmasını bekle */
    k_sem_take(&bt_connected_sem, K_FOREVER);
    //LOG_INF("Connection established. Waiting for user to press Enter...");
    printk("Connection established. \n");
    //report_result("BLE_TEST", true);


    bareterm_move_cursor(10, 20);
    bareterm_printf("Checkbox is now %s   ", state ? "☑️" : "☐");
	if (state) {
		bareterm_label_set_text(&lbl2, "Checkbox enabled");
		bareterm_modalpopup_show("Checkbox Enabled", "You have enabled the checkbox feature.");
	} else {
		bareterm_label_set_text(&lbl2, "Checkbox disabled");
        bareterm_modalpopup_show("Checkbox Disabled", "The feature has been turned off.");
	}
}

void on_choice_change(bareterm_widget_t *w, int sel) {
    bareterm_move_cursor(4, 18);
    bareterm_printf("Selected: %s   ", choices[sel]);
}

void on_list_select(bareterm_widget_t *w, int sel) {
    bareterm_move_cursor(18, 22);
    bareterm_printf("Listbox selected: %-20s", list_items[sel]);
}

// This function is called when the screen becomes active.
// It is responsible for creating all the widgets for this screen.
static void on_main_screen_enter(bareterm_screen_t *screen) {
    // This is where all the bareterm_*_init() calls from main() go.
    bareterm_draw_box(1, 1, 80, 30, " MAX32657 EVKit Test Tool v0.1.0");
	bareterm_label_init(&lbl1, 15, 4, "Welcome", bareterm_WHITE, bareterm_BLUE, bareterm_STYLE_BOLD);
	bareterm_label_init(&lbl2, 14, 24, "Click somewhere to start", bareterm_YELLOW, bareterm_BLUE, bareterm_STYLE_BOLD);
    bareterm_button_init(&btn1, 4, 6, 20, 3, "[ Button1 ]", on_button_click1);
    bareterm_button_init(&btn2, 26, 6, 20, 3, "[ Settings ]", on_button_click2);
	bareterm_textbox_init(&memo, 3, 10, 36, 5, "To start tests please follow the instructions below: Here are the instructions. Under construction...", bareterm_CYAN, bareterm_BLACK, bareterm_STYLE_NONE);
    bareterm_progressbar_init(&bar, 4,  15, 38, 1, 30, 100, bareterm_GREEN, bareterm_BLACK, bareterm_STYLE_NONE);
    bareterm_checkbox_init(&cb1, 12, 27, "Enable feature 1", false, on_toggle);
    bareterm_checkbox_init(&cb2, 12, 28, "Enable feature 2", false, on_toggle);
    bareterm_radiogroup_init(&rg, 4, 19, choices, 4, 1, on_choice_change);
    bareterm_statusindicator_init(&status_led, 40, 17, "System Status", bareterm_GREEN);
    bareterm_listbox_init(&listbox, 52, 2, 25, 12, list_items, 10, on_list_select);
    
    // The text input 
    // bareterm_textinput_init(&input, ...);
}

// This is the definition of our screen.
static bareterm_screen_t main_screen_def = {
    .on_enter = on_main_screen_enter,
    .on_leave = NULL, // No special cleanup needed
    .user_data = NULL,
};

// This is the public function to get a pointer to our screen.
bareterm_screen_t* main_screen_get(void) {
    return &main_screen_def;
}
