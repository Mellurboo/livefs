#pragma once

/*
    As it stands the config file system is very primitive, for example its basically typeless and
    theres no real comment support, i suppose its fast enough? but it only runs on start anyway,

    at some point I will add comment support so large comments are skipped at parsing :)
    when it comes to types we basically only have VALUE or INT because they're the only two really important ones
    everything can be represented as a VALUE (basically a string) or an INT

    I have no intention in adding boolean stuff because even livefs doesnt use booleans
*/
static const char DEFAULT_CONFIG[] =
    "# Generic Settings - Livefs Configuration File\n"
    "# Please take a moment to configure Livefs before you have too much fun\n"
    "# NOTE: We reccomend changing the default port\n"
    "port = 8080\n"
    "root = ~/livefs\n"
    "\n"
    "# Security Settings\n"
    "# Note: if you are not exposing livefs to the internet, you dont need to enable SSL\n"
    "#       other than that we reccomend using SSL where possible\n"
    "enable_ssl = 0\n"
    "ssl_cert_path = place_certificate_here\n"
    "ssl_key_path = place_key_here\n"
    "allow_insecure_HTTP_connections = 1\n"
    "\n"
    "# Danger Zone\n"
    "\n"
    "# If this is disabled, Livefs will not enforce descriptor file settings including\n"
    "# the hidden folder settings, if the descriptor IS there webpages will still be sent\n"
    "enforce_descriptor_files = 1\n"
    ;

/// @brief Creates .config/livefs file if it doesn't exsist
/// @return success?
int create_config_directory();

/// @brief creates the config.cfg file
/// @return success?
int create_config_file();