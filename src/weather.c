#include <pebble.h>

//#define DEBUG 1
//#define XLATE 1
//#define DIAGNOSTICS 1

//#undef APP_LOG
//#define APP_LOG(a,b,...)

#ifndef MAX
#define MAX(a, b) ((a) >= (b) ? (a) : (b))
#define MIN(a, b) ((a) <= (b) ? (a) : (b))
#endif

#define KEY_TEMPERATURE     0
#define KEY_CONDITIONS      1
#define KEY_TEMP_FORMAT     2
#define KEY_COND_ICON       3
#define KEY_FACE_ID         4
#define KEY_HANDS           5
#define KEY_DIGITIME        6
#define KEY_WEEKDAY         7
#define KEY_BT              8
#define KEY_BATTERY         9
#define KEY_SECONDS         10
#define KEY_WEATHERTEMP     11
#define KEY_WEATHERCOND     12
#define KEY_DATE            13
#define KEY_FORECAST_DAY0_TEMP_MIN 14
#define KEY_FORECAST_DAY0_TEMP_MAX 15
#define KEY_FORECAST_DAY0_ICON 16
#define KEY_FORECAST_DAY1_TEMP_MIN 17
#define KEY_FORECAST_DAY1_TEMP_MAX 18
#define KEY_FORECAST_DAY1_ICON 19
#define KEY_FORECAST_DAY2_TEMP_MIN 20
#define KEY_FORECAST_DAY2_TEMP_MAX 21
#define KEY_FORECAST_DAY2_ICON 22
#define KEY_FORECAST_DAY3_TEMP_MIN 23
#define KEY_FORECAST_DAY3_TEMP_MAX 24
#define KEY_FORECAST_DAY3_ICON  25
#define KEY_CITY                26
#define KEY_WEATHER_SOURCE      27
#define KEY_PREMIUM_KEY         28
#define KEY_REQUEST             29
#define KEY_BT_ALERT            30
#define KEY_BATTERY_ALERT       31
#define KEY_PREMIUM_KEY_VALID   32
#define KEY_HOUR_ALERT          33
#define KEY_DND                 34
#define KEY_DND_START           35
#define KEY_DND_END             36
#define KEY_LANG                37
#define KEY_FACE_COLOR          38
#define KEY_HAND_COLOR          39
#define KEY_unused              40
#define KEY_INVERT              41
#define KEY_FACE_BG             42
#define KEY_ERROR               43
#define KEY_TIMEZONE            44
#define KEY_DEBUG               45
#define KEY_SHAKE               46
#define KEY_LARGE_FONTS         47
#define KEY_WEATHER_FETCH       48
#define KEY_WEATHER_LOCATION    49
#define KEY_FILLHANDS           50

#define KEY_LAST_WEATHER_UPDATE_TIME 1000
#define KEY_LAST_SUCCESSFUL_WEATHER_UPDATE_TIME 1001

#define REQUEST_WEATHER             0
#define REQUEST_PREMIUM_KEY_LOOKUP  1

#define ERROR_NONE                 0
#define ERROR_GET_LOCATION         1
#define ERROR_GET_WEATHER          2
#define ERROR_GET_WEATHER_FORECAST 3
#define ERROR_TIMEOUT              4
#define ERROR_APPMSG_OPEN          100
#define ERROR_APPMSG_INBOX_DROPPED 101
#define ERROR_APPMSG_OUTBOX_FAILED 102
#define ERROR_APPMSG_OUTBOX_BEGIN  103
#define ERROR_APPMSG_OUTBOX_SEND   104

#define OPTION_OFF          0
#define OPTION_DIGITAL_TIME 1
#define OPTION_STEPS        2
#define OPTION_ACTIVE_TIME  3
#define OPTION_DISTANCE     4
#define OPTION_CALORIES     5
#define OPTION_HEART_RATE   6

#ifdef DIAGNOSTICS
static int s_error = ERROR_NONE;
static AppMessageResult s_appMsgResult = APP_MSG_OK;
#endif

#ifdef DEBUG
#define WEATHER_UPDATE_INTERVAL 300
#else
#define WEATHER_UPDATE_INTERVAL 1800 //1800 sec == 30 minutes
#endif

#define BATTERY_LOW 20

#define TEMP_FORMAT_CELCIUS     0
#define TEMP_FORMAT_FAHRENHEIT  1
static const char *s_temps[2] = { "C", "F" };
#define C2F(t) (t * 9./5. + 32.)

static int s_lang_index = 5;

static const char *s_langs[][8] = {
    {"AF", "Son", "Maa", "Din", "Woe", "Don", "Vry", "Sat" },  // Afrikaans
    {"LI", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },  // British English
    {"CZ", "ned", "pon", "úte", "stř", "čtv", "pát", "sob" },  // Czech
    {"DK", "søn", "man", "tir", "ons", "tor", "fre", "lør" },  // Danish
    {"NL", "zon", "maa", "din", "woe", "don", "vri", "zat" },  // Dutch
    {"EN", "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },  // English
    {"EO", "dim", "lun", "mar", "mer", "ĵaŭ", "ven", "sab" },  // Esperanto
    {"ET", "püh", "esm", "tei", "kol", "nel", "ree", "lau" },  // Estonian
    {"FI", "sun", "maa", "tii", "kes", "tor", "per", "lau" },  // Finnish
    {"FR", "dim", "lun", "mar", "mer", "jeu", "ven", "sam" },  // French
    {"FC", "dim", "lun", "mar", "mer", "jeu", "ven", "sam" },  // French Canadian
    {"DL", "So.", "Mo.", "Di.", "Mi.", "Do.", "Fr.", "Sa." },  // German
    {"GR", "Hêl", "Sel", "Áre", "Her", "Dió", "Aph", "Kró" },  // Greek
    {"HU", "vas", "hét", "ked", "sze", "csü", "pén", "szo" },  // Hungarian
    {"IT", "dom", "lun", "mar", "mer", "gio", "ven", "sab" },  // Italian
    {"NO", "søn", "man", "tir", "ons", "tor", "fre", "lør" },  // Norwegian
    {"PL", "nie", "pon", "wto", "śro", "czw", "pią", "sob" },  // Polish
    {"BR", "dom", "lue", "mar", "mér", "jov", "ver", "sáb" },  // Portuguese
    {"RO", "dum", "lun", "mar", "mie", "joi", "vin", "sâm" },  // Romanian
    {"SK", "ned", "pon", "uto", "str", "štv", "pia", "sob" },  // Slovak
    {"SP", "dom", "lun", "mar", "mié", "jue", "vie", "sáb" },  // Spanish
    {"SW", "sön", "mån", "tis", "ons", "tor", "fre", "lör" },  // Swedish
    {"RU", "Вс ", "Пн ", "Вт ", "Ср ", "Чт ", "Пт ", "Сб " },  // Russian
    {"", "", "", "", "", "", "", "" } // end
};

static char s_weekday_buf[] = "xxxxxx";

static char * WEEKDAY_STR(int wday, unsigned numChar) {
    int len = strlen(s_langs[s_lang_index][wday + 1]);
    snprintf(s_weekday_buf, MIN(sizeof(s_weekday_buf), len - numChar + 1), s_langs[s_lang_index][wday + 1]);
    return s_weekday_buf;
}

static bool updateLang(char *lang) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "updateLang(%s) curlang:%d", lang, s_lang_index);

    int oldLang = s_lang_index;
    s_lang_index = -1;
    
    int i = 0;
    
    while (true) {
        if (strlen(s_langs[i][0]) == 0) {
            s_lang_index = i;
            break;
        } else if (strcmp(s_langs[i][0], lang) == 0) {
            s_lang_index = i;
            break;
        } else {
            i++;
        }
    }
    
    if (s_lang_index == -1) {
        // not found
        s_lang_index = oldLang;
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "updateLang(%s) NOT FOUND curlang:%d", lang, s_lang_index);
    }
    
    // Always save to ensure the language index is persistently saved
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "updateLang(%s) SAVE curlang:%d", lang, s_lang_index);
    persist_write_int(KEY_LANG, s_lang_index);

    if (oldLang != s_lang_index) {
        return true;
    } else {
        return false;
    }
}

static char *time_font = FONT_KEY_GOTHIC_28;
static char *date_font = FONT_KEY_GOTHIC_24_BOLD;
static char *temperature_font = FONT_KEY_GOTHIC_24_BOLD;
static int fLargeFonts = 0;
static int tempFormat = TEMP_FORMAT_CELCIUS;
static int faceId = 1;
static int handsId = 0;
static int digitime = OPTION_DIGITAL_TIME;
static int fweekday = 1;
static int fBt = 0;
static int fBtAlert = 0;
static int fBattery = 0;
static int fBatteryAlert = 0;
static int fHourAlert = 0;
static int fseconds = 0;
static int fweathertemp = 1;
static int fweathercond = 1;
static int fweatherfetch = 1;
static int fdate = 1;
static int fdnd = 0;
static bool fFillHands = true;
static int dndStart = 0;
static int dndEnd = 0;
static bool fBtConnected = true;
static int fInvert = 0;
static float tempCelcius = 0;
static char conditions_buffer[32] = "";
static char temperature_buffer[8] = "";
static char s_iconStr[2] = ")";
static char city_buffer[20] = "";
static char s_premium_key[32] = "";
static char s_weather_location[100];
static int s_premium_key_valid = 0;
static int batteryPct = 100;
static GColor bgColor, fgColor, handColor;
static GCompOp bgComp;
static int s_timezone = 10;

#ifndef PBL_PLATFORM_APLITE
static int s_shake = 2;
#else
static int s_shake = 1;
#endif

static int timerInterval = SECOND_UNIT;

static char timezone2_buf[] = "00:00";
static char update_time_buf[] = "updated 00:00";
#ifndef PBL_PLATFORM_APLITE
static char local_timezone_buf[] = "00:00";
#else
#define local_timezone_buf timezone2_buf
#endif

static int s_hand_color_index = 0;
static int s_face_color_index = 0;
static int s_face_bg_index = 0;

#ifdef PBL_COLOR
static GBitmap *s_offscreen_bitmap = 0;
static GRect s_offscreen_bitmap_bounds;
#endif
#ifdef PBL_COLOR
static unsigned s_face_colors[] =         { 0x000055, 0x0055AA, 0x00AAFF, 0x005500, 0xAA5500, 0x550000, 0x550055, 0xAA0000, 0x000000, 0x0000AA, 0x555500, 0x5500AA };
static unsigned s_face_bg_colors[2][12] = {{0x0055AA, 0x00FFFF, 0x00FFFF, 0x00AA55, 0xFFAA00, 0xAA0000, 0xAA55AA, 0xFF0000, 0xaaaaaa, 0x5555ff, 0xaaaa55, 0x55AAAA },
    /*                                 */ { 0xAAAAAA, 0xFFFFFF, 0xffaa55, 0xAA55AA, 0xAAFFFF, 0xFFFF55, 0x55FF55, 0x55AAAA, 0xFFFF55, 0xaaffaa, 0xaa5555, 0xaaaaff }};
static unsigned s_hand_colors[] =         { 0xFFFFFF, 0xAAAAAA, 0xFFFF55, 0xFFAAFF, 0x55AAAA, 0xffaa55, 0xAAFFAA, 0xAAFFFF, 0xFFAA00, 0x00FF00, 0xff0000, 0x5555ff };
#endif

#define WEATHER_SOURCE_OPEN_WEATHER 0
#define WEATHER_SOURCE_WEATHER_UNDERGROUND 1
#define WEATHER_SOURCE_WEATHER_UNDERGROUND_SPECIFIED_LOCATION 2
static int s_weather_source = WEATHER_SOURCE_OPEN_WEATHER;

#define MAX_FORECAST 4
static float forecastLo[MAX_FORECAST];
static float forecastHi[MAX_FORECAST];
static char forecastIcon[MAX_FORECAST][2] = {{ 0, 0 }, {0, 0}, {0, 0}, {0, 0}};
static AppTimer *weather_request_timer;
static bool weather_request_started = false;
static int weather_request_retry_cnt = 0;
static void weather_timer_callback(void *data);
static time_t lastWeatherUpdateTime = 0;
static time_t lastSuccessfulWeatherUpdateTime = 0;

static Window *s_main_window, *s_forecast_window = 0;
static TextLayer *text_layer;
static GPath *minute_arrow;
static GPath *hour_arrow;
static Layer *hands_layer;
static char num_buffer[4] = "    ";
static GFont fontMeteo28, fontFoundation18;
static GRect s_bt_icon_rect, s_battery_icon_rect, s_time_rect, s_weather_conditions_rect, s_weather_conditions_icon_rect, s_weather_temp_rect, s_weekday_rect, s_date_rect;

#ifdef DIAGNOSTICS
static int fDebug = 0;
static TextLayer *s_error_text_layer;
static GRect s_error_msg_rect;
static char s_error_msg_buffer[] = "nbt 999 999 * abc 999 999";
#endif

#ifndef NO
#define NO false
#define YES true
#endif

