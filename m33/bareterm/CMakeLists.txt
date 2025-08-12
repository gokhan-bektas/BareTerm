if(ZEPHYR_BASE)
  add_library(bareterm
    bareterm.c
    bareterm_backend_stdio.c
    bareterm_uart_input.c
    bareterm_input.c
    bareterm_widget.c
    bareterm_label.c
    bareterm_textbox.c
    bareterm_progressbar.c
    bareterm_checkbox.c
    bareterm_modalpopup.c
    bareterm_radiogroup.c
    bareterm_textinput.c
  )

# COMUI library includes
target_include_directories(bareterm
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}   # so consumers can include "mylib.h"
)

target_link_libraries(bareterm PUBLIC zephyr_interface)

endif()