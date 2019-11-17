#include <Arduino.h>
#include "commandReader.h"
#include "constants.h"

char safeRead() {
  while (!Serial.available());

  return Serial.read();
}

String readCommand() {
  String output;
  char c;

  // Clear newlines in buffer
  do {
    c = safeRead();
  } while (c == '\r' || c == '\n');

  // Read line
  while (c && c != '\r' && c != '\n') {
    // Block comment, but only if the line is not already a comment
    if (c == '(' && output.charAt(0) != ';') {
      // Ignore block comment
      while ((c = safeRead()) != ')');
    } else {
      output += c;

      if (output.length() == MAX_COMMAND_LENGTH) {
        // @todo show warning that line is discarded
        output = ";"; 
      }
    }

    c = safeRead();
  }

  output.trim();

  if (output.charAt(0) == ';') {
    output = "";
  }

  if (output.length() == 0) {
    // Lets hope the compiler removes the recursion
    return readCommand();
  } 

  return output;
}