#ifdef XLATE
/*
APP_MSG_OK(0) All good, operation was successful.
APP_MSG_SEND_TIMEOUT(2) The other end did not confirm receiving the sent data with an (n)ack in time.
APP_MSG_SEND_REJECTED(4) The other end rejected the sent data, with a "nack" reply.
APP_MSG_NOT_CONNECTED(8) The other end was not connected.
APP_MSG_APP_NOT_RUNNING(16) The local application was not running.
APP_MSG_INVALID_ARGS(32) The function was called with invalid arguments.
APP_MSG_BUSY(64) There are pending (in or outbound) messages that need to be processed first before new ones can be received or sent.
APP_MSG_BUFFER_OVERFLOW(128) The buffer was too small to contain the incoming message.
APP_MSG_ALREADY_RELEASED(512) The resource had already been released.
APP_MSG_CALLBACK_ALREADY_REGISTERED(1024) The callback was already registered.
APP_MSG_CALLBACK_NOT_REGISTERED(2048) The callback could not be deregistered, because it had not been registered before.
APP_MSG_OUT_OF_MEMORY(4096) The system did not have sufficient application memory to perform the requested operation.
APP_MSG_CLOSED(8192) App message was closed.
APP_MSG_INTERNAL_ERROR(16384) An internal OS error prevented AppMessage from completing an operation.
APP_MSG_INVALID_STATE(32768) The function was called while App Message was not in the appropriate state.
*/
char *translate_error(AppMessageResult result) {
    switch (result) {
        case APP_MSG_OK: return "APP_MSG_OK";
        case APP_MSG_SEND_TIMEOUT: return "APP_MSG_SEND_TIMEOUT";
        case APP_MSG_SEND_REJECTED: return "APP_MSG_SEND_REJECTED";
        case APP_MSG_NOT_CONNECTED: return "APP_MSG_NOT_CONNECTED";
        case APP_MSG_APP_NOT_RUNNING: return "APP_MSG_APP_NOT_RUNNING";
        case APP_MSG_INVALID_ARGS: return "APP_MSG_INVALID_ARGS";
        case APP_MSG_BUSY: return "APP_MSG_BUSY";
        case APP_MSG_BUFFER_OVERFLOW: return "APP_MSG_BUFFER_OVERFLOW";
        case APP_MSG_ALREADY_RELEASED: return "APP_MSG_ALREADY_RELEASED";
        case APP_MSG_CALLBACK_ALREADY_REGISTERED: return "APP_MSG_CALLBACK_ALREADY_REGISTERED";
        case APP_MSG_CALLBACK_NOT_REGISTERED: return "APP_MSG_CALLBACK_NOT_REGISTERED";
        case APP_MSG_OUT_OF_MEMORY: return "APP_MSG_OUT_OF_MEMORY";
        case APP_MSG_CLOSED: return "APP_MSG_CLOSED";
        case APP_MSG_INTERNAL_ERROR: return "APP_MSG_INTERNAL_ERROR";
        //case APP_MSG_INVALID_STATE: return "APP_MSG_INVALID_STATE";
        default: return "UNKNOWN ERROR";
    }
}
#endif

static GPath *s_hour_mark_ptr = NULL;
static GPath *s_hour_mark2_ptr = NULL;

static GPoint point4Angle(double angle, int length, GPoint center) {
    return GPoint((int16_t)(sin_lookup(angle) * length / TRIG_MAX_RATIO) + center.x,
                  (int16_t)(-cos_lookup(angle) * length / TRIG_MAX_RATIO) + center.y);
}

#ifdef PBL_COLOR
static void captureOffScreenBitmap(GContext *ctx) {
    GBitmap *buffer = graphics_capture_frame_buffer(ctx);
    GBitmapFormat fmt = gbitmap_get_format(buffer);
    s_offscreen_bitmap_bounds = gbitmap_get_bounds(buffer);
    
    s_offscreen_bitmap = gbitmap_create_blank(s_offscreen_bitmap_bounds.size, fmt);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "create offscreen bitmap:%ld fmt:%d size:%d, %d", (uint32_t)s_offscreen_bitmap, fmt, s_offscreen_bitmap_bounds.size.w, s_offscreen_bitmap_bounds.size.h);
    
    if (s_offscreen_bitmap) {
        // copy all visible pixels
        for(int y = 0; y < s_offscreen_bitmap_bounds.size.h; y++) {
            // Get the min and max x values for this row
            GBitmapDataRowInfo srcinfo = gbitmap_get_data_row_info(buffer, y);
            GBitmapDataRowInfo tgtinfo = gbitmap_get_data_row_info(s_offscreen_bitmap, y);
            
            // Iterate over visible pixels in that row
            for(int x = srcinfo.min_x; x < srcinfo.max_x; x++) {
                memset(&tgtinfo.data[x], srcinfo.data[x], 1);
            }
        }
    }
    
    graphics_release_frame_buffer(ctx, buffer);
}
#endif

char * getBatteryResource()
{
    int charge_percent = battery_state_service_peek().charge_percent;
    if (charge_percent <= 10)
    {
        return "0";
    }
    else if (charge_percent <= 25)
    {
        return "1";
    }
    else if (charge_percent <= 50)
    {
        return "2";
    }
    else if (charge_percent <= 80)
    {
        return "3";
    }
    else
    {
        return "4";
    }
}

int getBtResource()
{
    if (bluetooth_connection_service_peek())
    {
        return RESOURCE_ID_IMAGE_BT;
    }
    else
    {
        return RESOURCE_ID_IMAGE_NO_BT;
    }
}

static void aplite_fill_rect(GContext * ctx, GRect rect) {
#ifndef PBL_COLOR
    if (faceId == 4) {
        rect.origin.x -= 1;
        rect.origin.y -= 1;
        rect.size.w += 2;
        rect.size.h += 2;
        graphics_fill_rect(ctx, rect, 0, GCornerNone);
    }
#endif
}

static const GPathInfo MINUTE_HAND_POINTS[] = {
    { // 0
        5,
        (GPoint []) {
            { -5, 0 },
            { 5, 0 },
            { 5, -60 },
            { 0, -70 },
            { -5, -60 }
        }
    },
    { // 1
        5,
        (GPoint []) {
            { -4, 0 },
            { 4, 0 },
            { 6, -60 },
            { 0, -70 },
            { -6, -60 }
        }
    },
    { // 2
        5,
        (GPoint []) {
            { -2, 0 },
            { 2, 0 },
            { 7, -60 },
            { 0, -70 },
            { -7, -60 }
        }
    },
    { // 3
        5,
        (GPoint []) {
            { -3, 0 },
            { 3, 0 },
            { 3, -60 },
            { 0, -55 },
            { -3, -60 }
        }
    },
    { // 4
        4,
        (GPoint []) {
            { -4, 0 },
            { 4, 0 },
            { 4, -60 },
            { -4, -60 }
        }
    },
    { // 5
        9,
        (GPoint []) {
            { -3, 0 },
            { 3, 0 },
            { 3, -15 },
            { 5, -15 },
            { 7, -55 },
            { 0, -65 },
            { -7, -55 },
            { -5, -15 },
            { -3, -15 }
        }
    },
    { // 6
        3,
        (GPoint []) {
            { -7, -38 },
            { 7, -38 },
            { 0, -70 },
        }
    },
    { // 7
        3,
        (GPoint []) {
            { -7, 0 },
            { 7, 0 },
            { 0, -70 },
        }
    },
    { // 8
        4,
        (GPoint []) {
            { 0, -16 },
            { -6, -30 },
            { 0, -70 },
            { 6, -30 },
        }
    },
};

static const GPathInfo HOUR_HAND_POINTS[] = {
    { // 0
        5, (GPoint []){
            {-5, 0},
            {5, 0},
            {5, -45},
            {0, -55},
            {-5, -45}
        }
    },
    { // 1
        5, (GPoint []){
            {-4, 0},
            {4, 0},
            {7, -45},
            {0, -55},
            {-7, -45}
        }
    },
    { // 2
        5, (GPoint []){
            {-2, 0},
            {2, 0},
            {7, -45},
            {0, -55},
            {-7, -45}
        }
    },
    { // 3
        5, (GPoint []){
            { -3, 0 },
            { 3, 0 },
            { 3, -40 },
            { 0, -45 },
            { -3, -40 }
        }
    },
    { // 4
        4, (GPoint []){
            {-4, 0},
            {4, 0},
            {4, -45},
            {-4, -45}
        }
    },
    { // 5
        9,
        (GPoint []) {
            { -3, 0 },
            { 3, 0 },
            { 3, -15 },
            { 5, -15 },
            { 6, -45 },
            { 0, -50 },
            { -6, -45 },
            { -5, -15 },
            { -3, -15 }
        }
    },
    { // 6
        3,
        (GPoint []) {
            { -8, -35 },
            { 8, -35 },
            { 0, -55 },
        }
    },
    
    { // 7
        3,
        (GPoint []) {
            { -8, 0 },
            { 8, 0 },
            { 0, -50 },
        }
    },
    { // 8
        4,
        (GPoint []) {
            { 0, -16 },
            { -6, -28 },
            { 0, -50 },
            { 6, -28 },
        }
    },
};

static const GPathInfo HOUR_MARK[] = {
    { // 0
        4, (GPoint []){
            {-3, 0},
            {2, 0},
            {2, 8},
            {-3, 8}
        }
    },
    { // 1
        4, (GPoint []){
            {-4, -3},
            {4, -3},
            {4, -45},
            {-4, -45}
        }
    },
    { // 2
        4, (GPoint []){
            {-3, -3},
            {3, -3},
            {3, -45},
            {-3, -45}
        }
    },
    { // 3
        4, (GPoint []){
            {-1, 0},
            {1, 0},
            {9, -45},
            {-9, -45}
        }
    },
    { // 4
        4, (GPoint []){
            {-1, 2},
            {1, 2},
            {7, -45},
            {-7, -45}
        }
    },
    { // 5
        3, (GPoint []){
            {-6, -7},
            {6, -7},
            {0, 6},
        }
    },
    { // 6
        4, (GPoint []){
            {-5, -5},
            {-5, 5},
            {5, 5},
            {5, -5},
        }
    },
    { // 7
        4, (GPoint []){
            {0, -7},
            {-6, 0},
            {0, 6},
            {7, 0},
        }
    },
};

static void hands_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    const GPoint center = grect_center_point(&bounds);
    const int32_t secondHandLength = bounds.size.w / 2 - 5;
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    int hour = t->tm_hour;
    int min = t->tm_min;
    
#if 0
    fseconds = false;
    hour = 10;
    min = 10;
#endif
    
    // minute/hour hand
    if (fFillHands) {
        graphics_context_set_stroke_width(ctx, 1);
        graphics_context_set_fill_color(ctx, handColor);
        graphics_context_set_stroke_color(ctx, bgColor);
    } else {
        graphics_context_set_stroke_width(ctx, 3);
        graphics_context_set_fill_color(ctx, GColorClear);
        graphics_context_set_stroke_color(ctx, handColor);
    }
    gpath_rotate_to(hour_arrow, (TRIG_MAX_ANGLE * (((hour % 12) * 6) + (min / 10))) / (12 * 6));
    if (fFillHands) {
        gpath_draw_filled(ctx, hour_arrow);
    }
    gpath_draw_outline(ctx, hour_arrow);
    
    gpath_rotate_to(minute_arrow, TRIG_MAX_ANGLE * min / 60);
    if (fFillHands) {
        gpath_draw_filled(ctx, minute_arrow);
    }
    gpath_draw_outline(ctx, minute_arrow);
    
    if (fseconds)
    {
        // second hand
        int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
        GPoint secondHand = point4Angle(second_angle, secondHandLength, center);
        graphics_context_set_stroke_color(ctx, handColor);
        graphics_draw_line(ctx, secondHand, center);
    }
    
    // Center circle and do
    graphics_context_set_stroke_color(ctx, handColor);
    graphics_context_set_fill_color(ctx, bgColor);
    graphics_fill_circle(ctx, center, 8);
    graphics_draw_circle(ctx, center, 6);
    graphics_context_set_fill_color(ctx, handColor);
    graphics_fill_circle(ctx, center, 3);
}

static void setupColors() {
#ifdef PBL_COLOR
    gbitmap_destroy(s_offscreen_bitmap);
    s_offscreen_bitmap = 0;
#endif
    if (fInvert) {
#ifdef PBL_COLOR
        handColor =  GColorFromHEX(s_face_colors[s_face_color_index]);
        fgColor = GColorBlack;
        bgColor = GColorFromHEX(s_hand_colors[s_hand_color_index]);
        bgComp = GCompOpSet;
#else
        fgColor = GColorBlack;
        bgColor = GColorWhite;
        handColor = fgColor;
        bgComp = GCompOpAssignInverted;
#endif
    } else {
#ifdef PBL_COLOR
        bgColor =  GColorFromHEX(s_face_colors[s_face_color_index]);
        fgColor = GColorWhite;
        handColor = GColorFromHEX(s_hand_colors[s_hand_color_index]);
        bgComp = GCompOpSet;
#else
        bgColor = GColorBlack;
        fgColor = GColorWhite;
        handColor = fgColor;
        bgComp = GCompOpAssign;
#endif
    }
    
    if (s_main_window) {
        layer_mark_dirty(window_get_root_layer(s_main_window));
    }
}

static void setupHandsId(GPoint center) {
    gpath_destroy(minute_arrow);
    gpath_destroy(hour_arrow);
    minute_arrow = gpath_create(&MINUTE_HAND_POINTS[handsId]);
    hour_arrow = gpath_create(&HOUR_HAND_POINTS[handsId]);
    gpath_move_to(minute_arrow, center);
    gpath_move_to(hour_arrow, center);
}

