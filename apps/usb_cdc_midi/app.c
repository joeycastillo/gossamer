#include "app.h"
#include "usb.h"
#include "system.h"

static void cdc_task(void);
static void midi_task(void);

void app_init(void) {
    set_cpu_frequency(48000000);
}

void app_setup(void) {
    usb_init();
    usb_enable();
}

bool app_loop(void) {
    tud_task();
    cdc_task();
    midi_task();

    return false;
}

static void echo_serial_port(uint8_t itf, uint8_t buf[], uint32_t count) {
    for(uint32_t i=0; i<count; i++) {
        tud_cdc_n_write_char(itf, buf[i]);
    }
    tud_cdc_n_write_flush(itf);
}

// Variable that holds the current position in the sequence.
uint32_t note_pos = 0;

// Store example melody as an array of note values
uint8_t note_sequence[] = {
  74,78,81,86,90,93,98,102,57,61,66,69,73,78,81,85,88,92,97,100,97,92,88,85,81,78,
  74,69,66,62,57,62,66,69,74,78,81,86,90,93,97,102,97,93,90,85,81,78,73,68,64,61,
  56,61,64,68,74,78,81,86,90,93,98,102
};

static void play_next_note(void) {
    uint8_t const cable_num = 0; // MIDI jack associated with USB endpoint
    uint8_t const channel   = 0; // 0 for channel 1

    // Previous positions in the note sequence.
    int previous = (int) (note_pos - 1);

    // If we currently are at position 0, set the
    // previous position to the last note in the sequence.
    if (previous < 0) previous = sizeof(note_sequence) - 1;

    // Send Note On for current position at full velocity (127) on channel 1.
    uint8_t note_on[3] = { 0x90 | channel, note_sequence[note_pos], 127 };
    tud_midi_stream_write(cable_num, note_on, 3);

    // Send Note Off for previous note.
    uint8_t note_off[3] = { 0x80 | channel, note_sequence[previous], 0};
    tud_midi_stream_write(cable_num, note_off, 3);

    // Increment position
    note_pos++;

    // If we are at the end of the sequence, start over.
    if (note_pos >= sizeof(note_sequence)) note_pos = 0;
}

static void cdc_task(void) {
    if (tud_cdc_n_available(0)) {
        uint8_t buf[64];
        uint32_t count = tud_cdc_n_read(0, buf, sizeof(buf));

        // echo back what was typed
        echo_serial_port(0, buf, count);

        // play a note on the MIDI interface
        play_next_note();
    }
}

static void midi_task(void) {
  // The MIDI interface always creates input and output port/jack descriptors
  // regardless of these being used or not. Therefore incoming traffic should be read
  // (possibly just discarded) to avoid the sender blocking in IO
  uint8_t packet[4];
  while ( tud_midi_available() ) tud_midi_packet_read(packet);
}
