if(ZEPHYR_BASE)
  add_library(bareterm
    bareterm.c
    bareterm_backend_stdio.c
    bareterm_uart_input.c
    bareterm_input.c
    widget/bareterm_widget.c
    widget/bareterm_label.c
    widget/bareterm_textbox.c
    widget/bareterm_progressbar.c
    widget/bareterm_checkbox.c
    widget/bareterm_modalpopup.c
    widget/bareterm_radiogroup.c
    widget/bareterm_textinput.c
    widget/bareterm_listbox.c
  )

# COMUI library includes
target_include_directories(bareterm
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}   # so consumers can include "mylib.h"
)

target_link_libraries(bareterm PUBLIC zephyr_interface)

endif()