static void setupFaceId() {
#ifdef PBL_COLOR
    gbitmap_destroy(s_offscreen_bitmap);
    s_offscreen_bitmap = 0;
#endif
    
    if (s_hour_mark_ptr) {
        gpath_destroy(s_hour_mark_ptr);
        s_hour_mark_ptr = NULL;
    }
    
    if (s_hour_mark2_ptr) {
        gpath_destroy(s_hour_mark2_ptr);
        s_hour_mark2_ptr = NULL;
    }
    
    switch (faceId) {
        case 0:
        break;
        case 1:
        s_hour_mark_ptr = gpath_create(&HOUR_MARK[0]);
        break;
        case 2:
        s_hour_mark_ptr = gpath_create(&HOUR_MARK[0]);
        s_hour_mark2_ptr = gpath_create(&HOUR_MARK[1]);
        break;
        case 3:
        s_hour_mark_ptr = gpath_create(&HOUR_MARK[2]);
        break;
        case 4:
#ifdef PBL_PLATFORM_CHALK
        s_hour_mark_ptr = gpath_create(&HOUR_MARK[0]);
#else
        s_hour_mark_ptr = gpath_create(&HOUR_MARK[3]);
#endif
        break;
        case 5:
#ifdef PBL_PLATFORM_CHALK
        s_hour_mark2_ptr = gpath_create(&HOUR_MARK[7]);
#else
        s_hour_mark_ptr = gpath_create(&HOUR_MARK[0]);
        s_hour_mark2_ptr = gpath_create(&HOUR_MARK[4]);
#endif
        break;
        case 6:
        s_hour_mark2_ptr = gpath_create(&HOUR_MARK[5]);
        break;
        case 7:
        case 8:
        s_hour_mark2_ptr = gpath_create(&HOUR_MARK[6]);
        break;
        default:
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "setupFaceId: invalid ID=%d", faceId);
        break;
    }
}

static void setupLayout(GRect bounds) {
    const GPoint center = grect_center_point(&bounds);
    int timeH = 28;
    int tempH = 28;

    // WARNING -- Text fonts like day of week must be Gothic since Bitham does not work with unicode
    //
    if (fLargeFonts) {
#ifdef PBL_PLATFORM_CHALK
        time_font = FONT_KEY_BITHAM_30_BLACK;
        timeH = 30;
        temperature_font = FONT_KEY_BITHAM_30_BLACK;
        tempH = 26;
#else
        
        if (faceId == 3 || faceId == 4) {
            if (faceId == 3) {
                time_font = FONT_KEY_BITHAM_42_BOLD;
                timeH = 42;
            } else {
                time_font = FONT_KEY_BITHAM_30_BLACK;
                timeH = 30;
            }
        } else {
            time_font = FONT_KEY_BITHAM_30_BLACK;
            timeH = 30;
        }

        temperature_font = FONT_KEY_GOTHIC_28_BOLD;
        tempH = 26;
#endif
        date_font = FONT_KEY_GOTHIC_28_BOLD;
    } else {
        time_font = FONT_KEY_GOTHIC_28;
        timeH = 28;
        date_font = FONT_KEY_GOTHIC_24_BOLD;
        temperature_font = FONT_KEY_GOTHIC_24_BOLD;
        tempH = 28;
    }
    
#ifdef PBL_PLATFORM_CHALK
    int pad = 7;
    if (faceId == 2) {
        pad = 8;
    }
    else if (faceId >= 5) {
        pad = 11;
    }
    
    s_bt_icon_rect = GRect(bounds.size.w / 4 - 4, bounds.size.h / 4 + 3, 20, 20);
    s_battery_icon_rect = GRect(bounds.size.w - bounds.size.w / 4 - 14, bounds.size.h / 4 + 8, 20, 12);

    int timePad = 0;
    if (fLargeFonts) {
        timePad = 10;
    }
    s_weather_conditions_icon_rect = GRect(center.x - 30/2, center.y - bounds.size.w / 2 + pad + 2, 30, 30);
    s_weather_temp_rect = GRect(center.x - 50/2 + 1, center.y + bounds.size.w / 2 - tempH - (pad + 3), 50, tempH);
    s_date_rect = GRect(bounds.size.w - 70 - pad, center.y - 30/2 - 2, 70, 30);
    s_time_rect = GRect(0, center.y + (bounds.size.w / 4) - timeH - 2 + timePad, bounds.size.w, timeH);
    s_weekday_rect = GRect(pad + 6, center.y - 30/2 - 2, 70, 30);
#else
    int pad = 7;
    
    if (faceId == 3 || faceId == 4) {
        pad = 5;
        s_weather_conditions_icon_rect = GRect(center.x - 30/2, pad, 30, 30);
        int tempPad = 1;
        int timePad = 0;
        if (fLargeFonts) {
            if (faceId == 4) {
                tempPad = 4;
            } else {
                timePad = 10;
            }
        }
        s_weather_temp_rect = GRect(center.x - 50/2 + 1, bounds.size.h - tempH - pad - tempPad, 50, tempH);
        s_date_rect = GRect(bounds.size.w - 72 - pad - tempPad, center.y - 30/2 - 2, 70, 30);
        s_time_rect = GRect(0, center.y + (bounds.size.h / 4) - timeH + timePad, bounds.size.w, timeH);
    } else {
        if (faceId == 0) {
            pad = 0;
        } else if (faceId >= 7) {
            pad = 8;
        }
        s_weather_conditions_icon_rect = GRect(center.x - 30/2, center.y - bounds.size.w / 2 + pad + 2, 30, 30);
        s_weather_temp_rect = GRect(center.x - 50/2 + 1, center.y + bounds.size.w / 2 - tempH - pad - 2, 50, tempH);
        s_date_rect = GRect(bounds.size.w - 72 - pad, center.y - 30/2 - 2, 70, 30);
        s_time_rect = GRect(0, center.y + (bounds.size.w / 4) - timeH + 1, bounds.size.w, timeH);
    }
    s_weekday_rect = GRect(pad + 4, center.y - 30/2 - 2, 70, 30);
#endif
    
    s_weather_conditions_rect = GRect(center.x - 90/2, s_weather_conditions_icon_rect.origin.y + s_weather_conditions_icon_rect.size.h - 2, 90, 30);
#ifdef DIAGNOSTICS
    s_error_msg_rect = GRect(center.x - 65, center.y - 30, 130, 30);
#endif
}

#ifdef PBL_COLOR

static void bgVertical(Layer *layer, GContext *ctx, GRect bounds, int size) {
    for (int x = bounds.origin.x; x < bounds.origin.x + bounds.size.w; x += size) {
        graphics_draw_line(ctx, GPoint(x, bounds.origin.y ), GPoint(x, bounds.origin.y + bounds.size.h - 2));
    }
}

static void bgHorizontal(Layer *layer, GContext *ctx, GRect bounds, int size) {
    for (int y = bounds.origin.y; y < bounds.origin.y + bounds.size.h; y += size) {
        graphics_draw_line(ctx, GPoint(bounds.origin.x, y), GPoint(bounds.origin.x + bounds.size.w - 2, y));
    }
}

static void bgRadial(Layer *layer, GContext *ctx, int size) {
    GRect bounds = layer_get_bounds(layer);
    const GPoint center = grect_center_point(&bounds);
    static const double radials[9] = { 4, 12, 24, 48, 60, 120, 180, 240, 360 };
    for (int i = 0; i < radials[size]; i++) {
        int32_t angle = TRIG_MAX_ANGLE * (float)(i) / radials[size];
        const int32_t markLength = bounds.size.h * 2;
        GPoint markEnd = point4Angle(angle, markLength, center);
        graphics_draw_line(ctx, center, markEnd);
    }
}

static void bgCircles(Layer *layer, GContext *ctx, int size, bool fUseHeight) {
    GRect bounds = layer_get_bounds(layer);
    const GPoint center = grect_center_point(&bounds);
    
    for (int i = fUseHeight? bounds.size.h : bounds.size.w / 2 - 1; i >= 0; i -= size + 1) {
        graphics_draw_circle(ctx, center, i);
    }
}

static void bgBoxes(Layer *layer, GContext *ctx, GRect bounds, int size) {
    const GPoint center = grect_center_point(&bounds);
    const float ratio = (float)bounds.size.h / (float)bounds.size.w;
    const float cnt = bounds.size.w / (size + 1);
    
    for (int i = 0; i < cnt; i++) {
        const int sz = (i * (size + 1));
        graphics_draw_rect(ctx, GRect(center.x - sz / 2, center.y - sz * ratio / 2, sz, sz * ratio));
    }
}

static void bgSpiral(Layer *layer, GContext *ctx, int size, bool fUseHeight) {
    GRect bounds = layer_get_bounds(layer);
    const GPoint center = grect_center_point(&bounds);
    
    for (int i = 0; i < bounds.size.w / 2 + 1; i+=size) {
        graphics_draw_circle(ctx, GPoint(i, center.y), i);
    }
    for (int i = bounds.size.w; i >= bounds.size.w / 2; i-=size) {
        graphics_draw_circle(ctx, GPoint(i, center.y), bounds.size.w - i);
    }
    
    const int offset = fUseHeight? 0 : (bounds.size.h - bounds.size.w) / 2;
    int range = fUseHeight? bounds.size.h : bounds.size.w;
    
    for (int i = 0; i < range / 2 + 1; i+=size) {
        graphics_draw_circle(ctx, GPoint(center.x, i + offset), i);
    }
    for (int i = range; i >= range / 2; i-=size) {
        graphics_draw_circle(ctx, GPoint(center.x, i + offset), range - i);
    }
}

static void bgRect(Layer *layer, GContext *ctx, int size, bool fill) {
    GRect bounds = layer_get_bounds(layer);
    
    for (int i = 1; i < bounds.size.w; i += size + 1) {
        for (int j = 1; j < bounds.size.h; j += size + 1) {
            if (fill) {
                graphics_fill_rect(ctx, GRect(i, j, size, size), 0, GCornerNone);
            } else {
                graphics_draw_rect(ctx, GRect(i, j, size, size));
            }
        }
    }
}

static void bgRectOffset(Layer *layer, GContext *ctx, int size, bool fill) {
    GRect bounds = layer_get_bounds(layer);
    
    
    for (int i = 1; i < bounds.size.w + size; i += size + 1) {
        for (int j = 1; j < bounds.size.h; j += size + 1) {
            int offset = (i + j) % 2 == 0 ? 0 : size / 2;
            const int delta = offset / 2;
            if (fill) {
                graphics_fill_rect(ctx, GRect(i + delta, j + delta, size - delta * 2, size - delta * 2), 0, GCornerNone);
            } else {
                graphics_draw_rect(ctx, GRect(i + delta, j + delta, size - delta * 2, size - delta * 2));
            }
        }
    }
}

static void bgSpot(Layer *layer, GContext *ctx, GRect bounds, int size, bool fill) {
    for (int i = size / 2; i < bounds.size.w; i += size + 1) {
        for (int j = size / 2; j < bounds.size.h; j += size + 1) {
            if (fill) {
                graphics_fill_circle(ctx, GPoint(bounds.origin.x + i, bounds.origin.y + j), size / 2 - 1);
            } else {
                graphics_draw_circle(ctx, GPoint(bounds.origin.x + i, bounds.origin.y + j), size / 2 - 1);
            }
        }
    }
}

static void bgSpotOffset(Layer *layer, GContext *ctx, int size, bool fill) {
    GRect bounds = layer_get_bounds(layer);
    
    for (int i = size / 2; i < bounds.size.w + size; i += size + 1) {
        for (int j = size / 2; j < bounds.size.h; j += size + 1) {
            int offset = (i + j) % 2 == 0 ? 0 : size / 2;
            if (fill) {
                graphics_fill_circle(ctx, GPoint(i, j), size / 2 - 1 - offset / 2);
            } else {
                graphics_draw_circle(ctx, GPoint(i, j), size / 2 - 1 - offset / 2);
            }
        }
    }
}

static void bgDiamond(Layer *layer, GContext *ctx, int size, bool fill) {
    GRect bounds = layer_get_bounds(layer);
    GPathInfo DIAMOND_POINTS = {
        4, (GPoint []){
            {-size / 2, 0},
            {0, size / 2},
            {size / 2, 0},
            {0, -size / 2},
        }
    };
    GPath *diamond_gpath = gpath_create(&DIAMOND_POINTS);
    
    for (int i = size / 2; i < bounds.size.w + size; i += size) {
        for (int j = size / 2; j < bounds.size.h; j += size) {
            gpath_move_to(diamond_gpath, GPoint(i, j));
            
            if (fill) {
                gpath_draw_filled(ctx, diamond_gpath);
            } else {
                gpath_draw_outline(ctx, diamond_gpath);
            }
        }
    }
    
    gpath_destroy(diamond_gpath);
}

static void bgInvertedRect(Layer *layer, GContext *ctx, int gap) {
    GRect bounds = layer_get_bounds(layer);
    const GPoint center = grect_center_point(&bounds);
    const int size = gap + 1;
    const int cnt = bounds.size.h / 2 / size;
    
    for (int i = 0; i < cnt; i++) {
        { // LR
            GPoint ptStart = GPoint(MAX(bounds.size.w - i * size, center.x), bounds.size.h - i * size);
            
            GPoint ptEnd1 = GPoint(bounds.size.w + 1, ptStart.y);
            graphics_draw_line(ctx, ptStart, ptEnd1);
            
            //if (ptStart.x - center.x >= size)
            {
                GPoint ptEnd2 = GPoint(ptStart.x, bounds.size.h + 1);
                graphics_draw_line(ctx, ptStart, ptEnd2);
            }
        }
        { // LL
            GPoint ptStart = GPoint(MIN(i * size, center.x), bounds.size.h - i * size);
            
            GPoint ptEnd1 = GPoint(-1, ptStart.y);
            graphics_draw_line(ctx, ptStart, ptEnd1);
            
            //if (center.x - ptStart.x >= size)
            {
                GPoint ptEnd2 = GPoint(ptStart.x, bounds.size.h + 1);
                graphics_draw_line(ctx, ptStart, ptEnd2);
            }
        }
        { // UR
            GPoint ptStart = GPoint(MAX(bounds.size.w - i * size, center.x), i * size);
            
            GPoint ptEnd1 = GPoint(bounds.size.w + 1, ptStart.y);
            graphics_draw_line(ctx, ptStart, ptEnd1);
            
            //if (ptStart.x - center.x >= size)
            {
                GPoint ptEnd2 = GPoint(ptStart.x,  -1);
                graphics_draw_line(ctx, ptStart, ptEnd2);
            }
        }
        { // UL
            GPoint ptStart = GPoint(MIN(i * size, center.x), i * size);
            
            GPoint ptEnd1 = GPoint(-1, ptStart.y);
            graphics_draw_line(ctx, ptStart, ptEnd1);
            
            //if (center.x - ptStart.x >= size)
            {
                GPoint ptEnd2 = GPoint(ptStart.x,  -1);
                graphics_draw_line(ctx, ptStart, ptEnd2);
            }
        }
    }
}

