# codigo que une los dos anteriores 

import socket
from inputs import get_gamepad
import cv2
import threading

# Configura la dirección IP y el puerto del ESP32.
ESP32_IP = "192.168.4.22"  # Reemplaza esto con la dirección IP de tu ESP32.
ESP32_PORT = 1234  # Reemplaza esto con el puerto que estás usando en el ESP32.

# Dirección URL de la cámara.
CAMERA_URL = 'http://192.168.4.24:81/stream'

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

def gamepad_thread():
    """
    Lee los eventos del mando y los envía al ESP32.
    """
    while True:
        events = get_gamepad()
        for event in events:
            send_event(event)

def camera_thread():
    """
    Captura y muestra el video de la cámara.
    """
    cap = cv2.VideoCapture(CAMERA_URL)

    while True:
        ret, frame = cap.read()
        if not ret:
            print("No se pudo leer el frame")
            break

        # Aquí puedes procesar el frame como quieras

        cv2.imshow('Video Stream', frame)

        if cv2.waitKey(1) & 0xFF == 27:
            break

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "_main_":
    # Crear los threads
    thread1 = threading.Thread(target=gamepad_thread)
    thread2 = threading.Thread(target=camera_thread)

    # Iniciar los threads
    thread1.start()
    thread2.start()

    # Esperar a que los threads terminen
    thread1.join()
    thread2.join()