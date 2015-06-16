#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include "sine.h"

// start morse

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
  case 'a': memcpy(out, (Morse[10]){Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case 'b': memcpy(out, (Morse[10]){Dah, Dit, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case 'c': memcpy(out, (Morse[10]){Dah, Dit, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case 'd': memcpy(out, (Morse[10]){Dah, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case 'e': memcpy(out, (Morse[10]){Dit, End}, 10 * sizeof(Morse)); return;
  case 'f': memcpy(out, (Morse[10]){Dit, Dit, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case 'g': memcpy(out, (Morse[10]){Dah, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case 'h': memcpy(out, (Morse[10]){Dit, Dit, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case 'i': memcpy(out, (Morse[10]){Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case 'j': memcpy(out, (Morse[10]){Dit, Dah, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case 'k': memcpy(out, (Morse[10]){Dah, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case 'l': memcpy(out, (Morse[10]){Dit, Dah, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case 'm': memcpy(out, (Morse[10]){Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case 'n': memcpy(out, (Morse[10]){Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case 'o': memcpy(out, (Morse[10]){Dah, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case 'p': memcpy(out, (Morse[10]){Dit, Dah, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case 'q': memcpy(out, (Morse[10]){Dah, Dah, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case 'r': memcpy(out, (Morse[10]){Dit, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case 's': memcpy(out, (Morse[10]){Dit, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case 't': memcpy(out, (Morse[10]){Dah, End}, 10 * sizeof(Morse)); return;
  case 'u': memcpy(out, (Morse[10]){Dit, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case 'v': memcpy(out, (Morse[10]){Dit, Dit, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case 'w': memcpy(out, (Morse[10]){Dit, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case 'x': memcpy(out, (Morse[10]){Dah, Dit, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case 'y': memcpy(out, (Morse[10]){Dah, Dit, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case 'z': memcpy(out, (Morse[10]){Dah, Dah, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case '1': memcpy(out, (Morse[10]){Dit, Dah, Dah, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case '2': memcpy(out, (Morse[10]){Dit, Dit, Dah, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case '3': memcpy(out, (Morse[10]){Dit, Dit, Dit, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case '4': memcpy(out, (Morse[10]){Dit, Dit, Dit, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case '5': memcpy(out, (Morse[10]){Dit, Dit, Dit, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case '6': memcpy(out, (Morse[10]){Dah, Dit, Dit, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case '7': memcpy(out, (Morse[10]){Dah, Dah, Dit, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case '8': memcpy(out, (Morse[10]){Dah, Dah, Dah, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case '9': memcpy(out, (Morse[10]){Dah, Dah, Dah, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case '0': memcpy(out, (Morse[10]){Dah, Dah, Dah, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case '?': memcpy(out, (Morse[10]){Dit, Dit, Dah, Dah, Dit, Dit, End}, 10 * sizeof(Morse)); return;
  case '.': memcpy(out, (Morse[10]){Dit, Dah, Dit, Dah, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case ',': memcpy(out, (Morse[10]){Dah, Dah, Dit, Dit, Dah, Dah, End}, 10 * sizeof(Morse)); return;
  case '-': memcpy(out, (Morse[10]){Dah, Dit, Dit, Dit, Dit, Dah, End}, 10 * sizeof(Morse)); return;
  case '/': memcpy(out, (Morse[10]){Dah, Dit, Dit, Dah, Dit, End}, 10 * sizeof(Morse)); return;
  case ' ': memcpy(out, (Morse[10]){Space, End}, 10 * sizeof(Morse)); return;
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