static void clearFaceCircle(Layer *layer, GContext *ctx) {
    const GRect bounds = layer_get_bounds(layer);
    const GPoint center = grect_center_point(&bounds);
    graphics_context_set_fill_color(ctx, bgColor);
    graphics_fill_circle(ctx, center, bounds.size.w / 2);
    graphics_draw_circle(ctx, center, bounds.size.w / 2);
}

static void drawBg(Layer *layer, GContext *ctx, bool fClearFaceCircle, bool fUseHeight) {
    const GRect bounds = layer_get_bounds(layer);
    if (fInvert) {
        graphics_context_set_fill_color(ctx, GColorFromHEX(s_face_bg_colors[fInvert][s_hand_color_index]));
        graphics_context_set_stroke_color(ctx, GColorFromHEX(s_face_bg_colors[fInvert][s_hand_color_index]));
    } else {
        graphics_context_set_fill_color(ctx, GColorFromHEX(s_face_bg_colors[fInvert][s_face_color_index]));
        graphics_context_set_stroke_color(ctx, GColorFromHEX(s_face_bg_colors[fInvert][s_face_color_index]));
    }
#if defined(PBL_PLATFORM_BASALT)
    switch (s_face_bg_index) {
        case 1:
        bgCircles(layer, ctx, 2, fUseHeight);
        fClearFaceCircle = false;
        break;
        case 2:
        bgSpiral(layer, ctx, bounds.size.w / 8, fUseHeight);
        fClearFaceCircle = false;
        break;
        case 3:
        bgSpiral(layer, ctx, bounds.size.w / 16, fUseHeight);
        fClearFaceCircle = false;
        break;
        case 4:
        bgRadial(layer, ctx, 6);
        break;
        case 5:
        bgRect(layer, ctx, 7, true);
        break;
        case 6:
        bgDiamond(layer, ctx, 8, true);
        break;
        case 7:
        bgSpot(layer, ctx, bounds, 8, true);
        break;
        case 8:
        bgInvertedRect(layer, ctx, 3);
        break;
        case 9:
        bgDiamond(layer, ctx, 8, false);
        break;
        case 10:
        bgSpot(layer, ctx, bounds, 8, false);
        break;
        case 11:
        bgRectOffset(layer, ctx, 12, true);
        break;
        case 12:
        bgSpotOffset(layer, ctx, 12, true);
        break;
        case 13:
        bgSpot(layer, ctx, bounds, 4, true);
        break;
        case 14:
        bgRect(layer, ctx, 7, false);
        break;
        default:
        fClearFaceCircle = false;
        break;
    }
    
    if (fClearFaceCircle) {
        clearFaceCircle(layer, ctx);
    }
#elif defined(PBL_PLATFORM_CHALK)
    switch (s_face_bg_index) {
        case 1:
        bgCircles(layer, ctx, 2, fUseHeight);
        break;
        case 2:
        bgSpiral(layer, ctx, bounds.size.w / 8, fUseHeight);
        break;
        case 3:
        bgSpiral(layer, ctx, bounds.size.w / 16, fUseHeight);
        break;
        case 4:
        bgRadial(layer, ctx, 5);
        break;
        case 5:
        bgRect(layer, ctx, 7, true);
        break;
        case 6:
        bgDiamond(layer, ctx, 10, true);
        break;
        case 7:
        bgSpot(layer, ctx, bounds, 8, true);
        break;
        case 8:
        bgInvertedRect(layer, ctx, 2);
        break;
        case 9:
        bgDiamond(layer, ctx, 8, false);
        break;
        case 10:
        bgSpot(layer, ctx, bounds, 8, false);
        break;
        case 11:
        bgRectOffset(layer, ctx, 12, true);
        break;
        case 12:
        bgSpotOffset(layer, ctx, 12, true);
        break;
        case 13:
        bgSpot(layer, ctx, bounds, 4, true);
        break;
        case 14:
        bgRect(layer, ctx, 7, false);
        break;
        default:
        fClearFaceCircle = false;
        break;
    }
    
    if (fClearFaceCircle) {
        //clearFaceCircle(layer, ctx);
    }
#endif
    graphics_context_set_fill_color(ctx, fgColor);
    graphics_context_set_stroke_color(ctx, fgColor);
}
#endif

float my_sqrt(const float num) {
    const uint MAX_STEPS = 40;
    const float MAX_ERROR = 0.001;
    
    float answer = num;
    float ans_sqr = answer * answer;
    uint step = 0;
    while((ans_sqr - num > MAX_ERROR) && (step++ < MAX_STEPS)) {
        answer = (answer + (num / answer)) / 2;
        ans_sqr = answer * answer;
    }
    return answer;
}
//  public domain function by Darel Rex Finley, 2006
//  Determines the intersection point of the line segment defined by points A and B
//  with the line segment defined by points C and D.
//
//  Returns YES if the intersection point was found, and stores that point in X,Y.
//  Returns NO if there is no determinable intersection point, in which case X,Y will
//  be unmodified.

static bool lineSegmentIntersection( double Ax, double Ay,
                                    double Bx, double By,
                                    double Cx, double Cy,
                                    double Dx, double Dy,
                                    double *X, double *Y) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "lineSegmentIntersection(Ax:%i, Ay:%i, Bx:%i, By:%i, Cx:%i, Cy:%i, Dx:%i, Dy:%i)", (int)Ax, (int)Ay, (int)Bx, (int)By, (int)Cx, (int)Cy, (int)Dx, (int)Dy);
    
    double  distAB, theCos, theSin, newX, ABpos ;
    
    //  Fail if either line segment is zero-length.
    if ((Ax==Bx && Ay==By) || (Cx==Dx && Cy==Dy)) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "lineSegmentIntersection NO 1");
        return NO;
    }
    
    //  Fail if the segments share an end-point.
    if ((Ax==Cx && Ay==Cy) || (Bx==Cx && By==Cy) || (Ax==Dx && Ay==Dy) || (Bx==Dx && By==Dy)) {
        return NO;
    }
    
    //  (1) Translate the system so that point A is on the origin.
    Bx-=Ax; By-=Ay;
    Cx-=Ax; Cy-=Ay;
    Dx-=Ax; Dy-=Ay;
    
    //  Discover the length of segment A-B.
    distAB=my_sqrt((float)(Bx*Bx+By*By));
    
    //  (2) Rotate the system so that point B is on the positive X axis.
    theCos=Bx/distAB;
    theSin=By/distAB;
    newX=Cx*theCos+Cy*theSin;
    Cy  =Cy*theCos-Cx*theSin; Cx=newX;
    newX=Dx*theCos+Dy*theSin;
    Dy  =Dy*theCos-Dx*theSin; Dx=newX;
    
    //  Fail if segment C-D doesn't cross line A-B.
    if ((Cy<0. && Dy<0.) || (Cy>=0. && Dy>=0.)) {
        return NO;
    }
    
    //  (3) Discover the position of the intersection point along line A-B.
    ABpos=Dx+(Cx-Dx)*Dy/(Dy-Cy);
    
    //  Fail if segment C-D crosses line A-B outside of segment A-B.
    if (ABpos<0. || ABpos>distAB) {
        return NO;
    }
    
    //  (4) Apply the discovered position to line A-B in the original coordinate system.
    *X=Ax+ABpos*theCos;
    *Y=Ay+ABpos*theSin;
    
    //  Success.
    return YES;
}

