#ifdef OLED_ENABLE

oled_rotation_t oled_init_user(oled_rotation_t rotation) {
  //if (!is_keyboard_master()) return OLED_ROTATION_90;
  return rotation;
}

char *get_current_layer_name(void) {
  switch (get_highest_layer(default_layer_state)) {
    case 0: return "Qwert";
    case 1: return "Lower";
    case 2: return "Raise";
    case 3: return "Adj  ";
    default: return "Undef";
  }
}

static void render_left(const char* current_layer) {
  oled_write_ln_P(PSTR("Layer: "), false);

  switch (get_highest_layer(layer_state)) {
  case 0:
      oled_write_ln_P(PSTR("Default"), false);
      break;
  case 1:
      oled_write_ln_P(PSTR("Raise"), false);
      break;
  case 2:
      oled_write_ln_P(PSTR("Lower"), false);
      break;
  case 3:
      oled_write_ln_P(PSTR("Adjust"), false);
      break;
  default:
      oled_write_ln_P(PSTR("Undefined"), false);
  }
}

static void render_right(void) {
  oled_write_P(PSTR("WPM: "), false);
  oled_write(get_u8_str(get_current_wpm(), '0'), false);
}

bool oled_task_user(void) {
  if (is_keyboard_master()) {
    render_left(get_current_layer_name());
  } else {
    render_right();
  }
  return false;
}
#endif // OLED_ENABLE

