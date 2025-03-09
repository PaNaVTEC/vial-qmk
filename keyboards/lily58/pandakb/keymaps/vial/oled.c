#include <assets.h>
#include <sprites_goku.h>

// All of the animation logic is inspired from:
// https://github.com/HellSingCoder/qmk_firmware/blob/master/keyboards/sofle/keymaps/helltm/keymap.c

#define MIN_WALK_SPEED      10
#define MIN_RUN_SPEED       40
#define ANIM_FRAME_DURATION 80  // how long each frame lasts in ms

uint32_t anim_timer = 0;
uint8_t current_frame = 0;
bool showedJump = true;
bool showedVictory = true;

oled_rotation_t oled_init_user(oled_rotation_t rotation) { return rotation; }

static void render_goku(bool isSneaking, bool startedJump, bool startedVictory) {
  uint8_t current_wpm = get_current_wpm();

  void animate_goku(void) {
    bool isJumping = false;
    bool isShowingVictory = false;
    if (startedJump && showedJump) {
      current_frame = 0;
      isJumping = true;
      showedJump = false;
      showedVictory = true;
    } else if (startedVictory && showedVictory) {
      current_frame = 0;
      isShowingVictory = true;
      showedVictory = false;
      showedJump = true;
    } else {
      current_frame = (current_frame + 1) % 8;
      if (!showedJump) { isJumping = true; }
      if (!showedVictory) { isShowingVictory = true; }
    }

    if (isJumping) {
      oled_write_raw_P(jump[current_frame], ANIM_SIZE);
      if (current_frame == ANIM_FRAMES - 1) { showedJump = true; }
    } else if (isShowingVictory) {
      oled_write_raw_P(victory[current_frame], ANIM_SIZE);
      if (current_frame == ANIM_FRAMES - 1) {
        if (startedVictory) { current_frame = ANIM_FRAMES - 2; } else { showedVictory = true;}
      }
    } else if (current_wpm <= MIN_WALK_SPEED) {
      oled_write_raw_P(idle[current_frame], ANIM_SIZE);
    } else if (current_wpm <= MIN_RUN_SPEED) {
      oled_write_raw_P(walk[current_frame], ANIM_SIZE);
    } else {
      oled_write_raw_P(run[current_frame], ANIM_SIZE);
    }
  }
  #    if OLED_TIMEOUT > 0
    /* the animation prevents the normal timeout from occuring */
    if (last_input_activity_elapsed() > OLED_TIMEOUT && last_led_activity_elapsed() > OLED_TIMEOUT) {
      oled_off();
      return;
    } else {
      oled_on();
    }
#    endif

    /* animation timer */
    if (timer_elapsed32(anim_timer) > ANIM_FRAME_DURATION) {
      anim_timer = timer_read32();
      animate_goku();
    }
}

static void render_ctrl_shift(uint8_t const ctrl, uint8_t const shift) {
    oled_write_P(initial_filler,   false);

    // Top half with in between fillers
    oled_write_P(ctrl ? ctrl_on_1 : ctrl_off_1, false);
    if (ctrl && shift) oled_write_P(on_on_1,   false);
    else if (ctrl)     oled_write_P(on_off_1,  false);
    else if (shift)    oled_write_P(off_on_1,  false);
    else               oled_write_P(off_off_1, false);
    oled_write_P(shift ? shift_on_1 : shift_off_1, false);

    oled_write_P(later_filler,   false);
    oled_write_P(initial_filler,   false);

    // Bottom half with in between fillers
    oled_write_P(ctrl ? ctrl_on_2 : ctrl_off_2, false);
    if (ctrl && shift) oled_write_P(on_on_2,   false);
    else if (ctrl)     oled_write_P(on_off_2,  false);
    else if (shift)    oled_write_P(off_on_2,  false);
    else               oled_write_P(off_off_2, false);
    oled_write_P(shift ? shift_on_2 : shift_off_2, false);

    oled_write_P(later_filler,   false);
}

static void render_gui_alt(uint8_t const gui, uint8_t const alt) {
    // filler for 64x128 screen
    oled_write_P(initial_filler,   false);

    // Top half with in between fillers
    oled_write_P(gui ? gui_on_1 : gui_off_1, false);
    if (gui && alt) oled_write_P(on_on_1,   false);
    else if (gui)   oled_write_P(on_off_1,  false);
    else if (alt)   oled_write_P(off_on_1,  false);
    else            oled_write_P(off_off_1, false);
    oled_write_P(alt ? alt_on_1 : alt_off_1, false);

    // filler for 64x128 screen
    oled_write_P(later_filler,   false);
    oled_write_P(initial_filler,   false);

    // Bottom half with in between fillers
    oled_write_P(gui ? gui_on_2 : gui_off_2, false);
    if (gui && alt) oled_write_P(on_on_2,   false);
    else if (gui)   oled_write_P(on_off_2,  false);
    else if (alt)   oled_write_P(off_on_2,  false);
    else            oled_write_P(off_off_2, false);
    oled_write_P(alt ? alt_on_2 : alt_off_2, false);

    // filler for 64x128 screen
    oled_write_P(later_filler,   false);
}

static void render_layer_state(uint8_t const state) {
    if      (state == 1) oled_write_P(layer_1, false);
    else if (state == 2) oled_write_P(layer_2, false);
    else if (state == 3) oled_write_P(layer_3, false);
    else                 oled_write_P(layer_0, false);
}

static void render_wpm(void) {
  uint8_t n = get_current_wpm();
  char    wpm_str[4];
  wpm_str[3] = '\0';
  wpm_str[2] = '0' + n % 10;
  wpm_str[1] = '0' + (n /= 10) % 10;
  wpm_str[0] = '0' + n / 10;
  oled_write_P(wpm_str, false);
  oled_write_P(PSTR(" WPM"), false);
}

static void render_left(bool isSneaking, bool startedJump, bool startedVictory) {
  uint8_t const mods = get_mods();

  render_layer_state(get_highest_layer(layer_state));

  oled_set_cursor(0, 4);
  render_gui_alt(mods & MOD_MASK_GUI, mods & MOD_MASK_ALT);
  render_ctrl_shift(mods & MOD_MASK_CTRL, mods & MOD_MASK_SHIFT || host_keyboard_led_state().caps_lock);

  oled_set_cursor(2, 9);
  render_wpm();
  oled_set_cursor(0, 10);
  render_goku(isSneaking, startedJump, startedVictory);
}

static void render_logo(void) {
  oled_write_P(lily_logo,   false);
  oled_set_cursor(2, 4);
  oled_write_P(PSTR("Lily 57"), false);
}

static void render_name(void) {
  oled_set_cursor(1, 15);
  oled_write_P(PSTR("@panavtec"), false);
}

static void render_right(void) {
  render_logo();
  render_name();
}

static bool run_oled(bool isSneaking, bool startedJump, bool startedVictory) {
  if (is_keyboard_master()) {
    render_left(isSneaking, startedJump, startedVictory);
  } else {
     render_right();
  }
  return false;
}