static void face_update_text_proc(Layer *layer, GContext *ctx) {
    //GRect bounds = layer_get_bounds(layer);
    //const GPoint center = grect_center_point(&bounds);
    
    graphics_context_set_fill_color(ctx, bgColor);
    graphics_context_set_text_color(ctx, fgColor);
    
    if (fBt || !fBtConnected) {
        graphics_draw_text(ctx, bluetooth_connection_service_peek() ? "B" : "b", fontFoundation18, s_bt_icon_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    }
    
    if (fBattery || batteryPct <= BATTERY_LOW) {
        graphics_draw_text(ctx, getBatteryResource(), fontFoundation18, s_battery_icon_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    }
    
    if (digitime == OPTION_DIGITAL_TIME) {
        graphics_draw_text(ctx, timezone2_buf, fonts_get_system_font(time_font), s_time_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    } else if (digitime >= OPTION_STEPS) {
#if defined(PBL_HEALTH)
        // Check the metric has data available for today
        time_t start = time_start_of_today();
        HealthMetric metric = HealthMetricStepCount;
        switch (digitime) {
            case OPTION_ACTIVE_TIME: // Active Time
                metric = HealthMetricActiveSeconds;
                break;
            case OPTION_DISTANCE: // Walked Distance
                metric = HealthMetricWalkedDistanceMeters;
                break;
            case OPTION_CALORIES: // K Calories Total
                metric = HealthMetricActiveKCalories;
                break;
            case OPTION_HEART_RATE: // Heart rate
                metric = HealthMetricHeartRateBPM;
                start = time(NULL);
                break;
        }
        
        time_t end = time(NULL);
        
        HealthServiceAccessibilityMask mask = health_service_metric_accessible(metric, start, end);
        
        if(mask & HealthServiceAccessibilityMaskAvailable) {
            static char s_health[] = "999999 xxx";
            int sumToday = (int)health_service_sum_today(metric);
            switch (digitime) {
                case OPTION_STEPS: // Steps
                    snprintf(s_health, sizeof(s_health), "%d st", sumToday);
                    break;
                case OPTION_ACTIVE_TIME: { // Active Time
                    int hour = sumToday / 60 / 60;
                    int min = sumToday / 60 - hour * 60;
                    int sec = sumToday - min * 60 - hour * 60 * 60;
                    
                    if (hour > 0) {
                        snprintf(s_health, sizeof(s_health), "%d:%02d:%02d", hour, min, sec);
                    } else if (min > 0) {
                        snprintf(s_health, sizeof(s_health), "%d:%02d", min, sec);
                    } else {
                        snprintf(s_health, sizeof(s_health), "%d sec", sec);
                    }
                    break;
                }
                case OPTION_DISTANCE: // Walked Distance
                    if (tempFormat == TEMP_FORMAT_CELCIUS) {
                        float km = sumToday / 1000.0;
                        if (km >= 1) {
                            snprintf(s_health, sizeof(s_health), "%d.%02d km", (int)km, (int)((km - (int)km) * 100));
                        } else {
                            snprintf(s_health, sizeof(s_health), "%d m", sumToday);
                        }
                    } else {
                        float feet = sumToday * 3.28F;
                        float mi = feet / 5280;
                        if (mi >= 1.0) {
                            snprintf(s_health, sizeof(s_health), "%d.%02d mi", (int)mi, (int)((mi - (int)mi) * 100));
                        } else {
                            snprintf(s_health, sizeof(s_health), "%d ft", (int)feet);
                        }
                    }
                    break;
                case OPTION_CALORIES: // K Calories Total
                    sumToday += health_service_sum_today(HealthMetricRestingKCalories);  // Add resting to active (from above)
                    snprintf(s_health, sizeof(s_health), "%d ca", sumToday);
                    break;
                case OPTION_HEART_RATE: { // Heart rate
                    HealthValue val = health_service_peek_current_value(HealthMetricHeartRateBPM);
                    if(val > 0) {
                        // Display HRM value
                        snprintf(s_health, sizeof(s_health), "%lu bpm", (uint32_t)val);
                    } else {
                        snprintf(s_health, sizeof(s_health), "-- bpm");
                    }
                    break;
                }
            }
            graphics_draw_text(ctx, s_health, fonts_get_system_font(time_font), s_time_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
        }
#endif
    }
    
    if (fweatherfetch && fweathercond) {
        aplite_fill_rect(ctx, s_weather_conditions_icon_rect);
        graphics_draw_text(ctx, s_iconStr, fontMeteo28, s_weather_conditions_icon_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
#ifndef PBL_PLATFORM_CHALK
        graphics_draw_text(ctx, conditions_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_18), s_weather_conditions_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
#endif
    }
    if (fweatherfetch && fweathertemp) {
        aplite_fill_rect(ctx, s_weather_temp_rect);
        graphics_draw_text(ctx, temperature_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), s_weather_temp_rect, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    }
    
    if (fweekday) {
        aplite_fill_rect(ctx, s_weekday_rect);
        time_t now = time(NULL);
        struct tm *tick_time =  localtime(&now);
        graphics_draw_text(ctx, WEEKDAY_STR(tick_time->tm_wday, (faceId == 3 || faceId == 4) && fLargeFonts ? 1 : 0), fonts_get_system_font(date_font), s_weekday_rect, GTextOverflowModeWordWrap, GTextAlignmentLeft, 0);
    }
    
    if (fdate) {
        aplite_fill_rect(ctx, s_date_rect);
        graphics_draw_text(ctx, num_buffer, fonts_get_system_font(date_font), s_date_rect, GTextOverflowModeWordWrap, GTextAlignmentRight, 0);
    }
}

#ifdef PBL_PLATFORM_CHALK

static void face_update_proc(Layer *layer, GContext *ctx) {
    if (s_offscreen_bitmap) {
        graphics_draw_bitmap_in_rect(ctx, s_offscreen_bitmap, s_offscreen_bitmap_bounds);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "draw offscreen bitmap:%08lx", (uint32_t)s_offscreen_bitmap);
    } else {
        GRect bounds = layer_get_bounds(layer);
        const int inset = 2;
        bounds.origin.x += inset;
        bounds.origin.y += inset;
        bounds.size.w -= inset * 2;
        bounds.size.h -= inset * 2;
        
        const GPoint center = grect_center_point(&bounds);
        graphics_context_set_stroke_color(ctx, fgColor);
        graphics_context_set_fill_color(ctx, bgColor);
        graphics_fill_rect(ctx, bounds, 0, GCornerNone);
        graphics_context_set_fill_color(ctx, fgColor);
        setupLayout(bounds);
        
        drawBg(layer, ctx, false, false);
        
        if (faceId != 0) {
            int32_t markLength = bounds.size.w / 2;
            if (faceId >= 5) {
                markLength -= 7;
            }
            
            for (int i = 0; i < 60; i++) {
                int32_t angle = TRIG_MAX_ANGLE * (float)(i) / 60.0;
                GPoint markStart = point4Angle(angle, markLength - 3, center);
                GPoint markEnd = point4Angle(angle, markLength, center);
                
                if (faceId < 7 && faceId != 5 && faceId != 4) {
                    graphics_draw_line(ctx, markStart, markEnd);
                }
                
                if ((i % 5) == 0) {
                    if (faceId == 8) {
                        if (i == 0 || i == 15 || i == 30 || i == 45) {
                            gpath_rotate_to(s_hour_mark2_ptr, angle);
                            gpath_move_to(s_hour_mark2_ptr, markEnd);
                            gpath_draw_filled(ctx, s_hour_mark2_ptr);
                        } else {
                            graphics_fill_circle(ctx, markEnd, 5);
                        }
                    } else {
                        if (faceId == 2) {
                            GPoint mark = point4Angle(angle, markLength - 12, center);
                            gpath_rotate_to(s_hour_mark2_ptr, angle);
                            gpath_move_to(s_hour_mark2_ptr, mark);
                            gpath_draw_filled(ctx, s_hour_mark2_ptr);
                        } else if (faceId == 3) {
                            GPoint mark = point4Angle(angle, markLength - 3, center);
                            graphics_fill_circle(ctx, mark, 3);
                        } else {
                            if (faceId < 6) {
                                gpath_rotate_to(s_hour_mark_ptr, angle);
                                gpath_move_to(s_hour_mark_ptr, markEnd);
                                gpath_draw_filled(ctx, s_hour_mark_ptr);
                            }
                            
                            if (s_hour_mark2_ptr) {
                                gpath_rotate_to(s_hour_mark2_ptr, angle);
                                gpath_move_to(s_hour_mark2_ptr, markEnd);
                                gpath_draw_filled(ctx, s_hour_mark2_ptr);
                            }
                        }
                    }
                }
            }
        }
        
        captureOffScreenBitmap(ctx);
    }
}

#else // not PBL_PLATFORM_CHALK

static void face_update_proc(Layer *layer, GContext *ctx) {
#ifdef PBL_COLOR
    if (s_offscreen_bitmap) {
        graphics_draw_bitmap_in_rect(ctx, s_offscreen_bitmap, s_offscreen_bitmap_bounds);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "draw offscreen bitmap:%08lx", (uint32_t)s_offscreen_bitmap);
    } else {
#endif
        GRect bounds = layer_get_bounds(layer);
        const GPoint center = grect_center_point(&bounds);
        graphics_context_set_stroke_color(ctx, fgColor);
        graphics_context_set_fill_color(ctx, bgColor);
        graphics_fill_rect(ctx, bounds, 0, GCornerNone);
        graphics_context_set_fill_color(ctx, fgColor);
        setupLayout(bounds);
        
        if (faceId == 0) {
#ifdef PBL_COLOR
            drawBg(layer, ctx, true, false);
#endif
        }
        else if (faceId == 0 || faceId == 1 || faceId == 2 || faceId == 5 || faceId >= 6) {
#ifdef PBL_COLOR
            drawBg(layer, ctx, true, false);
#endif
            int32_t markLength = bounds.size.w / 2;
            if (faceId >= 7) {
                markLength -= 4;
            }
            
            for (int i = 0; i < 60; i++) {
                int32_t angle = TRIG_MAX_ANGLE * (float)(i) / 60.0;
                GPoint markStart = point4Angle(angle, markLength - 3, center);
                GPoint markEnd = point4Angle(angle, markLength, center);
                
                if (faceId < 7) {
                    graphics_draw_line(ctx, markStart, markEnd);
                }
                
                if ((i % 5) == 0) {
                    if (faceId == 8) {
                        if (i == 0 || i == 15 || i == 30 || i == 45) {
                            gpath_rotate_to(s_hour_mark2_ptr, angle);
                            gpath_move_to(s_hour_mark2_ptr, markEnd);
                            gpath_draw_filled(ctx, s_hour_mark2_ptr);
                        } else {
                            graphics_fill_circle(ctx, markEnd, 5);
                        }
                    } else {
                        
                        if (faceId < 6) {
                            gpath_rotate_to(s_hour_mark_ptr, angle);
                            gpath_move_to(s_hour_mark_ptr, markEnd);
                            gpath_draw_filled(ctx, s_hour_mark_ptr);
                        }
                        
                        if (s_hour_mark2_ptr) {
                            gpath_rotate_to(s_hour_mark2_ptr, angle);
                            gpath_move_to(s_hour_mark2_ptr, markEnd);
                            gpath_draw_filled(ctx, s_hour_mark2_ptr);
                        }
                    }
                }
            }
        } else if (faceId == 3) {
            
            for (int i = 0; i < 60; i++) {
                const int32_t markLength = bounds.size.h;
                int32_t angle = TRIG_MAX_ANGLE * (float)(i) / 60.0;
                GPoint markEnd = point4Angle(angle, markLength, center);
                graphics_draw_line(ctx, center, markEnd);
            }
            
            graphics_context_set_fill_color(ctx, bgColor);
            GRect innerBounds = bounds;
            innerBounds.origin.x += 4;
            innerBounds.origin.y += 4;
            innerBounds.size.w -= 8;
            innerBounds.size.h -= 8;
            graphics_fill_rect(ctx, innerBounds, 0, GCornerNone);
            
#ifdef PBL_COLOR
            drawBg(layer, ctx, false, true);
#endif
            graphics_context_set_fill_color(ctx, fgColor);
            
            for (int i = 0; i < 60; i+=5) {
                int32_t markLength = bounds.size.w / 2 - 8;
                if (i == 0 || i == 5 || i == 25 || i == 30 || i == 35 || i == 55) {
                    markLength = bounds.size.h / 2 - 5;
                }
                int32_t angle = TRIG_MAX_ANGLE * (float)(i) / 60.0;
                GPoint markEnd = point4Angle(angle, markLength, center);
                
                gpath_rotate_to(s_hour_mark_ptr, angle);
                gpath_move_to(s_hour_mark_ptr, markEnd);
                gpath_draw_filled(ctx, s_hour_mark_ptr);
            }
            
        } else if (faceId == 4) {
#ifdef PBL_COLOR
            if (s_face_bg_index > 3) {
                drawBg(layer, ctx, false, false);
            }
#endif
            const double InsetX = 25;
            const double InsetY = InsetX;
            graphics_context_set_fill_color(ctx, fgColor);
            
            for (int i = 0; i < 60; i+=5) {
                double angle = TRIG_MAX_ANGLE * (double)(i) / 60.0;
                const double markLength = bounds.size.h ;
                GPoint markStart = center;
                GPoint markEnd = point4Angle(angle, markLength, center);
                double iX = 0;
                double iY = 0;
                
                if (lineSegmentIntersection(InsetX, InsetY,
                                            bounds.size.w - InsetX, InsetY,
                                            markStart.x, markStart.y,
                                            markEnd.x, markEnd.y,
                                            &iX, &iY)) {
                } else if (lineSegmentIntersection(InsetX, bounds.size.h - InsetY,
                                                   bounds.size.w - InsetX, bounds.size.h - InsetY,
                                                   markStart.x, markStart.y,
                                                   markEnd.x, markEnd.y,
                                                   &iX, &iY)) {
                } else if (lineSegmentIntersection(bounds.size.w - InsetX, InsetY,
                                                   bounds.size.w - InsetX, bounds.size.h - InsetY,
                                                   markStart.x, markStart.y,
                                                   markEnd.x, markEnd.y,
                                                   &iX, &iY)) {
                } else if (lineSegmentIntersection(InsetX, InsetY,
                                                   InsetX, bounds.size.h - InsetY,
                                                   markStart.x, markStart.y,
                                                   markEnd.x, markEnd.y,
                                                   &iX, &iY)) {
                }
                
                markEnd.x = iX;
                markEnd.y = iY;
                double dist = my_sqrt((center.x - iX)*(center.x - iX) + (center.y - iY)*(center.y - iY));
                markStart = point4Angle(angle, dist - 10, center);
                gpath_rotate_to(s_hour_mark_ptr, angle);
                gpath_move_to(s_hour_mark_ptr, markStart);
                gpath_draw_filled(ctx, s_hour_mark_ptr);
            }
            
            GRect innerRect = GRect(InsetX - 1, InsetY - 1, bounds.size.w - InsetX * 2 + 4, bounds.size.h - InsetY * 2 + 4);
            GRect innerRect2 = GRect(innerRect.origin.x + 8, innerRect.origin.y + 8, innerRect.size.w - 8 * 2, innerRect.size.h - 8 * 2);
            graphics_context_set_fill_color(ctx, bgColor);
            graphics_fill_rect(ctx, innerRect, 0, GCornerNone);
            
#ifdef PBL_COLOR
            graphics_context_set_fill_color(ctx, GColorFromHEX(s_face_bg_colors[fInvert][s_face_color_index]));
            graphics_context_set_stroke_color(ctx, GColorFromHEX(s_face_bg_colors[fInvert][s_face_color_index]));
            
            switch (s_face_bg_index) {
                case 1:
                bgBoxes(layer, ctx, innerRect2, 8);
                break;
                case 2:
                bgVertical(layer, ctx, innerRect2, innerRect2.size.w / 8);
                bgHorizontal(layer, ctx, innerRect2, innerRect2.size.h / 8);
                break;
                case 3:
                bgSpot(layer, ctx, innerRect2, 4, true);
                break;
                default:
                break;
            }
            graphics_context_set_stroke_color(ctx, fgColor);
#endif
            graphics_context_set_fill_color(ctx, fgColor);
            
            for (int i = 0; i < 60; i++) {
                const double markLength = bounds.size.h ;
                double angle = TRIG_MAX_ANGLE * (double)(i) / 60.0;
                GPoint markStart = center;
                GPoint markEnd = point4Angle(angle, markLength, center);
                
                double iX = 0;
                double iY = 0;
                
                if (lineSegmentIntersection(InsetX, InsetY,
                                            bounds.size.w - InsetX, InsetY,
                                            markStart.x, markStart.y,
                                            markEnd.x, markEnd.y,
                                            &iX, &iY)) {
                } else if (lineSegmentIntersection(InsetX, bounds.size.h - InsetY,
                                                   bounds.size.w - InsetX, bounds.size.h - InsetY,
                                                   markStart.x, markStart.y,
                                                   markEnd.x, markEnd.y,
                                                   &iX, &iY)) {
                } else if (lineSegmentIntersection(bounds.size.w - InsetX, InsetY,
                                                   bounds.size.w - InsetX, bounds.size.h - InsetY,
                                                   markStart.x, markStart.y,
                                                   markEnd.x, markEnd.y,
                                                   &iX, &iY)) {
                } else if (lineSegmentIntersection(InsetX, InsetY,
                                                   InsetX, bounds.size.h - InsetY,
                                                   markStart.x, markStart.y,
                                                   markEnd.x, markEnd.y,
                                                   &iX, &iY)) {
                }
                
                markEnd.x = iX;
                markEnd.y = iY;
                double dist = my_sqrt((center.x - iX)*(center.x - iX) + (center.y - iY)*(center.y - iY));
                markStart = point4Angle(angle, dist - 4, center);
                graphics_draw_line(ctx, markStart, markEnd);
            }
#ifdef PBL_COLOR
            int pad = 4;
            graphics_context_set_fill_color(ctx, bgColor);
            graphics_fill_rect(ctx, GRect(center.x - 24/2, pad, 24, 24), 0, GCornerNone);
            graphics_fill_rect(ctx, GRect(center.x - 22/2 + 1, bounds.size.h - 18 - 4 - pad, 22, 18), 0, GCornerNone);
            graphics_fill_rect(ctx, GRect(pad, center.y - 18/2, 30, 18), 0, GCornerNone);
            graphics_fill_rect(ctx, GRect(bounds.size.w - 22 - pad, center.y - 18/2, 22, 18), 0, GCornerNone);
#endif
        }
        
#ifdef PBL_COLOR
        captureOffScreenBitmap(ctx);
    }
#endif
}
#endif

#ifndef PBL_PLATFORM_APLITE
static struct tm *GetTime() {
    time_t now = time(NULL);
    int timezone = s_timezone / 10;
    if (timezone == 1) {
        return localtime(&now);
    } else {
        struct tm *ptm = gmtime(&now);
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "gmt:%2d:%02d", ptm->tm_hour, ptm->tm_min);
        int hr = timezone/60;
        int min = timezone - hr*60;
        
        ptm->tm_hour = ((ptm->tm_hour + hr) + 24) % 24;
        if (ptm->tm_min + min > 60) {
            ptm->tm_hour = (ptm->tm_hour + 1)%24;
        } else if (ptm->tm_min + min < 0) {
            ptm->tm_hour = (ptm->tm_hour - 1)%24;
        }
        ptm->tm_min = (ptm->tm_min + min)%60;
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "tz=%3d hr=%3d min=%3d %2d:%02d", s_timezone, hr, min, ptm->tm_hour, ptm->tm_min);
        return ptm;
    }
}
#endif

static void update_time() {
    time_t now = time(NULL);

    // Digital time displays user-selected timezone color models and local time on BW models
#ifndef PBL_PLATFORM_APLITE
    struct tm *tick_time = GetTime();
#else
    struct tm *tick_time =  localtime(&now);
#endif

    int hour = tick_time->tm_hour;
    int min = tick_time->tm_min;
    
    if(clock_is_24h_style() == true) {
        snprintf(timezone2_buf, sizeof(timezone2_buf), "%d:%02d", hour, min);
    } else {
        int hr = hour % 12;
        if (hr == 0)
            hr = 12;
        snprintf(timezone2_buf, sizeof(timezone2_buf), "%d:%02d", hr, min);
    }

#ifndef PBL_PLATFORM_APLITE
    now = time(NULL);
    tick_time = localtime(&now);
    hour = tick_time->tm_hour;
    min = tick_time->tm_min;
    
    // Write the current hours and minutes into the buffer
    if(clock_is_24h_style() == true) {
        snprintf(local_timezone_buf, sizeof(local_timezone_buf), "%d:%02d", hour, min);
    } else {
        int hr = hour % 12;
        if (hr == 0)
            hr = 12;
        snprintf(local_timezone_buf, sizeof(local_timezone_buf), "%d:%02d", hr, min);
    }
#endif 
    
    // Date is from local timezone
    strftime(num_buffer, sizeof(num_buffer), "%e", tick_time);
    
    //display last weather update time on forecast screen
    if (lastSuccessfulWeatherUpdateTime > 0) {
        tick_time = localtime(&lastSuccessfulWeatherUpdateTime);
        hour = tick_time->tm_hour;
        min = tick_time->tm_min;
        
        // Write the current hours and minutes into the buffer
        if(clock_is_24h_style() == true) {
            snprintf(update_time_buf, sizeof(update_time_buf), "updated %d:%02d", hour, min);
        } else {
            int hr = hour % 12;
            if (hr == 0)
                hr = 12;
            snprintf(update_time_buf, sizeof(update_time_buf), "updated %d:%02d", hr, min);
        }
    }
    
    // Update hands
    layer_mark_dirty(text_layer_get_layer(text_layer));
    layer_mark_dirty(hands_layer);
}

static bool canVibe()
{
    if (!fdnd) return true;
    
    time_t temp = time(NULL);
    struct tm *now = localtime(&temp);
    
    if (dndEnd >= dndStart)
    {
        return now->tm_hour < dndStart || now->tm_hour > dndEnd;
    }
    else
    {
        return now->tm_hour < dndStart && now->tm_hour > dndEnd;
    }
}

static AppTimer *timer;

static void timer_callback(void *data) {
    if (s_forecast_window) {
        window_stack_pop(true);
    }
}

#if 0
static char loTempBuffer[MAX_FORECAST][8] = {"", "", "", ""};
static char hiTempBuffer[MAX_FORECAST][8] = {"", "", "", ""};
static char weekayBuffer[MAX_FORECAST][8] = {"", "", "", ""};
#endif

static void forecast_update_proc(Layer *layer, GContext *ctx) {
    GRect bounds = layer_get_bounds(layer);
    graphics_context_set_fill_color(ctx, bgColor);
    graphics_fill_rect(ctx, bounds, 0, GCornerNone);
    
#ifdef PBL_PLATFORM_CHALK
    const int topPad = 8;
    const int btmPad = 40;
    const int edgePad = 15;
#else
    const int edgePad = 2;
    const int btmPad = 34;
    const int topPad = -2;
#endif
    const int wPad = 2;
    const int w = (bounds.size.w - wPad * 3 - edgePad*2) / MAX_FORECAST;
    
    graphics_context_set_text_color(ctx, fgColor);
    
    GRect cityR = GRect(0, topPad, bounds.size.w, 28);
    graphics_draw_text(ctx, city_buffer, fonts_get_system_font(FONT_KEY_GOTHIC_24), cityR, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);

    graphics_draw_text(ctx, local_timezone_buf, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK), GRect(0, bounds.size.h - btmPad, bounds.size.w, 30), GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    
    graphics_draw_text(ctx, update_time_buf, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0, bounds.size.h - btmPad - 12, bounds.size.w, 16), GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    
    for (int i = 0; i < MAX_FORECAST; i++)
    {
        time_t temp = time(NULL) + i * 60 * 60 * 24;
        struct tm *tick_time = localtime(&temp);
        static char tempBuffer[] = "012345678";

        
        GRect weekdayR = GRect(edgePad + (i * (w + wPad)), topPad + cityR.size.h, w, 28);
        snprintf(tempBuffer, sizeof(tempBuffer), "%s", WEEKDAY_STR(tick_time->tm_wday, 0));
        graphics_draw_text(ctx, tempBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD), weekdayR, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
        
        GRect condR = GRect(weekdayR.origin.x, weekdayR.origin.y + weekdayR.size.h, w, 28);
        graphics_draw_text(ctx, forecastIcon[i], fontMeteo28, condR, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
        
        
        float tempC = forecastHi[i];
        if (tempFormat == TEMP_FORMAT_CELCIUS)
        {
            snprintf(tempBuffer, sizeof(tempBuffer), "%d", (int)tempC);
        }
        else
        {
            snprintf(tempBuffer, sizeof(tempBuffer), "%d", (int)C2F(tempC));
        }
        
        GRect hiR = GRect(condR.origin.x, condR.origin.y + condR.size.h - 4, w, 28);
        graphics_draw_text(ctx, tempBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_24), hiR, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
        
        tempC = forecastLo[i];
        if (tempFormat == TEMP_FORMAT_CELCIUS)
        {
            snprintf(tempBuffer, sizeof(tempBuffer), "%d", (int)tempC);
        }
        else
        {
            snprintf(tempBuffer, sizeof(tempBuffer), "%d", (int)C2F(tempC));
        }
        
        GRect loR = hiR;
        loR.origin.y += hiR.size.h - 4;
        graphics_draw_text(ctx, tempBuffer, fonts_get_system_font(FONT_KEY_GOTHIC_18), loR, GTextOverflowModeWordWrap, GTextAlignmentCenter, 0);
    }
}

static void forecast_window_load(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    layer_set_update_proc(window_layer, forecast_update_proc);
    timer = app_timer_register(7000 /* milliseconds */, timer_callback, NULL);
}

static int tapCnt = 0;

static void forecast_window_unload(Window *window)
{
    tapCnt = 0;
    window_destroy(s_forecast_window);
    s_forecast_window = NULL;
}

char getOpenWeatherIcon(char *cstring)
{
    if (cstring[0] == '0')
    {
        if (cstring[1] == '1')
        {
            if (cstring[2] == 'd')
            {
                return 'B';
            }
            else
            {
                return 'C';
            }
        }
        else if (cstring[1] == '2')
        {
            return 'H';
        }
        else if (cstring[1] == '3')
        {
            return 'N';
        }
        else if (cstring[1] == '4')
        {
            return 'Y';
        }
        else if (cstring[1] == '9')
        {
            return 'Q';
        }
    }
    else if (cstring[0] == '1')
    {
        if (cstring[1] == '0')
        {
            return 'R';
        }
        else if (cstring[1] == '1')
        {
            return 'Z';
        }
        else if (cstring[1] == '3')
        {
            return 'U';
        }
    }
    else if (cstring[0] == '5')
    {
        if (cstring[1] == '0')
        {
            return 'M';
        }
    }
    
    return ')';
}

char getWeatherUndergroundIcon(char *cstring) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "getWeatherUndergroundIcon(%s)", cstring);
    bool nightTime = false;
    if (cstring[0] == 'n' && cstring[1] == 't' && cstring[2] == '_') {
        nightTime = true;
        cstring = &cstring[3];
    }
    
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "cstring=%s)", cstring);
    
    if (strcmp(cstring, "chanceflurries") == 0) {
        return 'V';
    } else if (strcmp(cstring, "chancerain") == 0) {
        return 'Q';
    } else if (strcmp(cstring, "chancesleet") == 0) {
        return 'X';
    } else if (strcmp(cstring, "chancesnow") == 0) {
        return 'V';
    } else if (strcmp(cstring, "chancetstorms") == 0) {
        return 'P';
    } else if (strcmp(cstring, "clear") == 0 || strcmp(cstring, "sunny") == 0) {
        if (nightTime) {
            return 'C';
        }
        return 'B';
    } else if (strcmp(cstring, "cloudy") == 0) {
        return 'N';
    } else if (strcmp(cstring, "flurries") == 0) {
        return 'U';
    } else if (strcmp(cstring, "hazy") == 0 || strcmp(cstring, "fog") == 0) {
        return 'M';
    } else if (strcmp(cstring, "mostlycloudy") == 0 || strcmp(cstring, "mostlysunny") == 0 ||
             strcmp(cstring, "partlycloudy") == 0 || strcmp(cstring, "partlysunny") == 0) {
        if (nightTime) {
            return 'I';
        }
        return 'H';
    } else if (strcmp(cstring, "rain") == 0) {
        return 'Q';
    } else if (strcmp(cstring, "sleet") == 0) {
        return 'U';
    } else if (strcmp(cstring, "snow") == 0) {
        return 'U';
    } else if (strcmp(cstring, "tstorms") == 0) {
        return 'Q';
    } else {
        return ')';
    }
}

