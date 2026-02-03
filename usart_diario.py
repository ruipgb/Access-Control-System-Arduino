import serial
from datetime import datetime
import asyncio
from telegram import Bot

PORT = "COM3"
BAUD = 9600
BOT_TOKEN = "8535941190:AAGSS0F0oHQZNTQEt0Vq8A5jGLLsgTGjcaE"

def get_log_filename():
    """Gera nome do ficheiro de log com base na data atual"""
    date_str = datetime.now().strftime("%Y-%m-%d")
    return f"logs_{date_str}.txt"

async def main_async():
    # lê o chat id
    with open("chat_id.txt", "r", encoding="utf-8") as f:
        chat_id = int(f.read().strip())

    bot = Bot(token=BOT_TOKEN)

    ser = serial.Serial(PORT, BAUD, timeout=1)
    print("A ouvir:", ser.port)

    current_log_file = get_log_filename()
    log = open(current_log_file, "a", encoding="utf-8")
    print("A gravar em:", current_log_file)

    try:
        while True:
            raw = ser.readline().decode(errors="ignore").strip()
            if not raw:
                continue

            # se mudou o dia, troca de ficheiro
            new_log_file = get_log_filename()
            if new_log_file != current_log_file:
                log.close()
                current_log_file = new_log_file
                log = open(current_log_file, "a", encoding="utf-8")
                print("Novo ficheiro de log:", current_log_file)

            ts = datetime.now().strftime("%d/%m/%Y %H:%M:%S")
            line = f"[{ts}] {raw}"

            log.write(line + "\n")
            log.flush()
            print(line)

            # Envio para Telegram com tratamento de erro
            try:
                await bot.send_message(chat_id=chat_id, text=line)
            except Exception as e:
                print(f"[AVISO] Falha ao enviar para Telegram: {e}")

    finally:
        log.close()
        ser.close()

def main():
    asyncio.run(main_async())

if __name__ == "__main__":
    main()