import socket
from inputs import get_gamepad

# Configura la dirección IP y el puerto del ESP32.
ESP32_IP = "192.168.4.22"  # Reemplaza esto con la dirección IP de tu ESP32.
ESP32_PORT = 1234  # Reemplaza esto con el puerto que estás usando en el ESP32.

# Crea un socket UDP.
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def send_event(event):
    """
    Envía un evento al ESP32.
    """
    # Crea un mensaje con el tipo y el valor del evento.
    message = f"{event.ev_type}:{event.code}:{event.state}"
    # Envía el mensaje al ESP32.
    sock.sendto(message.encode(), (ESP32_IP, ESP32_PORT))

def main():
    """
    Lee los eventos del mando y los envía al ESP32.
    """
    while True:
        events = get_gamepad()
        for event in events:
            send_event(event)

if __name__ == "_main_":
    main()