char getIcon(char *cstring) {
    if (s_weather_source == WEATHER_SOURCE_OPEN_WEATHER) {
        return getOpenWeatherIcon(cstring);
    } else {
        return getWeatherUndergroundIcon(cstring);
    }
}

static void update_weather_text() {
    time_t now = time(NULL);
    time_t delta = MAX(0, now - lastSuccessfulWeatherUpdateTime);

    if (delta >= WEATHER_UPDATE_INTERVAL * 2) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "update_weather_text; deltaSuccessful:%d", (int)delta);
        // invalid data
        snprintf(temperature_buffer, sizeof(temperature_buffer), "*");
        snprintf(conditions_buffer, sizeof(conditions_buffer), " ");
        s_iconStr[0] = getIcon("");
    } else {
        if (persist_exists(KEY_TEMPERATURE)) {
            tempCelcius = persist_read_int(KEY_TEMPERATURE) / 100.;
            persist_read_string(KEY_CONDITIONS, conditions_buffer, sizeof(conditions_buffer));
            persist_read_string(KEY_COND_ICON, s_iconStr, sizeof(s_iconStr));
        }
        if (tempFormat == TEMP_FORMAT_CELCIUS) {
            snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)tempCelcius);
        } else {
            snprintf(temperature_buffer, sizeof(temperature_buffer), "%d°", (int)C2F(tempCelcius));
        }
    }
    
    layer_mark_dirty(text_layer_get_layer(text_layer));
}

static void weather_request_if_needed_delayed();

static int sendCnt = 0;
static int sentCnt = 0;

static void weather_request() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "weather_request; retry:%d", weather_request_retry_cnt);
    weather_request_started = true;
    
    DictionaryIterator *iter;
    AppMessageResult result = app_message_outbox_begin(&iter);
    if (result == APP_MSG_OK && iter) {
        dict_write_uint8(iter, KEY_REQUEST, REQUEST_WEATHER);
        dict_write_uint8(iter, KEY_WEATHER_SOURCE, s_weather_source);
        result = app_message_outbox_send();
        if (result != APP_MSG_OK) {
#ifdef DIAGNOSTICS
            APP_LOG(APP_LOG_LEVEL_ERROR, "weather_request app_message_outbox_send failed. %s", translate_error(result));
            s_error = ERROR_APPMSG_OUTBOX_SEND;
            s_appMsgResult = result;
#endif
        } else {
            sendCnt++;
        }
    } else {
#ifdef DIAGNOSTICS
        APP_LOG(APP_LOG_LEVEL_ERROR, "weather_request app_message_outbox_begin failed. %s", translate_error(result));
        s_error = ERROR_APPMSG_OUTBOX_BEGIN;
        s_appMsgResult = result;
#endif
    }

    weather_request_timer = app_timer_register(MIN(10000 * (weather_request_retry_cnt + 1) * (weather_request_retry_cnt + 1), (WEATHER_UPDATE_INTERVAL * 1000) / 3), weather_timer_callback, NULL);
}

static void weather_request_if_needed() {
    int delta = MAX(0, time(NULL) - lastWeatherUpdateTime);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "weather_request_if_needed f:%d delta:%d ntrvl:%d started:%d bt:%d", fweatherfetch, delta, WEATHER_UPDATE_INTERVAL, weather_request_started, bluetooth_connection_service_peek());
    
    if (fweatherfetch && delta >= WEATHER_UPDATE_INTERVAL && bluetooth_connection_service_peek() && !weather_request_started) {
#ifdef DIAGNOSTICS
        s_error = ERROR_NONE;
        snprintf(s_error_msg_buffer, sizeof(s_error_msg_buffer), " ");
#endif
        weather_request_retry_cnt = 0;
        weather_request();
    }
}

static void weather_request_callback(void *data) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "weather_request_callback");
    weather_request_if_needed();
}

static void weather_request_if_needed_delayed() {
    app_timer_register(10000, weather_request_callback, NULL);
}

static void weather_timer_callback(void *data) {
    if (weather_request_started)
    {
        if (!bluetooth_connection_service_peek()) {
#ifdef DIAGNOSTICS
            s_error = ERROR_NONE;
            snprintf(s_error_msg_buffer, sizeof(s_error_msg_buffer), " ");
#endif
            weather_request_retry_cnt = 0;
            weather_request_started = false;
        } else {
#ifdef DIAGNOSTICS
            if (s_error == ERROR_NONE) {
                APP_LOG(APP_LOG_LEVEL_DEBUG, "weather_timer_callback weather retry %d ERROR_TIMEOUT", weather_request_retry_cnt);
                s_error = ERROR_TIMEOUT;
            }
#endif
            APP_LOG(APP_LOG_LEVEL_DEBUG, "weather_timer_callback weather retry %d", weather_request_retry_cnt);
            weather_request_retry_cnt++;
            weather_request();
        }
    }
}

