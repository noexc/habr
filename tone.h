typedef enum { Dah, Dit, Space, End } Morse;

void convert_morse(char letter, Morse out[10]);
void dit(void);
void dah(void);
void word_space(void);
void char_space(void);
void word(const char *);
void play_morse(Morse out[10]);
void make_tone(uint16_t, float);
void make_silence(float);