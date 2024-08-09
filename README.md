
# NFC Rolling code

This project uses an NFC card reading system to control access based on the unique ID (UID) of each card. It supports a limited number of authorized cards and enhances security by using a rolling code system, generating a new random code for each card every time it is used. The MFRC522 library is used to interact with the NFC card reader, and the Servo library controls a servo motor, which can open a door or activate a mechanism when a card is successfully verified.


## Why?

it's a basic prototype for an NFC-based access control system designed to prevent unauthorized entry and card cloning with tools like Flipper Zero. The system uses rolling codes for added security, making it suitable for securing restricted areas in corporate environments. However, this is only a conceptual implementation and not intended for actual deployment in businesses.

## How to use

Add your card's UID to the allowedUIDs[] array in the source code. Each UID should be represented as a sequence of four hexadecimal bytes. For example:
``` bash
const byte allowedUIDs[][3] = {
  {0xDE, 0xAD, 0xBE, 0xEF}, // Example UID
  {0xCA, 0xFE, 0xBA, 0xBE}, // Another example UID
  // Add your UID here
};
```