static void handle_bluetooth(bool connected)
{
    if (connected == fBtConnected) {
        // nothing changed
        return;
    }
    
    if (connected) {
        // switched to connected
        weather_request_if_needed_delayed();
    } else {
        // switched to not connected
        if (fBtAlert && canVibe()) vibes_short_pulse();
    }

    fBtConnected = connected;
    layer_mark_dirty(text_layer_get_layer(text_layer));
}

static time_t tapSec = 0;

static void handle_tap(AccelAxisType axis, int32_t direction) {
    
    if (s_forecast_window || !fweatherfetch) {
        return;
    }

    switch (s_shake) {
        case 0:
            // Off; nothing to do
            tapCnt = 0;
            break;
        case 1: {
            tapCnt++;
            APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap(%d) tapCnt %d", s_shake, tapCnt);
            
            if (tapCnt == 1) {
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap tapCnt %d, tapSec delta %d", tapCnt, (int)(now - tapSec));
                s_forecast_window = window_create();
                if (s_forecast_window) {
                    window_set_window_handlers(s_forecast_window, (WindowHandlers) {
                        .load = forecast_window_load,
                        .unload = forecast_window_unload,
                    });
                    window_stack_push(s_forecast_window, true);
                } else {
                    tapCnt = 0;
                    APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap s_forecast_window == nil");
                }
            }
            break;
        }
        case 2: {
            tapCnt++;
            time_t now = time(NULL);
            APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap(%d) tapCnt %d", s_shake, tapCnt);
            
            if (tapCnt == 1) {
                tapSec = now;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap tapCnt %d, tapSec %d", tapCnt, (int)tapSec);
            } else if (tapCnt == 2) {
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap tapCnt %d, tapSec delta %d", tapCnt, (int)(now - tapSec));
                if (now - tapSec < 4) {
                    s_forecast_window = window_create();
                    if (s_forecast_window) {
                        window_set_window_handlers(s_forecast_window, (WindowHandlers) {
                            .load = forecast_window_load,
                            .unload = forecast_window_unload,
                        });
                        window_stack_push(s_forecast_window, true);
                    } else {
                        tapCnt = 0;
                        //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap s_forecast_window == nil");
                    }
                } else {
                    tapCnt = 0;
                    //APP_LOG(APP_LOG_LEVEL_DEBUG, "handle_tap tapCnt %d time out", tapCnt);
                }
            }
            break;
        }
        default:
            tapCnt = 0;
            break;
    }
}

static int tick_count = 0;
static int tick_interval = 60;

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "tic %2d:%02d:%02d delta:%d ntrvl:%d free:%d used:%d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec, (int)(time(NULL) - lastWeatherUpdateTime), WEATHER_UPDATE_INTERVAL, heap_bytes_free(), heap_bytes_used());

    tick_count++;
    if ((tick_count % tick_interval) != 0) {
        return;
    }
    tick_count = 0;

    update_time();
    
    if (fHourAlert && tick_time->tm_min == 0 && tick_time->tm_sec == 0) {
        if (canVibe()) vibes_short_pulse();
    }
    
#ifdef DIAGNOSTICS
    if (s_error != ERROR_NONE) {
        snprintf(s_error_msg_buffer, sizeof(s_error_msg_buffer), "%s %d %d * %s %d %d",
             bluetooth_connection_service_peek() ? "" : "nbt",
             weather_request_retry_cnt,
             sendCnt - sentCnt,
             s_error == ERROR_NONE ? "" :
             s_error == ERROR_GET_LOCATION ? "gl" :
             s_error == ERROR_GET_WEATHER ? "gw" :
             s_error == ERROR_GET_WEATHER_FORECAST ? "gwf" :
             s_error == ERROR_TIMEOUT ? "to" :
             s_error >= ERROR_APPMSG_OPEN && s_error <= ERROR_APPMSG_OUTBOX_SEND ? "am" : "?",
             s_error >= ERROR_APPMSG_OPEN && s_error <= ERROR_APPMSG_OUTBOX_SEND ? s_appMsgResult : 0,
             s_error
             );
        APP_LOG(APP_LOG_LEVEL_DEBUG, "tic error:%s", s_error_msg_buffer);
    } else {
        snprintf(s_error_msg_buffer, sizeof(s_error_msg_buffer), " ");
    }
#endif
    weather_request_if_needed();
}

static void setupTimer() {
    if (fseconds) {
        tick_interval = 1;
//    } else if (digitime == OPTION_HEART_RATE) {
//        tick_interval = 5;
    } else {
        tick_interval = 60;
    }
}

static void handle_battery(BatteryChargeState charge_state)
{
    if (!charge_state.is_charging && charge_state.charge_percent <= BATTERY_LOW && batteryPct > BATTERY_LOW) {
        if (fBatteryAlert && canVibe()) vibes_short_pulse();
    }
    
    batteryPct = charge_state.charge_percent;
    layer_mark_dirty(text_layer_get_layer(text_layer));
}

static void main_window_load(Window *window) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_load() free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);
    const GPoint center = grect_center_point(&bounds);
    
    fontMeteo28 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_METEO_28));
    fontFoundation18 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FOUNDATION_20));
    
    s_bt_icon_rect = GRect(2, 13, 20, 20);
    s_battery_icon_rect = GRect(bounds.size.w - 23, 16, 20, 12);
    s_time_rect = GRect(0, center.y + 22, bounds.size.w, 32);
    s_weather_conditions_rect = GRect(center.x - 45, center.y - 42, 90, 30);
    s_weather_conditions_icon_rect = GRect(center.x - 30/2, 2, 30, 30);
    s_weather_temp_rect = GRect(center.x - 40/2, bounds.size.h - 30 - 2, 40, 30);
    s_weekday_rect = GRect(2, center.y - 30/2 - 2, 35, 30);
    s_date_rect = GRect(bounds.size.w - 28 - 2, center.y - 30/2 - 2, 28, 30);
    
    text_layer = text_layer_create(bounds);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_layer));

    setupHandsId(center);
    hands_layer = layer_create(bounds);
    layer_add_child(window_layer, hands_layer);
    
#ifdef DIAGNOSTICS
    s_error_msg_rect = GRect(0, center.y - 28, bounds.size.w, 60);
    snprintf(s_error_msg_buffer, sizeof(s_error_msg_buffer), " ");
    s_error_text_layer = text_layer_create(s_error_msg_rect);
    text_layer_set_text_color(s_error_text_layer, fgColor);
    text_layer_set_background_color(s_error_text_layer, GColorClear);
    text_layer_set_font(s_error_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    text_layer_set_text_alignment(s_error_text_layer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(s_error_text_layer, GTextOverflowModeWordWrap);
    layer_set_hidden(text_layer_get_layer(s_error_text_layer), !fDebug);
    text_layer_set_text(s_error_text_layer, s_error_msg_buffer);
    layer_add_child(window_layer, text_layer_get_layer(s_error_text_layer));
#endif
    setupColors();
    
    layer_set_update_proc(window_layer, face_update_proc);
    layer_set_update_proc(text_layer_get_layer(text_layer), face_update_text_proc);
    layer_set_update_proc(hands_layer, hands_update_proc);
    
    handle_bluetooth(bluetooth_connection_service_peek());
    bluetooth_connection_service_subscribe(&handle_bluetooth);
    
    handle_battery(battery_state_service_peek());
    battery_state_service_subscribe(&handle_battery);
    
    accel_tap_service_subscribe(handle_tap);
    
    update_time();
    update_weather_text();
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "load END free:%d", (int) heap_bytes_free());
}

static void main_window_unload(Window *window) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload() free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
#ifdef PBL_COLOR
    gbitmap_destroy(s_offscreen_bitmap);
#endif
#ifdef DIAGNOSTICS
    text_layer_destroy(s_error_text_layer);
#endif
    text_layer_destroy(text_layer);
    layer_destroy(hands_layer);
    gpath_destroy(minute_arrow);
    gpath_destroy(s_hour_mark_ptr);
    gpath_destroy(s_hour_mark2_ptr);
    fonts_unload_custom_font(fontMeteo28);
    fonts_unload_custom_font(fontFoundation18);
    gpath_destroy(hour_arrow);
    window_destroy(s_main_window);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "main_window_unload() END free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "inbox_received_callback");
    // Store incoming information
    
    bool isConfigMsg = false;
    bool fRequestWeather = false;
    
    Layer *window_layer = window_get_root_layer(s_main_window);
    GRect bounds = layer_get_bounds(window_layer);
    const GPoint center = grect_center_point(&bounds);
    
    // Read first item
    Tuple *t = dict_read_first(iterator);
    
    // For all items
    while(t != NULL) {
        // Which key was received?
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "key: %d", (int)t->key);
        switch(t->key) {
#ifdef DIAGNOSTICS
            case KEY_ERROR:
                s_error = t->value->int32;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "error: %d", s_error);
                break;
#endif
            case KEY_HOUR_ALERT:
                fHourAlert = atoi(t->value->cstring);
                persist_write_int(t->key, fHourAlert);
                break;
            case KEY_BATTERY_ALERT:
                fBatteryAlert = atoi(t->value->cstring);
                persist_write_int(t->key, fBatteryAlert);
                break;
            case KEY_BT_ALERT:
                fBtAlert = atoi(t->value->cstring);
                persist_write_int(t->key, fBtAlert);
                break;
            case KEY_DND:
                fdnd = atoi(t->value->cstring);
                persist_write_int(t->key, fdnd);
                break;
            case KEY_DND_START:
                dndStart = atoi(t->value->cstring);
                persist_write_int(t->key, dndStart);
                break;
            case KEY_DND_END:
                dndEnd = atoi(t->value->cstring);
                persist_write_int(t->key, dndEnd);
                break;
            case KEY_WEATHER_SOURCE: {
                int temp = atoi(t->value->cstring);
                if (temp != s_weather_source) {
                    s_weather_source = temp;
                    persist_write_int(t->key, s_weather_source);
                    fRequestWeather = true;
                }
                break;
            }
            case KEY_PREMIUM_KEY_VALID:
                s_premium_key_valid = t->value->int32;
                APP_LOG(APP_LOG_LEVEL_DEBUG, "KEY_PREMIUM_KEY_VALID %d", (int)t->value->int32);
                persist_write_int(t->key, s_premium_key_valid);
                break;
            case KEY_PREMIUM_KEY:
                snprintf(s_premium_key, sizeof(s_premium_key), "%s", t->value->cstring);
                persist_write_string(KEY_PREMIUM_KEY, s_premium_key);
                break;
            case KEY_WEATHER_LOCATION:
                if (strcmp(t->value->cstring, s_weather_location) != 0) {
                    snprintf(s_weather_location, sizeof(s_weather_location), "%s", t->value->cstring);
                    persist_write_string(KEY_PREMIUM_KEY, s_weather_location);
                    fRequestWeather = true;
                }
                break;
            case KEY_CITY:
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "city %s", t->value->cstring);
                snprintf(city_buffer, sizeof(city_buffer), "%s", t->value->cstring);
                persist_write_string(t->key, city_buffer);
                break;
            case KEY_FORECAST_DAY0_TEMP_MIN:
                weather_request_started = false;
                lastSuccessfulWeatherUpdateTime = lastWeatherUpdateTime = time(NULL);
                persist_write_int(KEY_LAST_WEATHER_UPDATE_TIME, lastWeatherUpdateTime);
                persist_write_int(KEY_LAST_SUCCESSFUL_WEATHER_UPDATE_TIME, lastSuccessfulWeatherUpdateTime);
#ifdef DIAGNOSTICS
                s_error = ERROR_NONE;
                snprintf(s_error_msg_buffer, sizeof(s_error_msg_buffer), " ");
#endif
                weather_request_retry_cnt = 0;
                update_weather_text();
                forecastLo[0] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 0 lo %d", (int)forecastLo[0]);
                persist_write_int(t->key, forecastLo[0] * 100);
                break;
            case KEY_FORECAST_DAY0_TEMP_MAX:
                forecastHi[0] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 0 hi %d", (int)forecastHi[0]);
                persist_write_int(t->key, forecastHi[0] * 100);
                break;
            case KEY_FORECAST_DAY0_ICON:
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "icon %s", t->value->cstring);
                forecastIcon[0][0] = getIcon(t->value->cstring);
                persist_write_string(t->key, forecastIcon[0]);
                break;
            case KEY_FORECAST_DAY1_TEMP_MIN:
                forecastLo[1] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 1 lo %d", (int)forecastLo[1]);
                persist_write_int(t->key, forecastLo[1] * 100);
                break;
            case KEY_FORECAST_DAY1_TEMP_MAX:
                forecastHi[1] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 1 hi %d", (int)forecastHi[1]);
                persist_write_int(t->key, forecastHi[1] * 100);
                break;
            case KEY_FORECAST_DAY1_ICON:
                forecastIcon[1][0] = getIcon(t->value->cstring);
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 1 icon %s", forecastIcon[3]);
                persist_write_string(t->key, forecastIcon[1]);
                break;
            case KEY_FORECAST_DAY2_TEMP_MIN:
                forecastLo[2] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 2 lo %d", (int)forecastLo[2]);
                persist_write_int(t->key, forecastLo[2] * 100);
                break;
            case KEY_FORECAST_DAY2_TEMP_MAX:
                forecastHi[2] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 2 hi %d", (int)forecastHi[2]);
                persist_write_int(t->key, forecastHi[2] * 100);
                break;
            case KEY_FORECAST_DAY2_ICON:
                forecastIcon[2][0] = getIcon(t->value->cstring);
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 2 icon %s", forecastIcon[3]);
                persist_write_string(t->key, forecastIcon[2]);
                break;
            case KEY_FORECAST_DAY3_TEMP_MIN:
                forecastLo[3] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 3 lo %d", (int)forecastLo[3]);
                persist_write_int(t->key, forecastLo[3] * 100);
                break;
            case KEY_FORECAST_DAY3_TEMP_MAX:
                forecastHi[3] = (float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 3 hi %d", (int)forecastHi[3]);
                persist_write_int(t->key, forecastHi[3] * 100);
                break;
            case KEY_FORECAST_DAY3_ICON:
                forecastIcon[3][0] = getIcon(t->value->cstring);
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "day 3 icon %s", forecastIcon[3]);
                persist_write_string(t->key, forecastIcon[3]);
                break;
            case KEY_TEMPERATURE:
                tempCelcius =(float)t->value->int32 / 100.;
                //APP_LOG(APP_LOG_LEVEL_DEBUG, "temp %s", temperature_buffer);
                //text_layer_set_text(s_weather_temp_layer, temperature_buffer);
                persist_write_int(t->key, tempCelcius * 100);
                break;
            case KEY_CONDITIONS:
                snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", t->value->cstring);
                //text_layer_set_text(s_weather_conditions_layer, conditions_buffer);
                persist_write_string(t->key, conditions_buffer);
                break;
            case KEY_COND_ICON:
                s_iconStr[0] = getIcon(t->value->cstring);
                //text_layer_set_text(s_weather_conditions_icon_layer, s_iconStr);
                persist_write_string(t->key, s_iconStr);
                break;
            case KEY_LANG:
                if (updateLang(t->value->cstring)) {
                    fRequestWeather = true;
                }
                break;
