#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "sine.h"

// start morse

#define MORSE_COPY(...) memcpy(out,                                 \
                               (Morse[10]){__VA_ARGS__, End},       \
                               10 * sizeof(Morse));                 \
  return;

#include "tone.h"

// Morse settings
// volume is 1 - 8, 8 being loudest
inline uint8_t volume() { return 8; }
inline uint16_t tone_hz() { return 600; }
inline uint16_t wpm() { return 20; }
inline float dit_time() { return 1.2/wpm(); }
inline float dah_time() { return dit_time() * 3; }
inline float between_elements() { return dit_time(); }
inline float between_chars() { return dah_time(); }
inline float between_words() { return dit_time() * 7; }

// Ugly looking morse map.
void convert_morse(char letter, Morse out[10]) {
  switch (letter) {
  case 'a': MORSE_COPY(Dit, Dah)
  case 'b': MORSE_COPY(Dah, Dit, Dit, Dit)
  case 'c': MORSE_COPY(Dah, Dit, Dah, Dit)
  case 'd': MORSE_COPY(Dah, Dit, Dit)
  case 'e': MORSE_COPY(Dit)
  case 'f': MORSE_COPY(Dit, Dit, Dah, Dit)
  case 'g': MORSE_COPY(Dah, Dah, Dit)
  case 'h': MORSE_COPY(Dit, Dit, Dit, Dit)
  case 'i': MORSE_COPY(Dit, Dit)
  case 'j': MORSE_COPY(Dit, Dah, Dah, Dah)
  case 'k': MORSE_COPY(Dah, Dit, Dah)
  case 'l': MORSE_COPY(Dit, Dah, Dit, Dit)
  case 'm': MORSE_COPY(Dah, Dah)
  case 'n': MORSE_COPY(Dah, Dit)
  case 'o': MORSE_COPY(Dah, Dah, Dah)
  case 'p': MORSE_COPY(Dit, Dah, Dah, Dit)
  case 'q': MORSE_COPY(Dah, Dah, Dit, Dah)
  case 'r': MORSE_COPY(Dit, Dah, Dit)
  case 's': MORSE_COPY(Dit, Dit, Dit)
  case 't': MORSE_COPY(Dah)
  case 'u': MORSE_COPY(Dit, Dit, Dah)
  case 'v': MORSE_COPY(Dit, Dit, Dit, Dah)
  case 'w': MORSE_COPY(Dit, Dah, Dah)
  case 'x': MORSE_COPY(Dah, Dit, Dit, Dah)
  case 'y': MORSE_COPY(Dah, Dit, Dah, Dah)
  case 'z': MORSE_COPY(Dah, Dah, Dit, Dit)
  case '1': MORSE_COPY(Dit, Dah, Dah, Dah, Dah)
  case '2': MORSE_COPY(Dit, Dit, Dah, Dah, Dah)
  case '3': MORSE_COPY(Dit, Dit, Dit, Dah, Dah)
  case '4': MORSE_COPY(Dit, Dit, Dit, Dit, Dah)
  case '5': MORSE_COPY(Dit, Dit, Dit, Dit, Dit)
  case '6': MORSE_COPY(Dah, Dit, Dit, Dit, Dit)
  case '7': MORSE_COPY(Dah, Dah, Dit, Dit, Dit)
  case '8': MORSE_COPY(Dah, Dah, Dah, Dit, Dit)
  case '9': MORSE_COPY(Dah, Dah, Dah, Dah, Dit)
  case '0': MORSE_COPY(Dah, Dah, Dah, Dah, Dah)
  case '?': MORSE_COPY(Dit, Dit, Dah, Dah, Dit, Dit)
  case '.': MORSE_COPY(Dit, Dah, Dit, Dah, Dit, Dah)
  case ',': MORSE_COPY(Dah, Dah, Dit, Dit, Dah, Dah)
  case '-': MORSE_COPY(Dah, Dit, Dit, Dit, Dit, Dah)
  case '/': MORSE_COPY(Dah, Dit, Dit, Dah, Dit)
  case ' ': MORSE_COPY(Space)
  default: return;
  }
}

void dit() {
  make_tone(tone_hz(), dit_time());
  make_silence(between_elements());
}

void dah() {
  make_tone(tone_hz(), dah_time());
  make_silence(between_elements());
}

void word_space() {
  make_silence(between_words());
}

void char_space() {
  make_silence(between_chars());
}

void play_morse(Morse out[10]) {
  for (int i = 0; i < 10; i++) {
    switch (out[i]) {
    case Dit:
      dit();
      continue;
    case Dah:
      dah();
      continue;
    case Space:
      word_space();
      continue;
    case End: return;
    default: return;
    }
  }
}

void word(const char *word) {
  Morse converted[10];
  for (unsigned int i = 0; i < strlen(word); i++) {
    convert_morse(word[i], converted);
    play_morse(converted);
    char_space();
  }
}

// end morse




/* interrupt variables */
volatile uint16_t phase_acc = 0;
volatile uint16_t dphase = 0;
volatile uint32_t samples = 0;
volatile uint8_t go = 0;
/* end interrupt variables */

uint16_t phase_max = 65535;
uint16_t pwm_clock = 62500; /* if no prescale */

void make_tone(uint16_t freq, float seconds)
{
  /* wait for previous tone to stop */
  while(go) __asm__("NOP");
  samples = (uint32_t)(seconds*pwm_clock);
  dphase = (uint16_t)((phase_max*(float)freq)/pwm_clock);
  go = 1;
}

void make_silence(float seconds)
{
  /* wait for previous tone to stop */
  while(go) __asm__("NOP");
  samples = (uint32_t)(seconds*pwm_clock);
  phase_acc = 0;
  dphase = 0;
  go = 1;
}

int main(void)
{
  /* setup pins for output */
  DDRD |= _BV(3);
  DDRB |= _BV(5);
  PORTB = (0<<5);

  /* setup counter 2 for fast PWM output on pin 3 (arduino) */
  TCCR2A = _BV(COM2B1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20); /* clock, no prescale */
  /*TCCR2B = _BV(CS21);*/  /* clock/8 */
  TIMSK2 = _BV(TOIE2);
  sei();

  /* dphase = ((2^bit_resolution)*(frequency))/(PWM_clockrate) */
  /*float pwm_clock = 7812.5; */ /* if clock/8 */

  while(1) {
    word("de kd8zrc/r");
    make_silence(1);
  }

  return 0;
}

ISR(TIMER2_OVF_vect) {
  if(!go) return;
  cli();

  /* 16bit phase_acc will roll over on its own */
  phase_acc += dphase;

  /* only take most significant 8 bits to find sine value in table */
  OCR2B = (uint8_t)(sine_table[phase_acc>>8]>>(8-volume()));

  samples--;
  if(samples == 0) go = 0;
  sei();
}
