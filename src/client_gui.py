"""
Arduino WiFi Radio Client
Send commands over UDP to control melody playback on the Arduino.

Commands:
  1      – Play Never Gonna Give You Up (Rick Astley)
  2      – Play Für Elise (Beethoven)
  3      – Play Canon in D (Pachelbel)
  stop   – Stop playback
  clear  – Clear the OLED display
  <any>  – Show text on OLED ("Here is your <text> marble")
"""

import socket
import sys
from typing import Optional

ARDUINO_IP = "192.168.2.188"   # ← update to your Arduino's IP
UDP_PORT   = 12345
TIMEOUT    = 3.0               # seconds to wait for a reply
BUFFER     = 255

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

MENU = """
┌──────────────────────────────────────┐
│         Arduino WiFi Radio           │
├──────────────────────────────────────┤
│  1     Never Gonna Give You Up       │
│  2     Für Elise                     │
│  3     Canon in D                    │
│  stop  Stop playback                 │
│  clear Clear OLED display            │
│  quit  Exit this client              │
└──────────────────────────────────────┘
"""


def send_command(cmd: str) -> Optional[str]:
    """Send a command and return the Arduino's reply, or None on timeout."""
    sock.settimeout(TIMEOUT)
    sock.sendto(cmd.encode("utf-8"), (ARDUINO_IP, UDP_PORT))
    print(f"  → Sent   : {cmd!r}")
    try:
        data, addr = sock.recvfrom(BUFFER)
        reply = data.decode("utf-8").strip()
        print(f"  ← Received: {reply!r}  (from {addr[0]}:{addr[1]})")
        return reply
    except socket.timeout:
        print(f"  ✗ No reply within {TIMEOUT}s — check Arduino IP/port.")
        return None


def main() -> None:
    try:
        # One-shot mode:  python client.py stop
        if len(sys.argv) > 1:
            send_command(" ".join(sys.argv[1:]))
            return

        print(MENU)
        print(f"Target : {ARDUINO_IP}:{UDP_PORT}  |  Timeout: {TIMEOUT}s\n")

        while True:
            raw = input("Command: ").strip()
            if not raw:
                continue
            if raw.lower() == "quit":
                print("Exiting.")
                break
            send_command(raw)
            print()

    except KeyboardInterrupt:
        print("\nExiting.")
    finally:
        sock.close()


if __name__ == "__main__":
    main()