#ifndef PBL_PLATFORM_APLITE
            case KEY_TIMEZONE:
                s_timezone = atoi(t->value->cstring);
                persist_write_int(t->key, s_timezone);
                update_time();
                break;
#endif
            case KEY_TEMP_FORMAT:
                isConfigMsg = true;
                
                if (strcmp(t->value->cstring, "C") == 0) {
                    tempFormat = TEMP_FORMAT_CELCIUS;
                } else {
                    tempFormat = TEMP_FORMAT_FAHRENHEIT;
                }
                
                persist_write_int(t->key, tempFormat);
                update_weather_text();
                break;
            case KEY_FACE_ID:
                faceId = atoi(t->value->cstring);
                persist_write_int(t->key, faceId);
                setupFaceId();
                layer_mark_dirty(window_get_root_layer(s_main_window));
                break;
            case KEY_HANDS:
                handsId = atoi(t->value->cstring) - 100;
                setupHandsId(center);
                layer_mark_dirty(hands_layer);
                persist_write_int(t->key, handsId);
                break;
#ifdef PBL_COLOR
            case KEY_FACE_COLOR:
                s_face_color_index = atoi(t->value->cstring) - 200;
                persist_write_int(t->key, s_face_color_index);
                setupColors();
                break;
            case KEY_FACE_BG:
                s_face_bg_index = atoi(t->value->cstring) - 400;
                persist_write_int(t->key, s_face_bg_index);
                layer_mark_dirty(window_get_root_layer(s_main_window));
                gbitmap_destroy(s_offscreen_bitmap);
                s_offscreen_bitmap = 0;
                break;
            case KEY_HAND_COLOR:
                s_hand_color_index = atoi(t->value->cstring) - 300;
                persist_write_int(t->key, s_hand_color_index);
                setupColors();
                layer_mark_dirty(hands_layer);
                break;
#endif
            case KEY_DIGITIME:
                digitime = atoi(t->value->cstring);
                setupTimer();
                persist_write_int(t->key, digitime);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_INVERT:
                fInvert = atoi(t->value->cstring);
                persist_write_int(t->key, fInvert);
                setupColors();
                break;
            case KEY_FILLHANDS:
                fFillHands = atoi(t->value->cstring);
                persist_write_int(t->key, fFillHands);
                layer_mark_dirty(hands_layer);
                break;
            case KEY_SHAKE:
                s_shake = atoi(t->value->cstring);
                persist_write_int(t->key, s_shake);
                break;
            case KEY_LARGE_FONTS:
                fLargeFonts = atoi(t->value->cstring);
                persist_write_int(t->key, fLargeFonts);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
#ifdef DIAGNOSTICS
            case KEY_DEBUG:
                fDebug = atoi(t->value->cstring);
                persist_write_int(t->key, fDebug);
                layer_set_hidden(text_layer_get_layer(s_error_text_layer), !fDebug);
                break;
#endif
            case KEY_WEEKDAY:
                fweekday = atoi(t->value->cstring);
                persist_write_int(t->key, fweekday);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_BT:
                fBt = atoi(t->value->cstring);
                persist_write_int(t->key, fBt);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_BATTERY:
                fBattery = atoi(t->value->cstring);
                persist_write_int(t->key, fBattery);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_SECONDS:
                fseconds = atoi(t->value->cstring);
                setupTimer();
                layer_mark_dirty(hands_layer);
                persist_write_int(t->key, fseconds);
                break;
            case KEY_WEATHERTEMP:
                fweathertemp = atoi(t->value->cstring);
                persist_write_int(t->key, fweathertemp);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_WEATHERCOND:
                fweathercond = atoi(t->value->cstring);
                persist_write_int(t->key, fweathercond);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_WEATHER_FETCH:
                fweatherfetch = atoi(t->value->cstring);
                persist_write_int(t->key, fweatherfetch);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
            case KEY_DATE:
                fdate = atoi(t->value->cstring);
                persist_write_int(t->key, fdate);
                layer_mark_dirty(text_layer_get_layer(text_layer));
                break;
                
            default:
                //APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
                break;
        }
        
        // Look for next item
        t = dict_read_next(iterator);
    }
    
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "isConfigMsg:%d s_premium_key_valid:%d s_premium_key:%s", isConfigMsg, s_premium_key_valid, s_premium_key);
    if (isConfigMsg && !s_premium_key_valid && strlen(s_premium_key) > 0) {
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "Looking up s_premium_key:%s", s_premium_key);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);
        if (iter) {
            dict_write_uint8(iter, KEY_REQUEST, REQUEST_PREMIUM_KEY_LOOKUP);
            dict_write_cstring(iter, KEY_PREMIUM_KEY, s_premium_key);
            app_message_outbox_send();
            sendCnt++;
        }
    }
    
    if (fRequestWeather) {
        weather_request();
    }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    //APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped. %s", translate_error(reason));
#ifdef DIAGNOSTICS
    s_error = ERROR_APPMSG_INBOX_DROPPED;
    s_appMsgResult = reason;
#endif
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    //APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed. %s", translate_error(reason));
#ifdef DIAGNOSTICS
    s_error = ERROR_APPMSG_OUTBOX_FAILED;
    s_appMsgResult = reason;
#endif
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "Outbox send success!");
    sentCnt++;
}

static void init() {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "init() free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
    fBtConnected = bluetooth_connection_service_peek();
    batteryPct = battery_state_service_peek().charge_percent;
    
    if (persist_exists(KEY_TEMP_FORMAT)) {
        tempFormat = persist_read_int(KEY_TEMP_FORMAT);
        faceId = persist_read_int(KEY_FACE_ID);
        handsId = persist_read_int(KEY_HANDS);
        digitime = persist_read_int(KEY_DIGITIME);
        fweekday = persist_read_int(KEY_WEEKDAY);
        fdate = persist_read_int(KEY_DATE);
        fBt = persist_read_int(KEY_BT);
        fBattery = persist_read_int(KEY_BATTERY);
        fseconds = persist_read_int(KEY_SECONDS);
        fweathertemp = persist_read_int(KEY_WEATHERTEMP);
        fweathercond = persist_read_int(KEY_WEATHERCOND);
    }
    if (persist_exists(KEY_WEATHER_FETCH)) {
        fweatherfetch = persist_read_int(KEY_WEATHER_FETCH);
    }
    if (persist_exists(KEY_CITY)) {
        persist_read_string(KEY_CITY, city_buffer, sizeof(city_buffer));
        lastWeatherUpdateTime = persist_read_int(KEY_LAST_WEATHER_UPDATE_TIME);
        if (lastWeatherUpdateTime > time(NULL)) {
            lastWeatherUpdateTime = 0;
        }
        lastSuccessfulWeatherUpdateTime = persist_read_int(KEY_LAST_SUCCESSFUL_WEATHER_UPDATE_TIME);
        if (lastSuccessfulWeatherUpdateTime > time(NULL)) {
            lastSuccessfulWeatherUpdateTime = 0;
        }
        forecastLo[0] = persist_read_int(KEY_FORECAST_DAY0_TEMP_MIN) / 100.;
        forecastHi[0] = persist_read_int(KEY_FORECAST_DAY0_TEMP_MAX) / 100.;
        persist_read_string(KEY_FORECAST_DAY0_ICON, forecastIcon[0], sizeof(forecastIcon[0]));
        forecastLo[1] = persist_read_int(KEY_FORECAST_DAY1_TEMP_MIN) / 100.;
        forecastHi[1] = persist_read_int(KEY_FORECAST_DAY1_TEMP_MAX) / 100.;
        persist_read_string(KEY_FORECAST_DAY1_ICON, forecastIcon[1], sizeof(forecastIcon[1]));
        forecastLo[2] = persist_read_int(KEY_FORECAST_DAY2_TEMP_MIN) / 100.;
        forecastHi[2] = persist_read_int(KEY_FORECAST_DAY2_TEMP_MAX) / 100.;
        persist_read_string(KEY_FORECAST_DAY2_ICON, forecastIcon[2], sizeof(forecastIcon[2]));
        forecastLo[3] = persist_read_int(KEY_FORECAST_DAY3_TEMP_MIN) / 100.;
        forecastHi[3] = persist_read_int(KEY_FORECAST_DAY3_TEMP_MAX) / 100.;
        persist_read_string(KEY_FORECAST_DAY3_ICON, forecastIcon[3], sizeof(forecastIcon[3]));
    }
    if (persist_exists(KEY_SHAKE)) {
        s_shake = persist_read_int(KEY_SHAKE);
#ifdef DIAGNOSTICS
        fDebug = persist_read_int(KEY_DEBUG);
#endif
    }
    if (persist_exists(KEY_LARGE_FONTS)) {
        fLargeFonts = persist_read_int(KEY_LARGE_FONTS);
    }
    if (persist_exists(KEY_TEMPERATURE)) {
        tempCelcius = persist_read_int(KEY_TEMPERATURE) / 100.;
        persist_read_string(KEY_CONDITIONS, conditions_buffer, sizeof(conditions_buffer));
        persist_read_string(KEY_COND_ICON, s_iconStr, sizeof(s_iconStr));
    }
    if (persist_exists(KEY_PREMIUM_KEY)) {
        persist_read_string(KEY_PREMIUM_KEY, s_premium_key, sizeof(s_premium_key));
        s_premium_key_valid = persist_read_int(KEY_PREMIUM_KEY_VALID);
        
        if (s_premium_key_valid) {
            if (s_premium_key_valid > 1) {
                // fix a bug
                s_premium_key_valid = 0;
                persist_write_int(KEY_PREMIUM_KEY_VALID, s_premium_key_valid);
            } else {
                s_weather_source = persist_read_int(KEY_WEATHER_SOURCE);
                persist_read_string(KEY_WEATHER_LOCATION, s_weather_location, sizeof(s_weather_location));
                fBtAlert = persist_read_int(KEY_BT_ALERT);
                fBatteryAlert = persist_read_int(KEY_BATTERY_ALERT);
                fHourAlert = persist_read_int(KEY_HOUR_ALERT);
                fdnd = persist_read_int(KEY_DND);
                dndStart = persist_read_int(KEY_DND_START);
                dndEnd = persist_read_int(KEY_DND_END);
                s_lang_index = persist_read_int(KEY_LANG);
                fInvert = persist_read_int(KEY_INVERT);
                if (persist_exists(KEY_FILLHANDS)) {
                    fFillHands = persist_read_int(KEY_FILLHANDS);
                }
#ifdef PBL_COLOR
                s_face_color_index = persist_read_int(KEY_FACE_COLOR);
                s_hand_color_index = persist_read_int(KEY_HAND_COLOR);
                s_face_bg_index = persist_read_int(KEY_FACE_BG);
#endif
#ifndef PBL_PLATFORM_APLITE
                if (persist_exists(KEY_TIMEZONE)) {
                    s_timezone = persist_read_int(KEY_TIMEZONE);
                }
#endif
            }
        }
    }
    
#if 0
    faceId = 0;
    handsId = 3;
    digitime = 1;
    fweekday = 1;
    fdate = 1;
    fBt = 1;
    fBattery = 1;
    fseconds = 1;
    fweathertemp = 1;
    fweathercond = 1;
    fInvert = 1;
    fLargeFonts = 1;
    s_face_bg_index = 14;
#endif
    
    setupFaceId();
    setupColors();
    
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_stack_push(s_main_window, true);
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "init() PUSH free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
    
    setupTimer();
    tick_timer_service_subscribe(SECOND_UNIT, handle_tick);
    
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    AppMessageResult result = app_message_open(300, 300);
    if (result != APP_MSG_OK) {
#ifdef DIAGNOSTICS
        APP_LOG(APP_LOG_LEVEL_ERROR, "init app_message_open failed. %s", translate_error(result));
        s_error = ERROR_APPMSG_OPEN;
        s_appMsgResult = result;
#endif
    }
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "init() END free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
}


static void deinit() {
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit() free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
    app_message_deregister_callbacks();
    //APP_LOG(APP_LOG_LEVEL_DEBUG, "deinit() END free:%d, used:%d", (int) heap_bytes_free(), heap_bytes_used());
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}
