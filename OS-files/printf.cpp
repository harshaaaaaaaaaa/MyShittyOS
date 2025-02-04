extern "C" void printf(const char* str) {
    //minimal implementation
    while(*str) {
        // Replace with actual character output code
        // outb(COLOR_PORT, *str); 
        str++;
    }
}
