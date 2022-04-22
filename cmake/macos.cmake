# Create a launchd user agent and install it
set(LAUNCHD_AGENT_FILE nd.${PROJECT_NAME}.Agent.plist)
configure_file("${config_base}/${LAUNCHD_AGENT_FILE}" "${LAUNCHD_AGENT_FILE}")
install(FILES "${CMAKE_BINARY_DIR}/${LAUNCHD_AGENT_FILE}" DESTINATION "$ENV{HOME}/Library/LaunchAgents")
# Load the user agent to start the server now instead of waiting for next reboot/login
install(CODE "message(\"EXECUTING: launchctl load $ENV{HOME}/Library/LaunchAgents/${LAUNCHD_AGENT_FILE}\")")
install(CODE "execute_process(COMMAND \"launchctl load $ENV{HOME}/Library/LaunchAgents/${LAUNCHD_AGENT_FILE}\")")

