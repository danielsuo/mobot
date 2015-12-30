#ifndef UTILITES_H
#define UTILITES_H

#include <stdlib.h>
#include <stdio.h>

// Manipulate file descriptors
#include <fcntl.h>

// Check system limits
#include <limits.h>

// Check if directory exists
#include <sys/stat.h>

#include <stddef.h>
#include <assert.h>

// Get character substrings
char *substr(char *arr, int begin, int len);

// Create file directories recursively
void mkdirp(char *dir, mode_t mode, bool is_dir);

void composeTransform(float *A, float *B, float *out);

class MovingAverage {
public:
  MovingAverage(unsigned int period) :
    period(period), window(new double[period]), head(NULL), tail(NULL),
    total(0), stale(true) {
    assert(period >= 1);
  }
  ~MovingAverage() {
    delete[] window;
  }

  // Adds a value to the average, pushing one out if nescessary
  void add(double val) {
    // Special case: Initialization
    if (head == NULL) {
      head = window;
      *head = val;
      tail = head;
      inc(tail);
      total = val;
      return;
    }

    // Were we already full?
    if (head == tail) {
      // Fix total-cache
      total -= *head;
      // Make room
      inc(head);
    }

    // Write the value in the next spot.
    *tail = val;
    inc(tail);

    // Update our total-cache
    total += val;

    stale = true;
  }

  // Returns the average of the last P elements added to this MovingAverage.
  // If no elements have been added yet, returns 0.0
  double avg() {
    ptrdiff_t size = this->size();
    if (size == 0) {
      return 0; // No entries => 0 average
    }

    if (stale) {
      stale = !stale;
      _avg = total / (double) size; // Cast to double for floating point arithmetic
    }

    return _avg;
  }

private:
  unsigned int period;
  double * window; // Holds the values to calculate the average of.

  // Logically, head is before tail
  double * head; // Points at the oldest element we've stored.
  double * tail; // Points at the newest element we've stored.

  double total; // Cache the total so we don't sum everything each time.

  bool stale; // Only recalculate average if we need to
  double _avg; // Cached average

  // Bumps the given pointer up by one.
  // Wraps to the start of the array if needed.
  void inc(double * & p) {
    if (++p >= window + period) {
      p = window;
    }
  }

  // Returns how many numbers we have stored.
  ptrdiff_t size() const {
    if (head == NULL)
      return 0;
    if (head == tail)
      return period;
    return (period + tail - head) % period;
  }
};

#endif