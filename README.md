StoveRemote
===========

Software for the Itead Studio Gboard1.0. I'm using it to remote trigger a wood pellet stove. My family's
cabin is uncomfortable to use during the winter since it is at ambient temperature when not in use. It takes
several hours to heat to comfort.

This software takes a number sequence from a text and matches it against the preloaded "passcode". If it
matches, it toggles a single pin (turn on stove) and texts back confirmation along with the current
temperature from a TMP36 sensor. It also allows the stove to be turned off via "STOP". The debug portion
needs your phone number to test one of the control commands.

This requires the standard arduino SIM900 (GSM shield) and SoftwareSerial libraries.

Curiously, this board has a micro SD card slot, but the GSM and SD uses 98% of SRAM, so essentially,
you can't do both. Probably.
