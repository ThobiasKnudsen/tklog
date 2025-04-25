logging 8 levels: debug, info, notice, warning, error, critical, alert, emergency.
You can enable them by defining TKLOG_[LEVEL] and enable exit on logging with TKLOG_EXIT_ON_[LEVEL].
For each logging message you can show the level by enabling TKLOG_SHOW_LOG_LEVEL, show time in ms since start with TKLOG_SHOW_TIME,
show thread with TKLOG_SHOW_THREAD and show the call path with TKLOG_SHOW_PATH. 
The path is constructed like this: fiel_a:31 → file_b:632 → file_c:31 etc. To show all file-line pairs except the last you have to 
enable TKLOG_SHOW_SCOPE and then use the macro tklog_Scope([code here]); to add current file-line pair to all log functions called within
tklog_Scope. You can also track all memory allocations that use malloc, realloc and free by overriding these by defining TKLOG_MEMORY
