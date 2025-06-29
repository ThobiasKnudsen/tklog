// tklog_undef.h - Helper file to clean up tklog macros between configurations
//
// Include this file before defining a new configuration for tklog.h
// to ensure all previous configuration macros are properly undefined.

// Undefine log levels
#undef TK_TRACE
#undef TK_DEBUG
#undef TK_INFO
#undef TK_NOTICE
#undef TK_WARNING
#undef TK_ERROR
#undef TK_CRITICAL
#undef TK_FATAL

// Undefine configuration options
#undef TK_LOG_LEVEL
#undef TK_LOG_EXIT_LEVEL
#undef TK_LOG_FN
#undef TK_SHOW_LEVEL
#undef TK_SHOW_TIME
#undef TK_SHOW_THREAD
#undef TK_SHOW_PATH
#undef TK_ENABLE_SCOPE

// Undefine implementation-specific macros
#undef TK_LOG_CONFIG
#undef _TK_XCAT
#undef _TK_CAT

// Undefine the logging macro itself
#undef TK_LOG