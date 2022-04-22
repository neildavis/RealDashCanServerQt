# Create a systemd service config file and install it
configure_file("${config_base}/${PROJECT_NAME}.service" "${PROJECT_NAME.service}")
install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.service" DESTINATION "$ENV{HOME}/.config/systemd/user")
