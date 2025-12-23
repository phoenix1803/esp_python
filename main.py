import requests
import time
import os

ESP_IP = "10.10.180.224"   # ← PUT YOUR SERIAL MONITOR IP HERE
SAVE_DIR = "esp_readings"

os.makedirs(SAVE_DIR, exist_ok=True)

downloaded = set()

while True:
    for i in range(1, 21):
        filename = f"reading_{i}.json"
        if filename in downloaded:
            continue

        url = f"http://{ESP_IP}/download?file={filename}"

        try:
            r = requests.get(url, timeout=2)

            if r.status_code == 200:
                path = os.path.join(SAVE_DIR, filename)
                with open(path, "w") as f:
                    f.write(r.text)

                downloaded.add(filename)
                print(f"✅ Downloaded {filename}")

        except Exception:
            pass

    time.sleep(2)